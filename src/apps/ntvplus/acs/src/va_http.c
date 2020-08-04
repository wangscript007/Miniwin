#include<va_http.h>
#define CURL_STATICLIB
#include "curl/curl.h"
#include <va_os.h>
#include <net/if.h>
#include <errno.h>
#include <ngl_types.h>
#include <ngl_log.h>
NGL_MODULE(VAHTTP)
typedef struct _sVA_HTTP_Request
{
    CURL                    * curl;
    struct curl_slist       * headers;
    tVA_HTTP_ReceiveNotifyFn  pfReceiveNotifyCbk;
    tVA_HTTP_HANDLE           cbk_context;
    char                    * response_data;
    UINT32                    response_len;
    BOOL                      is_alive;
} tVA_HTTP_Request;

static tVA_HTTP_NetworkStateNotifyFn g_pfVA_HTTP_NetworkStateNotify = NULL;
static size_t _WriteMemoryCbk(void *contents, size_t size, size_t nmemb, void *userp);
static void * _VA_HTTP_RequestThread(void * param);
#define TRACE(x) NGLOG_DEBUG(x)
struct ethtool_value 
{
    __uint32_t cmd;
    __uint32_t data;
};
 
/*return 1:has cable;   return 0:no cable*/
static int detect_eth_cable(char *ifname){
    struct ethtool_value edata;
    struct ifreq ifr;
    int fd = -1, err = 0;
 
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, ifname);
 
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return -1;
    }
    edata.cmd = 0x0000000A;
    ifr.ifr_data = (caddr_t)&edata;
    err = ioctl(fd, 0x8946, &ifr);
    return(edata.data==1 ? 1:0);
}

static INT lastState=-1;
static INT CheckETH(DWORD data){
    int isUP=detect_eth_cable("eth0");
    if( (isUP!=lastState) && g_pfVA_HTTP_NetworkStateNotify){
        NGLOG_DEBUG("CheckETH isUp=%d",isUP);
        g_pfVA_HTTP_NetworkStateNotify(isUP);
    }
    lastState=isUP;
}

void CURL_Init(){
    static int CURL_INITED=0;
    if(0==CURL_INITED){
        NGLOG_DEBUG("===========CURL_Init===============");
        CURL_INITED++;
        curl_global_init(CURL_GLOBAL_ALL);
        VA_OS_StartPeriodicCall(1000,CheckETH,0);
    }
}
BOOL VA_HTTP_Init(tVA_HTTP_NetworkStateNotifyFn pfVA_HTTP_NetworkStateNotify)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    g_pfVA_HTTP_NetworkStateNotify = pfVA_HTTP_NetworkStateNotify;
    return lastState==1;
}

void VA_HTTP_Terminate(void)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    g_pfVA_HTTP_NetworkStateNotify = NULL;
    //g_VA_HTTP_NetworkIsUp          = TRUE;
    curl_global_cleanup();
}

tVA_HTTP_HANDLE VA_HTTP_CreateRequest(void)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = NULL;
    CURL * curl;
    curl = curl_easy_init();
    if (!curl)
        return kVA_INVALID_HANDLE;
    request = (tVA_HTTP_Request*) malloc(sizeof(tVA_HTTP_Request));
    if (request == NULL)
    {
        curl_easy_cleanup(curl);
        return kVA_INVALID_HANDLE;
    }
    request->curl               = curl;
    request->headers            = NULL;
    request->response_data      = NULL;
    request->response_len       = 0;
    request->pfReceiveNotifyCbk = NULL;
    request->cbk_context        = 0;
    request->is_alive           = TRUE;
    return (tVA_HTTP_HANDLE) request;
}

INT VA_HTTP_DeleteRequest(tVA_HTTP_HANDLE hHttpHandle)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (request == NULL)
        return kVA_HTTP_INVALID_HANDLE;
    TRACE(("delete request %d",(int)request )) ;
    if (request->pfReceiveNotifyCbk == NULL)
    {
        curl_easy_cleanup(request->curl);
        free(request);
    }
    else
    {
        /* A request is in progress : Wait for the end of request to free it */
        request->is_alive = FALSE;
    }
    return kVA_HTTP_OK;
}

/*
    This function called by libcurl (CURLOPT_WRITEFUNCTION) allows to get the response data.
 */
static size_t _WriteMemoryCbk(void *contents, size_t size, size_t nmemb, void *userp)
{
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)userp;
    size_t realsize = size * nmemb;
    TRACE(("request %d callback : %u bytes (%.30s)",(int)request,realsize,contents )) ;
    request->response_data = (char*) realloc(request->response_data, request->response_len + realsize + 1);
    memcpy(&(request->response_data[request->response_len]), contents, realsize);
    request->response_len += (UINT32)realsize;
    request->response_data[request->response_len] = 0;
    return realsize;
}
/*
    This function is used to perform a HTTP request. By default, it is a GET method.
    @warning This function must not be blocking !
*/
INT VA_HTTP_DoRequest(tVA_HTTP_HANDLE hHttpHandle, const char * szUri,
                      tVA_HTTP_ReceiveNotifyFn pfVA_HTTP_ReceiveNotifyCbk, tVA_HTTP_HANDLE hCbkContext)
{
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    pthread_t thread;
    NGLOG_VERBOSE("uri=%s",szUri);
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    if (!szUri || !pfVA_HTTP_ReceiveNotifyCbk)
        return kVA_HTTP_INVALID_PARAMETER;
    request->pfReceiveNotifyCbk = pfVA_HTTP_ReceiveNotifyCbk;
    request->cbk_context        = hCbkContext;
    if (request->response_data)
        free(request->response_data);
    request->response_data      = (char*) malloc(1);
    request->response_len       = 0;
    curl_easy_setopt(request->curl, CURLOPT_HTTPHEADER, request->headers);
    curl_easy_setopt(request->curl, CURLOPT_URL, szUri);
    curl_easy_setopt(request->curl, CURLOPT_WRITEFUNCTION, _WriteMemoryCbk);
    curl_easy_setopt(request->curl, CURLOPT_WRITEDATA, (void *)request);
    /* if szUri starts with "https://,
           curl_easy_setopt(request->curl, CURLOPT_CAPATH, "E:\\VSProjects\\ZAP\\certs");
           curl_easy_setopt(request->curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
           if szUri host is xxx,
               curl_easy_setopt(curl, CURLOPT_CAPATH, xxx_cert_cat); */
    if (pthread_create(&thread, NULL, _VA_HTTP_RequestThread, (void*) request) != 0)
    {
        return kVA_HTTP_ERROR;
    }
    return kVA_HTTP_OK;
}
/*
    Thread used to perform each request.
    It is deleted at the end of the request.
 */
static void * _VA_HTTP_RequestThread(void * param)
{
    tVA_HTTP_Request * request = (tVA_HTTP_Request*) param;
    CURLcode code;
#define _mCurl2VoHttp(_code) \
    (((_code) == CURLE_OK)                 ? eVA_HTTP_Ok         : \
     ((_code) == CURLE_COULDNT_CONNECT)    ? eVA_HTTP_OpenFailed : \
     ((_code) == CURLE_OPERATION_TIMEDOUT) ? eVA_HTTP_Timedout   : \
                                             eVA_HTTP_Disconnected )
    pthread_detach(pthread_self());
    TRACE(("enter request %d thread",(int)request )) ;
    if (request != NULL)
    {
        code = curl_easy_perform(request->curl);
        TRACE(("request %d performed : %d (%s)",(int)request,code,curl_easy_strerror( code ))) ;
        if (request->is_alive == FALSE)
        {
            /* The request has been deleted */
            TRACE(("request %d is already dead!",(int)request )) ;
            curl_easy_cleanup(request->curl);
            free(request);
        }
        else if (request->pfReceiveNotifyCbk != NULL)
        {
            /* Notify the request is done.
               Note : save the callback function and reset it in the request
               in order to notice that the request is not in progress anymore */
            tVA_HTTP_RequestStatus status = _mCurl2VoHttp(code);
            tVA_HTTP_ReceiveNotifyFn pfReceiveNotifyCbk = request->pfReceiveNotifyCbk;
            request->pfReceiveNotifyCbk = NULL;
            pfReceiveNotifyCbk((tVA_HTTP_HANDLE)request, request->cbk_context, status);
        }
    }
    TRACE(("exit request %d thread",(int)request )) ;
    pthread_exit(NULL);
    return NULL;
}

INT VA_HTTP_SetHeader(tVA_HTTP_HANDLE hHttpHandle, const char * szHeader)
{
    NGLOG_VERBOSE("Header=%s",szHeader);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!szHeader)
        return kVA_HTTP_INVALID_PARAMETER;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    request->headers = curl_slist_append(request->headers, szHeader);
    return kVA_HTTP_OK;
}

INT VA_HTTP_ResetHeaders(tVA_HTTP_HANDLE hHttpHandle)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    if (request->headers)
        curl_slist_free_all(request->headers);
    request->headers = NULL;
    return kVA_HTTP_OK;
}

INT VA_HTTP_SetCookiePersistence(tVA_HTTP_HANDLE hHttpHandle)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    curl_easy_setopt(request->curl, CURLOPT_COOKIEFILE, "cookies.txt");
    curl_easy_setopt(request->curl, CURLOPT_COOKIEJAR, "cookies.txt");
    return kVA_HTTP_OK;
}

INT VA_HTTP_SetCookieList(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_StrList *pCookieList)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    if (!pCookieList)
    {
        /* delete all cookies */
        curl_easy_setopt(request->curl, CURLOPT_COOKIELIST, "ALL");
        /* and activate cookie engine */
        curl_easy_setopt(request->curl, CURLOPT_COOKIELIST, "");
    }
    else
    {
        while (pCookieList)
        {
            curl_easy_setopt(request->curl, CURLOPT_COOKIELIST, pCookieList->szString);
            pCookieList = pCookieList->pNext;
        }
    }
    return kVA_HTTP_OK;
}

INT VA_HTTP_SetMethod(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_RequestMethod eMethod)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request) return kVA_HTTP_INVALID_HANDLE;
    switch(eMethod)
    {
        case eVA_HTTP_Get: curl_easy_setopt(request->curl, CURLOPT_HTTPGET, 1); break;
        case eVA_HTTP_Post: curl_easy_setopt(request->curl, CURLOPT_POST, 1); break;
        default: return kVA_HTTP_INVALID_PARAMETER;
    }
    return kVA_HTTP_OK;
}

INT VA_HTTP_SetPostData(tVA_HTTP_HANDLE hHttpHandle, const BYTE * pData, UINT32 uiLength, BOOL bCopyData)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
   tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request) return kVA_HTTP_INVALID_HANDLE;
    if (!pData != !uiLength) return kVA_HTTP_INVALID_PARAMETER;
    curl_easy_setopt(request->curl, pData ? CURLOPT_POST : CURLOPT_HTTPGET, 1);
    if (pData)
    {
        curl_easy_setopt(request->curl, CURLOPT_POSTFIELDSIZE, uiLength);
        if (bCopyData)
            curl_easy_setopt(request->curl, CURLOPT_COPYPOSTFIELDS, (char*) pData);
        else
            curl_easy_setopt(request->curl, CURLOPT_POSTFIELDS, (char*) pData);
    }
    return kVA_HTTP_OK;
}

INT VA_HTTP_SetTimeout(tVA_HTTP_HANDLE hHttpHandle, UINT32 uiNbSeconds)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    curl_easy_setopt(request->curl, CURLOPT_TIMEOUT, uiNbSeconds);
    return kVA_HTTP_OK;
}

INT VA_HTTP_SetTcpKeepAlive(tVA_HTTP_HANDLE hHttpHandle, UINT32 uiNbSeconds)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    if (uiNbSeconds > 0)
    {
        curl_easy_setopt(request->curl, CURLOPT_TCP_KEEPALIVE, 1);
        curl_easy_setopt(request->curl, CURLOPT_TCP_KEEPIDLE, uiNbSeconds);
        curl_easy_setopt(request->curl, CURLOPT_TCP_KEEPINTVL, uiNbSeconds);
    }
    else
        curl_easy_setopt(request->curl, CURLOPT_TCP_KEEPALIVE, 0);
    return kVA_HTTP_OK;
}


INT VA_HTTP_GetResponseData(tVA_HTTP_HANDLE hHttpHandle, BYTE ** ppData, UINT32 * pLength)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request) return kVA_HTTP_INVALID_HANDLE;
    if (!ppData || !pLength) return kVA_HTTP_INVALID_PARAMETER;
    *ppData = NULL;
    *pLength = 0;
    TRACE(("request %d : get response data %u bytes",(int)request,request->response_len )) ;
    if (request->response_data)
    {
        *ppData = (BYTE*) malloc(request->response_len);
        memcpy(*ppData, request->response_data, request->response_len);
        *pLength = request->response_len;
    }
    return kVA_HTTP_OK;
}

INT VA_HTTP_ReleaseBuffer(void * pBuffer)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    if (!pBuffer)
        return kVA_HTTP_INVALID_PARAMETER;
    free(pBuffer);
    return kVA_HTTP_OK;
}

INT VA_HTTP_GetResponseCode(tVA_HTTP_HANDLE hHttpHandle, UINT32 * pCode)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    if (!pCode)
        return kVA_HTTP_INVALID_PARAMETER;
    curl_easy_getinfo(request->curl, CURLINFO_RESPONSE_CODE, (long*) pCode);
    return kVA_HTTP_OK;
}

INT VA_HTTP_GetCookieList(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_StrList **ppCookieList)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    tVA_HTTP_Request * request = (tVA_HTTP_Request *)hHttpHandle;
    if (!request)
        return kVA_HTTP_INVALID_HANDLE;
    if (!ppCookieList)
        return kVA_HTTP_INVALID_PARAMETER;
    *ppCookieList = NULL;
    curl_easy_getinfo(request->curl, CURLINFO_COOKIELIST, (struct curl_slist **)ppCookieList);
    return kVA_HTTP_OK;
}

INT VA_HTTP_ReleaseStrList(tVA_HTTP_StrList * pList)
{
    NGLOG_VERBOSE("%s",__FUNCTION__);
    if (!pList)
        return kVA_HTTP_INVALID_PARAMETER;
    curl_slist_free_all((struct curl_slist *)pList);
    return kVA_HTTP_OK;
}


