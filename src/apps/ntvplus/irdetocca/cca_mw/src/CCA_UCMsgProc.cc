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
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CCA_UCMSGPROC_C
#define CCA_UCMSGPROC_C




//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------
#include <ntvwindow.h>
#include <ngl_log.h>
#include <appCCADecoderMsg.h>
#include <CCA_UCMsgProc.h>
#include <MApp_ZUI_ACTCCACasCmd.h>
#include <MApp_ZUI_ACTCCAUIControl.h>
#include <UniversalClient_SPI.h>

NGL_MODULE(IRDCCAMSGPROC);
namespace cca{
//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------


extern UINT16 MApp_CCA_SCP_ErrorCodeHandle(uc_service_status_st *Service_Status,UINT8* severity,UINT16* status);


//-------------------------------------------------------------------------------------------------
// Local Definitions
//------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Local Variable
//-------------------------------------------------------------------------------------------------
// Common
uc_device_platform_identifiers stDeviceId;


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//For messages with this type, the lpVoid argument points to a uc_buffer_st structure.
//uc_buffer_st::bytes points to a buffer containing new serial number,
//uc_buffer_st::length is the length of the serial number (currently defined as 4 bytes).
UINT8 UC_Global_Serial_Num_Chg(void* lpVoid)
{
	uc_buffer_st		*pucBuffer = NULL;
	pucBuffer = (uc_buffer_st *)lpVoid;

    MApp_CCAUI_CasCmd_UpdateLoaderStatus();
    MApp_CCAUI_CasCmd_UpdateClientStatus();
    
	return 1;

}



//For messages with this type, the lpVoid argument is NULL.
//The application should use UniversalClient_GetProductList to check the changes.
UINT8 UC_Global_Notify_Entitlement_Chg(void)
{
    MApp_CCAUI_CasCmd_UpdateProductStatus();

	return 1;
}


//For messages with this type, the lpVoid argument points to uc_ca_section_count structure. The format of uc_ca_section_count is as follows:
//      typedef struct _uc_ca_section_count
//      {
//        uc_uint32 ecm_count;
//        uc_uint32 emm_count;
//      }uc_ca_section_count;
UINT8 UC_Global_Notify_Casection_Count(void* lpVoid)
{
	uc_ca_section_count *Ca_Section_Count = NULL;
	Ca_Section_Count = (uc_ca_section_count *)lpVoid;

    MApp_CCAUI_CasCmd_UpdateClientStatus();

	return 1;

}

//For messages with this type, the lpVoid argument points to uc_region_information structure.
UINT8 UC_Global_Notify_Region_Subregion_Info(void* lpVoid)
{
	uc_region_information *Region_Info = NULL;
	Region_Info = (uc_region_information *)lpVoid;


	return 1;
}


//For messages with this type, the lpVoid argument is NULL.
//The application should use UniversalClient_GetTmsData to get the new TMS data.
UINT8 UC_Global_Notify_TMS_Data_Chg(void)
{
    MApp_CCAUI_CasCmd_UpdateClientStatus();
    
	return 1;

}


//For messages with this type, the lpVoid argument is NULL
//The application should use UniversalClient_GetNationality to check the change.
UINT8 UC_Global_Notify_Nationality_Chg(void)
{
    MApp_CCAUI_CasCmd_UpdateClientStatus();
    
	return 1;
}


//For messages with this type, the lpVoid argument is NULL
//The application should use UniversalClient_GetHomingChannelData to check the change.
UINT8 UC_Global_Notify_Homing_Channel_Data_Chg(void)
{

	return 1;
}


//For messages with this type, the lpVoid argument is NULL
//The application should use UniversalClient_GetMiddlewareUserData to check the change.
UINT8 UC_Global_Notify_MW_User_Data_Chg(void)
{

	return 1;
}


//For messages with this type, the lpVoid argument point to uc_uint32 caSystemID.
UINT8 UC_Global_Notify_Reset_To_Factory(void* lpVoid)
{

	return 1;
}

//For messages with this type, the lpVoid argument points to uc_flexiflash_msg value.
UINT8 UC_Global_Notify_Flexflash_Msg(void* lpVoid)
{

	uc_flexiflash_msg *Flexflash_msg = NULL;
	Flexflash_msg = (uc_flexiflash_msg *)lpVoid;


    NGLOG_DEBUG("Flexflash_msg Status: %s\n",Flexflash_msg->secureCoreListStatus);
    NGLOG_DEBUG("Flexflash_msg download Prog Info: %s\n",Flexflash_msg->packagesDownloadProgressInfo);


    MApp_CCAUI_CasCmd_UpdateFlexiFlashMsg((UINT8 *)Flexflash_msg->secureCoreListStatus, (UINT8 *)Flexflash_msg->packagesDownloadProgressInfo);

	return 1;
}

UINT8 UC_Global_Notify_IFCP_Msg(void* lpVoid)
{

    uc_IFCP_image_msg *pIFCPstatus = NULL;
    pIFCPstatus = (uc_IFCP_image_msg *)lpVoid;


    NGLOG_DEBUG("pIFCPstatus imageStatus: %s\n",pIFCPstatus->imageStatus);
    NGLOG_DEBUG("pIFCPstatus download Prog Info: %s\n",pIFCPstatus->packagesDownloadProgressInfo);


    MApp_CCAUI_CasCmd_UpdateFlexiFlashIFCPMsg((UINT8 *)pIFCPstatus->imageStatus, (UINT8 *)pIFCPstatus->packagesDownloadProgressInfo);

    return 1;
}

//This is the message that indicates the entitled status of product entitlement changed, from FALSE to TRUE, or from TRUE to FALSE.
//For messages with this type, the \a lpVoid argument is uc_product_status.
UINT8 UC_Global_Notify_Entitled_Status_Chg(void* lpVoid)
{
	uc_product_status *Product_Status = NULL;
	Product_Status = (uc_product_status *)lpVoid;

	return 1;
}

//Please refer to 705410 CCP IRD Messages for more detail information.
//For messages with this type, the lpVoid argument points to a uc_raw_ird_msg structure.
//uc_raw_ird_msg::caSystemID identifies which operator sends the message,
//uc_raw_ird_msg::rawIrdMsg points to a buffer containing the raw message,
//uc_raw_ird_msg::length is the length of the message.
UINT8 UC_Global_Ext_Raw_Ird_Msg(void* lpVoid)
{
	uc_raw_ird_msg *Raw_Ird_Msg = NULL;
    BOOL         bHandled    = FALSE;

	Raw_Ird_Msg = (uc_raw_ird_msg *)lpVoid;
    bHandled = MApp_DecoderMsg_Parser(Raw_Ird_Msg);
	return 1;
}

//extern uc_result UniversalClientSPI_Stream_CleanDescramblingKey(
    //uc_stream_handle streamHandle);

//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_ecm_status_st.
UINT8 UC_Ecm_Status(void* lpVoid)
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
        UniversalClientSPI_Stream_CleanDescramblingKey(::Current_StreamHandle);
        NGLOG_DEBUG("Clean Key!!");
    }

    if (Ecm_Status->caStream.protocolType == UC_STREAM_DVB)
    {
        #if 1
        NGLOG_DEBUG("[ECM] UC_ECM_STATUS::PID(0x%04X) ... %s\n", Ecm_Status->caStream.pid, Ecm_Status->statusMessage);
        #endif

        MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus(SERVICE_DESCRAMBLE, Ecm_Status->caStream.pid, (UINT8 *)Ecm_Status->statusMessage);

    }

	return 1;

}


//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_emm_status_st.

UINT8 UC_Emm_Status(void* lpVoid)
{
	uc_emm_status_st *Emm_Status = NULL;
	Emm_Status = (uc_emm_status_st *)lpVoid;



	if (Emm_Status->caStream.protocolType == UC_STREAM_DVB)
	{

	#if 1
		NGLOG_DEBUG("[EMM] UC_EMM_STATUS::PID(0x%04X) ... %s\n", Emm_Status->caStream.pid, Emm_Status->statusMessage);
	#endif
    
        MApp_CCAUI_CasCmd_UpdateServiceItemCaStatus(SERVICE_BROADCAST_EMM, Emm_Status->caStream.pid, (UINT8 *)Emm_Status->statusMessage);
	
	}

	return 1;
}

//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_service_status_st.
UINT8 UC_Service_Status(void* lpVoid)
{

	uc_service_status_st	*Service_Status = NULL;
	UINT8 			severity = 0;
	UINT16		status = 0;
	UINT8			source = 0;

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

    MApp_CCAUI_CasCmd_UpdateServiceItemStatus(Service_Status->serviceHandle, (UINT8 *)Service_Status->statusMessage);

	MApp_CCA_SCP_ErrorCodeHandle(Service_Status,&severity,&status);
	return 1;

}




//For messages with this type, the lpVoid argument is a pointer to a structure of type uc_service_monitor_status_st.
UINT8 UC_Service_Ecm_Monitor_Status(void* pVoid)
{
	uc_service_monitor_status_st	*Service_Monitor_Status= NULL;
	Service_Monitor_Status = (uc_service_monitor_status_st *)pVoid;


#if 1
	NGLOG_DEBUG("[ECM_MONITOR] UC_SERVICE_ECM_MONITOR_STATUS::%s\n", Service_Monitor_Status->pMessage);
#endif

    if(MApp_CCAUI_CasCmd_UpdateServiceMonitorStatus(Service_Monitor_Status->serviceHandle, (UINT8 *)Service_Monitor_Status->pMessage) != TRUE)
    {
        NGLOG_DEBUG("[ECM_MONITOR] update ECM monitor FAIL");
    }
    
	return 1;


}

UINT8 UC_Service_Emm_Monitor_Status(void* pVoid)
{

	uc_service_monitor_status_st	*Service_Monitor_Status = NULL;

	Service_Monitor_Status = (uc_service_monitor_status_st *)pVoid;

#if 1
	NGLOG_DEBUG("[EMM_MONITOR] UC_SERVICE_EMM_MONITOR_STATUS::%s\n", Service_Monitor_Status->pMessage);
#endif

    if(MApp_CCAUI_CasCmd_UpdateServiceMonitorStatus(Service_Monitor_Status->serviceHandle, (UINT8 *)Service_Monitor_Status->pMessage) != TRUE)
    {
        NGLOG_DEBUG("[EMM_MONITOR] update EMM monitor FAIL");
    }
    
	return 1;

}


UINT8 UC_Service_Status_For_Convergent_Client(void* pVoid)
{
	uc_convergent_client_service_status_st	  *Convergent_Info = NULL;
	Convergent_Info = (uc_convergent_client_service_status_st *)pVoid;


	NGLOG_DEBUG("Service Handle 0x%x\n",Convergent_Info->serviceHandle);
	NGLOG_DEBUG("CCA status\n");
	NGLOG_DEBUG("isVaild %d\n",Convergent_Info->cloakedCAStatus.isValid);
	if(Convergent_Info->cloakedCAStatus.isValid == TRUE)
	{
		NGLOG_DEBUG("internalServiceHandle 0x%x\n",Convergent_Info->cloakedCAStatus.internalServiceHandle);
		NGLOG_DEBUG("statusMessage %s\n",Convergent_Info->cloakedCAStatus.statusMessage);

	}
	NGLOG_DEBUG("Softcell status\n");
	NGLOG_DEBUG("isVaild %d\n",Convergent_Info->softcellStatus.isValid);
	if(Convergent_Info->softcellStatus.isValid == TRUE)
	{
		NGLOG_DEBUG("internalServiceHandle 0x%x\n",Convergent_Info->softcellStatus.internalServiceHandle);
		NGLOG_DEBUG("statusMessage %s\n",Convergent_Info->softcellStatus.statusMessage);

	}

	return 1;



}


UINT32 UC_Status_Get_Emm_Service_Handle(void)
{
    return gEMMStreamInfo.serviceHandle;
}

UINT32 UC_Status_Get_Ecm_Service_Handle(void)
{
    return gECMStreamInfo.serviceHandle;
}

#if(CA_CCA_PVR_SUP == 1)
UINT32 UC_Status_Get_Pvr_Playback_Service_Handle(void)
{
    return gPVRStreamInfo.pvrPlaybackServiceHandle;
}
#endif

BOOL UC_Status_Get_Service_Status(UINT32 handle,  uc_service_status_st *Service_Status)
{
    if(UniversalClient_Extended_GetServiceStatus(handle, Service_Status) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Stream_Status(UINT32 handle,  UINT32 *pStreamCount, uc_service_stream_status_st **ppStreamStatusList)
{
    if(UniversalClient_GetStreamStatus(handle, (uc_uint32 *)pStreamCount, ppStreamStatusList) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Free_Stream_Status(uc_service_stream_status_st **ppStreamStatusList)
{
    if((ppStreamStatusList != NULL) && (UniversalClient_FreeStreamStatus(ppStreamStatusList) == UC_ERROR_SUCCESS))
    {
        return TRUE;
    }

    return FALSE;
}


BOOL UC_Status_Enable_Monitor_Status(UINT32 handle,  BOOL enable)
{
    uc_byte TLV_Monitor_Switch[4] = {UC_TLV_TAG_FOR_MONITOR_SWITCH, 

                                     0x00, 0x01, 

                                     0x01};
    TLV_Monitor_Switch[3] = enable;
        
    if(UniversalClient_ConfigService(handle, sizeof(TLV_Monitor_Switch), TLV_Monitor_Switch) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CCA_Status_Get_Device_Status(void)
{
    if(UC_Status_Get_Device_Status(&stDeviceId) == TRUE)
    {    
        NGLOG_DEBUG("stDeviceId.systmeid %x ",__FUNCTION__,__LINE__,stDeviceId.systemId);
        NGLOG_DEBUG("stDeviceId.hardwareVersion %x ",__FUNCTION__,__LINE__,stDeviceId.hardwareVersion);
        NGLOG_DEBUG("stDeviceId.keyVersion %x ",__FUNCTION__,__LINE__,stDeviceId.keyVersion);
        NGLOG_DEBUG("stDeviceId.variant %x \n\033[m",__FUNCTION__,__LINE__,stDeviceId.variant);
        NGLOG_DEBUG("stDeviceId.manufacturerId %x ",__FUNCTION__,__LINE__,stDeviceId.manufacturerId);
        return TRUE;
    }
    NGLOG_DEBUG("Get Status Fail!!",__FUNCTION__,__LINE__);
    return FALSE;
}

BOOL UC_Status_Get_Device_Status(uc_device_platform_identifiers *pDevicePlatformIdentifiers)
{
    if(UniversalClientSPI_Device_GetPlatformIdentifiers(pDevicePlatformIdentifiers) == UC_ERROR_SUCCESS)
    {
        NGLOG_DEBUG("\033[1;34m[%s:%d]  \n\033[m",__FUNCTION__,__LINE__);
        return TRUE;
    }
    NGLOG_ERROR("\033[1;34m[%s:%d]  \n\033[m",__FUNCTION__,__LINE__);
    return FALSE;
}

BOOL UC_Status_Get_CSSN(uc_buffer_st *pbuf)
{
    if(UniversalClient_GetCSSN(pbuf) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_SerialNo(uc_buffer_st *pbuf)
{
    if(UniversalClient_GetSerialNumber(pbuf) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Product_Status(UINT32 *ProductCount, uc_product_status **pProductList)
{
    if(UniversalClient_GetProductList((uc_uint32 *)ProductCount, pProductList) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    *ProductCount = 0;
    return FALSE;
}

BOOL UC_Status_Free_Product_Status(uc_product_status **pProductList)
{
    if((pProductList != NULL) && (UniversalClient_FreeProductList(pProductList)) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL UC_Status_Get_CCAVersion(uc_buffer_st *pbuf)
{
    if(UniversalClient_GetVersion(pbuf) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_BuildInfo(uc_buffer_st *pbuf)
{
    if(UniversalClient_GetBuildInformation(pbuf) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_LockId(UINT16 *pu16LockId)
{
    if(UniversalClient_GetLockId(pu16LockId) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_SecureType(UINT8 *pu8SecureType)
{
    if(UniversalClient_GetSecureType(pu8SecureType) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

//_Ext_xxx: for multiple secure core support
BOOL UC_Status_Get_Ext_ClientId(uc_client_id *pClientIDString)
{
    if(UniversalClient_Extended_GetClientIDString(pClientIDString) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Ext_SeriaNo(uc_serial_number *pSN)
{
    if(UniversalClient_Extended_GetSerialNumber(pSN) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}


BOOL UC_Status_Get_Ext_CASystemId(uc_operator *pOperatorInfo)
{
    if(UniversalClient_Extended_GetOperatorInfo(pOperatorInfo) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Ext_Nationality(uc_nationality *pNationality)
{
    if(UniversalClient_Extended_GetNationality(pNationality) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Ext_TMS(uc_tms_data *pTmsData)
{
    if(UniversalClient_Extended_GetTmsData(pTmsData) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Capabilities(uc_buffer_st *pbuf)
{
    if(UniversalClient_GetCapabilities(pbuf) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Status_Get_Ext_EcmEmm_Count(uc_ca_extended_section_count *pCaSectionCount)
{
    if(UniversalClient_Extended_GetEcmEmmCount(pCaSectionCount) == UC_ERROR_SUCCESS)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UC_Reset_Ca_Data(void)
{
    uc_operator OperatorInfo = {0};
    uc_ca_system_id CASystemID = {0};
    UINT8 i = 0;
    
    if(UniversalClient_Extended_GetOperatorInfo(&OperatorInfo) == UC_ERROR_SUCCESS)
    {
        CASystemID.validOperatorCount = OperatorInfo.validOperatorCount;
        for (i = 0; i < OperatorInfo.validOperatorCount; i++)
        {
            CASystemID.caSystemID[i] = OperatorInfo.operatorInfo[i].activeCaSystemID;
        }

        if(UniversalClient_Extended_ResetCAData(&CASystemID) == UC_ERROR_SUCCESS)
        {
            return TRUE;
        }
    }

    return FALSE;
}



///////////////////////////////////////////////////////////////////////////////
///  global  UC_Ird_Process_Command
///  IRD message dispatcher
///
///  @param [in]      pu8CmdData
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL UC_Ird_Process_Command(UINT8* pu8CmdData)
{
    BOOL bHandled  = FALSE;

    switch ((tCCAIRD_UI_EventType)pu8CmdData[0])
    {
        NGLOG_DEBUG("UC_Ird_Process_Command = %d\n", pu8CmdData[0]);

        case E_APP_CMD_CCAIRD_SHOW_ERROR_MSG:
        {
            //bHandled = MApp_CCAUI_ShowErrorMsg(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_PIN_CODE:
        {
            UINT16 u16PinCode = 0;

            u16PinCode = ((UINT16)pu8CmdData[1] << 8) | ((UINT16)pu8CmdData[2]);
            NGLOG_DEBUG("PinCode = 0x%x\n", u16PinCode);
            //bHandled = MApp_CCAUI_ShowPinInputBox(u16PinCode);
            break;
        }
        case E_APP_CMD_CCAIRD_IPPV:
        {
            break;
        }
        case E_APP_CMD_CCAIRD_KEY_INPUT:
        {
            break;
        }
        case E_APP_CMD_CCAIRD_FINGERPRINT:
        {
            //MApp_CCAUI_ShowFingerprint(&pu8CmdData[1]);
            bHandled = TRUE;
            break;
        }
        case E_APP_CMD_CCAIRD_STATUS:
        {
           Window::broadcast(View::WM_CCAMSG, (DWORD)pu8CmdData[1], 0);
           break;
        }
        case E_APP_CMD_CCAIRD_FINGERPRINTING_OPTION:
        {
#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
            //MApp_CCAUI_StoreFingerprintingOption(&pu8CmdData[1]);
            bHandled = TRUE;
#endif
            break;
        }
        case E_APP_CMD_CCAIRD_TMS:
        {
            //bHandled = MApp_CCAUI_ShowTMS(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_FORCED_MSG:
        {
           // bHandled = MApp_CCAUI_ShowForcedMsg(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_MAIL:
        {
            //bHandled = MApp_CCAUI_ShowMail(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_ICON_MSG:
        {
            //bHandled = MApp_CCAUI_ShowIcon(&pu8CmdData[1]);
            break;
        }
		case E_APP_CMD_CCAIRD_PREVIEW:
        {
            //bHandled = MApp_CCAUI_ShowPreview(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_PIN_CHANGE:
        {
            //bHandled = MApp_CCAUI_SetPinCode(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_OTA_MSG_SHOW_EVENT:
        {
            //bHandled = MApp_CCAUI_OTA_PopupShow(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_OTA_MSG_DRV_EVENT:
        {
            //bHandled = MApp_CCAUI_OTA_DrvEvent(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_OTA_MSG_KEY_EVENT:
        {
            //bHandled = MApp_CCAUI_OTA_PopupEvent(&pu8CmdData[1]);
            break;
        }
        case E_APP_CMD_CCAIRD_OTA_MENU_SIGNAL_INFO:
        {
            break;
        }
        default:
            return FALSE;
    }

    return bHandled;
}
}
#endif  // CCA_UCMSGPROC_C
