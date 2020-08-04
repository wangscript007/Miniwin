//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!§MStar Confidential Information!?? by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    MApp_ZUI_ACCCACasCmd.c
/// @brief  functions to handle CCA IRD commands for UI control
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MApp_ZUI_ACTCCACASCMD_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>
#include <string.h>
#include <MApp_ZUI_ACTCCACasCmd.h>
#include <CCA_UCMsgProc.h>
#include <CCA_StatusCodeProc.h>
#include <appCCADecoderMsg.h>
#include <ngl_log.h>
#include <ngl_timer.h>
NGL_MODULE(IRDCCACASCMD);
namespace cca{
	
/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#define CCA_CAS_COMPONENTS_MAX_STRING            15


/******************************************************************************/
/*                                 Extern Variables or Function                                  */
/******************************************************************************/
extern uc_device_platform_identifiers stDeviceId;

/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
BOOL LoaderStauts_result= FALSE;


/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/
typedef struct
{
    UINT8 u8SecureCore[FlexiFlash_MSG_MAX_LINE_LENGTH];
    UINT8 u8DownloadStatus[FlexiFlash_MSG_MAX_LINE_LENGTH];
} __attribute__((packed)) stClientFlexiFlash;
static stClientFlexiFlash gstClientFlexiFlash;

typedef struct
{
    UINT8 u8FlexiCore[FlexiFlash_MSG_MAX_LINE_LENGTH];
    UINT8 u8DownloadStatus[FlexiFlash_MSG_MAX_LINE_LENGTH];
} __attribute__((packed)) stClientFlexiFlashIFCP;
static stClientFlexiFlashIFCP gstClientFlexiFlashIFCP;

typedef struct
{
    UINT8 u8SrvType;
    UINT32 u32Handle;
    UINT8 u8StatusCode[STATUS_CODE_LENGTH];
    UINT16 u16CaPid;
    UINT8 u8CaStatusCode[STATUS_CODE_LENGTH];
    BOOL isMonitoring;
    UINT8 pMonitorMessage[STATUS_ITEM_MAX_LINE_LENGTH];
} __attribute__((packed)) stServiceList;

static stServiceTypeUnit serviceType[SERVICE_MAX_NUM];
static stServiceList gstServiceList[CCA_CAS_TOTAL_HANDLE_NUM];

static UINT32 gu32ServiceCount = 0;


/********************************************************************************/
/*                            extern Function                                */
/********************************************************************************/

/********************************************************************************/
/*                            Local Function                                */
/********************************************************************************/
void _MApp_UTCToYMD(UINT32 utcDate, UINT32 *pYear, UINT32 *pMonth, UINT32 *pDay)
{
    UINT32 y = 0;
    UINT32 m = 0;
    UINT32 d = 0;
    UINT32 k = 0;

    utcDate +=  745518;/*745518: UTC: from 1900.1.1 to 2000.01.01*/
    y =  (UINT32) ((utcDate  - 15078.2) / 365.25);
    m =  (UINT32) ((utcDate - 14956.1 - (UINT32)(y * 365.25) ) / 30.6001);
    d =  (UINT32) (utcDate - 14956 - (UINT32)(y * 365.25) - (UINT32)(m * 30.6001));
    k =  (m == 14 || m == 15) ? 1 : 0;
    y = y + k;
    m = m - 1 - k*12;
    *pYear = y;
    *pMonth = m;
    *pDay = d;
}


void _MApp_Service_Status_Update(void)
{
	tCCAIRD_UI_Event e;
	e.u8CmdData[0]=E_APP_CMD_CCAIRD_STATUS;
	e.u8CmdData[1]=E_APP_CMD_CCA_CA_SERVICE_STATUS;
	CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);
}

static void _MApp_Loader_Status_Update(void)
{	
	tCCAIRD_UI_Event e;
	e.u8CmdData[0]   = E_APP_CMD_CCAIRD_STATUS;
    e.u8CmdData[1]   = E_APP_CMD_CCA_CA_LOADER_STATUS;
	CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);
}

static void _MApp_Product_Status_Update(void)
{	
	tCCAIRD_UI_Event e;
	e.u8CmdData[0]   = E_APP_CMD_CCAIRD_STATUS;
    e.u8CmdData[1]   = E_APP_CMD_CCA_CA_PRODUCT_STATUS;
	CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);	
}

static void _MApp_Client_Status_Update(void)
{
	tCCAIRD_UI_Event e;
	e.u8CmdData[0]   = E_APP_CMD_CCAIRD_STATUS;
    e.u8CmdData[1]   = E_APP_CMD_CCA_CA_CLIENT_STATUS;
	CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);	
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetServiceHandle
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
stServiceTypeUnit* MApp_CCAUI_CasCmd_GetServiceTypeUnit(UINT8 type)
{
    UINT32 i = 0;
    uc_service_handle handle= (uc_service_handle)UC_INVALID_HANDLE_VALUE;
    

    //FIXME one service support one handle now
    serviceType[type].u16TotalHandleNum = 1;
    switch(type)
	{
        case SERVICE_BROADCAST_EMM:
            handle = UC_Status_Get_Emm_Service_Handle();
            break;

        case SERVICE_DESCRAMBLE:
            handle = UC_Status_Get_Ecm_Service_Handle();
            break;

        #if(CA_CCA_PVR_SUP == 1)
        case SERVICE_PVR_PLAYBACK:
            handle = UC_Status_Get_Pvr_Playback_Service_Handle();
            break;
        #endif

        default:
            return NULL;
    }

    for (i = 0; i < serviceType[type].u16TotalHandleNum; i++)
    {
        serviceType[type].u16ServiceHandle[i] = handle;
    }

    return &serviceType[type];
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetServiceHandle
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT32 MApp_CCAUI_CasCmd_GetServiceIndexByHandle(UINT32 u32Handle)
{
    UINT32 i = 0;

    for (i = 0; i < CCA_CAS_TOTAL_HANDLE_NUM; i++)
    {
        if (gstServiceList[i].u32Handle == u32Handle)
        {

            return i;
        }
    }

    return 0xffffffff;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetServiceItemName
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_CasCmd_GetServiceItemName(UINT32 u32SrvIndex, INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf == NULL) || (u16BufSize == 0))
    {
        return FALSE;
    }

    if (u32SrvIndex >= gu32ServiceCount)
    {
        return FALSE;
    }

    switch (gstServiceList[u32SrvIndex].u8SrvType)
    {
        case SERVICE_BROADCAST_EMM:
            snprintf((char*)pu8Buf, u16BufSize, "Broadcast EMM Service");
            return TRUE;

        case SERVICE_PULL_EMM:
            snprintf((char*)pu8Buf, u16BufSize, "Pull EMM Service");
            return TRUE;    

        case SERVICE_DESCRAMBLE:
            snprintf((char*)pu8Buf, u16BufSize, "Descramble Service");
            return TRUE;

        #if(CA_CCA_PVR_SUP == 1)
        case SERVICE_PVR_PLAYBACK:
            snprintf((char*)pu8Buf, u16BufSize, "PVR Playback Service");
            return TRUE;
        #endif

        case SERVICE_SMARTCARD_EMM:
            snprintf((char*)pu8Buf, u16BufSize, "Smart card EMM Service");
            return TRUE;
    }

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetDvbClientServices
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetDvbClientServices(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT32 i = 0, j = 0, k = 0;
    INT8* pu8Str = pu8Buf;
    INT8  au8buf[32];
    stServiceTypeUnit *pstSevicesTypeUnit;

    NGLOG_DEBUG("%s\n", pu8Buf);

    if ((pu8Buf == NULL) || (u16LineLength == 0))
    {
        return 0;
    }

    gu32ServiceCount = 0;
    memset(gstServiceList, 0x0, sizeof(stServiceList) * CCA_CAS_TOTAL_HANDLE_NUM);

    for (i = SERVICE_BROADCAST_EMM; i < SERVICE_MAX_NUM; i++)
    {
        pstSevicesTypeUnit = MApp_CCAUI_CasCmd_GetServiceTypeUnit(i);
        if (pstSevicesTypeUnit != NULL)
        {
            for (j = 0; j < pstSevicesTypeUnit->u16TotalHandleNum; j++)
            {
                gu32ServiceCount++;
                gstServiceList[k].u8SrvType   = i;
                gstServiceList[k].u32Handle  = pstSevicesTypeUnit->u16ServiceHandle[j];

                memset(au8buf, '\0', 32);
                snprintf((char*)pu8Str, u16LineLength, "0x%08X", pstSevicesTypeUnit->u16ServiceHandle[j]);
                MApp_CCAUI_CasCmd_GetServiceItemName(k, au8buf, sizeof(au8buf));
                snprintf((char*)pu8Str, u16LineLength, "%s        %s", pu8Str, au8buf);				
				pu8Str += u16LineLength;
                k++; // Next entry in Service List
            }
        }        
        
    }

    return gu32ServiceCount;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetServiceStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_CasCmd_GetServiceItemStatus(UINT32 u32SrvIndex, INT8 *pu8Buf, UINT16 u16LineLength)
{
    uc_service_status_st stServiceStatus;
    UINT8 severity = 0;
    UINT16 status = 0;
    INT8 *au8Buf = pu8Buf;

    if (pu8Buf == NULL)
    {
        return FALSE;
    }
    
    if (UC_Status_Get_Service_Status(gstServiceList[u32SrvIndex].u32Handle, &stServiceStatus))
    {
        memcpy(gstServiceList[u32SrvIndex].u8StatusCode, stServiceStatus.statusMessage, STATUS_CODE_LENGTH);
        
        //append status text for service status page
        //snprintf((char*)pu8Buf, u16LineLength, "%s", stServiceStatus.statusMessage);
    	switch((unsigned char)stServiceStatus.statusMessage[0])
    	{
    		case 'N':		severity = 0/*N_STR*/;		break;
    		case 'F':		severity = 1/*F_STR*/;			break;
    		case 'E':		severity = 2/*E_STR*/;		break;
    		case 'W':	    severity = 3/*W_STR*/;		break;
    		case 'I':		severity = 4/*I_STR*/;			break;
    		case 'D':		severity = 5/*D_STR*/;		break;
    		default:		break;
    	}

    	status = (unsigned char)stServiceStatus.statusMessage[1] - '0';
    	status = status*10 + (unsigned char)stServiceStatus.statusMessage[2] - '0';
    	status = status*10 + (unsigned char)stServiceStatus.statusMessage[3] - '0';

        MApp_CCA_SCP_StatusCodeHandle(&stServiceStatus,&severity,&status, au8Buf);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateServiceItemStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateServiceItemStatus(UINT32 u32handle, UINT8 *u8StatusCode)
{
    UINT32 u32SrvIndex = MApp_CCAUI_CasCmd_GetServiceIndexByHandle(u32handle);

    NGLOG_DEBUG("srv handle:%d, prev code:%s, new code:%s", u32handle, gstServiceList[u32SrvIndex].u8StatusCode, u8StatusCode);
    if(memcmp(gstServiceList[u32SrvIndex].u8StatusCode, u8StatusCode, STATUS_CODE_LENGTH))
    {
        memcpy(gstServiceList[u32SrvIndex].u8StatusCode, u8StatusCode, STATUS_CODE_LENGTH);
        _MApp_Service_Status_Update();
        NGLOG_DEBUG("service status updated!!");  
    }
    else
    {
        NGLOG_DEBUG("same service status code");
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetServiceItemPidStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetServiceItemCaStatus(UINT32 u32SrvIndex, INT8 *pu8Str, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT32 i = 0, j = 0;
    BOOL MULTI_SECURE_CORE = 0;
    UINT8 u8TotalLines = 0;
    UINT32 u32StreamCount;
    uc_service_stream_status_st *pStreamStatusList;
    
    UC_Status_Get_Stream_Status(gstServiceList[u32SrvIndex].u32Handle, &u32StreamCount, &pStreamStatusList);

    for(i = 0; i < u32StreamCount; i++)
    {
        //FIXME multiple stream count when multiple secour core support
        gstServiceList[u32SrvIndex].u16CaPid = (pStreamStatusList+i)->caStream.pid;
        memcpy(gstServiceList[u32SrvIndex].u8StatusCode, (pStreamStatusList+i)->streamStatusMessage, STATUS_CODE_LENGTH);
        
        //CA PID info       
        if(gstServiceList[u32SrvIndex].u8SrvType == SERVICE_BROADCAST_EMM || gstServiceList[u32SrvIndex].u8SrvType == SERVICE_SMARTCARD_EMM)
        {
            snprintf((char *)pu8Str, u16LineLength, "        EMM: 0x%04X, %s", (pStreamStatusList+i)->caStream.pid, (pStreamStatusList+i)->streamStatusMessage);
        }
        else if(gstServiceList[u32SrvIndex].u8SrvType == SERVICE_DESCRAMBLE)
        {
            snprintf((char *)pu8Str, u16LineLength, "        ES:");
            for(j=0; j<(pStreamStatusList+i)->componentCount; j++ )
            {
                //Get the PID of the component stream
                snprintf((char *)pu8Str, u16LineLength, "%s 0x%04X,", pu8Str, (pStreamStatusList+i)->componentStreamArray[j].pid);
            }
            snprintf((char *)pu8Str, u16LineLength, "%s ECM: 0x%04X, %s", pu8Str, (pStreamStatusList+i)->caStream.pid, (pStreamStatusList+i)->streamStatusMessage);
        }
        else
        {
        }

        //FIXME MULTI_SECURE_CORE support
        if(MULTI_SECURE_CORE)
        {
            snprintf((char*)pu8Str, u16LineLength, "%s, 0x%04X", pu8Str, (pStreamStatusList+i)->caSystemID);            
        }

        u8TotalLines++;
        if (u8LineCount <= u8TotalLines)
        {
            UC_Status_Free_Stream_Status(&pStreamStatusList);
            return u8TotalLines;
        }
        pu8Str += u16LineLength;
    }

    UC_Status_Free_Stream_Status(&pStreamStatusList);
    return u8TotalLines;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus(UINT8 u8SrvType, UINT16 u16CaPid, UINT8 *u8StatusCode)
{
    UINT32 i;
    UINT32 u32SrvIndex = 0xffffffff;
    
    for (i = 0; i < CCA_CAS_TOTAL_HANDLE_NUM; i++)
    {
        if (gstServiceList[i].u8SrvType == u8SrvType)
        {
            u32SrvIndex = i;
        }
    }

    if(u32SrvIndex == 0xffffffff)
    {
        return FALSE;
    }
        
    NGLOG_DEBUG("srv type:%d, prev code:%s, new code:%s", u8SrvType, gstServiceList[u32SrvIndex].u8StatusCode, u8StatusCode);
    NGLOG_DEBUG("prev pid:%d, new pid:%d", u8SrvType, gstServiceList[u32SrvIndex].u16CaPid, u16CaPid);

    if((gstServiceList[u32SrvIndex].u16CaPid != u16CaPid) ||
        (memcmp(gstServiceList[u32SrvIndex].u8CaStatusCode, u8StatusCode, STATUS_CODE_LENGTH)))
    {
        gstServiceList[u32SrvIndex].u16CaPid = u16CaPid;
        memcpy(gstServiceList[u32SrvIndex].u8CaStatusCode, u8StatusCode, STATUS_CODE_LENGTH);
        _MApp_Service_Status_Update();
        NGLOG_DEBUG("service status updated!!");
    }
    else
    {
        NGLOG_DEBUG("same service status code");        
    }

    return TRUE;
    
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_EnableServiceMonitorStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_CasCmd_EnableServiceMonitorStatus(UINT32 u32SrvIndex, BOOL enable)
{
    BOOL ret = FALSE;

    if(gstServiceList[u32SrvIndex].isMonitoring == enable)
    {
        NGLOG_DEBUG("service idx:%ld already set monitoring %d\n", u32SrvIndex, enable);
        return ret;
    }
    
    ret = UC_Status_Enable_Monitor_Status(gstServiceList[u32SrvIndex].u32Handle, enable);
    if(ret)
    {
        NGLOG_DEBUG("service idx:%ld set monitoring to %d\n", u32SrvIndex, enable);
        gstServiceList[u32SrvIndex].isMonitoring = enable;
    }

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_SetServiceMonitorStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateServiceMonitorStatus(UINT32 u32Handle, UINT8 *pMonitorMsg)
{
    UINT32 u32SrvIndex = MApp_CCAUI_CasCmd_GetServiceIndexByHandle(u32Handle);

    if(u32SrvIndex == 0xffffffff)
    {
        return FALSE;
    }
    
    memcpy(gstServiceList[u32SrvIndex].pMonitorMessage, pMonitorMsg, strlen((const INT8*)pMonitorMsg));

    _MApp_Service_Status_Update();
    return TRUE;

}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetServiceMonitorStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetServiceItemMonitorStatus(UINT32 u32SrvIndex, INT8 *pu8Str, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;

    if(!strlen((const INT8*)gstServiceList[u32SrvIndex].pMonitorMessage))
    {
        NGLOG_DEBUG("No monitor msg in service idx:%ld\n", u32SrvIndex);
        return 0;
    }

    //Monitor title
    snprintf((char *)pu8Str, u16LineLength, "  %s", "Monitoring:");
    
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;


    //FIXME add ca system id stSrvMonitor.caSystemID and go next line if MULTI_SECURE_CORE enabled
    snprintf((char *)pu8Str, u16LineLength, "        %s", gstServiceList[u32SrvIndex].pMonitorMessage);
    
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;


    return u8TotalLines;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderItemDeviceValue
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetLoaderItemDeviceValue(INT8* pu8Buf, UINT16 u16BufSize)
{
    //uc_device_platform_identifiers stDeviceId;
        
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        if( LoaderStauts_result == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "Device                    0x%04x, 0x%04x, 0x%04x", stDeviceId.manufacturerId, stDeviceId.hardwareVersion, stDeviceId.variant);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Device                    0xffff, 0xffff, 0xffff");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderItemKey
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetLoaderItemKey(INT8* pu8Buf, UINT16 u16BufSize)
{
    //uc_device_platform_identifiers stDeviceId;
        
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        if( LoaderStauts_result == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "Key                        0x%04x, 0x%04x, 0x%04x", stDeviceId.systemId, stDeviceId.keyVersion, stDeviceId.signatureVersion);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Key                        0xffff, 0xffff, 0xffff");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderItemCssn
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetLoaderItemCssn(INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT8 au8Buf[128];
        uc_buffer_st CSSN; 
        CSSN.bytes = au8Buf;
        CSSN.length = 128;
        if (UC_Status_Get_CSSN(&CSSN) == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "CSSN                      %02X%02X%02X%02X", CSSN.bytes[0],CSSN.bytes[1],CSSN.bytes[2],CSSN.bytes[3]);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "CSSN                      N/A");      
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderItemDoadloadId
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT32 MApp_CCAUI_CasCmd_GetLoaderItemDoadloadId(void)
{
    //uc_device_platform_identifiers stDeviceId;

    if( LoaderStauts_result == TRUE)
    {
        return stDeviceId.loadVersion;
    }
    else
    {
        return 0;
    }


    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderFirmwareVersion
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetLoaderFirmwareVersion(INT8* pu8Buf, UINT16 u16BufSize)
{
    //uc_device_platform_identifiers stDeviceId;
        
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        if( LoaderStauts_result == TRUE)
        {
            //FIXME
            snprintf((char*)pu8Buf, u16BufSize, "Firmware version    12345.67890");
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Firmware version    N/A");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderItemLoaderVersion
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT16 MApp_CCAUI_CasCmd_GetLoaderItemLoaderVersion(void)
{
    //uc_device_platform_identifiers stDeviceId;

    if( LoaderStauts_result == TRUE)
    {
        return stDeviceId.loaderVersion;
    }
    else
    {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderItemSerialNo
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetLoaderItemSerialNo(INT8* pu8Buf, UINT16 u16BufSize)
{   
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT8 au8Buf[128];
        uc_buffer_st SerialNo; 
        SerialNo.bytes = au8Buf;
        SerialNo.length = 128;
        if (UC_Status_Get_SerialNo(&SerialNo) == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "Serial #                  %02X%02X%02X%02X", SerialNo.bytes[0],SerialNo.bytes[1],SerialNo.bytes[2],SerialNo.bytes[3]);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Serial #                  N/A");      
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetLoaderSpecificInfo
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetLoaderSpecificInfo(INT8 *pu8Buf, UINT16 u16BufSize)
{
    // FIXME
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        snprintf((char*)pu8Buf, u16BufSize, "MSTAR IRDETO CA SYSTEM");
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateLoaderStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateLoaderStatus()
{

    _MApp_Loader_Status_Update();
    return TRUE;

}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetProductCount
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT32 MApp_CCAUI_CasCmd_GetProductCount()
{   
    UINT32 ProductCount = 0;
    uc_product_status *pProductList = NULL;
    

    if(UC_Status_Get_Product_Status(&ProductCount, &pProductList) == TRUE)
    {        
        return ProductCount;
    }
    else
    {
        return 0;
    }
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetProductStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetProductStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength, UINT32 u32Index)
{   
    UINT32 ProductCount = 0;
    uc_product_status *pProductList = NULL;

    INT8 *pu8Str = pu8Buf;
    UINT8 u8TotalLines = 0;
    UINT32 i = 0;
    UINT8 strTrue[] = "TRUE ";
    UINT8 strFalse[] = "FALSE";
    UINT8 strProductType[32] = {0};
    UINT8 strSourceType[32] = {0};
    UINT32 year = 0;
    UINT32 month = 0;
    UINT32 day = 0;
    BOOL MULTI_SECURE_CORE = 0;
    
    NGLOG_DEBUG("%s\n", pu8Buf);

    if ((pu8Buf == NULL) || (u16LineLength == 0))
    {
        return FALSE;
    }
    

    if(UC_Status_Get_Product_Status(&ProductCount, &pProductList) == TRUE)
    {
        NGLOG_DEBUG("==============================================================\n");
        NGLOG_DEBUG("Product count: %u \n", ProductCount);
        NGLOG_DEBUG("Sector Num  Product ID  Start Date  Duration  Entitled  Product Type Source\n");

        //append count and title for first page
        if(u32Index == 0)
        {
            //product count
            snprintf((char*)pu8Str, u16LineLength, "Product Count:    %ld", ProductCount);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                UC_Status_Free_Product_Status(&pProductList);
                return FALSE;
            }
            pu8Str += u16LineLength;

            //title
            //FIXME MULTI_SECURE_CORE
            if(MULTI_SECURE_CORE)
            {
                snprintf((char*)pu8Str, u16LineLength, "Sector Num  Product ID  Start Date  Duration  Entitled  Product Type  CASystemID  Source");
            }
            else
            {
                snprintf((char*)pu8Str, u16LineLength, "Sector Num  Product ID  Start Date  Duration  Entitled  Product Type  Source");
            }
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                UC_Status_Free_Product_Status(&pProductList);
                return FALSE;
            }
            pu8Str += u16LineLength;
            
        }
        

        //content
        for(i=u32Index; i<ProductCount; i++)       
        {       
            year = 0;       
            month = 0;        
            day = 0;       
            _MApp_UTCToYMD((pProductList+i)->startingDate, &year, &month, &day);
        
            memset(strProductType, 0x00, sizeof(strProductType));        
            switch((pProductList+i)->productType)        
            {       
                case UC_PRODUCT_TYPE_NORMAL:        
                    sprintf((INT8*)strProductType, "Normal Product");       
                    break;
        
                case UC_PRODUCT_TYPE_PVR:      
                    sprintf((INT8*)strProductType, "PVR Product");        
                    break;
        
                case UC_PRODUCT_TYPE_PUSH_VOD:        
                    sprintf((INT8*)strProductType, "Push VOD Product");       
                    break;
        
                case UC_PRODUCT_TYPE_PRE_ENABLEMENT:       
                    sprintf((INT8*)strProductType, "Pre Enable Product");       
                    break;
        
                default:        
                    sprintf((INT8*)strProductType, "Normal Product");        
                    break;     
            }

            
            memset(strSourceType, 0x00, sizeof(strSourceType));        
            switch((pProductList+i)->sourceType)        
            {
                case UC_SOURCE_TYPE_CCA:        
                    sprintf((INT8*)strSourceType, "CCA");       
                    break;
        
                case UC_SOURCE_TYPE_SCA:      
                    sprintf((INT8*)strSourceType, "SCA");        
                    break;
        
                default:        
                    sprintf((INT8*)strSourceType, "N/A");        
                    break;     
            }

            NGLOG_DEBUG("%-6u             0x%02x%02x       %04ld/%02ld/%02ld   %-6u     %s   %s  %s\n",     
                      (pProductList+i)->sector_number,     
                      (pProductList+i)->product_id[0], (pProductList+i)->product_id[1],    
                      year, month, day,
                      (pProductList+i)->duration,
                      ((pProductList+i)->entitled)?strTrue:strFalse,   
                      strProductType,
                      strSourceType);

            //FIXME MULTI_SECURE_CORE
            if(MULTI_SECURE_CORE)
            {
                snprintf((char*)pu8Str, u16LineLength, "%-6u             0x%02x%02x       %04ld/%02ld/%02ld   %-6u     %s   %s     0x04%X  %s",     
                      (pProductList+i)->sector_number,     
                      (pProductList+i)->product_id[0], (pProductList+i)->product_id[1],    
                      year, month, day,
                      (pProductList+i)->duration,
                      ((pProductList+i)->entitled)?strTrue:strFalse,   
                      strProductType,
                      (pProductList+i)->caSystemID,
                      strSourceType);
            }
            else
            {
                snprintf((char*)pu8Str, u16LineLength, "%-6u             0x%02x%02x       %04ld/%02ld/%02ld   %-6u     %s   %s  %s",     
                      (pProductList+i)->sector_number,     
                      (pProductList+i)->product_id[0], (pProductList+i)->product_id[1],    
                      year, month, day,
                      (pProductList+i)->duration,
                      ((pProductList+i)->entitled)?strTrue:strFalse,   
                      strProductType,
                      strSourceType);
            }

            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                UC_Status_Free_Product_Status(&pProductList);

                NGLOG_DEBUG("out of product status line range, u8TotalLines:%d\n",u8TotalLines);
                return FALSE;
            }
            pu8Str += u16LineLength;
        }
        
        UC_Status_Free_Product_Status(&pProductList);

        NGLOG_DEBUG("==============================================================\n");
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateProductStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateProductStatus()
{

    _MApp_Product_Status_Update();
    return TRUE;

}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemCCAVersion
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetClientItemCCAVersion(INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT8 au8Buf[128];
        uc_buffer_st CCAVersion; 
        CCAVersion.bytes = au8Buf;
        CCAVersion.length = 128;
        if (UC_Status_Get_CCAVersion(&CCAVersion) == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "Irdeto Cloaked CA Agent version %s", CCAVersion.bytes);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Irdeto Cloaked CA Agent version N/A");      
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemBuildInfo
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetClientItemBuildInfo(INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT8 au8Buf[128];
        uc_buffer_st BuildInfo; 
        BuildInfo.bytes = au8Buf;
        BuildInfo.length = 128;
        if (UC_Status_Get_BuildInfo(&BuildInfo) == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "Build:                  %02X%02X%02X%02X", BuildInfo.bytes[0],BuildInfo.bytes[1],BuildInfo.bytes[2],BuildInfo.bytes[3]);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Build:                  N/A");      
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemCssn
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetClientItemCssn(INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT8 au8Buf[128];
        uc_buffer_st CSSN; 
        CSSN.bytes = au8Buf;
        CSSN.length = 128;
        if (UC_Status_Get_CSSN(&CSSN) == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "CSSN:                  %02X%02X%02X%02X", CSSN.bytes[0],CSSN.bytes[1],CSSN.bytes[2],CSSN.bytes[3]);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "CSSN:                  N/A");      
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemLockId
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetClientItemLockId(INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT16 u16LockId;
        if (UC_Status_Get_LockId(&u16LockId) == TRUE)
        {
            snprintf((char*)pu8Buf, u16BufSize, "Lock ID:               %d", u16LockId);
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Lock ID:               N/A");      
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemSecureType
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetClientItemSecureType(INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        UINT8 u8SecureType;
        if (UC_Status_Get_SecureType(&u8SecureType) == TRUE)
        {
            if(u8SecureType == 0)
            {
                snprintf((char*)pu8Buf, u16BufSize, "Secure Type:        %s", "Secure Chipset");
            }
            else if(u8SecureType == 1)
            {
                snprintf((char*)pu8Buf, u16BufSize, "Secure Type:        %s", "Security ID");
            }
            else
            {
                snprintf((char*)pu8Buf, u16BufSize, "Secure Type:        N/A");     
            }
            
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "Secure Type:        N/A");      
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemClientId
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemClientId(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 i = 0;
    uc_client_id ClientIDString;

    if (UC_Status_Get_Ext_ClientId(&ClientIDString) == TRUE)
    {
        for(i = 0; i < ClientIDString.validOperatorCount; i++)
        {
            snprintf((char*)pu8Buf, u16LineLength, "Client ID:             %s", ClientIDString.clientID[i].clientIDString);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                return u8TotalLines;
            }
            pu8Buf += u16LineLength;
        }         
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "Client ID:             N/A");     
        u8TotalLines++;
    }
    
    return u8TotalLines;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemSerialNo
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemSerialNo(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 i = 0;
    uc_serial_number pSN;

    if (UC_Status_Get_Ext_SeriaNo(&pSN) == TRUE)
    {
        for(i = 0; i < pSN.validOperatorCount; i++)
        {
            snprintf((char*)pu8Buf, u16LineLength, "SN:                     %02X%02X%02X%02X", 
                pSN.serialNumber[i].serialNumberBytes[0], pSN.serialNumber[i].serialNumberBytes[1],
                pSN.serialNumber[i].serialNumberBytes[2], pSN.serialNumber[i].serialNumberBytes[3]);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                return u8TotalLines;
            }
            pu8Buf += u16LineLength;
        }         
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "SN:                     N/A");     
        u8TotalLines++;
    }
    
    return u8TotalLines;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemCASystemId
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemCASystemId(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 i = 0;
    uc_operator OperatorInfo;

    if (UC_Status_Get_Ext_CASystemId(&OperatorInfo) == TRUE)
    {
        for(i = 0; i < OperatorInfo.validOperatorCount; i++)
        {
            snprintf((char*)pu8Buf, u16LineLength, "CA System ID:       0x%04X", OperatorInfo.operatorInfo[i].activeCaSystemID);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                return u8TotalLines;
            }
            pu8Buf += u16LineLength;
        }         
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "CA System ID:       N/A");     
        u8TotalLines++;
    }
    
    return u8TotalLines;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemNationality
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemNationality(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 i = 0;
    uc_nationality Nationality;

    if (UC_Status_Get_Ext_Nationality(&Nationality) == TRUE)
    {
        for(i = 0; i < Nationality.validOperatorCount; i++)
        {
            //TODO multiple secure core
            //snprintf((char*)pu8Buf, u16LineLength, "Nationality:         %s, 0x%04X", Nationality.nationality[i].nationalityData, Nationality.nationality[i].caSystemID);
            snprintf((char*)pu8Buf, u16LineLength, "Nationality:         %s", Nationality.nationality[i].nationalityData);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                return u8TotalLines;
            }
            pu8Buf += u16LineLength;
        }         
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "Nationality:         N/A");     
        u8TotalLines++;
    }
    
    return u8TotalLines;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemTMS
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemTMS(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 i = 0, j = 0;
    INT8 au8Buf[STATUS_ITEM_MAX_LINE_LENGTH] = {0};
    uc_tms_data TmsData = {0};


    if (UC_Status_Get_Ext_TMS(&TmsData) == TRUE)
    {
        //title
        snprintf((char*)pu8Buf, u16LineLength, "TMS Data:");
        u8TotalLines++;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Buf += u16LineLength;

        //content 
        for(i = 0; i < TmsData.validOperatorCount; i++)
        {
            //TODO multiple secure core

            for(j = 0; j < sizeof(TmsData.tms[i].tmsData); j++)
            {
                snprintf((char*)(au8Buf), u16LineLength, "%s%02X", au8Buf, TmsData.tms[i].tmsData[j]);
            }
    
            snprintf((char*)pu8Buf, u16LineLength, "%s", au8Buf);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                return u8TotalLines;
            }
            pu8Buf += u16LineLength;
            
        }         
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "TMS Data:            N/A");     
        u8TotalLines++;
    }
    
    return u8TotalLines;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemBuildInfo
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemCapabilities(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 au8Str[UC_BUFFER_GENERIC_SIZE];
    UINT8 *pu8Str = au8Str;
    uc_buffer_st Capabilities; 
    Capabilities.bytes = au8Str;
    Capabilities.length = UC_BUFFER_GENERIC_SIZE;
    if (UC_Status_Get_Capabilities(&Capabilities) == TRUE)
    {
        //title
        snprintf((char*)pu8Buf, u16LineLength, "Capabilities:");
        u8TotalLines++;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Buf += u16LineLength;

        //content
        UINT8 au8Tmp[64] = {0};
        UINT8 *pu8Tmp = au8Tmp;

        while(*pu8Str != '\0')
        {
            if(*pu8Str != '\n')
            {
                *pu8Tmp = *pu8Str;
                pu8Tmp++;              
            }
            else
            {
                *pu8Tmp = '\0';

                snprintf((char*)pu8Buf, u16LineLength, (const INT8*)au8Tmp);
                u8TotalLines++;
                if (u8LineCount <= u8TotalLines)
                {
                    return u8TotalLines;
                }
                pu8Buf += u16LineLength;

                memset(au8Tmp, 0, sizeof(au8Tmp));
                pu8Tmp = au8Tmp;
            }

            pu8Str++;
        }
        
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "Capabilities:            N/A"); 
        u8TotalLines++;
    }

    return u8TotalLines;

}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientItemTMS
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetClientItemEcmEmmCount(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;
    UINT8 i = 0;
    uc_ca_extended_section_count CaSectionCount;

    if (UC_Status_Get_Ext_EcmEmm_Count(&CaSectionCount) == TRUE)
    {
        //title
        snprintf((char*)pu8Buf, u16LineLength, "Section Count:");
        u8TotalLines++;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Buf += u16LineLength;

        //content
        for(i = 0; i < CaSectionCount.validOperatorCount; i++)
        {
            //FIXME multiple secure core
            //snprintf((char*)pu8Buf, u16LineLength, "EMM: %d, ECM: %d, 0x%04X", 
            //    CaSectionCount.caSectionCount[i].emm_count, CaSectionCount.caSectionCount[i].ecm_count, CaSectionCount.caSectionCount[i].caSystemID);
            snprintf((char*)pu8Buf, u16LineLength, "EMM: %d, ECM: %d", 
                CaSectionCount.caSectionCount[i].emm_count, CaSectionCount.caSectionCount[i].ecm_count);
            u8TotalLines++;
            if (u8LineCount <= u8TotalLines)
            {
                return u8TotalLines;
            }
            pu8Buf += u16LineLength;
        }         
    }
    else
    {
        snprintf((char*)pu8Buf, u16LineLength, "Section Count:            N/A");     
        u8TotalLines++;
    }
    
    return u8TotalLines;
}

UINT8 MApp_CCAUI_CasCmd_GetClientItemSecurecore(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;

    snprintf((char*)pu8Buf, u16LineLength, "Secure Core:   %s", gstClientFlexiFlash.u8SecureCore);
    u8TotalLines++;

    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Buf += u16LineLength;

    return u8TotalLines;
}

UINT8 MApp_CCAUI_CasCmd_GetClientItemSecurecoreDownloadStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;

    snprintf((char*)pu8Buf, u16LineLength, "Download Status:   %s", gstClientFlexiFlash.u8DownloadStatus);
    u8TotalLines++;

    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Buf += u16LineLength;	

    return u8TotalLines;
}

UINT8 MApp_CCAUI_CasCmd_GetClientItemFlexicore(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;

    snprintf((char*)pu8Buf, u16LineLength, "FlexiCore:          %s", gstClientFlexiFlashIFCP.u8FlexiCore);
    u8TotalLines++;

    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Buf += u16LineLength;

    return u8TotalLines;
}

UINT8 MApp_CCAUI_CasCmd_GetClientItemFlexicoreDownloadStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{
    UINT8 u8TotalLines = 0;

    snprintf((char*)pu8Buf, u16LineLength, "Download Status:   %s", gstClientFlexiFlashIFCP.u8DownloadStatus);
    u8TotalLines++;

    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Buf += u16LineLength;	

    return u8TotalLines;
}
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetClientStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT16 MApp_CCAUI_CasCmd_GetClientStatus(INT8 *pu8Buf, UINT8 u8LineCount, UINT16 u16LineLength)
{   

    INT8 *pu8Str = pu8Buf;
    UINT8 u8TotalLines = 0;
    UINT8 u8SplitLines = 0;
    //UINT32 index = 0;

    INT8 au8Buf[128];
    uc_buffer_st ucBuf; 
    ucBuf.bytes = (UINT8*)au8Buf;
    ucBuf.length = 128;

    BOOL MULTI_SECURE_CORE = 0;
    BOOL FlexiFlash = 1;
    BOOL IFCP = 1;
    
    NGLOG_DEBUG("%s\n", pu8Buf);

    if ((pu8Buf == NULL) || (u16LineLength == 0))
    {
        return 0;
    }
    
    //CCA version
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetClientItemCCAVersion(au8Buf, sizeof(au8Buf)); 
    snprintf((char*)pu8Str, u16LineLength, (const INT8*)au8Buf);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Build Id
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetClientItemBuildInfo(au8Buf, sizeof(au8Buf));
    snprintf((char*)pu8Str, u16LineLength, (const INT8*)au8Buf);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //CSSN
    memset(au8Buf, 0, sizeof(au8Buf));   
    MApp_CCAUI_CasCmd_GetClientItemCssn(au8Buf, sizeof(au8Buf));
    snprintf((char*)pu8Str, u16LineLength, (const INT8*)au8Buf);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Lock Id
    memset(au8Buf, 0, sizeof(au8Buf));   
    MApp_CCAUI_CasCmd_GetClientItemLockId(au8Buf, sizeof(au8Buf));
    snprintf((char*)pu8Str, u16LineLength, (const INT8*)au8Buf);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Secure Type
    memset(au8Buf, 0, sizeof(au8Buf));   
    MApp_CCAUI_CasCmd_GetClientItemSecureType(au8Buf, sizeof(au8Buf));
    snprintf((char*)pu8Str, u16LineLength, (const INT8*)au8Buf);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Client Id
    u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemClientId(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;
    
    //SN
    u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemSerialNo(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;

    //CA System Id
    if(!MULTI_SECURE_CORE)
    {
        u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemCASystemId(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
        u8TotalLines += u8SplitLines;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Str += u16LineLength * u8SplitLines;
    }

    //Nationality
    u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemNationality(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;
    
    if(FlexiFlash)
    {
        //Secure Core & Download Status:
        u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemSecurecore(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
        u8TotalLines += u8SplitLines;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Str += u16LineLength * u8SplitLines;
    
        u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemSecurecoreDownloadStatus(pu8Str, u8LineCount - u8TotalLines, u16LineLength);        
        u8TotalLines += u8SplitLines;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Str += u16LineLength * u8SplitLines;
    }
     
    if(FlexiFlash && IFCP)
    {
        //Flexicore & DownloadStatus
        u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemFlexicore(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
        u8TotalLines += u8SplitLines;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Str += u16LineLength * u8SplitLines;

        u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemFlexicoreDownloadStatus(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
        u8TotalLines += u8SplitLines;
        if (u8LineCount <= u8TotalLines)
        {
            return u8TotalLines;
        }
        pu8Str += u16LineLength * u8SplitLines;
    }
    
    //TMS
    u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemTMS(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;

    //Capabilities
    u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemCapabilities(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;
    
    //Section Count
    u8SplitLines = MApp_CCAUI_CasCmd_GetClientItemEcmEmmCount(pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;

    //Unified Client TODO

    

    return u8TotalLines;
    
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateClientStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateClientStatus()
{

    _MApp_Client_Status_Update();
    return TRUE;

}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateClientStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateFlexiFlashMsg(UINT8 *pu8SecureCore, UINT8 *pu8DownloadStatus)
{
    BOOL update = FALSE;
    if(memcmp(gstClientFlexiFlash.u8SecureCore, pu8SecureCore, strlen((const INT8*)pu8SecureCore)))
    {
        memcpy(gstClientFlexiFlash.u8SecureCore, pu8SecureCore, strlen((const INT8*)pu8SecureCore));
        update = TRUE;
    }

    if(memcmp(gstClientFlexiFlash.u8DownloadStatus, pu8DownloadStatus, strlen((const INT8*)pu8DownloadStatus)))
    {
        memcpy(gstClientFlexiFlash.u8DownloadStatus, pu8DownloadStatus, strlen((const INT8*)pu8DownloadStatus));
        update = TRUE;
    }

    if(update)
    {
        MApp_CCAUI_CasCmd_UpdateClientStatus();
    }

    return TRUE;

}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_UpdateClientStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///
////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_UpdateFlexiFlashIFCPMsg(UINT8 *pu8FlexiCore, UINT8 *pu8DownloadStatus)
{
    BOOL update = FALSE;
    if(memcmp(gstClientFlexiFlashIFCP.u8FlexiCore, pu8FlexiCore, strlen((const INT8*)pu8FlexiCore)))
    {
        memcpy(gstClientFlexiFlashIFCP.u8FlexiCore, pu8FlexiCore, strlen((const INT8*)pu8FlexiCore));
        update = TRUE;
    }

    if(memcmp(gstClientFlexiFlashIFCP.u8DownloadStatus, pu8DownloadStatus, strlen((const INT8*)pu8DownloadStatus)))
    {
        memcpy(gstClientFlexiFlashIFCP.u8DownloadStatus, pu8DownloadStatus, strlen((const INT8*)pu8DownloadStatus));
        update = TRUE;
    }

    if(update)
    {
        MApp_CCAUI_CasCmd_UpdateClientStatus();
    }

    return TRUE;

}

#if 1
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetMailMaxItem
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetMailId(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize)
{
    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        snprintf((char*)pu8Buf, u16BufSize, "0x%04x", MApp_DecoderMsg_GetMailInfoID(u8Index)); //FIXME
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetMailMaxItem
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetMailCreateTime(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize)
{
    UINT32  u32CreateTime;
    NGL_TM stDate;
    UINT16 u16TotalMails    = 0;
    UINT16 u16TotalAnncmnts = 0;

    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        u16TotalMails    = MApp_DecoderMsg_GetTotalMails();
        u16TotalAnncmnts = MApp_DecoderMsg_GetTotalAnncmnts();

        if (u8Index < u16TotalMails)
        {
            u32CreateTime = MApp_DecoderMsg_GetMailInfoMailCreateTime(u8Index);
        }
        else if (u8Index < (u16TotalMails + u16TotalAnncmnts))
        {
            u32CreateTime = MApp_DecoderMsg_GetMailInfoAnncmntCreateTime(u8Index - u16TotalMails);
        }
        else
        {
            u32CreateTime = 0;
        }

        nglTimeToTm((const NGL_TIME*)&u32CreateTime, &stDate);

        if (u32CreateTime != 0)
        {
            snprintf((char*)pu8Buf, u16BufSize, "%04d/%02d/%02d %02d:%02d:%02d",
                                                          stDate.uiYear,
                                                          stDate.uiMonth,
                                                          stDate.uiMonthDay,
                                                          stDate.uiHour,
                                                          stDate.uiMin,
                                                          stDate.uiSec
                                                          ); //FIXME, Create time
        }
        else
        {
            snprintf((char*)pu8Buf, u16BufSize, "0000/00/00 00:00:00");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetMailMaxItem
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT16 MApp_CCAUI_CasCmd_GetMailMaxItem(void)
{
    return (MApp_DecoderMsg_GetTotalMails() + MApp_DecoderMsg_GetTotalAnncmnts());
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetMailContent
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetMailContent(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize)
{
    UINT16 u16TotalMails     = 0;
    UINT16 u16TotalAnncmnts  = 0;
    stIrdTextMsg* pstMail    = NULL;

    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
         u16TotalMails     = MApp_DecoderMsg_GetTotalMails();
         u16TotalAnncmnts  = MApp_DecoderMsg_GetTotalAnncmnts();

         if (u16BufSize >= IRD_MAX_TEXT_LENGTH_IN_BYTE)
         {
             if (u8Index < u16TotalMails)
             {
                 pstMail = MApp_DecoderMsg_GetMailMsg(u8Index);
                 snprintf((char*)pu8Buf, u16BufSize, "%s", pstMail->au8MsgByte);
             }
             else if (u8Index < (u16TotalMails + u16TotalAnncmnts))
             {
                 pstMail = MApp_DecoderMsg_GetAnncmentMsg(u8Index - u16TotalMails);
                 snprintf((char*)pu8Buf, u16BufSize, "%s", pstMail->au8MsgByte);
             }
             else
             {
                 snprintf((char*)pu8Buf, u16BufSize, (const char*)"Empty");
             }
         }
         else
         {
             snprintf((char*)pu8Buf, u16BufSize, (const char*)"The buffer is not big enough to store the mail content.");
         }
     }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetMailType
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_GetMailType(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize)
{
    INT8* au8TypeStr[] =    // For OSD display, add the padding
    {
        "Mail",
        "Anncmnt",
        "Spare  ",
        "Spare  "
    };

    EN_CCA_TEXT_MSG_TYPE eTextType = E_CCA_TEXT_MSG_TYPE_SPARE;

    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        eTextType = MApp_DecoderMsg_GetMailInfoType(u8Index);
        if (eTextType <= E_CCA_TEXT_MSG_TYPE_SPARE)
        {
            snprintf(pu8Buf, u16BufSize, au8TypeStr[eTextType]);
        }
        else
        {
            snprintf(pu8Buf, u16BufSize, au8TypeStr[E_CCA_TEXT_MSG_TYPE_SPARE]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetMailCheck
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_CasCmd_CheckNewMailItem(UINT8 u8Index)
{
    UINT16 u16TotalMails    = MApp_DecoderMsg_GetTotalMails();
    UINT16 u16TotalAnncmnts = MApp_DecoderMsg_GetTotalAnncmnts();

    if (u8Index < u16TotalMails)
    {
        return (MApp_DecoderMsg_GetMailInfoIDNewMailItems(u8Index));
    }
    else if (u8Index < (u16TotalMails + u16TotalAnncmnts))
    {
        return (MApp_DecoderMsg_GetMailInfoIDNewAnncmntItems(u8Index - u16TotalMails));
    }
    else
    {
        NGLOG_DEBUG("\n No change. The selected Mail (index = %d) does not exit\n", u8Index);
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_GetNewMailItemNumber
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetNewMailItemNumber(void)
{
    UINT8  u8Count    = 0;
    UINT16 u16ItemNum = MApp_CCAUI_CasCmd_GetMailMaxItem();

    while (u16ItemNum > 0)
    {
       u16ItemNum--;
       u8Count += (UINT8)(MApp_CCAUI_CasCmd_CheckNewMailItem(u16ItemNum));
    }

    return (u8Count);
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_SetMailDataReadCheck
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_DelMailData(UINT8 u8Index)
{
    UINT16 u16TotalMails    = MApp_DecoderMsg_GetTotalMails();
    UINT16 u16TotalAnncmnts = MApp_DecoderMsg_GetTotalAnncmnts();

    NGLOG_DEBUG("\n Mail box del list %d\n", u8Index);    // 
    if (u8Index < u16TotalMails)
    {
        MApp_DecoderMsg_DeleteMail(u8Index);
    }
    else if (u8Index < (u16TotalMails + u16TotalAnncmnts))
    {
        MApp_DecoderMsg_DeleteAnncmnt(u8Index - u16TotalMails);
    }
    else
    {
        NGLOG_DEBUG("\n No change. The selected Mail (index = %d) does not exit\n", u8Index);
    }
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_CasCmd_SetMailDataReadCheck
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_DelAllMailData(void)
{
    UINT16 u16TotalMails    = MApp_DecoderMsg_GetTotalMails();
    UINT16 u16TotalAnncmnts = MApp_DecoderMsg_GetTotalAnncmnts();

    NGLOG_DEBUG("\n Mail box del all in the list\n");    // 

    if (u16TotalMails > 0)
    {
        MApp_DecoderMsg_DeleteAllMails();
    }

    if (u16TotalAnncmnts > 0)
    {
        MApp_DecoderMsg_DeleteAllAnncmnts();
    }
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetComponentsMaxString
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_CCAUI_CasCmd_GetComponentsMaxString(void)
{
   return CCA_CAS_COMPONENTS_MAX_STRING;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_GetComponentsIndexString
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_CasCmd_GetComponentsIndexString(UINT8 u8Index, INT8* pu8Buf, UINT16 u16BufSize)
{
    // FIXME: Review the strings
    static INT8* au8COMPONENTS_STR[] =
    {
        "Core Version : 7.31.1",
        "Services and modules Version : 4.12.6",
        "Smartcard cord Version : 10.1.27",
        "Shared transport component Version : 3.0.7",
        "DVB Services Version : 5.0.5",
        "ECM Pagesearch Version : 3.1.5",
        "Shared DVB and IPTV components Version : 7.0.13",
        "Product interface Version : 1.8.1",
        "CustomCA Version : 1.0.6",
        "RSA Challenge Version : 1.1.6",
        "RSA Crypto Version : 1.0.4",
        "TMS Version : 1.8.1",
        "Smartcard marriage Version : 2.0.10",
        "Crypto Component Version : 2.0.9",
        "PVR services Version : 3.4.23"
    };

    if ((pu8Buf != NULL) && (u16BufSize > 0))
    {
        snprintf((char*)pu8Buf, u16BufSize, au8COMPONENTS_STR[u8Index]);
    }
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_SetMailData
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_SetMailData(void)
{
    //FIXME
    //MApp_ird_ui_set_mail_data();
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_SaveMailData
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_CasCmd_SaveMailData(void)
{
   //FIXME
   //MApp_ird_ui_save_mail_data();
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_CasCmd_IfNewMailOrAnncRead
///
///
///  @param [in] u8Type   :  0 to check mail if any not read,  1 to check annc if any not read
/// 
///  @return  BOOL: True for not read in any one. False for all read.
///
///  @author MStarSemi @date 2014/10/31
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_CasCmd_IfNewMailOrAnncRead(UINT8 u8Type)
{

	UINT16 u16totalNum,u16Count;
	u16totalNum = u16Count = 0;

	if(u8Type == 0) // check the mail
	{
		u16totalNum = MApp_DecoderMsg_GetTotalMails();
		
	}
	else if(u8Type == 1)
	{
		u16totalNum = MApp_DecoderMsg_GetTotalAnncmnts();
	}


	for(u16Count = 0;u16Count < u16totalNum; u16Count++ )
	{

		if(u8Type == 0)
		{
			if(MApp_DecoderMsg_GetMailInfoIDNewMailItems(u16Count) == TRUE)
			{
				return TRUE;
			}

		}
		else if(u8Type == 1)
		{
			if(MApp_DecoderMsg_GetMailInfoIDNewAnncmntItems(u16Count) == TRUE)
			{
				return TRUE;
			}
		}
	}

    return FALSE;
}
#endif
}
#undef _MApp_ZUI_ACTCCACASCMD_C

