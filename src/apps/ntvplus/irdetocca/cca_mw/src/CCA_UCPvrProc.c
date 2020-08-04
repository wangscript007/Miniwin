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
#if(CA_CCA_PVR_SUP == 1)

#ifndef CCA_UCPVRPROC_C
#define CCA_UCPVRPROC_C




//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------

#include <ngl_types.h>


//#include "appCCADecoderMsg.h"
#include "CCA_UCPvrProc.h"

#include "MApp_ZUI_ACTCCACasCmd.h"

#include "apiFS.h"

//#include "MApp_ZUI_ACTCCAUIControl.h"

//#define CCA_UCMSG_DEBUG
//#define CCA_UCMSG_TEST

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#define CCA_UCMSG_DEBUG 1
#ifdef CCA_UCMSG_DEBUG
#define CCA_UCPVR_DBG(fmt, arg...)   {printf("\033[0;32m[CCA_UCPVR_DBG]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define CCA_UCPVR_DBG(fmt, arg...)
#endif


//-------------------------------------------------------------------------------------------------
//  Extern Functions
//-------------------------------------------------------------------------------------------------
extern ULONGLONG	gBuffAddr;
extern UINT32	gBuffLeng; 


extern void *AP_NC_Alloc(UINT32 u32Size );
extern void AP_NC_Free(UINT8* pAddr);
extern void MyServiceMessageProc(void *pMessageProcData, uc_service_message_type message, void *pVoid);
extern UINT16 MApp_CCA_SCP_ErrorCodeHandle(UINT8* statusMessage,UINT8* severity,UINT16* status);
extern void MApp_Pvr_GetCAMetaName(UINT16 *FineName);

//-------------------------------------------------------------------------------------------------
// Local Definitions
//------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Local Variable
//-------------------------------------------------------------------------------------------------
static UINT32			s_ulNextKeyIndex = 0;


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
uc_result UC_Pvr_Request_PlaybackKey(UINT32 ulIndex)
{
	uc_buffer_st		ucMetaData;

	if(ulIndex >= gPVRStreamInfo.pvrPlaybackMetaCount)
	{
		return UC_ERROR_INVALID_PVR_METADATA;
	}

	ucMetaData.length = gPVRStreamInfo.pvrPlaybackMeta[ulIndex].pvrMetaData.length;
	ucMetaData.bytes = AP_NC_Alloc((UINT32)ucMetaData.length);
	if(ucMetaData.bytes == NULL)
	{
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}

	memset(ucMetaData.bytes, 0x00, ucMetaData.length);
	memcpy(ucMetaData.bytes, gPVRStreamInfo.pvrPlaybackMeta[ulIndex].pvrMetaData.bytes, ucMetaData.length);

	UniversalClient_SubmitPVRSessionMetadata(gPVRStreamInfo.pvrPlaybackServiceHandle, &ucMetaData);

	AP_NC_Free(ucMetaData.bytes);

	return UC_ERROR_SUCCESS;
}

unsigned char UC_Pvr_getMetadataIndex(UINT32 *size)
{
	uc_uint8		ucIndex = 0;
	uc_uint8		ucNext = 0;	
	UINT32		u32Time = 0;
	int			i = 0;

	for(i=0; i<gPVRStreamInfo.pvrPlaybackMetaCount; i++)
	{
		// Get current playback index (Postion need to equal and need has session key)
		if((gBuffAddr> gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress)&&(gPVRStreamInfo.pvrPlaybackSession[i].statusSession!=0))
		{
			ucIndex = i;
		}
	}

	// Get next key.	
	for(i=gPVRStreamInfo.pvrPlaybackMetaCount-1; i>ucIndex; i--)
	{
		if(gBuffAddr < gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress)
		{
			ucNext = i;
			break;
		}	
	}

	// check next key, if no key and closely require the key. 
	// (K2.time -K1.time) * (K2.addr-current addr) / (K2.addr-K1.addr)
	// if not 0, Mean a requestPlaybackKey not finish. ucNext!=ucIndex==>if equal, it has div 0 crash
	//if(s_ulNextKeyIndex == 0 && (g_ucPVR.pvrPlaybackMeta[ucNext].pvrMetaAddress != g_ucPVR.pvrPlaybackMeta[ucIndex].pvrMetaAddress))
	if(s_ulNextKeyIndex == 0 && (gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaAddress > gPVRStreamInfo.pvrPlaybackMeta[ucIndex].pvrMetaAddress))
	{
		//u32Time = (g_ucPVR.pvrPlaybackMeta[ucNext].pvrMetaTime - g_ucPVR.pvrPlaybackMeta[ucIndex].pvrMetaTime)*(g_ucPVR.pvrPlaybackMeta[ucNext].pvrMetaAddress-gBuffAddr)/(gBuffAddr<g_ucPVR.pvrPlaybackMeta[ucNext].pvrMetaAddress-g_ucPVR.pvrPlaybackMeta[ucIndex].pvrMetaAddress);

		u32Time = (gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaAddress-gBuffAddr);
		u32Time = u32Time / (gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaAddress - gPVRStreamInfo.pvrPlaybackMeta[ucIndex].pvrMetaAddress);
		u32Time = u32Time * (gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaTime - gPVRStreamInfo.pvrPlaybackMeta[ucIndex].pvrMetaTime);
		if(u32Time < 0 && gPVRStreamInfo.pvrPlaybackSession[ucNext].statusSession == 0)
		{
			s_ulNextKeyIndex = ucNext;
			UC_Pvr_Request_PlaybackKey(s_ulNextKeyIndex);
		}
	}

	// change the next key.
	*size = 0;

	// Need have the key.If no, It should  not change the key, it need to discussion. 
	// &*&*&*j1_130116 add safe protected, due to current key should not be enter.  
	if((gPVRStreamInfo.pvrPlaybackSession[ucNext].statusSession != 0)&&(ucNext!=ucIndex))
	{
		if(((gBuffAddr + gBuffLeng) > gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaAddress)
			&& (gBuffAddr < gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaAddress))
		{
			*size = (gBuffAddr+gBuffLeng) - gPVRStreamInfo.pvrPlaybackMeta[ucNext].pvrMetaAddress;
		}
	}

	return ucIndex;
}


unsigned int UC_Pvr_getSessionKey(UINT8 type, unsigned char *pPVRSessionKey)
{
	unsigned char		ucIndex = 0;
	UINT32			u32Size = 0;

	if(type == 0 && gPVRStreamInfo.pvrRecordSessionCount > 0)
	{
		ucIndex = gPVRStreamInfo.pvrRecordSessionCount - 1;

		memset(pPVRSessionKey, 0x00, SESSION_DATA_SIZE);
		memcpy(pPVRSessionKey, &gPVRStreamInfo.pvrRecordSession[ucIndex].strSession[0], SESSION_DATA_SIZE);
		
	}
	else if(type == 1 && gPVRStreamInfo.pvrPlaybackMetaCount > 0)
	{
		ucIndex = UC_Pvr_getMetadataIndex(&u32Size);

		memset(pPVRSessionKey, 0x00, SESSION_DATA_SIZE);
		memcpy(pPVRSessionKey,&gPVRStreamInfo.pvrPlaybackSession[ucIndex].strSession[0],  SESSION_DATA_SIZE);
	}

	return u32Size;
}


uc_result UC_Pvr_Init_MetaData(void)
{
	INT32		fp = -1;
	UINT32		u32Index = 0;
	UINT32		u32Time = 0;
	ULONGLONG		u64Address = 0;  
	UINT32		u32Length = 0;
	UINT16	    pvrMetaName[256/*FILE_PATH_SIZE*/];
	UINT8		i = 0;


	memset(&pvrMetaName[0], 0x00, 256);
	MApp_Pvr_GetCAMetaName(&pvrMetaName[0]);

	fp = MApi_FS_Open((const char*)&pvrMetaName[0], O_RDONLY);
	if(fp < 0)
	{
		return UC_ERROR_BAD_RESOURCE_NAME;
	}

	for(i=0; i<MAX_METADATA; i++)
	{
		MApi_FS_Read(fp, &u32Index, sizeof(uc_uint32));
		if(u32Index == 0xFFFFFFFF)
		{
			gPVRStreamInfo.pvrPlaybackMetaCount = i;
			MApi_FS_Close(fp);

			return UC_ERROR_SUCCESS;
		}

		MApi_FS_Read(fp, &u32Time, sizeof(uc_uint32));
		MApi_FS_Read(fp, &u64Address, sizeof(ULONGLONG));
		MApi_FS_Read(fp, &u32Length, sizeof(uc_uint32));

		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaIndex = u32Index;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaTime = u32Time;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress = u64Address;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.length = u32Length;
		
		if(u32Length > 0)
		{
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes = AP_NC_Alloc((UINT32)u32Length);
			if(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes == NULL)
			{
				return UC_ERROR_INSUFFICIENT_BUFFER;
			}

			memset(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes, 0x00, u32Length);

			MApi_FS_Read(fp, gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes, u32Length);
		}
	}

	for(i=0; i<gPVRStreamInfo.pvrPlaybackMetaCount; i++)
	{
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaIndex = 0xFFFFFFFF;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaTime = 0;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress = 0xFFFFFFFF;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.length = 0;

		if(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes)
		{
			AP_NC_Free(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes);
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes = NULL;
		}
	}

	gPVRStreamInfo.pvrPlaybackMetaCount = 0;

	MApi_FS_Close(fp);

	return UC_ERROR_UNABLE_TO_CREATE;
}


UINT8 UC_Pvr_Append_Metadata(void* lpVoid)
{
    uc_pvr_service_status_st	*Pvr_Service_Status = NULL;
	uc_buffer_st				*pMetaData = NULL;

	Pvr_Service_Status = (uc_pvr_service_status_st *)lpVoid;
	pMetaData = (uc_buffer_st *)Pvr_Service_Status->pMessage;

    if(pMetaData == NULL)
	{
        CCA_UCPVR_DBG("[CA PVR] Append Metadata: ERROR get NULL metadata from CA\n");
		return FALSE;
	}

	#if 1
	{
		UINT32 i = 0;

		printf("\n");
		printf("============CA PVR METADATA============\n");
		printf("[RECORD] serviceHandle=0x%08X\n", Pvr_Service_Status->serviceHandle);
		printf("[RECORD] pMetaData->length=%d\n", pMetaData->length);
		printf("[RECORD] pMetaData->bytes=");

		for(i=0; i<pMetaData->length; i++)
		{
			if((i%80) == 0)
				printf("\n");
			printf("%02X ", *(pMetaData->bytes+i));
		}

		printf("\n");
		printf("========================================\n");
		printf("\n");
	}
	#endif

	#if 0
	MApi_PVR_Init_RecMetaData(UINT16 * mount_path, PVRProgramInfo_t * programInfo, UINT16 * fileName);
	MApi_PVR_RecordStart(PVRInfo_t * pPVRInfo, MSAPI_PVR_RECMETADATA * metadata, MS_S8 enFileFormat, UINT32 u32FreeDiskSpaceInMB, MS_BOOL bRecordAll);
	#endif

	UINT32 u32Idx = gPVRStreamInfo.pvrRecordMetaCount;

	gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaIndex = u32Idx;
	gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaTime = MsOS_GetSystemTime();
    //FIXME
	//gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaAddress = gBuffAddr + gBuffLeng;
	gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.length = pMetaData->length;

	gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.bytes = AP_NC_Alloc((UINT32)pMetaData->length);
	if(gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.bytes == NULL)
	{
		gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaIndex = 0xFFFFFFFF;
		gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaTime = 0;
		gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaAddress = 0xFFFFFFFF;
		gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.length = 0;
		gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.bytes = NULL;

        CCA_UCPVR_DBG("[CA PVR] ERROR unable to alloc mem for metadata\n");
		return FALSE;
	}

	memset(gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.bytes, 0x00, pMetaData->length);
	memcpy(gPVRStreamInfo.pvrRecordMeta[u32Idx].pvrMetaData.bytes, pMetaData->bytes, pMetaData->length);

	gPVRStreamInfo.pvrRecordMetaCount++;

	return TRUE;


}

MS_BOOL UC_Pvr_Save_Metadata(void)
{
	INT32		fp = -1;
	UINT32		endIndex = 0xFFFFFFFF;

	UINT16		pvrMetaName[256/*FILE_PATH_SIZE*/];


	if(gPVRStreamInfo.pvrRecordMetaCount == 0)
	{
        CCA_UCPVR_DBG("[CA PVR] ERROR no metadata to store to file\n");
		return FALSE;
	}

	memset(&pvrMetaName[0], 0x00, 256);
	MApp_Pvr_GetCAMetaName(&pvrMetaName[0]);

	fp = MApi_FS_Open((const char *)&pvrMetaName[0], O_WRONLY | O_CREAT);
	if(fp < 0)
	{
        CCA_UCPVR_DBG("[CA PVR] ERROR file open error\n");
		return FALSE;
	}

    UINT8 i;
	for(i=0; i<gPVRStreamInfo.pvrRecordMetaCount; i++)
	{
		MApi_FS_Write(fp, &gPVRStreamInfo.pvrRecordMeta[i].pvrMetaIndex, sizeof(UINT32));
		MApi_FS_Write(fp, &gPVRStreamInfo.pvrRecordMeta[i].pvrMetaTime, sizeof(UINT32));
		MApi_FS_Write(fp, &gPVRStreamInfo.pvrRecordMeta[i].pvrMetaAddress, sizeof(ULONGLONG));
		MApi_FS_Write(fp, &gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.length, sizeof(UINT32));
		MApi_FS_Write(fp, gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.bytes, gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.length);
	}
	
	MApi_FS_Write(fp, &endIndex, sizeof(uc_uint32));
	MApi_FS_Close(fp);

	return TRUE;
}


MS_BOOL UC_Pvr_Init_Record(UINT32 handle)
{    
    uc_byte TLV_PVR_Record[4] = {UC_TLV_TAG_FOR_PVR_RECORD, 

                                 0x00, 0x00,

                                 0x00};

    uc_result ret = UniversalClient_ConfigService(handle, sizeof(TLV_PVR_Record), TLV_PVR_Record);
    CCA_UCPVR_DBG("UC_TLV_TAG_FOR_PVR_RECORD for handle:%d ret:%d\n", handle, ret);
        
    if(ret == UC_ERROR_SUCCESS)
    {
        //UniversalClient_PVR_recordSetEnable(0);
        UINT8 i;
    	for(i=0; i<MAX_METADATA; i++)
    	{
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaIndex = 0xFFFFFFFF;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaTime = 0;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaAddress = 0xFFFFFFFF;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.length = 0;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.bytes = NULL;
    	}

    	gPVRStreamInfo.pvrRecordSessionCount = 0;
    	gPVRStreamInfo.pvrRecordMetaCount = 0;
        
        return TRUE;
    }

    return FALSE;

}

MS_BOOL UC_Pvr_Stop_Record(UINT32 handle)
{
    uc_byte TLV_Stop_PVR_Record[3] = {UC_TLV_TAG_FOR_STOP_PVR_RECORD, 

                                 0x00,

                                 0x00};

    uc_result ret = UniversalClient_ConfigService(handle, sizeof(TLV_Stop_PVR_Record), TLV_Stop_PVR_Record);
    CCA_UCPVR_DBG("UC_TLV_TAG_FOR_PVR_RECORD for handle:%d ret:%d\n", handle, ret);
        
    if(ret == UC_ERROR_SUCCESS)
    {
        UC_Pvr_Save_Metadata();
    	//UniversalClient_PVR_recordSetEnable(0);

        UINT8 i;
    	for(i=0; i<MAX_METADATA; i++)
    	{
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaIndex = 0xFFFFFFFF;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaTime = 0;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaAddress = 0xFFFFFFFF;
    		gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.length = 0;

    		if(gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.bytes)
    		{
    			AP_NC_Free(gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.bytes);
    			gPVRStreamInfo.pvrRecordMeta[i].pvrMetaData.bytes = NULL;
    		}
    	}

    	gPVRStreamInfo.pvrRecordSessionCount = 0;
    	gPVRStreamInfo.pvrRecordMetaCount = 0;
        
        return TRUE;
    }

    return FALSE;

}

MS_BOOL UC_Pvr_Init_Playback(void)
{
	uc_result ret = UC_ERROR_SUCCESS;
	uc_service_handle serviceHandle = UC_INVALID_HANDLE_VALUE;

	ret = UniversalClient_OpenService(  
			PVR_PLAYBACK_SERVICE_CONTEXT, 
			MyServiceMessageProc, 
			PVR_PLAYBACK_SERVICE_PROC_DATA,
			&serviceHandle);

	if(ret != UC_ERROR_SUCCESS)
	{
        CCA_UCPVR_DBG("PVR_PLAYBACK_SERVICE_CONTEXT ERROR : %d\n", ret);
		return ret;
	}

	gPVRStreamInfo.pvrPlaybackServiceHandle = serviceHandle;
	gPVRStreamInfo.pvrPlaybackSessionCount = 0;
	gPVRStreamInfo.pvrPlaybackMetaCount = 0;

	s_ulNextKeyIndex = 0;

	if(UC_Pvr_Init_MetaData() == FALSE)
	{
		UniversalClient_CloseService((uc_service_handle *)&serviceHandle);

		gPVRStreamInfo.pvrPlaybackServiceHandle = (uc_service_handle)UC_INVALID_HANDLE_VALUE;
		gPVRStreamInfo.pvrPlaybackSessionCount = 0;
		gPVRStreamInfo.pvrPlaybackMetaCount = 0;

		s_ulNextKeyIndex = 0;

		return ret;
	}

	if(UC_Pvr_Request_PlaybackKey(0) == FALSE)
	{
		unsigned char		i = 0;

		for(i=0; i<MAX_METADATA; i++)
		{
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaIndex = 0xFFFFFFFF;
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaTime = 0;
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress = 0xFFFFFFFF;
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.length = 0;

			if(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes)
			{
				AP_NC_Free(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes);
				gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes = NULL;
			}
		}

		UniversalClient_CloseService((uc_service_handle *)&serviceHandle);

		gPVRStreamInfo.pvrPlaybackServiceHandle = (uc_service_handle)UC_INVALID_HANDLE_VALUE;
		gPVRStreamInfo.pvrPlaybackSessionCount = 0;
		gPVRStreamInfo.pvrPlaybackMetaCount = 0;

		s_ulNextKeyIndex = 0;

		return ret;
	}

	return TRUE;
}

void UC_Pvr_Stop_Playback( void )
{
	uc_service_handle serviceHandle;
	unsigned char			i = 0;


	if(gPVRStreamInfo.pvrPlaybackServiceHandle == UC_INVALID_HANDLE_VALUE)
	{
		return;
	}

	serviceHandle = gPVRStreamInfo.pvrPlaybackServiceHandle;

	UniversalClient_CloseService((uc_service_handle *)&serviceHandle);

	for(i=0; i<gPVRStreamInfo.pvrPlaybackMetaCount; i++)
	{
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaIndex = 0xFFFFFFFF;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaTime = 0;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaAddress = 0xFFFFFFFF;
		gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.length = 0;

		if(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes)
		{
			AP_NC_Free(gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes);
			gPVRStreamInfo.pvrPlaybackMeta[i].pvrMetaData.bytes = NULL;
		}
	}

	gPVRStreamInfo.pvrPlaybackServiceHandle = (uc_service_handle)UC_INVALID_HANDLE_VALUE;
	gPVRStreamInfo.pvrPlaybackSessionCount = 0;
	gPVRStreamInfo.pvrPlaybackMetaCount = 0;

	s_ulNextKeyIndex = 0;

	return;
}



MS_BOOL UC_Pvr_Proc_Status(void* lpVoid, MS_BOOL bIsPlayBack)
{

	uc_pvr_service_status_st	*Pvr_Service_Status = NULL;
    UINT8*          pMessage = NULL;
	UINT8 			severity = 0;
	UINT16		    status = 0;
	UINT8			source = 0;

	Pvr_Service_Status = (uc_pvr_service_status_st *)lpVoid;
    pMessage = (UINT8*)Pvr_Service_Status->pMessage;

	switch(*pMessage)
	{
		case 'N':		severity = 0/*N_STR*/;		break;
		case 'F':		severity = 1/*F_STR*/;			break;
		case 'E':		severity = 2/*E_STR*/;		break;
		case 'W':	    severity = 3/*W_STR*/;		break;
		case 'I':		severity = 4/*I_STR*/;			break;
		case 'D':		severity = 5/*D_STR*/;		break;
		default:		break;
	}

	status = *(pMessage+1) - '0';
	status = status*10 + *(pMessage+2) - '0';
	status = status*10 + *(pMessage+3) - '0';

	source = *(pMessage+5) - '0';


	#if 1
	printf("\33[32m[%s][%d][PVR] UC_PVR_RECORD_STATUS ... %s\33[m\n",__func__,__LINE__ ,Pvr_Service_Status->pMessage);
	#endif

    MApp_CCAUI_CasCmd_UpdateServiceItemStatus(Pvr_Service_Status->serviceHandle, pMessage);

	MApp_CCA_SCP_ErrorCodeHandle(pMessage,&severity,&status);
	return 1;

}




#endif  // CCA_UCPVRPROC_C

#endif
