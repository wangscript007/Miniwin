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

#ifndef CCA_STATUSCODEPROC_C
#define CCA_STATUSCODEPROC_C

//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------

#include <ngl_types.h>
#include <string.h>
#include <stdio.h>
#include <CCA_StatusCodeProc.h>
#include <ngl_log.h>
NGL_MODULE(IRDCCACODEPROC);

namespace cca{
//-------------------------------------------------------------------------------------------------
// Local Definitions
//------------------------------------------------------------------------------------------------

#define CELL_PHONE_NUM "+44 14 4940283"
#define ERROR_MSG_LEN 256
#define FIXED_CHAR '\a'   // resver for '\r' or ' '

//-------------------------------------------------------------------------------------------------
// Local Variable
//-------------------------------------------------------------------------------------------------
// Common

INT8 Error_Msg_String[ERROR_MSG_LEN];
uc_client_id ClientIDString;


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


UINT16 MApp_CCA_SCP_AppendTestDescription(INT8 *pu8Buf,UINT8* pu8String)
{
	UINT16 u16StringLen = 0;
	memcpy(pu8Buf,pu8String,strlen((const INT8*)pu8String));
	u16StringLen += strlen((const INT8*)pu8String);
	return u16StringLen;
}




UINT16 MApp_CCA_SCP_AppendClientNum(INT8 *pu8Buf,UINT16 u16BufSize,UINT8 u8StringType)
{



	uc_client_id* pClientIDString;
	UINT32 ClientIDCount,Count,Msglen;
	uc_result ret;
	Msglen = Count = ClientIDCount = 0;
	pClientIDString = &ClientIDString;
	memset(pClientIDString,0,sizeof(uc_client_id));
	
	ret = UniversalClient_Extended_GetClientIDString(pClientIDString);


	if(ret !=  UC_ERROR_SUCCESS)
	{
		NGLOG_DEBUG("get client id string fail ret = %d\n",ret);
		return 0;
	}
	
	
	ClientIDCount = pClientIDString->validOperatorCount;
	memset(pu8Buf,'\0',u16BufSize);

	if(ClientIDCount == 0)
	{
		NGLOG_DEBUG("Client ID does not exist\n");
		return 0;
	}
	
	while(1)
	{
		if(u8StringType == 0)
		{
			snprintf((char*)pu8Buf+Msglen, u16BufSize-Msglen, "Your client number is: <%s>.",pClientIDString->clientID[Count].clientIDString); 
		}
		else if(u8StringType == 1)
		{
			snprintf((char*)pu8Buf+Msglen, u16BufSize-Msglen, "Your receiver number is: <%s>.",pClientIDString->clientID[Count].clientIDString); 			
		}
		else
		{
			snprintf((char*)pu8Buf+Msglen, u16BufSize-Msglen, "Your client number is: <%s>.",pClientIDString->clientID[Count].clientIDString); 
		}
		Msglen = strlen((const INT8*)pu8Buf);
		pu8Buf[Msglen] = FIXED_CHAR;
		Count++;
		ClientIDCount--;

		if(ClientIDCount == 0)
		{
			return Msglen+1; // 1 for ''/r
		}
	}
	
	return 1;


}


UINT16 MApp_CCA_SCP_AppendPhoneNum(INT8 *pu8Buf,UINT16 u16BufSize,UINT8 u8StringType)
{

	memset(pu8Buf,'\0',u16BufSize);

	if(u8StringType == 0)
	{
		snprintf((char*)pu8Buf, u16BufSize, "To update your subscription, call <%s>.",CELL_PHONE_NUM);
		pu8Buf[strlen((const INT8*)pu8Buf)] = FIXED_CHAR;
	}
	else if(u8StringType == 1)
	{
		snprintf((char*)pu8Buf, u16BufSize, "If the problem persists, call <%s>.",CELL_PHONE_NUM);
		pu8Buf[strlen((const INT8*)pu8Buf)] = FIXED_CHAR;

	}
	else if(u8StringType == 2)
	{
		snprintf((char*)pu8Buf, u16BufSize, "To enable your receiver, call <%s>.",CELL_PHONE_NUM);
		pu8Buf[strlen((const INT8*)pu8Buf)] = FIXED_CHAR;

	}	
	else
	{
		snprintf((char*)pu8Buf, u16BufSize, "If the problem persists, call <%s>.",CELL_PHONE_NUM);
		pu8Buf[strlen((const INT8*)pu8Buf)] = FIXED_CHAR;

	}

	return strlen((const INT8*)pu8Buf);

}

UINT16 MApp_CCA_SCP_ErrorCodeHandle(uc_service_status_st *Service_Status,UINT8* severity,UINT16* status)
{

	UINT16 Msg_Str_Len = 0;
	memset(Error_Msg_String,'\0',ERROR_MSG_LEN);
	memcpy(Error_Msg_String,Service_Status->statusMessage,strlen(Service_Status->statusMessage));
	Msg_Str_Len += strlen(Service_Status->statusMessage);	
	//memcpy(Error_Msg_String+Msg_Str_Len," ",1);
	//Msg_Str_Len += 1;	

	if(*severity == 2) //mean the status is "E"
	{
		switch(*status)
		{
			case 16:
			case 17:
			case 18:
				
				memcpy(Error_Msg_String+Msg_Str_Len,"No permission to view this channel.",strlen("No permission to view this channel."));
				Msg_Str_Len += strlen("No permission to view this channel.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				// connect to ui to update the msg status;
			
			break;
			case 30:
			case 31:
			case 32:
			case 33:

				memcpy(Error_Msg_String+Msg_Str_Len,"Currently unable to view this channel.",strlen("Currently unable to view this channel."));
				Msg_Str_Len += strlen("Currently unable to view this channel.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,1);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);

				// connect to ui to update the msg status;
			

			break;
			case 42:
				
				memcpy(Error_Msg_String+Msg_Str_Len,"Parental Control Lock.",strlen("Parental Control Lock."));
				Msg_Str_Len += strlen("Parental Control Lock.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				// connect to ui to update the msg status;
				
			break;
			case 44:

				memcpy(Error_Msg_String+Msg_Str_Len,"No Event Information.",strlen("No Event Information."));
				Msg_Str_Len += strlen("No Event Information.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				// connect to ui to update the msg status;

				
			break;
			case 55:

				memcpy(Error_Msg_String+Msg_Str_Len,"Currently unable to view this channel.",strlen("Currently unable to view this channel."));
				Msg_Str_Len += strlen("Currently unable to view this channel.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			
				// connect to ui to update the msg status;
				
			break;
			case 94:
				
				memcpy(Error_Msg_String+Msg_Str_Len,"Unable to playback the recording.",strlen("Unable to playback the recording."));
				Msg_Str_Len += strlen("Unable to playback the recording.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			
				// connect to ui to update the msg status;
			break;
			case 101:
			case 102:	
			case 103:
			case 106:{
				memcpy(Error_Msg_String+Msg_Str_Len,"Currently unable to view this channel.",strlen("Currently unable to view this channel."));
				Msg_Str_Len += strlen("Currently unable to view this channel.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			
#if 1
				UINT16 c = 0;
			    NGLOG_DEBUG("YJC show string Msg_Str_Len %d\n",Msg_Str_Len);
				for(c = 0;c< Msg_Str_Len ; c++)
				{
					NGLOG_DEBUG("%c",Error_Msg_String[c]);
				}
			    NGLOG_DEBUG("\nYJC show string \n");
#endif
				// connect to ui to update the msg status;
				}	
			break;
			case 111:
			case 128:
			case 131:{				
				memcpy(Error_Msg_String+Msg_Str_Len,"No permission to carry out any recording or playback.",strlen("No permission to carry out any recording or playback."));
				Msg_Str_Len += strlen("No permission to carry out any recording or playback.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			}
			break;
			case 129:
				
				memcpy(Error_Msg_String+Msg_Str_Len,"Unable to playback the recording.",strlen("Unable to playback the recording."));
				Msg_Str_Len += strlen("Unable to playback the recording.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				// connect to ui to update the msg status;				
			break;				
			case 130:

				memcpy(Error_Msg_String+Msg_Str_Len,"Currently unable to carry out any recording.",strlen("Currently unable to carry out any recording."));
				Msg_Str_Len += strlen("Currently unable to carry out any recording.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,1);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			
				// connect to ui to update the msg status;
			
			break;			
			case 139:

				memcpy(Error_Msg_String+Msg_Str_Len,"Validating subscription, please wait.",strlen("Validating subscription, please wait."));
				Msg_Str_Len += strlen("Validating subscription, please wait.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,1);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			
			// connect to ui to update the msg status;

				
			break;	
			case 140:


				memcpy(Error_Msg_String+Msg_Str_Len,"This channel is not available in this location.",strlen("This channel is not available in this location."));
				Msg_Str_Len += strlen("This channel is not available in this location.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,0);
			
			    // connect to ui to update the msg status;
		
			break;
			case 600:

				memcpy(Error_Msg_String+Msg_Str_Len,"Receiver is not yet enabled to view this service",strlen("Receiver is not yet enabled to view this service"));
				Msg_Str_Len += strlen("Receiver is not yet enabled to view this service");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
			
				Msg_Str_Len += MApp_CCA_SCP_AppendPhoneNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,2);
				Msg_Str_Len += MApp_CCA_SCP_AppendClientNum(Error_Msg_String+Msg_Str_Len,ERROR_MSG_LEN-Msg_Str_Len,1);
			
				// connect to ui to update the msg status;

				
			break;	
			case 501:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"FlexiFlash not initialized.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 502:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid CFG file.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 503:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid CCA package data.");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 504:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Cloaked CA package not loaded");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 505:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid Cloaked CA package index");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 506:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Duplicate VM ID");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 507:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Create VM context failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 508:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Create VM failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 509:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Too many VM created");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 510:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"VM load image failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 511:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"VM memory map failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 512:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"VM execution failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;	
			case 513:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"VM IO failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 514:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"VM reset failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 515:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Root key check failed");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 516:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid package signature");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 517:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Unsupported compression algorithm");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 518:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid SYS ID");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;	
			case 519:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Client type mismatch");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 520:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"The client version is too low to support FlexiFlash download");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			case 521:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid Variant");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;	
			case 522:
				
				Msg_Str_Len+=MApp_CCA_SCP_AppendTestDescription(Error_Msg_String+Msg_Str_Len,(UINT8*)"Invalid White Box algorithm");
				Error_Msg_String[Msg_Str_Len] = FIXED_CHAR;
				Msg_Str_Len++;
				// connect to ui to update the msg status;	
				
			break;
			default:
			NGLOG_DEBUG("[%s][%d] not support error status\n",__func__,__LINE__);
				return 0;
				break;	

		}
	}

    
    if((*severity == 2) || (*severity == 5))
    {
		extern BOOL MApp_CCAUI_ShowErrorMsg(UINT8* pu8Data);
		UINT8 u8Data = 0;
		u8Data = 0;
		//MApp_CCAUI_ShowErrorMsg(&u8Data); -d by MX tmp 
    }



	

#if 0
	NGLOG_DEBUG("\nYJC nstatus string\n");
	NGLOG_DEBUG("%s",Error_Msg_String);
	NGLOG_DEBUG("\nYJC status string\n");
#endif
	
	return 1;
}


void MApp_CCAUI_CasCmd_GetStatusTextString(INT8* pu8Buf)
{
    strncpy((INT8*)pu8Buf, (INT8*)Error_Msg_String, strlen((const INT8*)Error_Msg_String) + 1); // Including the null char
    NGLOG_DEBUG("Status Text String = %s. Length = %d\n", pu8Buf, strlen((const INT8*)pu8Buf));
}

//Status code mapping for service status page
UINT16 MApp_CCA_SCP_StatusCodeHandle(uc_service_status_st *Service_Status,UINT8* severity,UINT16* status, INT8* pu8Buf)
{

	UINT16 Msg_Str_Len = 0;
    INT8 *Status_Msg_String = pu8Buf;

	memcpy(Status_Msg_String,Service_Status->statusMessage,strlen(Service_Status->statusMessage));
	Msg_Str_Len += strlen(Service_Status->statusMessage);	

	if(*severity == 2) //mean the status is "E"
	{
		switch(*status)
		{
			case 16:
                memcpy(Status_Msg_String+Msg_Str_Len,"No valid entitlement found",strlen("No valid entitlement found"));
				break;
				
			case 17:
                memcpy(Status_Msg_String+Msg_Str_Len,"No valid sector found",strlen("No valid sector found"));
				break;

            case 18:
                memcpy(Status_Msg_String+Msg_Str_Len,"Product blackout",strlen("Product blackout"));
				break;

            case 30:
                memcpy(Status_Msg_String+Msg_Str_Len,"P-Key mismatch",strlen("P-Key mismatch"));
				break;

            case 31:
                memcpy(Status_Msg_String+Msg_Str_Len,"G-Key mismatch",strlen("G-Key mismatch"));
				break;

            case 32:
                memcpy(Status_Msg_String+Msg_Str_Len,"TG mismatch",strlen("TG mismatch"));
				break;

            case 33:
                memcpy(Status_Msg_String+Msg_Str_Len,"CWDK mismatch",strlen("CWDK mismatch"));
				break;

            case 55:
                memcpy(Status_Msg_String+Msg_Str_Len,"Macrovision blocking enforced",strlen("Macrovision blocking enforced"));
				break;

            case 94:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid PVR metadata",strlen("Invalid PVR metadata"));
				break;

            case 101:
                memcpy(Status_Msg_String+Msg_Str_Len,"Incomplete Definition",strlen("Incomplete Definition"));
				break;

            case 102:
                memcpy(Status_Msg_String+Msg_Str_Len,"No ECM/EMM data received",strlen("No ECM/EMM data received"));
				break;

            case 103:
                memcpy(Status_Msg_String+Msg_Str_Len,"No valid ECM found",strlen("No valid ECM found"));
				break;

            case 104:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid EMM received",strlen("Invalid EMM received"));
				break;

            case 106:
                memcpy(Status_Msg_String+Msg_Str_Len,"No matching CA System ID in PMT",strlen("No matching CA System ID in PMT"));
				break;

            case 111:
                memcpy(Status_Msg_String+Msg_Str_Len,"No PVR Session Key to perform PVR operation",strlen("No PVR Session Key to perform PVR operation"));
				break;

            case 128:
                memcpy(Status_Msg_String+Msg_Str_Len,"Not entitled to perform PVR operation",strlen("Not entitled to perform PVR operation"));
				break;

            case 129:
                memcpy(Status_Msg_String+Msg_Str_Len,"The PVR content is expired",strlen("The PVR content is expired"));
				break;

            case 130:
                memcpy(Status_Msg_String+Msg_Str_Len,"PVR Record is not allowed",strlen("PVR Record is not allowed"));
				break;

            case 131:
                memcpy(Status_Msg_String+Msg_Str_Len,"No PVR Master Session Key to perform PVR operation",strlen("No PVR Master Session Key to perform PVR operation"));
				break;

            case 139:
                memcpy(Status_Msg_String+Msg_Str_Len,"No valid CA Regional Information found",strlen("No valid CA Regional Information found"));
				break;

            case 140:
                memcpy(Status_Msg_String+Msg_Str_Len,"Region or Sub-Region mismatch",strlen("Region or Sub-Region mismatch"));
				break;

            case 150:
                memcpy(Status_Msg_String+Msg_Str_Len,"The Pre-Enablement product is expired",strlen("The Pre-Enablement product is expired"));
				break;

            case 151:
                memcpy(Status_Msg_String+Msg_Str_Len,"PESK mismatch",strlen("PESK mismatch"));
				break;

            case 600:
                memcpy(Status_Msg_String+Msg_Str_Len,"Initialization has not been performed yet",strlen("Initialization has not been performed yet"));
				break;

            default:
                break;
		}
	}
    else if(*severity == 4) //mean the status is "I"
    {
        switch(*status)
		{
            case 101:
                memcpy(Status_Msg_String+Msg_Str_Len,"Incomplete Definition",strlen("Incomplete Definition"));
				break;
                
			case 102:
                memcpy(Status_Msg_String+Msg_Str_Len,"No ECM/EMM data received",strlen("No ECM/EMM data received"));
				break;

            case 300:
                memcpy(Status_Msg_String+Msg_Str_Len,"New CG for FSU product has been received",strlen("New CG for FSU product has been received"));
				break;

            case 301:
                memcpy(Status_Msg_String+Msg_Str_Len,"New CG for Push VOD product has been received",strlen("New CG for Push VOD product has been received"));
				break;

            case 302:
                memcpy(Status_Msg_String+Msg_Str_Len,"CG Mismatch",strlen("CG Mismatch"));
				break;

            case 303:
                memcpy(Status_Msg_String+Msg_Str_Len,"SG Mismatch",strlen("SG Mismatch"));
				break;

            case 304:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid P-Key Index",strlen("Invalid P-Key Index"));
				break;

            case 305:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid G-Key Index",strlen("Invalid G-Key Index"));
				break;

            case 306:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid Timestamp received",strlen("Invalid Timestamp received"));
				break;

            case 307:
                memcpy(Status_Msg_String+Msg_Str_Len,"Variant Mismatch",strlen("Variant Mismatch"));
				break;

            case 308:
                memcpy(Status_Msg_String+Msg_Str_Len,"P-Key Hash Mismatch",strlen("P-Key Hash Mismatch"));
				break;

            case 309:
                memcpy(Status_Msg_String+Msg_Str_Len,"Opcode Restricted",strlen("Opcode Restricted"));
				break;

            case 310:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid VOD Nonce",strlen("Invalid VOD Nonce"));
				break;

            case 311:
                memcpy(Status_Msg_String+Msg_Str_Len,"TMS Failed",strlen("TMS Failed"));
				break;

            case 312:
                memcpy(Status_Msg_String+Msg_Str_Len,"Homing Channel Failed",strlen("Homing Channel Failed"));
				break;

            case 313:
                memcpy(Status_Msg_String+Msg_Str_Len,"Invalid Middleware User Data",strlen("Invalid Middleware User Data"));
				break;

            case 314:
                memcpy(Status_Msg_String+Msg_Str_Len,"Flexi Flash Failed",strlen("Flexi Flash Failed"));
				break;

            case 315:
                memcpy(Status_Msg_String+Msg_Str_Len,"SN Mapping EMM handled successfully",strlen("SN Mapping EMM handled successfully"));
				break;

            case 316:
                memcpy(Status_Msg_String+Msg_Str_Len,"No valid entitlement found",strlen("No valid entitlement found"));
				break;

            case 317:
                memcpy(Status_Msg_String+Msg_Str_Len,"No valid sector found",strlen("No valid sector found"));
				break;

            case 318:
                memcpy(Status_Msg_String+Msg_Str_Len,"Sector Overwrite EMM handled successfully",strlen("Sector Overwrite EMM handled successfully"));
				break;    

            case 319:
                memcpy(Status_Msg_String+Msg_Str_Len,"Pairing EMM handled successfully",strlen("Pairing EMM handled successfully"));
				break;   

            case 320:
                memcpy(Status_Msg_String+Msg_Str_Len,"Product Key EMM handled successfully",strlen("Product Key EMM handled successfully"));
				break;   

            case 321:
                memcpy(Status_Msg_String+Msg_Str_Len,"Product Overwrite EMM handled successfully",strlen("Product Overwrite EMM handled successfully"));
				break;

            case 322:
                memcpy(Status_Msg_String+Msg_Str_Len,"Region Control EMM handled successfully",strlen("Region Control EMM handled successfully"));
				break;

            case 323:
                memcpy(Status_Msg_String+Msg_Str_Len,"PVR MSK EMM handled successfully",strlen("PVR MSK EMM handled successfully"));
				break;

            case 324:
                memcpy(Status_Msg_String+Msg_Str_Len,"VOD Product Overwrite EMM handled successfully",strlen("VOD Product Overwrite EMM handled successfully"));
				break;

            case 325:
                memcpy(Status_Msg_String+Msg_Str_Len,"VOD Asset ID EMM handled successfully",strlen("VOD Asset ID EMM handled successfully"));
				break;

            case 326:
                memcpy(Status_Msg_String+Msg_Str_Len,"Nationality EMM handled successfully",strlen("Nationality EMM handled successfully"));
				break;

            case 327:
                memcpy(Status_Msg_String+Msg_Str_Len,"Product Delete EMM handled successfully",strlen("Product Delete EMM handled successfully"));
				break;

            case 328:
                memcpy(Status_Msg_String+Msg_Str_Len,"Entitlement Property EMM handled successfully",strlen("Entitlement Property EMM handled successfully"));
				break;

            case 329:
                memcpy(Status_Msg_String+Msg_Str_Len,"Timestamp EMM handled successfully",strlen("Timestamp EMM handled successfully"));
				break;

            case 330:
                memcpy(Status_Msg_String+Msg_Str_Len,"P-Key mismatch",strlen("P-Key mismatch"));
				break;

            case 331:
                memcpy(Status_Msg_String+Msg_Str_Len,"User Area EMM handled successfully",strlen("User Area EMM handled successfully"));
				break;

            case 332:
                memcpy(Status_Msg_String+Msg_Str_Len,"Application Data EMM handled successfully",strlen("Application Data EMM handled successfully"));
				break;

            case 333:
                memcpy(Status_Msg_String+Msg_Str_Len,"Filter Criteria EMM handled successfully",strlen("Filter Criteria EMM handled successfully"));
				break;

            case 334:
                memcpy(Status_Msg_String+Msg_Str_Len,"Package EMM handled successfully",strlen("Package EMM handled successfully"));
				break;

            case 335:
                memcpy(Status_Msg_String+Msg_Str_Len,"Block Download EMM handled successfully.",strlen("Block Download EMM handled successfully."));
				break;

            case 336:
                memcpy(Status_Msg_String+Msg_Str_Len,"IRD EMM handled successfully",strlen("IRD EMM handled successfully"));
				break;

            case 337:
                memcpy(Status_Msg_String+Msg_Str_Len,"Unique SN Mapping EMM handled successfully",strlen("Unique SN Mapping EMM handled successfully"));
				break;

            case 338:
                memcpy(Status_Msg_String+Msg_Str_Len,"Signed CCP CAM EMM handled successfully",strlen("Signed CCP CAM EMM handled successfully"));
				break;

            case 339:
                memcpy(Status_Msg_String+Msg_Str_Len,"TM Message EMM handled successfully",strlen("TM Message EMM handled successfully"));
				break;

            case 340:
                memcpy(Status_Msg_String+Msg_Str_Len,"Macrovision Configuration EMM handled successfully",strlen("Macrovision Configuration EMM handled successfully"));
				break;

            case 341:
                memcpy(Status_Msg_String+Msg_Str_Len,"Extended TMS Message EMM handled successfully",strlen("Extended TMS Message EMM handled successfully"));
				break;

            case 342:
                memcpy(Status_Msg_String+Msg_Str_Len,"Reset To Factory State EMM handled successfully",strlen("Reset To Factory State EMM handled successfully"));
				break;
                
            default:
                break;
		}
    }
    else if(*severity == 5) //mean the status is "D"
    {
        switch(*status)
		{
			case 29:
                memcpy(Status_Msg_String+Msg_Str_Len,"Service is currently descrambled",strlen("Service is currently descrambled"));
				break;

            case 100:
                memcpy(Status_Msg_String+Msg_Str_Len,"EMM service OK",strlen("EMM service OK"));
				break;

            case 101:
                memcpy(Status_Msg_String+Msg_Str_Len,"No matching CA System ID in CAT",strlen("No matching CA System ID in CAT"));
				break;

            case 126:
                memcpy(Status_Msg_String+Msg_Str_Len,"PVR Record Request OK",strlen("PVR Record Request OK"));
				break;

            case 127:
                memcpy(Status_Msg_String+Msg_Str_Len,"PVR Playback Request OK",strlen("PVR Playback Request OK"));
				break;

            default:
                break;
		}
    }


    return 1;
}
}
#endif  // CCA_STATUSCODEPROC_C
