/**
 * @file UniversalClient.c
 */

//////////////////////////////////////////////////////////////////////////////////////////
//  include header file
//////////////////////////////////////////////////////////////////////////////////////////
#include "string.h"
#include "stdio.h"
#include "common/gxmalloc.h"
#include "UniversalClient_API.h" // needed for base API calls
#include "UniversalClient_Common_API.h" // needed for service management functions
#include "UniversalClient_DVB_API.h" //needed for DVB-specific functions
#include "UniversalClient.h"
#include <ngl_msgq.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <ngl_log.h>
#include <ngl_os.h>
#include <ngl_dmx.h>
#include <dvbepg.h>
#include <tvdata.h>
#include <UniversalClient_Stdlib.h>
#include <descriptors.h>
#include <LoaderCoreAPI_ApplicationHelper.h>
#include "UniversalClient_SPI.h"
#include <CCA_UCMsgProc.h>
#include <appCCADecoderMsg.h>
#include <MApp_ZUI_ACTCCACasCmd.h>
#include <ccairdevent.h>
#include <dvbapp.h>
NGL_MODULE(UNIVERSALCLIENT);

using namespace cca;
//#include "CCA_UCMsgProc.h"
#if(CA_CCA_PVR_SUP == 1)
#include "CCA_UCPvrProc.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Definition
//////////////////////////////////////////////////////////////////////////////////////////
#define UC_SUPPORT_MACROVISION
#define NGL_OK    0
#define NGL_ERROR -1
#define NGL_INVALID_PARA -2

typedef enum
{
    PVR_CCA_REC_NOT_ALLOW,
    PVR_CCA_REC_ALLOW,
    PVR_CCA_PLAY_NOT_ALLOW,
    PVR_CCA_PLAY_ALLOW,

} PVR_CCA_REPLY;


//////////////////////////////////////////////////////////////////////////////////////////
//  global variable
//////////////////////////////////////////////////////////////////////////////////////////
uc_emm_st    gEMMStreamInfo;
uc_ecm_st    gECMStreamInfo;
#if(CA_CCA_PVR_SUP == 1)
uc_pvr_st    gPVRStreamInfo;
#endif
uc_system_st    g_ucSystem;
//Task_Info       g_ucPsTask;
spi_uc_system_st    s_spi_ucSystem;
UCSPI_SC_INFO   Sc_Info; //yenjen.0428

//#define ECM_QUEUE_SIZE						(1024)
//#define EMM_QUEUE_SIZE						(4096+8)
uc_buffer_st		g_ucCatBuff;
uc_buffer_st		g_ucPmtBuff;
#if 0
MS_S32 gs32CAMemPoolID = 0;
#endif

//////////////////////////////////////////////////////////////////////////////////////////
//  static variable
//////////////////////////////////////////////////////////////////////////////////////////
static unsigned short pmtPids[64]={0,0};
static unsigned short num_elements=0;
static unsigned short elements[16]={0,0};
static unsigned int last_tsid=0xFFFFFFFF;//networkid<<16|tsid
static unsigned int last_tpid=0xFFFFFFFF;
//static  unsigned short last_svcid=0xFFFFFFFF;
static SERVICELOCATOR lastplayed={0xFFFF,0xFFFF,0xFFFF,0xFFFF};
static HANDLE filter_cat=0,filter_pmt=0;
static DWORD playerq=0;
static DWORD playerevent=0;
static int switchon=1;
//char PMTSection[1024];
//static char CATSection[1024];
CCAIRDEventSource* CCAIRDEventSource::mInst=nullptr;

//////////////////////////////////////////////////////////////////////////////////////////
//  external function
//////////////////////////////////////////////////////////////////////////////////////////
 static void SectionCBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer ,UINT uiBufferLength ,void *pUserData );
//yenjen.0428 {.

//extern void Ird_DebugParam(uc_uint32 debugParam);

//yenjne.0428 .}

unsigned char arrCAT[] = {0x01,0xB0,0x0F,0xFF,0xFF,0xC1,0x00,0x00,
                          0x09,0x04,0x06,0x93,0xE5,0x00,0x84,0x6B,
                          0xD1,0x49};

unsigned char arrPMT101[] = {0x02,0xB0,0x17,0x01,0x01,0xC1,0x00,0x00,
                             0xE2,0x00,0xF0,0x00,0x02,0xE2,0x00,0xF0,
                             0x00,0x04,0xE2,0x01,0xF0,0x00,0xBD,0x1D,
                             0xA2,0x28};

#if 0 //the cat and pmt of smartinit stream
unsigned char arrCAT[] = {0x01,0xb0,0x15,0xff,0xff,0xc1,0x00,0x00,0x09,0x04,0x06,0x24,0xe5,0x00,0x09,0x04,
						  0x06,0x04,0xe5,0x01,0x35,0x23,0x7d,0xa9};



unsigned char arrPMT101[] = {0x02,0xb0,0x29,0x00,0x65,0xc3,0x00,0x00,0xe2,0x10,0xf0,0x0c,0x09,0x04,0x06,0x24,
						     0xe2,0x12,0x09,0x04,0x06,0x04,0xe2,0x13,0x02,0xe2,0x10,0xf0,0x00,0x04,0xe2,0x11,
							 0xf0,0x06,0x0a,0x04,0x65,0x6e,0x67,0x00,0x2b,0x3d,0x7f,0xa1};
#endif

unsigned char arrPMT102[] = {0x02,0xB0,0x1D,0x01,0x02,0xC1,0x00,0x00,
                             0xE2,0x10,0xF0,0x06,0x09,0x04,0x06,0x93,
                             0xE2,0x12,0x02,0xE2,0x10,0xF0,0x00,0x04,
                             0xE2,0x11,0xF0,0x00,0x71,0x74,0x32,0x8F};


#define LIVE_MODE_DMX_CAT 0
#if (LIVE_MODE_DMX_CAT == 1)
#define INVALID_ID          0xFF

typedef struct
{
    uc_uint16 u16Pid;
    uc_uint32 u32Pat0;
    uc_uint32 u32Pat1;
    uc_uint32 u32Mask0;
    uc_uint32 u32Mask1;
    void* pCB;
    uc_bool bCrcChk;
} _stDMXSecArg;

static uc_uint16 _u16PmtDmxId = INVALID_ID;
static uc_uint16 _u16CatDmxId = INVALID_ID;

uc_uint8 Dmx_Sec_StartByArg(_stDMXSecArg *stArg)
{
    uc_uint8 DmxIdSect;
    uc_uint16 u16Pid;
    DMX_FILTER_TYPE FilterType = 0;
    uc_uint8 pattern[16] = {0};
    uc_uint8 mask[16] = {0};
    uc_uint8 nmask[16] = {0};

    DMX_Flt_info FltInfo;
    uc_uint8* pu8BufAddr = NULL;

    memset(&FltInfo, 0x0, sizeof(DMX_Flt_info));

    u16Pid = stArg->u16Pid;

    pattern[0] = (stArg->u32Pat0 >> 24);
    pattern[1] = (stArg->u32Pat0 >> 16) & 0xFF;
    pattern[2] = (stArg->u32Pat0 >> 8) & 0xFF;
    pattern[3] = (stArg->u32Pat0) & 0xFF;

    pattern[4] = (stArg->u32Pat1 >> 24);
    pattern[5] = (stArg->u32Pat1 >> 16) & 0xFF;
    pattern[6] = (stArg->u32Pat1 >> 8) & 0xFF;
    pattern[7] = (stArg->u32Pat1) & 0xFF;

    mask[0] = (stArg->u32Mask0 >> 24);
    mask[1] = (stArg->u32Mask0 >> 16) & 0xFF ;
    mask[2] = (stArg->u32Mask0 >> 8) & 0xFF ;
    mask[3] = (stArg->u32Mask0) & 0xFF ;

    mask[4] = (stArg->u32Mask1 >> 24);
    mask[5] = (stArg->u32Mask1 >> 16) & 0xFF ;
    mask[6] = (stArg->u32Mask1 >> 8) & 0xFF ;
    mask[7] = (stArg->u32Mask1) & 0xFF ;


    // Set Parameter of Allocate Demux Filter type and input Source type.
    FilterType = DMX_FILTER_SOURCE_TYPE_LIVE | DMX_FILTER_TYPE_SECTION;

    // Allocate a Filter and set Filter Basic Type
    if (DMX_FILTER_STATUS_OK != MApi_DMX_Open(FilterType, &DmxIdSect))
    {
        NGLOG_ERROR("[%s] Allocate filter fail \n",__FUNCTION__);
    }

    // Here we use pre-defined physical address of reserved section buffer.
    // [Note] The section buffe MUST be continus in physical address space.(for DMA usage)
    pu8BufAddr = (uc_uint8*)Section_Buf_Addr[DmxIdSect] ;
    memset((uc_uint8*)MsOS_PA2KSEG1((uc_uint32)pu8BufAddr), 0 , SECTION_BUFFER_SIZE);

    // Transfer Virtual Address to Phisical Hardware Address
    // Section buffer is structed as ring buffer, keep 4 pointer of start,end ,read and write.
    FltInfo.Info.SectInfo.SectBufAddr = (uc_uint32)pu8BufAddr ;

    NGLOG_DEBUG("Physical Address = %08lx\n" ,FltInfo.Info.SectInfo.SectBufAddr);
    // Set buffer size
    FltInfo.Info.SectInfo.SectBufSize =     SECTION_BUFFER_SIZE;
    NGLOG_DEBUG("Size = %08lx\n" ,FltInfo.Info.SectInfo.SectBufSize);

    if(stArg->bCrcChk == TRUE)
    {
        FltInfo.Info.SectInfo.SectMode    |=  DMX_SECT_MODE_CRCCHK ;
    }


    FltInfo.Info.SectInfo.SectMode    |=  DMX_SECT_MODE_ONESHOT;


    // <DMX_EVENT_DATA_READY/DMX_EVENT_BUF_OVERFLOW>
    // Event trigger condition for driver, Driver will call ap's callback  < CallBack Mode Type2 >
    FltInfo.Info.SectInfo.Event        =     DMX_SECT_MODE_CONTI|DMX_EVENT_DATA_READY | DMX_EVENT_BUF_OVERFLOW | DMX_EVENT_CB_SELF ;

    // Set the pointer of the event CB function into Demux driver
    FltInfo.Info.SectInfo.pNotify =   (DMX_NotifyCb) stArg->pCB;

    // Set Advanced Filter infomation
    if (DMX_FILTER_STATUS_OK != MApi_DMX_Info( DmxIdSect, &FltInfo, &FilterType, TRUE))
    {
        NGLOG_ERROR("[%s] MApi_DMX_Info fail \n",__FUNCTION__);
    }

    // Set Filter PID --> Section PID
    if (DMX_FILTER_STATUS_OK != MApi_DMX_Pid( DmxIdSect , &u16Pid , TRUE))
    {
        NGLOG_ERROR("[%s] MApi_DMX_Pid fail \n",__FUNCTION__);
    }

    // Set Section Match pattern
    // The Match pattern is used for Getting specific section
    // Pattern[16] =  Pattern for match
    // Mask[16]    =  Match Mask : set 1 for match / 0 for ignore
    NGLOG_DEBUG("DMX set section pattern\n");
    MApi_DMX_SectPatternSet(DmxIdSect, pattern, mask, nmask, 16);

    // Reset Section filter and section Buffer status;
    NGLOG_DEBUG("DMX section reset\n");
    MApi_DMX_SectReset(DmxIdSect);

    NGLOG_DEBUG("DMX start\n");
    // Start Filter and record section into Section Buffer.
    if (DMX_FILTER_STATUS_OK!= MApi_DMX_Start(DmxIdSect))
    {
        NGLOG_ERROR("enable section filter fail\n");
    }

    return DmxIdSect;
}

static void _CAT_Parse(uc_uint8 u8DmxId, DMX_EVENT eFilterStatus, uc_uint32 u32Type2NotifyParam1)
{
    uc_uint32 u32ActualSize = 0;
    uc_uint32 u32RmnSize = 0;
//    uc_uint32 u32Seconds = 0;
    uc_uint8 au8Section[1024]; //CAT section length + 3

    memset(au8Section, 0 , sizeof(au8Section));

    if(eFilterStatus == DMX_EVENT_DATA_READY)
    {
        NGLOG_DEBUG( "<<<<-----CAT Table Updated\n");
        if(DMX_FILTER_STATUS_OK!= MApi_DMX_CopyData(u8DmxId, au8Section, 1024, &u32ActualSize, &u32RmnSize, NULL))
        {
            NGLOG_ERROR("[%s][%d] error\n", __FUNCTION__, __LINE__);
        }

        if(u32ActualSize == 0)
        {
            NGLOG_ERROR("no data\n");
        }

        uc_uint32 idx = 0;
        uc_buffer_st stCAT;

        memset(&stCAT, 0x0, sizeof(uc_buffer_st));

        stCAT.bytes = au8Section;
        stCAT.length = u32ActualSize;

        for(idx=0;idx<stCAT.length;idx++)
        {
            NGLOG_DEBUG("%02x ", *(stCAT.bytes+idx));
        }
        NGLOG_DEBUG("\n");

        uc_result ret = UniversalClient_DVB_NotifyCAT(gEMMStreamInfo.serviceHandle, &stCAT);
        if(ret == UC_ERROR_SUCCESS)
        {
            NGLOG_DEBUG("NotifyCAT OK\n");
        }
        else
        {
            NGLOG_DEBUG("NotifyCAT Fail [%x]\n", ret);
        }
    }
    else if(eFilterStatus == DMX_EVENT_BUF_OVERFLOW)
    {
        NGLOG_DEBUG("======= BUFFER OVERFLOW =======\n");
        MApi_DMX_Stop(u8DmxId);
        MApi_DMX_Start(u8DmxId);
    }
}

static void _PMT_Parse(uc_uint8 u8DmxId, DMX_EVENT eFilterStatus, uc_uint32 u32Type2NotifyParam1)
{
    uc_uint32 u32ActualSize = 0;
    uc_uint32 u32RmnSize = 0;
//    uc_uint32 u32Seconds = 0;
    uc_uint8 au8Section[1024]; //CAT section length + 3

    memset(au8Section, 0 , sizeof(au8Section));

    if(eFilterStatus == DMX_EVENT_DATA_READY)
    {
        NGLOG_DEBUG( "<<<<-----PMT Table Updated\n");
        if(DMX_FILTER_STATUS_OK!= MApi_DMX_CopyData(u8DmxId, au8Section, 1024, &u32ActualSize, &u32RmnSize, NULL))
        {
            NGLOG_ERROR("error\n");
        }

        if(u32ActualSize == 0)
        {
            NGLOG_ERROR("no data\n");
        }

        uc_buffer_st stPMT;
        uc_uint32 idx = 0;

        memset(&stPMT, 0x0, sizeof(uc_buffer_st));

        stPMT.bytes = au8Section;
        stPMT.length = u32ActualSize;

        for(idx=0;idx<stPMT.length;idx++)
        {
            NGLOG_DEBUG("%02x ", *(stPMT.bytes+idx));
        }
        NGLOG_DEBUG("\n");

        stPMT.bytes = au8Section;
        stPMT.length = u32ActualSize;

        uc_result ret = UniversalClient_DVB_NotifyPMT(gECMStreamInfo.serviceHandle, &stPMT);
        if(ret == UC_ERROR_SUCCESS)
        {
            NGLOG_DEBUG("NotifyPMT OK\n");
        }
        else
        {
            NGLOG_DEBUG("NotifyPMT Fail [%x]\n", ret);
        }

    }
    else if(eFilterStatus == DMX_EVENT_BUF_OVERFLOW)
    {
        NGLOG_DEBUG("======= BUFFER OVERFLOW =======\n");
        MApi_DMX_Stop(u8DmxId);
        MApi_DMX_Start(u8DmxId);
    }
}

void Dmx_NotifyCAT(void)
{
//    uc_uint32 idx = 0;
    uc_uint32 CASysID = 0;

    UniversalClient_GetCASystemID(&CASysID);
    NGLOG_DEBUG("\n");
    NGLOG_DEBUG("----Irdeto CA Info----\n");
    NGLOG_DEBUG("CA System ID: %x \n", CASysID);
    NGLOG_DEBUG("----Irdeto CA Info----\n");
    NGLOG_DEBUG("\n");

    _stDMXSecArg stSecArg;
    memset(&stSecArg, 0, sizeof(_stDMXSecArg));

    stSecArg.u16Pid = 0x1;
    stSecArg.u32Pat0 = 0x01<<24; //tableid
    stSecArg.u32Pat1 = 0;
    stSecArg.u32Mask0 = 0xFF<<24;
    stSecArg.u32Mask1 = 0;
    stSecArg.pCB = (void*)_CAT_Parse;
    stSecArg.bCrcChk = TRUE;

    if(_u16CatDmxId != INVALID_ID)
    {
        MApi_DMX_Stop(_u16CatDmxId);
        MApi_DMX_Close(_u16CatDmxId);

        NGLOG_DEBUG("CAT MApi_DMX_Close id = %d\n",_u16CatDmxId);
        _u16CatDmxId = INVALID_ID;
    }

    _u16CatDmxId = Dmx_Sec_StartByArg(&stSecArg);

}

void Dmx_NotifyPMT(uc_uint16 *u16Pid)
{
    _stDMXSecArg stSecArg;
    memset(&stSecArg, 0, sizeof(_stDMXSecArg));

    stSecArg.u16Pid = *u16Pid;
    stSecArg.u32Pat0 = 0x02<<24; //tableid
    stSecArg.u32Pat1 = 0;
    stSecArg.u32Mask0 = 0xFF<<24;
    stSecArg.u32Mask1 = 0;
    stSecArg.pCB = (void*)_PMT_Parse;
    stSecArg.bCrcChk = TRUE;

    if(_u16PmtDmxId != INVALID_ID)
    {
        MApi_DMX_Stop(_u16PmtDmxId);
        MApi_DMX_Close(_u16PmtDmxId);

        NGLOG_DEBUG("PMT MApi_DMX_Close id = %d\n",_u16PmtDmxId);
        _u16PmtDmxId = INVALID_ID;
    }

    _u16PmtDmxId = Dmx_Sec_StartByArg(&stSecArg);

}

#endif


void GetClientID(void)
{
    uc_uint32 idx = 0;
    uc_byte stData[128] = {0};
    uc_buffer_st ClientID;
    uc_result ret = UC_ERROR_SUCCESS;
    ClientID.length = 128;
    ClientID.bytes = stData;


    ret = UniversalClient_GetClientIDString(&ClientID);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("Client ID length: %u \n", ClientID.length);
        for(idx=0;idx<ClientID.length;idx++)
        {
            NGLOG_DEBUG("%x", (char)(*(ClientID.bytes+idx)));
        }
    }
    else
    {
        NGLOG_DEBUG("Client ID length: %u \n", ClientID.length);
        NGLOG_DEBUG("GetClientID Fail [%x]\n", ret);
    }

    NGLOG_DEBUG("\n");
    NGLOG_DEBUG("| |%s \n",stData);


    ClientID.length = 128;
    ret = UniversalClient_GetCSSN(&ClientID);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("CSSN length: %u \n", ClientID.length);
        for(idx=0;idx<ClientID.length;idx++)
        {
            NGLOG_DEBUG("%x", (char)(*(ClientID.bytes+idx)));
        }
    }
    else
    {
        NGLOG_DEBUG("GetCSSN Fail [%x]\n", ret);
    }

    NGLOG_DEBUG("\n");

    ClientID.length = 128;
    ret = UniversalClient_GetSerialNumber(&ClientID);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("SN length: %u \n", ClientID.length);
        for(idx=0;idx<ClientID.length;idx++)
        {
            NGLOG_DEBUG("%x", (char)(*(ClientID.bytes+idx)));
        }
    }
    else
    {
        NGLOG_DEBUG("GetSN Fail [%x]\n", ret);
    }

    NGLOG_DEBUG("\n");

    ClientID.length = 128;
    ret = UniversalClient_GetBuildInformation(&ClientID);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("BuildInformation: %u \n", ClientID.length);
        for(idx=0;idx<ClientID.length;idx++)
        {
            NGLOG_DEBUG("%x", (char)(*(ClientID.bytes+idx)));
        }
    }
    else
    {
        NGLOG_DEBUG("GetBuildInformation Fail [%x]\n", ret);
    }

    /*
    uc_uint32 pProductCount;
    uc_product_status **ppProductList;
    ret = UniversalClient_GetProductList(&pProductCount, ppProductList);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("ProductList: %u \n", pProductCount);
        for(idx=0;idx<pProductCount;idx++)
        {
            NGLOG_DEBUG("%s\n", (*(ppProductList+idx))->product_id);
        }
    }
    else
    {
        NGLOG_DEBUG("ProductList Fail [%x]\n", ret);
    }
    UniversalClient_FreeProductList(ppProductList);
    */

    NGLOG_DEBUG("\n");
}

void NotifyCAT(void)
{
#if (LIVE_MODE_DMX_CAT == 1)
    Dmx_NotifyCAT();
#else
    uc_uint32 idx = 0;
    uc_uint32 CASysID = 0;
    uc_buffer_st stCAT;
    UniversalClient_GetCASystemID(&CASysID);
    NGLOG_DEBUG("\n");
    NGLOG_DEBUG("----Irdeto CA Info----\n");
    NGLOG_DEBUG("CA System ID: %x \n", CASysID);
    NGLOG_DEBUG("----Irdeto CA Info----\n");
    NGLOG_DEBUG("\n");

    stCAT.bytes = arrCAT;
    stCAT.length = sizeof(arrCAT);
    //uc_uint32 idx = 0;
    for(idx=0;idx<stCAT.length;idx++)
    {
        NGLOG_DEBUG("%02x ", *(stCAT.bytes+idx));
    }
    NGLOG_DEBUG("\n");
    uc_result ret = UniversalClient_DVB_NotifyCAT(gEMMStreamInfo.serviceHandle, &stCAT);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("NotifyCAT OK\n");
    }
    else
    {
        NGLOG_DEBUG("NotifyCAT Fail [%x]\n", ret);
    }
#endif
}

void NotifyPMT101(void)
{
    uc_buffer_st stPMT;
    stPMT.bytes = arrPMT101;
    stPMT.length = sizeof(arrPMT101);
    uc_result ret = UniversalClient_DVB_NotifyPMT(gECMStreamInfo.serviceHandle, &stPMT);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("NotifyPMT OK\n");
    }
    else
    {
        NGLOG_DEBUG("NotifyPMT Fail [%x]\n", ret);
    }
    //MsOS_DelayTask(2000);
}

void NotifyPMT102(void)
{
    uc_buffer_st stPMT;
    stPMT.bytes = arrPMT102;
    stPMT.length = sizeof(arrPMT102);
    uc_result ret = UniversalClient_DVB_NotifyPMT(gECMStreamInfo.serviceHandle, &stPMT);
    if(ret == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("NotifyPMT OK\n");
    }
    else
    {
        NGLOG_DEBUG("NotifyPMT Fail [%x]\n", ret);
    }
    //MsOS_DelayTask(2000);
}

#if 0
//For messages with this type, the lpVoid argument points to uc_flexiflash_msg value.
uc_uint8 UC_Global_Notify_Flexflash_Msg(void* lpVoid)
{

	uc_flexiflash_msg *Flexflash_msg = NULL;
	Flexflash_msg = (uc_flexiflash_msg *)lpVoid;


    NGLOG_DEBUG("Flexflash_msg Status: %s\n",Flexflash_msg->secureCoreListStatus);
    NGLOG_DEBUG("Flexflash_msg download Prog Info: %s\n",Flexflash_msg->packagesDownloadProgressInfo);


    //MApp_CCAUI_CasCmd_UpdateFlexiFlashMsg(Flexflash_msg->secureCoreListStatus, Flexflash_msg->packagesDownloadProgressInfo);

	return 1;
}

uc_uint8 UC_Global_Notify_IFCP_Msg(void* lpVoid)
{

    uc_IFCP_image_msg *pIFCPstatus = NULL;
    pIFCPstatus = (uc_IFCP_image_msg *)lpVoid;


    NGLOG_DEBUG("pIFCPstatus imageStatus: %s\n",pIFCPstatus->imageStatus);
    NGLOG_DEBUG("pIFCPstatus download Prog Info: %s\n",pIFCPstatus->packagesDownloadProgressInfo);


    //MApp_CCAUI_CasCmd_UpdateFlexiFlashIFCPMsg(pIFCPstatus->imageStatus, pIFCPstatus->packagesDownloadProgressInfo);

    return 1;
}
#endif
//////////////////////////////////////////////////////////////////////////////////////////
//  local function
//////////////////////////////////////////////////////////////////////////////////////////
void MyGlobalMessageProc(uc_global_message_type message, void* lpVoid)
{
    NGLOG_DEBUG(" START message=0x%08X\n",message);
	
    if(UC_GLOBAL_NOTIFY_ENTITLEMENT_CHANGE != message
        && UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED != message
        && UC_GLOBAL_NOTIFY_NATIONALITY_CHANGED != message
        && UC_GLOBAL_NOTIFY_HOMING_CHANNEL_DATA_CHANGED != message
        && UC_GLOBAL_NOTIFY_MIDDLEWARE_USER_DATA_CHANGED != message
        && UC_GLOBAL_NOTIFY_RESET_TO_FACTORY != message
        && UC_GLOBAL_NOTIFY_CA_DATA_CLEARED != message
        && UC_GLOBAL_NOTIFY_SMARTCARD_STATUS != message
        && UC_GLOBAL_NOTIFY_SMARTCARD_NATIONALITY_CHANGED != message
        && UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED != message
        && UC_GLOBAL_SMARTCARD_RAW_IRD_MESSAGE != message
        && UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE != message
        && UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE != message
        && UC_GLOBAL_NOTIFY_IFCP_IMAGE_MESSAGE != message
        )
    {
        if(lpVoid == NULL)
        {
            return;
        }
    }

    switch(message)
    {


        case UC_GLOBAL_DEBUG:
        {
            unsigned char       *pBuffer = NULL;

            pBuffer = (unsigned char *)lpVoid;

            NGLOG_DEBUG("UC_GLOBAL_DEBUG:: %s", pBuffer);
        }
        break;

        case UC_GLOBAL_RAW_IRD_MESSAGE:
        {
			//in 2.9.x and above, it is replaced by the UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE
        }
        break;

        case UC_GLOBAL_SERIAL_NUMBER_CHANGED:
        {
			UC_Global_Serial_Num_Chg(lpVoid);
        }
        break;

        case UC_GLOBAL_OOB_EMM_STATUS_REPLY:
        {
			//not support OOB now
        }
        break;

        case UC_GLOBAL_NOTIFY_ENTITLEMENT_CHANGE:
        {
			 UC_Global_Notify_Entitlement_Chg();
        }
        break;

        case UC_GLOBAL_NOTIFY_CASECTION_COUNT:
        {
			UC_Global_Notify_Casection_Count(lpVoid);
        }
        break;

        case UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO:
        {
			//UC_Global_Notify_Region_Subregion_Info(lpVoid);
        }
        break;

        case UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED:
        {
			UC_Global_Notify_TMS_Data_Chg();
        }
        break;

        case UC_GLOBAL_NOTIFY_NATIONALITY_CHANGED:
        {
			UC_Global_Notify_Nationality_Chg();
        }
        break;

        case UC_GLOBAL_NOTIFY_HOMING_CHANNEL_DATA_CHANGED:
        {
			//UC_Global_Notify_Homing_Channel_Data_Chg();
        }
        break;

        case UC_GLOBAL_NOTIFY_MIDDLEWARE_USER_DATA_CHANGED:
        {
			//UC_Global_Notify_MW_User_Data_Chg();
        }
        break;

        case UC_GLOBAL_NOTIFY_RESET_TO_FACTORY:
        {
			//UC_Global_Notify_Reset_To_Factory(lpVoid);
        }
        break;

        case UC_GLOBAL_NOTIFY_CA_DATA_CLEARED:
        {
			//This is the message that notifies device application that the CA data has already been cleared by Cloaked CA Agent.
			//Device application doesn't need to perform any actions on this message
        }
        break;
        case UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE:
        {
			//UC_Global_Notify_Flexflash_Msg(lpVoid);
        }
		break;
        case UC_GLOBAL_NOTIFY_SMARTCARD_STATUS:
        {
            uc_smartcard_status* pSMC_Status;
            pSMC_Status = (uc_smartcard_status *)lpVoid;
            NGLOG_DEBUG("SMC: %s \n", pSMC_Status->status);
        }
        break;

        case UC_GLOBAL_NOTIFY_SMARTCARD_NATIONALITY_CHANGED:
        {
        }
        break;

        case UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED:
        {
			//using in the Unified Client is integrated.
        }
        break;

        case UC_GLOBAL_SMARTCARD_RAW_IRD_MESSAGE:
        {
        }
        break;
        case UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE:
        {
			UC_Global_Ext_Raw_Ird_Msg(lpVoid);
        }
        break;
        case UC_GLOBAL_NOTIFY_ENTITLED_STATUS_CHANGE:
        {
            //UC_Global_Notify_Entitled_Status_Chg(lpVoid);
        }
        break;
        case UC_GLOBAL_NOTIFY_IFCP_IMAGE_MESSAGE:
        {
            //it may need to notify some UI to change the status
           UC_Global_Notify_IFCP_Msg(lpVoid);            
        }
        break;
        default:
        {
        }
    }

    #if 0
    NGLOG_DEBUG("[%s %d] END\n", __FUNCTION__, __LINE__);
    NGLOG_DEBUG("\n");
    #endif
}
#if 0
//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_ecm_status_st.
uc_uint8 UC_Ecm_Status(void* lpVoid)
{
	uc_ecm_status_st *Ecm_Status = NULL;
	Ecm_Status = (uc_ecm_status_st *)lpVoid;
    int severity = 0;
    switch((unsigned char)Ecm_Status->statusMessage[0])
	{
		case 'N':		severity = 0/*N_STR*/;		break;
		case 'F':		severity = 1/*F_STR*/;			break;
		case 'E':		severity = 2/*E_STR*/;		break;
		case 'W':	severity = 3/*W_STR*/;		break;
		case 'I':		severity = 4/*I_STR*/;			break;
		case 'D':		severity = 5/*D_STR*/;		break;
		default:		break;
	}

    if(severity == 2)
    {
        //UniversalClientSPI_Stream_CleanDescramblingKey(Current_StreamHandle);
        NGLOG_DEBUG("Clean Key!!");
    }

    if (Ecm_Status->caStream.protocolType == UC_STREAM_DVB)
    {
        #if 1
        NGLOG_DEBUG("[ECM] UC_ECM_STATUS::PID(0x%04X) ... %s\n", Ecm_Status->caStream.pid, Ecm_Status->statusMessage);
        #endif

       // MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus(SERVICE_DESCRAMBLE, Ecm_Status->caStream.pid, Ecm_Status->statusMessage);

    }

	return 1;

}


//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_emm_status_st.

uc_uint8 UC_Emm_Status(void* lpVoid)
{
	uc_emm_status_st *Emm_Status = NULL;
	Emm_Status = (uc_emm_status_st *)lpVoid;



	if (Emm_Status->caStream.protocolType == UC_STREAM_DVB)
	{

	#if 1
		NGLOG_DEBUG("[EMM] UC_EMM_STATUS::PID(0x%04X) ... %s\n", Emm_Status->caStream.pid, Emm_Status->statusMessage);
	#endif
    
        //MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus(SERVICE_BROADCAST_EMM, Emm_Status->caStream.pid, Emm_Status->statusMessage);
	
	}

	return 1;
}

//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_service_status_st.
uc_uint8 UC_Service_Status(void* lpVoid)
{

	uc_service_status_st	*Service_Status = NULL;
	uc_uint8 			severity = 0;
	uc_uint16		status = 0;
	uc_uint8			source = 0;

	Service_Status = (uc_service_status_st *)lpVoid;

	switch((unsigned char)Service_Status->statusMessage[0])
	{
		case 'N':		severity = 0/*N_STR*/;		break;
		case 'F':		severity = 1/*F_STR*/;			break;
		case 'E':		severity = 2/*E_STR*/;		break;
		case 'W':	severity = 3/*W_STR*/;		break;
		case 'I':		severity = 4/*I_STR*/;			break;
		case 'D':		severity = 5/*D_STR*/;		break;
		default:		break;
	}

	status = (unsigned char)Service_Status->statusMessage[1] - '0';
	status = status*10 + (unsigned char)Service_Status->statusMessage[2] - '0';
	status = status*10 + (unsigned char)Service_Status->statusMessage[3] - '0';

	source = (unsigned char)Service_Status->statusMessage[5] - '0';


	#if 1
	NGLOG_DEBUG("[%s][%d][SERVICE] UC_SERVICE_STATUS ... %s\n",__func__,__LINE__ ,Service_Status->statusMessage);
	#endif

    //MApp_CCAUI_CasCmd_UpdateServiceItemStatus(Service_Status->serviceHandle, Service_Status->statusMessage);

	//MApp_CCA_SCP_ErrorCodeHandle(Service_Status,&severity,&status);
	return 1;

}
#endif
void MyServiceMessageProc(void *pMessageProcData, uc_service_message_type message, void *pVoid)
{
    if(pVoid == NULL)
    {
        return;
    }

    #if 1
    NGLOG_DEBUG("\n");
    NGLOG_DEBUG("[%s %d] START message=0x%08X\n", __FUNCTION__, __LINE__, message);
    #endif

    switch(message)
    {

        case UC_ECM_STATUS:
        {
            // check to make sure this is the right service.
            if (pMessageProcData == MYECMMESSAGEPROCDATA)
            {
				cca::UC_Ecm_Status(pVoid);
            }
        }
        break;


        case UC_EMM_STATUS:
        {
            // check to make sure this is the right service.
            if (pMessageProcData == MYEMMMESSAGEPROCDATA)
            {
            	cca::UC_Emm_Status(pVoid);
            }
        }
        break;


        case UC_SERVICE_STATUS:
        {

			cca::UC_Service_Status(pVoid);
        }
        break;

        case UC_SERVICE_ECM_MONITOR_STATUS:
        {

			UC_Service_Ecm_Monitor_Status(pVoid);

        }
        break;

        case UC_SERVICE_EMM_MONITOR_STATUS:
        {
			UC_Service_Emm_Monitor_Status(pVoid);
        }
        break;

        case UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT:
        {
			//UC_Service_Status_For_Convergent_Client(pVoid);
        }
		break;

#if(CA_CCA_PVR_SUP == 1)
        case UC_SERVICE_PVR_RECORD_STATUS_REPLY:
        {
            UC_Pvr_Proc_Status(pVoid, FALSE);
        }
        break;

        case UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY:
        {
            UC_Pvr_Proc_Status(pVoid, TRUE);
        }
        break;

        case UC_SERVICE_PVR_SESSION_METADATA_REPLY:
        {
            UC_Pvr_Append_Metadata(pVoid);
        }
        break;
#endif

        default:
        {
        }
        break;
    }
}


void UserPowersOnDevice(void)
{
    uc_result result = UC_ERROR_SUCCESS;
    uc_unified_client_config convergentClientConfig;
    uc_ca_client_config clientConfig;
    uc_bool bResult;

    convergentClientConfig.isCloakedCAWithHighPriority = UC_FALSE;
    convergentClientConfig.isAutoClientSwitching = UC_TRUE;
    convergentClientConfig.irdMessageSource = UC_SOURCE_BOTH;
    clientConfig.configType = UC_CONFIG_TYPE_FOR_UNIFIED_CLIENT;
    clientConfig.pConfigData = (void*)(&convergentClientConfig);

    //extern uc_bool MApp_DecoderMsg_InitParser(uc_uint16 u16MaxNumGroups);
    bResult = MApp_DecoderMsg_InitParser(1);
    NGLOG_DEBUG("[%s %d] return MApp_DecoderMsg_InitParser is 0x%08X\n", __FUNCTION__, __LINE__, bResult);

 //   Ird_DebugParam(1);
    // 1. Initialize the client, passing our implementation of uc_global_message_proc above
    result = UniversalClient_StartCaClient(MyGlobalMessageProc);//MyGlobalMessageProc);

    // 2. Regional filtering enable, default is false
    if (UC_ERROR_SUCCESS == result)
    {
        result = UniversalClient_EnableRegionalFiltering(UC_FALSE);
    #if 1
        NGLOG_DEBUG("[%s %d] return UniversalClient_EnableRegionalFiltering is 0x%08X\n", __FUNCTION__, __LINE__, result);
    #endif
    }
	else
		NGLOG_DEBUG("[%s %d] UniversalClient_StartCaClient return  is 0x%08X\n", __FUNCTION__, __LINE__, result);

    // 2-1. Macrovision enable, default is false
#if 0
    if (UC_ERROR_SUCCESS == result)
    {
        result = UniversalClient_EnableMacrovision(UC_TRUE);

    #if 1
        NGLOG_DEBUG("[%s %d] return UniversalClient_EnableMacrovision is 0x%08X\n", __FUNCTION__, __LINE__, result);
    #endif
    }
#endif

#if 0
    // 2-2. TMS data setting
    if (UC_ERROR_SUCCESS == result)
    {
        uc_tms_attribute_item_info      ucTMSAttribute[2];

        ucTMSAttribute[0].startbit = (uc_uint8)10;
        ucTMSAttribute[0].length = (uc_uint8)2;
        ucTMSAttribute[0].value = (uc_uint32)2;

        ucTMSAttribute[1].startbit = (uc_uint8)20;
        ucTMSAttribute[1].length = (uc_uint8)1;
        ucTMSAttribute[1].value = (uc_uint32)0;

        result = UniversalClient_SetTMSAttributes((uc_uint16)2, &ucTMSAttribute[0]);


    #if 0
        NGLOG_DEBUG("[%s %d] return UniversalClient_SetTMSAttributes is 0x%08X\n", __FUNCTION__, __LINE__, result);
    #endif
    }
#endif    

	NGLOG_DEBUG("YJC %s %d\n",__func__,__LINE__);

    // 2. Open a service for EMMs
    if (UC_ERROR_SUCCESS == result)
    {

        uc_service_handle           EmmServiceHandle = (uc_service_handle)NULL;
        result = UniversalClient_OpenService(
            MYEMMSERVICECONTEXT,
                    // pServiceContext -- normally we could use this field to identify the context
                    // for the stream open parameters (uc_stream_open_params). For example purposes,
                    // this is a hardcoded value.
            MyServiceMessageProc,//MyServiceMessageProc,
            MYEMMMESSAGEPROCDATA,
                    // pMessageProcData -- we can use this to identify which service we're receiving
                    // a message for. For example purposes, this is a hardcoded value.
            &EmmServiceHandle);
		NGLOG_DEBUG("\n emm openservice %d EmmServiceHandle %x\n",__LINE__,EmmServiceHandle);
        gEMMStreamInfo.serviceHandle = EmmServiceHandle;


    }

    // 3. Open a service for ECMs
    if (UC_ERROR_SUCCESS == result)
    {
        uc_service_handle           g_EcmServiceHandle = (uc_service_handle)NULL;
       result = UniversalClient_OpenService(
            MYECMSERVICECONTEXT,
                    // pServiceContext -- normally we could use this field to identify the context
                    // for the stream open parameters (uc_stream_open_params). For example purposes,
                    // this is a hardcoded value.
            MyServiceMessageProc,//MyServiceMessageProc,
            MYECMMESSAGEPROCDATA,
                    // pMessageProcData -- we can use this to identify which service we're receiving
                    // a message for. For example purposes, this is a hardcoded value.
            &g_EcmServiceHandle);
	   
			NGLOG_DEBUG("\n ecm openservice %d g_EcmServiceHandle %x result %d \n",__LINE__,g_EcmServiceHandle,result);
    gECMStreamInfo.serviceHandle = g_EcmServiceHandle;


    }
#if 0
    // 4. Notify the EMM service of a CAT (Conditional Access Table).
    // Normally this CAT is extracted from a bitstream, but for the purposes of this example,
    // a simple global value is used.
    // Normally there is only one CAT for an entire bitstream.
    if (UC_ERROR_SUCCESS == result)
    {
        uc_buffer_st bytes = {0};  // this 'bytes' structure is the standard way to pass around blocks of data with length.
        bytes.bytes = g_SampleCat;
        bytes.length = sizeof(g_SampleCat);
        result = UniversalClient_DVB_NotifyCAT(
            g_EmmServiceHandle,
            &bytes);
    }

    // 5. Notify the ECM service of a PMT (Program Map Table).
    // Normally this PMT is extracted from a bitstream, but for the purposes of this example,
    // a simple global value is used.
    // Normally there is one PMT per program in a bitstream.
    // This call corresponds to the application tuning to the 'default' channel that the user is watching.
    // See the sample 'UserChangesChannel.cpp' to see what happens as the user switches channels.
    if (UC_ERROR_SUCCESS == result)
    {
        uc_buffer_st bytes = {0};  // this 'bytes' structure is the standard way to pass around blocks of data with length.
        bytes.bytes = g_SamplePmt;
        bytes.length = sizeof(g_SamplePmt);
        result = UniversalClient_DVB_NotifyPMT(
            g_EcmServiceHandle,
            &bytes);
    }
#endif
    //UniversalClient_ConfigClient(&clientConfig);   // only need for UNIFIED_CLIENT
    NGLOG_DEBUG("[%s %d] return UniversalClient_EnableRegionalFiltering is 0x%08X\n", __FUNCTION__, __LINE__, result);
    // 6. Display a status message indicating success or failure:
    NGLOG_DEBUG("Power On Status: 0x%08X\n", result);


}


void UserPowersOffDevice(void)
{
    uc_result       result = UC_ERROR_SUCCESS;

    // 1. Close the service we opened in the 'UserPowersOnDevice.cpp' sample.
    result = UniversalClient_CloseService(&gEMMStreamInfo.serviceHandle);
    //@sa????how about ECM???

    // 2. Terminate the CA client to clean up resources.
    if (UC_ERROR_SUCCESS == result)
    {
        result = UniversalClient_StopCaClient();
    }

    // 3. Display a status message indicating success or failure:
    NGLOG_DEBUG("Power Off Status: 0x%08X\n", result);
}

void UniversalClient_Init(void)
{
    unsigned char       i = 0;
    unsigned char       j = 0;
    //MDrv_AESDMA_Init(0, 0, 1);

#if 0
    //creat memory pool
    gs32CAMemPoolID = MsOS_CreateMemoryPool( CA_BUFFER_LEN,
                                             0,
                                             (void*)MS_PA2KSEG1(CA_BUFFER_ADR),
                                              E_MSOS_FIFO,
                                              "CA_Mem");
    if(gs32CAMemPoolID <0)
    {
        NGLOG_DEBUG("create the ca mem pool fail\n");
        return;
    }
#endif
   //use in HB Zapper...
	/* CAT and PMT buffer */
	g_ucCatBuff.length = 0;
	g_ucPmtBuff.length = 0;
	g_ucCatBuff.bytes = (uc_byte *)GxCore_Mallocz((uc_uint32)ECM_QUEUE_SIZE);
	g_ucPmtBuff.bytes = (uc_byte *)GxCore_Mallocz((uc_uint32)ECM_QUEUE_SIZE);

    /* CCA system */
    g_ucSystem.setPrivateData = UC_FALSE;
    g_ucSystem.setCSSK = UC_FALSE;
    g_ucSystem.setSecure = UC_FALSE;
    g_ucSystem.setBBCB = UC_FALSE;
    g_ucSystem.setSPCB = UC_FALSE;
    g_ucSystem.setEEPROM = UC_FALSE;
    g_ucSystem.setDeviceId= UC_FALSE;
    g_ucSystem.typeCSSK = UC_DK_PROTECTION_UNKNOWN;

    memset((void *)&g_ucSystem.strCSSN[0], (int)0x00, sizeof(&g_ucSystem.strCSSN[0]));
    memset((void *)&g_ucSystem.strPrivateData[0], (int)0x00, sizeof(&g_ucSystem.strPrivateData[0]));
    memset((void *)&g_ucSystem.strCSSK[0], (int)0x00, sizeof(&g_ucSystem.strCSSK[0]));
    memset((void *)&g_ucSystem.strDeviceId[0], (int)0x00, sizeof(&g_ucSystem.strDeviceId[0]));

    s_spi_ucSystem.setPrivateData = UC_FALSE;
    s_spi_ucSystem.setCSSK = UC_FALSE;
    s_spi_ucSystem.setSecure =  UC_FALSE;
    s_spi_ucSystem.setDeviceId= UC_FALSE;

    memset((void *)&s_spi_ucSystem.strCSSN[0], (int)0x00, sizeof(&s_spi_ucSystem.strCSSN[0]));
    memset((void *)&s_spi_ucSystem.strPrivateData[0], (int)0x00, sizeof(&s_spi_ucSystem.strPrivateData[0]));
    memset((void *)&s_spi_ucSystem.strCSSK[0], (int)0x00, sizeof(&s_spi_ucSystem.strCSSK[0]));
    memset((void *)&s_spi_ucSystem.strDeviceId[0], (int)0x00, sizeof(&s_spi_ucSystem.strDeviceId[0]));



    /* CCA EMM structure */
    gEMMStreamInfo.serviceHandle = (uc_service_handle)UC_INVALID_HANDLE_VALUE;

    for(i = 0; i < MAX_EMM_STREAM_NUM; i++)
    {
        gEMMStreamInfo.streamHandle[i] = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
        gEMMStreamInfo.pid[i] = (uc_uint16)CCA_INVALID_PID;
        gEMMStreamInfo.connectionHandle[i] = (uc_connection_handle)UC_INVALID_HANDLE_VALUE;
        gEMMStreamInfo.notifyCallback[i] = (uc_notify_callback)NULL;
    }
	memset(gEMMStreamInfo.filterBuffer, 0x00, sizeof(gEMMStreamInfo.filterBuffer));

	for(i = 0; i < MAX_EMM_FLT_NUM; i++)
    {
        gEMMStreamInfo.filterHandle[i] = (uc_filter_handle)UC_INVALID_HANDLE_VALUE;
        gEMMStreamInfo.byteBufferStruct[i].bytes = (uc_byte *)NULL;
        gEMMStreamInfo.byteBufferStruct[i].length = 0;
        gEMMStreamInfo.filterToStream[i] = 0xFFFFFFFF;
    }

    /* CCA ECM structure */
    gECMStreamInfo.serviceHandle = (uc_service_handle)UC_INVALID_HANDLE_VALUE;
	memset(gECMStreamInfo.filterBuffer, 0x00, sizeof(gECMStreamInfo.filterBuffer));
    for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
    {
        gECMStreamInfo.streamHandle[i] = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
        gECMStreamInfo.pid[i] = (uc_uint16)CCA_INVALID_PID;
        gECMStreamInfo.connectionHandle[i] = (uc_connection_handle)UC_INVALID_HANDLE_VALUE;
        gECMStreamInfo.notifyCallback[i] = (uc_notify_callback)NULL;
        //note: MAX_ECM_STREAM_NUM == MAX_ECM_FLT_NUM
        gECMStreamInfo.filterHandle[i] = (uc_filter_handle)UC_INVALID_HANDLE_VALUE;
        gECMStreamInfo.byteBufferStruct[i].bytes = (uc_byte *)NULL;
        gECMStreamInfo.byteBufferStruct[i].length = 0;
        for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
        {
            gECMStreamInfo.componentPid[i][j] = (uc_uint16)CCA_INVALID_PID;
            gECMStreamInfo.dscmbHandle[i][j] = (uc_uint32)DRV_DSCMB_FLT_NULL;
			gECMStreamInfo.key_status[i][j].Odd_Key_Set_Status = 0;
			gECMStreamInfo.key_status[i][j].Even_Key_Set_Status = 0;

        }

		memset(gECMStreamInfo.even_key,0,8);
		memset(gECMStreamInfo.odd_key,0,8);


    }

#if(CA_CCA_PVR_SUP == 1)
    /* CCA PVR structure */
	gPVRStreamInfo.pvrPlaybackServiceHandle = (uc_service_handle)UC_INVALID_HANDLE_VALUE;
	gPVRStreamInfo.pvrPlaybackStreamHandle = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
	gPVRStreamInfo.pvrRecordStreamHandle = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;

	gPVRStreamInfo.pvrRecordPid = (uc_uint16)CCA_INVALID_PID;
	gPVRStreamInfo.pvrPlaybackPid = (uc_uint16)CCA_INVALID_PID;

	gPVRStreamInfo.pvrRecordSessionCount = 0;
	gPVRStreamInfo.pvrPlaybackSessionCount = 0;

	for(i=0; i<MAX_SESSION; i++)
	{
		gPVRStreamInfo.pvrRecordSession[i].statusSession = 0;
		gPVRStreamInfo.pvrRecordSession[i].typeSession = 0;
		gPVRStreamInfo.pvrPlaybackSession[i].statusSession = 0;
		gPVRStreamInfo.pvrPlaybackSession[i].typeSession = 0;

		memset(&gPVRStreamInfo.pvrRecordSession[i].strSession[0], 0x00, SESSION_DATA_SIZE);
		memset(&gPVRStreamInfo.pvrPlaybackSession[i].strSession[0], 0x00, SESSION_DATA_SIZE);
	}

	gPVRStreamInfo.pvrRecordMetaCount = 0;
	gPVRStreamInfo.pvrPlaybackMetaCount = 0;

	for(i=0; i<MAX_METADATA; i++)
	{
		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaIndex = 0;
		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaTime = 0;
		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaAddress = 0;
		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.length = 0;
		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.bytes = NULL;

		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaIndex = 0;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaTime = 0;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress = 0;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.length = 0;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes = NULL;
	}
#endif

    memset(&Sc_Info,0,sizeof(UCSPI_SC_INFO));

    /* execute CCA */
    NGLOG_DEBUG("#######   UserPowersOnDevice()!!\n\n   ");	
	LoaderCoreAPI_InitApplicationModule();
	GL_DESCR_Init();
    UserPowersOnDevice();
	NGLOG_DEBUG("#######   UserPowersOnDevice()!!\n\n   ");
	//for msg event 
    App::getInstance().addEventSource(CCAIRDEventSource::getInstance(),[&](EventSource&e)->bool{
        tCCAIRD_UI_Event cae; 
        if(((CCAIRDEventSource&)e).getEvent(cae)){
            UC_Ird_Process_Command((UINT8*)&(cae.u8CmdData));
            //Window::broadcast(WM_CAMSG,0,0); //pair with ui window onmessage()

            NGLOG_DEBUG(" %d MX ccairdevent  success! \n",cae.u8CmdData[0]);
        }
        return true;
    });
	//CCAIRD_UI_AddEventListener([](tCCAIRD_UI_Event*e){
         //((CCAIRDEventSource*)e->pUserArg)->addEvent(*e);
    //},&ccaird_events);

}

uc_uint32 GetEMMServiceHandle(void)
{
    return gEMMStreamInfo.serviceHandle;
}

uc_uint32 GetECMServiceHandle(void)
{
    return gECMStreamInfo.serviceHandle;
}

void CANOTIFY(UINT msg,const SERVICELOCATOR*svc,DWORD wp,ULONG lp,void*userdata){
	switch (msg)
	{
		case MSG_SERVICE_CHANGE:{
		    unsigned int msgq[4]={0};
		    msgq[0]=(unsigned int)svc;
			msgq[1]=(unsigned int)DtvGetServerPmtPid(svc);//pmtpid
		    nglMsgQSend((HANDLE)playerq,msgq,sizeof(msgq),100);
		}break;
		case MSG_PMT_CHANGE:	break;		
	    case MSG_CAT_CHANGE:	break;
	    default:break;
	}
	NGLOG_DEBUG("\n END \n");
}

static int VerCMP(BYTE*s1,BYTE*s2,UINT len){
    BYTE v1=(s1[5]&0x3E)>>1;
    BYTE v2=(s2[5]&0x3E)>>1;
    unsigned short sid1=(s1[3]<<8)|s1[4];
    unsigned short sid2=(s2[3]<<8)|s2[4];
    int res=memcmp(s1,s2,len);
    if((v1-v2)||(sid1-sid2))NGLOG_DEBUG("ver:%d->%d sid:%x(%d)->%x(%d) len=%d",v1,v2,sid1,sid1,sid2,sid2,len);
    return (v1-v2)||(sid1-sid2);//&&res;
}

static void SectionCBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer ,UINT uiBufferLength ,void *pUserData ){
    switch(pBuffer[0]){
    case 1:/*CAT*/
         if(memcmp(g_ucCatBuff.bytes,pBuffer,uiBufferLength)){
            NGLOG_VERBOSE("\n %s CAT %x %x NOTIFYCAT",__FUNCTION__,pBuffer[0],pBuffer[1]);
			memcpy(g_ucCatBuff.bytes,pBuffer,uiBufferLength);
			g_ucCatBuff.length=uiBufferLength;
			UniversalClient_DVB_NotifyCAT(gEMMStreamInfo.serviceHandle, &g_ucCatBuff);
         }
         break;
    case 2:/*PMT*/
         if(VerCMP(g_ucPmtBuff.bytes,(BYTE*)pBuffer,uiBufferLength)){
             NGLOG_VERBOSE("PMT0 %s %x %x  NOTIFYPMT",__FUNCTION__,pBuffer[0],pBuffer[1]);
			 memcpy(g_ucPmtBuff.bytes,pBuffer,uiBufferLength);
			 g_ucPmtBuff.length=uiBufferLength;
			 UniversalClient_DVB_NotifyPMT(gECMStreamInfo.serviceHandle, &g_ucPmtBuff);
         } 
         break; 
    default:break;
    }
}

static HANDLE CreateCAFilter(int pid,int num,...){
    va_list ap;
    HANDLE  hFilter;
    BYTE mask[16],value[16];
    int i,idx;
    char buffer[64];
    bzero(mask,sizeof(mask));
    bzero(value,sizeof(mask));
    bzero(buffer,sizeof(buffer));
    va_start(ap,num);
    for(i=0,idx=0;i<num;i++){
        mask[idx]=0xFF;
        value[idx]=(BYTE)va_arg(ap,int);
        idx++;//((i==0)*2+1);
    }
    va_end(ap);
    for(i=0;i<idx;i++)sprintf(buffer+i*3,"%02x,",value[i]);
	hFilter=nglAllocateSectionFilter(0,pid,SectionCBK,NULL,DMX_SECTION);
    nglSetSectionFilterParameters(hFilter,mask,value,num);
    nglStartSectionFilter(hFilter);
    NGLOG_VERBOSE("\n filter pid=%d %d pattern:%s hFilter%d \n",pid,num,buffer,hFilter);
    return hFilter;
}
#define SECLEN(sec) (((sec)[1]&0x0F)<<8|(sec)[2])
static void play_service(void*channel,int *pids,int pmtpid){
    unsigned int i,netid=0,tsid=0,tpid;
	SERVICELOCATOR* ploc=(SERVICELOCATOR*)channel; 
    unsigned short service_id=ploc->sid;
    netid=ploc->netid;
	tsid=ploc->tsid;
    tpid=ploc->tpid;
    if(service_id==lastplayed.sid){
       NGLOG_DEBUG("skipped duplcate play serviceid=%d",service_id);
       return;
    }
	if(filter_pmt!=0){
	  nglFreeSectionFilter(filter_pmt);
	  filter_pmt=0;
	}
    NGLOG_DEBUG("\n channel %p service_id=%d/0x%x pmtpid=%d tsid=%ud/%ud \n",channel,service_id,service_id,pmtpid,last_tsid,tsid);
	if(netid!=lastplayed.netid||tsid!=lastplayed.tsid||(tpid!=lastplayed.tpid)){
		nglFreeSectionFilter(filter_cat);
		filter_cat=CreateCAFilter(1,1,1);//cat
		}
	
	filter_pmt=CreateCAFilter(pmtpid,3,0x02,(service_id>>8),(service_id&0xFF));	
	lastplayed.sid=service_id;
	lastplayed.netid=netid;
	lastplayed.tpid=tpid;
	lastplayed.tsid=tsid;
}

static void PlayProc(void*p){
    while(1){
        unsigned int msg[4];
        int rc,cnt;
        do{ 
            rc=nglMsgQReceive((HANDLE)playerq,msg,sizeof(msg),1000);
            nglMsgQGetCount((HANDLE)playerq,(UINT*)&cnt);
            if(rc==0)NGLOG_DEBUG("player.q.count=%d",cnt);
        }while(cnt>0&&rc==0);
        if(rc==0){       
            play_service((void*)msg[0],NULL,msg[1]); 
            rc=nglWaitEvent((HANDLE)playerevent,10000);  
            NGLOG_DEBUG("*********switchon=%d  nglWaitEvent=%d",switchon,rc);
        }
    }
}

void  User_init_playq(void){
    DWORD tid;
    playerq=(DWORD)nglMsgQCreate(128,16);
    playerevent=(DWORD)nglCreateEvent(FALSE,FALSE);
    nglCreateThread((HANDLE *)&tid,0,0,PlayProc,NULL);
	DtvRegisterNotify(CANOTIFY,NULL);
}
void StartCA(){
  NGLOG_DEBUG("irdeto ca init enter !");
  UniversalClient_Init();
  User_init_playq();
}
