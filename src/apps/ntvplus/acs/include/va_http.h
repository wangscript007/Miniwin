/******************************************************************************
 
    Copyright
    This code is strictly confidential and the receiver is obliged to use it 
    exclusively for his or her own purposes. No part of Viaccess code may be 
    reproduced or transmitted in any form or by any means, electronic or 
    mechanical, including photocopying, recording, or by any information storage 
    and retrieval system, without permission in writing from Viaccess. 
    The information in this code is subject to change without notice. Viaccess 
    does not warrant that this code is error free. If you find any problems 
    with this code or wish to make comments, please report them to Viaccess.
 
    Trademarks 
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/


#ifndef _VA_HTTP_H_
#define _VA_HTTP_H_

#include "va_types.h"

/**
 @ingroup std_drivers
 */

/*-----------------------------------------------------------------------------------------------*/

/**
    @defgroup httpDriver VA_HTTP API
    @brief This API allows to manage HTTP requests.
    @{
 */

/*-----------------------------------------------------------------------------------------------*/

/**
    @defgroup vahttpConst VA_HTTP Constants and Types
    @brief The VA_HTTP constants and types
    @{
*/

/*-----------------------------------------------------------------------------------------------*/

/** Generic OK */
#define kVA_HTTP_OK                            0

/** Generic error code */
#define kVA_HTTP_ERROR                        -1

/** Invalid handle error */
#define kVA_HTTP_INVALID_HANDLE               -2

/** Invalid parameter */
#define kVA_HTTP_INVALID_PARAMETER            -3

/*-----------------------------------------------------------------------------------------------*/

/** Type definition of a request handle (non 64 bits compliant) */
typedef DWORD tVA_HTTP_HANDLE;

/** Predefined value of an invalid handle.
   @note As 0 is a normative value, it will never change. */
#define kVA_INVALID_HANDLE                     0

/*-----------------------------------------------------------------------------------------------*/

/**
    The HTTP request method.
*/
typedef enum
{
    eVA_HTTP_Get,            /**< Get method */
    eVA_HTTP_Post            /**< Post method */

} tVA_HTTP_RequestMethod;

/**
    The HTTP request status.
*/
typedef enum
{
    eVA_HTTP_Ok,            /**< No error */
    eVA_HTTP_OpenFailed,    /**< Connection to server failed */
    eVA_HTTP_Disconnected,  /**< Connection lost or any error */
    eVA_HTTP_Timedout       /**< Nothing received from server */

} tVA_HTTP_RequestStatus;

/**
    The string list structure.
*/
typedef struct _sVA_HTTP_StrList
{
    char *szString;
    struct _sVA_HTTP_StrList *pNext;

} tVA_HTTP_StrList;

/*-----------------------------------------------------------------------------------------------*/

/**
    This function type is used to notify of any network state changes.

    @param [in] bIsUp           This parameter is TRUE when the network state is up, else
                                it is FALSE when the network state is down.
*/
typedef void (*tVA_HTTP_NetworkStateNotifyFn)(BOOL bIsUp);

/**
    This function type is used to notify that the request is done.

    @param [in] hCbkContext     This parameter is sent back as a parameter of the @ref VA_HTTP_DoRequest function.
    @param [in] hHttpHandle     This parameter gives the HTTP request handle given when it is created. @n
                                Refer to @ref VA_HTTP_CreateRequest for details.
    @param [in] eStatus         This parameter gives the status of the request.

    @retval kVA_HTTP_INVALID_HANDLE   If the hHttpHandle parameter does not refer to a valid HTTP request handle.
    @retval kVA_HTTP_OK               If there is no error.

    @see VA_HTTP_DoRequest
*/
typedef INT (*tVA_HTTP_ReceiveNotifyFn)(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_HANDLE hCbkContext, tVA_HTTP_RequestStatus eStatus);


/*-----------------------------------------------------------------------------------------------*/

/** @} */

/*-----------------------------------------------------------------------------------------------*/

/**
    @defgroup vahttpFunc VA_HTTP STB Manufacturer Functions
    @brief The VA_HTTP functions to be implemented by the terminal manufacturer
    @{ 
*/

/**
    This function is used to initialize the VA_HTTP driver.

    @param [in] pfVA_HTTP_NetworkStateNotify This parameter gives a pointer to a callback function. @n
                                             This callback function must be called every time the network state changes.

    @retval TRUE    The current network state is up.
    @retval FALSE   The current network state is down.
*/
BOOL VA_HTTP_Init(tVA_HTTP_NetworkStateNotifyFn pfVA_HTTP_NetworkStateNotify);

/**
    This function is used to terminate the VA_HTTP driver.

    @note The network state notify function must not be called after this function returns.
*/
void VA_HTTP_Terminate(void);

/**
    This function is used to create an HTTP request handle.

    @retval kVA_INVALID_HANDLE      If there is an error.
    @retval hHttpHandle             Else : the request handle.

    @see VA_HTTP_DeleteRequest
*/
tVA_HTTP_HANDLE VA_HTTP_CreateRequest(void);

/**
    This function is used to delete an HTTP request handle.

    @retval kVA_HTTP_INVALID_HANDLE If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_OK             If there is no error.

    @see VA_HTTP_CreateRequest
*/
INT VA_HTTP_DeleteRequest(tVA_HTTP_HANDLE hHttpHandle);

/**
    This function is used to set a custom HTTP header.
    @note The driver can set some default headers automatically. This function overrides those headers but
    custom headers should not be set twice. Call @ref VA_HTTP_ResetHeaders() to remove all custom headers.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] szHeader                    This parameter gives the header to set (Name: value).

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If szHeader is NULL.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetHeader(tVA_HTTP_HANDLE hHttpHandle, const char * szHeader);

/**
    This function is used to reset all custom HTTP headers.
    @note The driver can set some default headers automatically. This function restores their default values.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_ResetHeaders(tVA_HTTP_HANDLE hHttpHandle);

/**
    This function is used to enable the cookies persistency. @n
    By default, cookies persistency is disabled.
    It must enable the cookie engine ie. the filtering of cookies against the URI.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetCookiePersistence(tVA_HTTP_HANDLE hHttpHandle);

/**
    This function is used to set a list of cookies. @n
    It must enable the cookie engine ie. the filtering of cookies against the URI.
    Each cookie must be in Netscape / Mozilla format.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] pCookieList                 This parameter gives the cookie list. @n
                                            Set it to NULL to clear all stored cookies.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetCookieList(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_StrList *pCookieList);

/**
    This function is used to set the HTTP method of the request (ie. GET, POST,...).

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] eMethod                     This parameter gives the HTTP method to use.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If eMethod is not a valid tVA_HTTP_RequestMethod value.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetMethod(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_RequestMethod eMethod);

/**
    This function is used to set the POST data of the request.
    @note This automatically sets the request to POST method if pData is not NULL;
    else the request is set to GET method.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] pData                       This parameter gives the data to set in the request body. @n
    @param [in] uiLength                    This parameter gives the length of pData.
    @param [in] bCopyData                   This parameter is TRUE when copy of the data by the function is required.
                                            If it is FALSE, the data are NOT copied by the function: as a consequence, they
                                            must be kept available by the calling application until the request finishes. 

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If pData is NULL and uiLength is not 0. @n
                                            If pData is not NULL and uiLength is 0.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetPostData(tVA_HTTP_HANDLE hHttpHandle, const BYTE * pData, UINT32 uiLength, BOOL bCopyData);

/**
    This function is used to set a timeout value. By default, there is no timeout.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] uiNbSeconds                 This parameter gives the value of the timeout. @n
                                            Set this parameter to 0 to have no timeout.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetTimeout(tVA_HTTP_HANDLE hHttpHandle, UINT32 uiNbSeconds);

/**
    This function is used to set TCP keep-alive probes. By default, the TCP keep-alive probes are disabled.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] uiNbSeconds                 This parameter gives the value of the time interval between probes.
                                            The first probe is sent after this delay when the connection is idle. @n
                                            Set this parameter to 0 to disable the TCP keep-alive probes.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_SetTcpKeepAlive(tVA_HTTP_HANDLE hHttpHandle, UINT32 uiNbSeconds);

/**
    This function is used to perform an HTTP request. By default, it is a GET method.
    @warning This function must not be blocking !
    @note If the scheme is HTTPS, the driver should automatically set the protocol to SSL and set the right certificate.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] szUri                       This parameter gives the absolute-URI of the request (scheme://host:port/path).
    @param [in] pfVA_HTTP_ReceiveNotifyCbk  This parameter gives a pointer to a callback function. @n
                                            The callback function must be called if a timeout is provided and reached.
    @param [in] hCbkContext                 This parameter is sent back as a parameter of the pfVA_HTTP_ReceiveNotifyCbk() function.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If szUri is NULL. @
                                            If pfVA_HTTP_ReceiveNotifyCbk is NULL.
    @retval kVA_HTTP_ERROR                  If an internal error occurred.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_DoRequest(tVA_HTTP_HANDLE hHttpHandle, const char * szUri, 
                      tVA_HTTP_ReceiveNotifyFn pfVA_HTTP_ReceiveNotifyCbk, tVA_HTTP_HANDLE hCbkContext);

/**
    This function is used to get the last response data.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [out] ppData                     This parameter gives the pointer to the buffer that will contain the pointer
                                            to the response data allocated by the driver.@n
                                            **The buffer is deallocated by the driver when the client calls @ref VA_HTTP_ReleaseBuffer.** @n
                                            *ppData is set to NULL if there is no response data.
    @param [out] pLength                    This parameter gives the pointer to the length of the buffer
                                            that will contain the response data.@n
                                            *pLength is set to 0 if there is no response data.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If ppData is NULL. @n
                                            If pLength is 0.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_GetResponseData(tVA_HTTP_HANDLE hHttpHandle, BYTE ** ppData, UINT32 * pLength);

/**
    This function is used to release a buffer allocated by the driver and passed to the client.

    @param pBuffer  This parameter gives the pointer to the buffer to release.

    @retval kVA_HTTP_INVALID_PARAMETER   If pBuffer is NULL.
    @retval kVA_HTTP_OK                  Else : if the buffer is successfully released.
*/
INT VA_HTTP_ReleaseBuffer(void * pBuffer);

/**
    This function is used to get the last response code.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [out] pCode                      This parameter gives the pointer to the response code.@n
                                            *pCode is set to 0 if there is no response.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If pCode is NULL.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_GetResponseCode(tVA_HTTP_HANDLE hHttpHandle, UINT32 * pCode);

/**
    This function is used to get a list of cookies. @n
    Each cookie must be in Netscape / Mozilla format.

    @param [in] hHttpHandle                 This parameter gives the HTTP request handle.
    @param [in] ppCookieList                This parameter gives the pointer to the cookie list.@n
                                            **The list is deallocated by the driver when the client calls @ref VA_HTTP_ReleaseStrList.** @n
                                            *ppCookieList is set to NULL if there is no cookie.

    @retval kVA_HTTP_INVALID_HANDLE         If hHttpHandle is not a valid HTTP request handle.
    @retval kVA_HTTP_INVALID_PARAMETER      If ppCookieList is NULL.
    @retval kVA_HTTP_OK                     If there is no error.
*/
INT VA_HTTP_GetCookieList(tVA_HTTP_HANDLE hHttpHandle, tVA_HTTP_StrList **ppCookieList);

/**
    This function is used to release a list allocated by the driver and passed to the client.

    @param pList  This parameter gives the pointer to the list to release.

    @retval kVA_HTTP_INVALID_PARAMETER   If pList is NULL.
    @retval kVA_HTTP_OK                  Else : if the list is successfully released.
*/
INT VA_HTTP_ReleaseStrList(tVA_HTTP_StrList * pList);

/** @} */

/*-----------------------------------------------------------------------------------------------*/

/** @} */

/*-----------------------------------------------------------------------------------------------*/

#endif /* _VA_HTTP_H_ */

