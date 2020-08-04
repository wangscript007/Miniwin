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
/// file    MApp_ZUI_ACTSCCAUIControl.c
/// @brief  functions to handle S3 IRD commands for UI control
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MAPP_ZUI_ACTCCAUICONTROL_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ntvwindow.h>
#include "appCCADecoderMsgDecControl.h"
#include "appCCADecoderMsgTextMsg.h"

#include "MApp_ZUI_ACTCCAIrd.h"
#include "MApp_ZUI_ACTCCAUIControl.h"
#include "MApp_ZUI_ACTCCAUICmd.h"
#include "MApp_ZUI_ACTCCAIrd.h"
#include "MApp_ZUI_ACTCCAMailInfo.h"
#include "MApp_ZUI_ACTCCAServiceStatus.h"
#include "MApp_ZUI_ACTCCACaStatus.h"

NGL_MODULE (CCAUICONTROL);


/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/


#define ERROR_CHECK_TIME         500
#define PVR_ERROR_DEL_CHECK_TIME 3000

#define ATTRIBUTE_POINT_SIZE     20
#define ATTRIBUTE_MAX_SIZE       63
#define FINGER_PRINTF_H_SIZE     28
#define FINGER_PRINTF_MAX_FONT   11
#define OSD_GWIN_W_SIZE          720
#define OSD_GWIN_H_SIZE_NTSC     480
#define OSD_GWIN_H_SIZE_PAL      576
#define ATTRIBUTE_OSD_MAX_W_SIZE 620//1180
#define ATTRIBUTE_OSD_MAX_H_SIZE 380//620
#define ATTRIBUTE_OSD_START_W    ((OSD_GWIN_W_SIZE - ATTRIBUTE_OSD_MAX_W_SIZE) / 2)
#define ATTRIBUTE_OSD_START_H    ((OSD_GWIN_H_SIZE - ATTRIBUTE_OSD_MAX_H_SIZE) / 2)
#define FINGERPRINTF_W_SIZE_OVER 21
#define FINGERPRINTF_W_SIZE_GAP  3
#define TMS_W_SIZE_OVER          13
#define TMS_W_SIZE_GAP           8

#define ERROR_MSG_TIME          10 // Seconds, FIXME
#define FORCED_MSG_TIME         20 // Seconds, FIXME

#define POS_X                   0x01
#define POS_Y                   0x02


#define X_MANUAL_TUNE 20
#define Y_MANUAL_TUNE 0 


#define OVERT_WIDTH_FACTOR  10

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/
//#if (CA_SOFTCELL_RUNNING_STATUS == 1)  
//extern void  MApp_ZUI_SyncUI(void);
//#endif

extern void MApp_CCAUI_CasCmd_GetStatusTextString(INT8* pu8Buf);


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/


static BOOL gbCheckMailIcon     = FALSE;
static BOOL gbCheckAnncmtIcon   = FALSE;


/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/

/********************************************************************************/
/*                            Local Function                                */
/********************************************************************************/





///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_CCAUI_GetOSDMaxHeight
///  OSD Max Height
///
///  @param [in]   none
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

static UINT16 _MApp_CCAUI_GetOSDMaxHeight(void)
{
    VE_DrvStatus    DrvStatus;

    MDrv_VE_GetStatus(&DrvStatus);
    if(DrvStatus.VideoSystem <= 3)
    {
        return (OSD_GWIN_H_SIZE_NTSC);
    }
    else
    {
        return (OSD_GWIN_H_SIZE_PAL);
    }
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_CCAUI_FingerprintRandomPosition
///  Fingerprint Random Position
///
///  @param [in]   u8PosIndex
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

static UINT16 _MApp_CCAUI_FingerprintRandomPosition(UINT8 u8PosIndex)
{
    UINT32 u16Postion;

    if (u8PosIndex == POS_X)
    {
        u16Postion = (MApp_ZUI_ACT_GetRandomNum() % ATTRIBUTE_OSD_MAX_W_SIZE) + 1;
        if(u16Postion > ATTRIBUTE_OSD_MAX_W_SIZE)
        {
            u16Postion = ATTRIBUTE_OSD_MAX_W_SIZE;
        }

    }
    else//POS_Y
    {
        u16Postion = (MApp_ZUI_ACT_GetRandomNum() % ATTRIBUTE_OSD_MAX_W_SIZE) + 1;
        if(u16Postion > ATTRIBUTE_OSD_MAX_H_SIZE)
        {
            u16Postion = ATTRIBUTE_OSD_MAX_H_SIZE;
        }
    }

    NGLOG_DEBUG("\nPosition %ld\n", u16Postion);

    return u16Postion;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_Rescan
///  Rescan the channel
///
///  @param [in]   u32Timout in unit of 100 msec
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

// FIXME, this API has need to hook up with UI control
BOOL MApp_CCAUI_Rescan(UINT32 u32Timout)
{
    BOOL bResult    = FALSE;
    UINT32 s16TimeMsec = 0;
    E_OSD_ID eOsdId    = MApp_ZUI_GetActiveOSD();

    // FIXME: check the active OSD
    while (s16TimeMsec < (u32Timout/3))
    {
        MsOS_DelayTask(100);

        s16TimeMsec = s16TimeMsec + 100;

        eOsdId = MApp_ZUI_GetActiveOSD();
		
		
        //if (eOsdId == E_OSD_MANUAL_SCAN)
        if (eOsdId == E_OSD_SCANNING)
        {
            NGLOG_DEBUG("[DecCtrl] Switched to Scan Menu OSD %d\n", eOsdId);
            break;
        }
    }

    if (s16TimeMsec > u32Timout)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to active manu scan menu\n");
        bResult = FALSE;
    }
    else
    {
		
        MApp_ZUI_ACT_HandleKey_MenuManualScan(VK_SELECT); // Trigger Scanning OSD for percentage bar

        // FIXME: check the active OSD
        s16TimeMsec = 0;
        while (s16TimeMsec < u32Timout)
        {
            MsOS_DelayTask(5000);
            s16TimeMsec = s16TimeMsec + 100;

            eOsdId = MApp_ZUI_GetActiveOSD();
					
            //if (eOsdId == E_OSD_SCANNING)
			if (eOsdId == E_OSD_TV_PROG)
            {
                NGLOG_DEBUG("[DecCtrl] Switched to Scanning OSD %d\n", eOsdId);
                break;
            }
        }

        if (s16TimeMsec > u32Timout)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to active scanning OSD\n");
            bResult = FALSE;
        }
        else
        {
            bResult = TRUE;
        }
    }

    return bResult;
}



#if 0 // keep but not use now
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowPinInputBox
///  Show pin input box for change the pin code
///
///  @param [in]   u16PinCode
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

// FIXME, this API has need to hook up with UI control
BOOL MApp_CCAUI_ShowPinInputBox(UINT16 u16PinCode)
{
    BOOL bHandled = FALSE;

    bHandled = MApp_ZUI_ACT_ShowS3IrdPIN();

    return bHandled;
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_ChangeParentalPin
///  Change pin code
///
///  @param [in]   u16PinCode
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

// FIXME, this API has need to hook up with UI control
BOOL MApp_S3UI_ChangeParentalPin(UINT32 u32Pwd)
{
    MApp_SetPWD(u32Pwd);

    return TRUE;
}
#endif



//////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowTMS
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

// FIXME, this API has need to hook up with UI control
BOOL MApp_CCAUI_ShowTMS(UINT8* pu8Data)
{
    stIrdTextMsg* pstMsg      = NULL;
    UINT16        u16Index    = ((UINT16)pu8Data[1] << 8) | pu8Data[2];
    UINT32        u32Duration = 0;

    //BOOL       bBanner        = (pu8Data[5] >> 1) & 0x1;      // Get the display method - 0, normal; 1, banner
    UINT8         u8Coverage = (pu8Data[5] >> 2) & 0x3f;
    UINT8         u8Percent  = 0;
    //FIXME
    stCCAUiDisplayAttribute stAttribute;
    EN_CCA_ERROR_TYPE eErrType = E_CCA_ERROR_TYPE_GENERAL_ERROR; //FIXME

    memset(&stAttribute, 0x0, sizeof(stAttribute));

    u32Duration = ((UINT16)pu8Data[3] << 8) | pu8Data[4];
    stAttribute.bFlashing   = (pu8Data[5] >> 0) & 0x1;
    u8Percent    = MApp_CCAUI_Cmd_GetCoveragPercentage(u8Coverage);
    stAttribute.u16PosX      = ((ATTRIBUTE_OSD_MAX_W_SIZE * (100 - u8Percent)) / 100 / 2);
    stAttribute.u16PosY      = ((ATTRIBUTE_OSD_MAX_H_SIZE * (100 - u8Percent)) / 100 / 2);
    stAttribute.u16Width     = OSD_GWIN_W_SIZE * u8Percent / 100;
    stAttribute.u16Height    = OSD_GWIN_H_SIZE_NTSC * u8Percent / 100;

    NGLOG_DEBUG("Text msg type = %d\n", pu8Data[0]);
    NGLOG_DEBUG("Duration = %d\n", u32Duration);
    NGLOG_DEBUG("Flashing = %d\n", stAttribute.bFlashing);
    NGLOG_DEBUG("u8Coverage = %d\n", u8Coverage);

    // Check the text msg type
    if (pu8Data[0] == E_CCA_TEXT_MSG_CLASS_NORMAL)
    {
        pstMsg = MApp_DecoderMsg_GetMailMsg(u16Index);
        u32Duration = 0;
    }
    else if (pu8Data[0] == E_CCA_TEXT_MSG_CLASS_FORCE)
    {
        pstMsg = MApp_DecoderMsg_GetMailMsg(u16Index);
    }
    else if (pu8Data[0] == E_CCA_TEXT_MSG_CLASS_TIMED)
    {
        pstMsg = MApp_DecoderMsg_GetAnncmentMsg(u16Index);
    }
    else
    {
        NGLOG_DEBUG("Beyond the range\n");
        return FALSE;
    }

    stAttribute.u32Duration = u32Duration;

    if (sizeof(stAttribute.au8StrBuf) >= sizeof(pstMsg->au8MsgByte))
    {
        memcpy(stAttribute.au8StrBuf, pstMsg->au8MsgByte, sizeof(pstMsg->au8MsgByte));
    }
    else
    {
        memcpy(stAttribute.au8StrBuf, pstMsg->au8MsgByte, sizeof(stAttribute.au8StrBuf));
    }

    MApp_CCAUI_Cmd_ShowTmsMsg(&stAttribute, eErrType, CCAIRD_POP_TYPE_TMS);

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowForcedMsg
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

// FIXME, this API has need to hook up with UI control
BOOL MApp_CCAUI_ShowForcedMsg(UINT8* pu8Data)
{
    stIrdTextMsg* pstMsg   = NULL;
    UINT16        u16Index = ((UINT16)pu8Data[1] << 8) | pu8Data[2];

    if (MApp_CCAUI_Cmd_GetSystemOsdCheck() == FALSE)  // FIXME, consider TV banner, PVR, time shift modes.
    {
        return FALSE;
    }

    NGLOG_DEBUG("Text msg type = %d\n", pu8Data[0]);
    NGLOG_DEBUG("Text msg %d, %d\n", pu8Data[1], pu8Data[2]);

    // Check the text msg type
    if (pu8Data[0] == E_CCA_TEXT_MSG_TYPE_MAILBOX)
    {
        pstMsg = MApp_DecoderMsg_GetMailMsg(u16Index);
    }
    else if (pu8Data[0] == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
    {
        pstMsg = MApp_DecoderMsg_GetAnncmentMsg(u16Index);
    }
    else
    {
        NGLOG_DEBUG("Beyond the range\n");
        return FALSE;
    }

    // FIXME
    if (pstMsg->u16Length > 0)
    {
        stCCAUiDisplayAttribute stAttribute;

        memset(&stAttribute, 0x0, sizeof(stAttribute));
        //FIXME
        stAttribute.u16PosX   = (ATTRIBUTE_OSD_MAX_W_SIZE * 30 / 100);
        stAttribute.u16PosY   = (ATTRIBUTE_OSD_MAX_H_SIZE * 40 / 100);
        stAttribute.u16Width  = (OSD_GWIN_W_SIZE + (1 * stAttribute.u16PosX));
        stAttribute.u16Height = (OSD_GWIN_H_SIZE_NTSC - (1 * stAttribute.u16PosY));

        stAttribute.bFlashing   = FALSE;
        stAttribute.u32Duration = FORCED_MSG_TIME;

        if (sizeof(stAttribute.au8StrBuf) >= sizeof(pstMsg->au8MsgByte))
        {
            memcpy(stAttribute.au8StrBuf, pstMsg->au8MsgByte, sizeof(pstMsg->au8MsgByte));
        }
        else
        {
            memcpy(stAttribute.au8StrBuf, pstMsg->au8MsgByte, sizeof(stAttribute.au8StrBuf));
        }
           MApp_CCAUI_Cmd_DrawForcedMsg(&stAttribute);
    }
    else
    {
      // temp remark      MApp_ZUI_ACT_Hide();
    }

    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowErrorMsg
///  Show the error message
///
///  @param [in]   u16Index
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_CCAUI_ShowErrorMsg(UINT8* pu8Data)
{
    stCCAUiDisplayAttribute stAttribute;
    EN_CCA_ERROR_TYPE eErrType = E_CCA_ERROR_TYPE_GENERAL_ERROR; //FIXME
    UINT16 popMsgType = CCAIRD_POP_TYPE_NONE;

    if (MApp_CCAUI_Cmd_GetSystemOsdCheck() == FALSE)  // FIXME, consider TV banner, PVR, time shift modes.
    {
          return FALSE;
    }

    memset(&stAttribute, 0x0, sizeof(stAttribute));
    eErrType = pu8Data[0]; // Error type
    //FIXME
    stAttribute.u16PosX   = (ATTRIBUTE_OSD_MAX_W_SIZE * 30 / 100) - X_MANUAL_TUNE;
    stAttribute.u16PosY   = (ATTRIBUTE_OSD_MAX_H_SIZE * 40 / 100) - Y_MANUAL_TUNE;
    stAttribute.u16Width  = (OSD_GWIN_W_SIZE + (1 * stAttribute.u16PosX ));
    stAttribute.u16Height = (OSD_GWIN_H_SIZE_NTSC - (1 * stAttribute.u16PosY));
    stAttribute.bFlashing = FALSE;

    NGLOG_DEBUG("Enter MApp_CCAUI_ShowErrorMsg\n");



// temp remark #if(CA_SOFTCELL_PVR_DRM == 1)
// temp remark
// temp remark
// temp remark
// temp remark
// temp remark	if(MApp_ZUI_GetActiveOSD() == E_OSD_MENU_MEDIA_PVR_PLAY)
// temp remark	{
// temp remark		CA_Task_PVR_Get_DRM_Status(stAttribute.au8StrBuf,254);
// temp remark	}
// temp remark	else
// temp remark#endif

   {
        MApp_CCAUI_CasCmd_GetStatusTextString(stAttribute.au8StrBuf);
   }


// temp remark#if(CA_SOFTCELL_PVR_DRM == 1)
// temp remark	//not show the e101-36
// temp remark	if((stAttribute.au8StrBuf[0] == 'E') && (stAttribute.au8StrBuf[1] == '1') && (stAttribute.au8StrBuf[2] == '0') && (stAttribute.au8StrBuf[3] == '1'))
// temp remark	{
// temp remark		return FALSE;
// temp remark	}
// temp remark#endif



    stAttribute.u32Duration = ERROR_MSG_TIME;

	if(stAttribute.au8StrBuf[0] == 'E')
	{
		popMsgType = CCAIRD_POP_TYPE_ERR_MSG_E;
	}
	else if(stAttribute.au8StrBuf[0] == 'D')
	{
		popMsgType = CCAIRD_POP_TYPE_ERR_MSG_D;
	}
//	else if(stAttribute.au8StrBuf[0] == 'I')
//	{
//		popMsgType = CCAIRD_POP_TYPE_ERR_MSG_I;
//	}
	else
	{
		return TRUE;
	}

    MApp_CCAUI_Cmd_ShowTmsMsg(&stAttribute, eErrType, popMsgType);

// temp remark #if (CA_SOFTCELL_RUNNING_STATUS == 1)    
// temp remark    //msg of running status need show imediately
// temp remark    MApp_ZUI_SyncUI();
// temp remark #endif

    return TRUE;
}





///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowFingerprint
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_ShowFingerprint(UINT8 *pu8Data)
{
    int i = 0;
    stCCAUiDisplayAttribute  stAttribute;
    UINT32                  u32Duration  = 0;
    UINT8                   u8Coverage   = 0;
    UINT8                   u8Percent   = 0;
    UINT16                  u16Length    = 0;

    memset(&stAttribute, 0x0, sizeof(stAttribute));

    u32Duration = ((UINT16)pu8Data[0] << 8) & 0xFF00;
    u32Duration = u32Duration | (((UINT16)pu8Data[1]) & 0xFF);

    NGLOG_DEBUG("\nDURATION %d\n", u32Duration);

    stAttribute.bFlashing = (pu8Data[2] >> 0) & 0x1;
    stAttribute.bBanner   = (pu8Data[2] >> 1) & 0x1;      // Get the display method - 0, normal; 1, banner

    u8Coverage = (pu8Data[2] >> 2) & 0x3F;

    u16Length   = ((UINT16)pu8Data[3] << 8) & 0x0F00;
    u16Length   = u16Length | (((UINT16)pu8Data[4]) & 0xFF);

    NGLOG_DEBUG("\nLENGTH %d\n", u16Length);

    pu8Data += 5;
    for (i = 0; i < u16Length; i++)
    {
        stAttribute.au8StrBuf[i] = pu8Data[i];
    }

    u8Percent = MApp_CCAUI_Cmd_GetCoveragPercentage(u8Coverage);

    NGLOG_DEBUG("\nPERCENTAAGE %d\n", u8Percent);

    stAttribute.u16Width  = ((OSD_GWIN_W_SIZE * u8Percent) / 100);
    stAttribute.u16Height = (stAttribute.u16Width/OVERT_WIDTH_FACTOR)*2;//((_MApp_CCAUI_GetOSDMaxHeight() * u8Percent) / 100);
    stAttribute.u16PosX   = _MApp_CCAUI_FingerprintRandomPosition( POS_X ); //X
    stAttribute.u16PosY   = _MApp_CCAUI_FingerprintRandomPosition( POS_Y ); //Y

    NGLOG_DEBUG("\n x:[%d], y:[%d], w:[%d], h:[%d]\n", stAttribute.u16PosX , stAttribute.u16PosY,
                                                          stAttribute.u16Width, stAttribute.u16Height);
    if(u8Percent == 100)
    {
        stAttribute.u16PosX    = stAttribute.u16PosY = 0;
    }
    else//boundary checking
    {
        if(OSD_GWIN_W_SIZE < (stAttribute.u16PosX + stAttribute.u16Width))
        {
            stAttribute.u16PosX = (stAttribute.u16PosX)/3;
        }

        if(_MApp_CCAUI_GetOSDMaxHeight() < (stAttribute.u16PosY + stAttribute.u16Height))
        {
            stAttribute.u16PosY = (stAttribute.u16PosY)/3;
        }

        //avoid character being hidden by left border
        if(stAttribute.u16PosX < 10)
        {
            stAttribute.u16PosX = 10;
        }
    }

    stAttribute.u32Duration = u32Duration;
    stAttribute.u8Percent   = u8Percent;

    MApp_CCAUI_Cmd_ShowFingerprint(&stAttribute);
}





#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowFingerprintingOption
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_StoreFingerprintingOption(UINT8 *pu8Data)
{
    stCCAUiDisplayAttribute stAttribute;
    UINT32 u32Duration = 0;
    UINT8  u8Coverage  = 0;
    UINT8  u8Percent   = 0;

    memset(&stAttribute, 0x0, sizeof(stAttribute));
    u32Duration = ((UINT16)pu8Data[0] << 8) & 0xFF00;
    u32Duration = u32Duration | (((UINT16)pu8Data[1]) & 0xFF);

    NGLOG_DEBUG("\nDURATION %d\n", u32Duration);

    stAttribute.bFlashing =  (pu8Data[2] >> 0) & 0x1;

    NGLOG_DEBUG("\nFLASHING %d\n", stAttribute.bFlashing);

    stAttribute.bBanner   = (pu8Data[2] >> 1) & 0x1;      // Get the display method - 0, normal; 1, banner

    u8Coverage = (pu8Data[2] >> 2) & 0x3F;

    NGLOG_DEBUG("\nCOVERAGE %d\n", u8Coverage);

    u8Percent = MApp_CCAUI_Cmd_GetCoveragPercentage(u8Coverage);

    stAttribute.u16Width  = ((OSD_GWIN_W_SIZE * u8Percent) / 100);
    stAttribute.u16Height = ((_MApp_CCAUI_GetOSDMaxHeight() * u8Percent) / 100);
    stAttribute.u16PosX   = _MApp_CCAUI_FingerprintRandomPosition( POS_X ); //X
    stAttribute.u16PosY   = _MApp_CCAUI_FingerprintRandomPosition( POS_Y ); //Y

    NGLOG_DEBUG("\n x:[%d], y:[%d], w:[%d], h:[%d]\n", stAttribute.u16PosX , stAttribute.u16PosY,
                                                          stAttribute.u16Width, stAttribute.u16Height);

    stAttribute.u32Duration = u32Duration;
    stAttribute.u8Percent   = u8Percent;

    stIrdFPOptionsTLV* pstFpoTlv;
    NGLOG_DEBUG("\nTag %d\n", pu8Data[3]);
    pstFpoTlv = MApp_DecoderMsg_AttributedDisplay_GetFPTLV((*pu8Data));
    MApp_CCAUI_Cmd_StoreEnhancedOvertFingerprint(pstFpoTlv);
    MApp_CCAUI_Cmd_StoreFingerprintingOption(&stAttribute);
}
#endif




///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_OTA_PopupEvent
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

//FIXME: Stream syntax and TMS/Mail data arrangement
BOOL MApp_CCAUI_OTA_PopupEvent(UINT8 *pu8Data)
{
    UINT8 u8Type = 0;

    if (pu8Data[0] == 0x01)
    {
        if (pu8Data[2] >= 1)
        {
            u8Type = (pu8Data[2] - 1);
        }
        else
        {
            NGLOG_DEBUG("\n Wrong text type %d\n", pu8Data[2]);
        }

        NGLOG_DEBUG("\nKey event button %x  Text type %d\n", pu8Data[1], u8Type);   

        if (pu8Data[1] == 0x01)
        {
            switch(u8Type)
            {
                case E_POP_TEXT_MENU_SELECT_BTN:
                {
//temp remark                    MApp_S3UI_CasCmd_OtaPopupBox((E_POP_TEXT_GET_NIT + 1), 2);
                    break;
                }
                case E_POP_TEXT_MENU_TUNE2CONNECT_TIMEOUT:
                case E_POP_TEXT_MENU_PROCESS_TIMEOUT:
                {
                    MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);
                    break;
                }
                case E_POP_TEXT_MENU_DOWNLOAD_MSG:
                {
                    // FIXME
                    // ucIRD_ProcessOTAFlag(TRUE);
                    break;
                }
                default:
                    NGLOG_DEBUG("\nWrong type\n");
                    return FALSE;

            }
        }
        else if (pu8Data[1] == 0x02)
        {
            if (u8Type == E_POP_TEXT_MENU_SELECT_BTN)
            {
                printf("\n exit key error\n");
            }
            else if (u8Type == E_POP_TEXT_MENU_DOWNLOAD_MSG)
            {
                // FIXME
                // extern void ucIRD_RestoreLoaderpt(void);
                MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);

                // FIXME
                // ucIRD_RestoreLoaderpt();

            //simon 2012.01.29 - remove this
            #if 0    
                SI_ParseParm SI_Parm;
                memset(&SI_Parm, 0x0, sizeof(SI_ParseParm));
                MApi_SI_Parser_Monitor_Start(0, &SI_Parm);
            #endif
            }
        }

    }
    else if (pu8Data[0] == 0x02)
    {
        if (pu8Data[1] >= 1)
        {
            u8Type = (pu8Data[1] - 1);
        }
        else
        {
            NGLOG_DEBUG("\n Wrong text type %d\n", pu8Data[1]);
        }

        NGLOG_DEBUG("\n Time event text type %d\n", u8Type);   

        switch (u8Type)
        {
            case E_POP_TEXT_EMM_TUNE2CONNECT_TIMEOUT:
            case E_POP_TEXT_EMM_PROCESS_TIMEOUT:
            {
                MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);
                break;
            }
            case E_POP_TEXT_EMM_DOWNLOAD_MSG:
            {
                // FIXME
                // ucIRD_ProcessOTAFlag(TRUE);
                break;
            }
            case E_POP_TEXT_MENU_BAT_CHECK:
            {
                // FIXME
                //ucIRD_DecoderMenuDownloadControl_GetBAT();
                break;
            }
            default:
                NGLOG_DEBUG("\nWrong type\n");
                return FALSE;
        }
    }

   return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_OTA_DrvEvent
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

//FIXME:
BOOL MApp_CCAUI_OTA_DrvEvent(UINT8 *pu8Data)
{
    switch ((pu8Data[0] - 1))
    {
        case E_POP_TEXT_GET_NIT:
        {
            // FIXME
            //ucIRD_DecoderMenuDownloadControl_NIT_check();
            break;
        }
        case E_POP_TEXT_TUNE2CONNETC:
        {
            // FIXME
            //ucIRD_DecoderMenuDownloadControl_tune_check();
            break;
        }
        case E_POP_TEXT_PROCESSDOWNPARAM:
        {
            // FIXME
            //ucIRD_DecoderMenuDownloadControl_process_check();
            break;
        }
        case 99:
        {
            // FIXME
            //ucIRD_DecoderEmmDownloadControl();
        }
        default:
            NGLOG_DEBUG("\nWrong type\n");
            return FALSE;
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_UpdateServiceStatus
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/07/15
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_CCAUI_SetPinCode(UINT8* pu8Data)
{
    BOOL bHandled = FALSE;
    UINT32 u32Pwd    = 0;

    if (pu8Data[0] == E_CCA_CONTROL_MESSAGE_PIN_CODE_PARENTAL)
    {
        NGLOG_DEBUG("Parental PIN Code:: PinIndex = %d, PinCode = 0x%x\n", pu8Data[1], ((UINT16)pu8Data[2] << 8) | ((UINT16)pu8Data[3]));

        if (pu8Data[1] != CCA_NVRAM_PARENTAL_PIN_INDEX)
        {
            NGLOG_DEBUG("[%s %d] The Pin index is beyond the current support = %d\n", pu8Data[1]);
            return FALSE;
        }

        // Convert 4 nibbles BCD to a number
        u32Pwd  = ((UINT32)(pu8Data[2] & 0xf0) >> 4) * 1000;
        u32Pwd += ((UINT32)(pu8Data[2] & 0x0f)) * 100;
        u32Pwd += ((UINT32)(pu8Data[3] & 0xf0) >> 4) * 10;
        u32Pwd += ((UINT32)(pu8Data[3] & 0x0f));

        MApp_SetParentalPinCode(u32Pwd);
        bHandled = TRUE;
    }
    else if (pu8Data[0] == E_CCA_CONTROL_MESSAGE_PIN_CODE_INSTALLER)
    {
        NGLOG_DEBUG("Installer PIN Code:: PinCode = 0x%x\n", ((UINT16)pu8Data[1] << 8) | ((UINT16)pu8Data[2]));

        // Convert 4 nibbles BCD to a number
        u32Pwd  = ((UINT32)(pu8Data[1] & 0xf0) >> 4) * 1000;
        u32Pwd += ((UINT32)(pu8Data[1] & 0x0f)) * 100;
        u32Pwd += ((UINT32)(pu8Data[2] & 0xf0) >> 4) * 10;
        u32Pwd += ((UINT32)(pu8Data[2] & 0x0f));

        //MApp_SetInstallerlPinCode(u16Pwd);
        MApp_SetPWD(u32Pwd);
        bHandled = TRUE;
    }

    return bHandled;
}










///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowMail
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////


//FIXME: Stream syntax and TMS/Mail data arrangement
BOOL MApp_CCAUI_ShowMail(UINT8 *pu8Data)
{

    if (pu8Data[0] == E_OSD_MENU_CCA_CA_MAIL_INFO)
    {
        if (MApp_ZUI_GetActiveOSD() == E_OSD_MENU_CCA_CA_MAIL_INFO)
        {
            NGLOG_DEBUG("Active OSD = %ld\n", E_OSD_MENU_CCA_CA_MAIL_INFO);

            MApp_ZUI_ACT_UpdateCCAIrdMailInfo();
            return TRUE;
        }
        else if (MApp_CCAUI_Cmd_GetSystemOsdCheck() == TRUE)
        {
            // FIXME, when any new mail/announcements exist, show the icon.
            if (pu8Data[1] == E_CCA_TEXT_MSG_TYPE_MAILBOX)
            {
                MApp_ZUI_ACT_ShowCCAIrdMailIcon(TRUE);
            }
            else if (pu8Data[1] == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
            {
                MApp_ZUI_ACT_ShowCCAIrdAnnouncementIcon(TRUE);
            }
            return TRUE;
        }
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_ShowIcon
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

//FIXME: Stream syntax and TMS/Mail data arrangement
BOOL MApp_CCAUI_ShowIcon(UINT8 *pu8Data)
{
    // FIXME, when any new mail/announcements exist, show the icon.
    //S3_UI_CTRL_DBG("pu8Data[1] = %d\n", pu8Data[1]);
    if (pu8Data[0] == E_CCA_TEXT_MSG_TYPE_MAILBOX)
    {
        MApp_ZUI_ACT_ShowCCAIrdMailIcon(pu8Data[1]);

    }
    else if (pu8Data[0] == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
    {
        MApp_ZUI_ACT_ShowCCAIrdAnnouncementIcon(pu8Data[1]);
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_UpdateServiceStatus
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/07/15
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_CCAUI_UpdateServiceStatus(UINT8 *pu8Data)
{
#if 0
	switch(pu8Data[0]){
		case E_APP_CMD_CCA_CA_SERVICE_LIST_STATUS:{
			Window::broadcast(WM_CCAMSG, pu8Data[1], 0);
		};break;
		case E_APP_CMD_CCA_CA_SERVICE_STATUS:;break;
		case E_APP_CMD_CCA_CA_LOADER_STATUS:;break;
		case E_APP_CMD_CCA_CA_PRODUCT_STATUS:;break;
		case E_APP_CMD_CCA_CA_CLIENT_STATUS:{
			Window::broadcast(WM_CAMSG, DWORD wParam, ULONG lParam)
		};break;
		default : return false;	
	}
	#endif
    if (MApp_CCAUI_Cmd_GetSystemOsdCheck() != TRUE)
    {
        if (pu8Data[0] == E_OSD_MENU_CCA_CA_SERVICE_STATUS)
        {
            if (MApp_ZUI_GetActiveOSD() == E_OSD_MENU_CCA_CA_SERVICE_STATUS)
            {
                NGLOG_DEBUG("Active OSD = %ld\n", E_OSD_MENU_CCA_CA_SERVICE_STATUS);
                MApp_ZUI_ACT_AppShowCCAServiceStatus();
                return TRUE;
            }
        }
        else if (pu8Data[0] == E_OSD_MENU_CCA_CA_STATUS)
        {
            if (MApp_ZUI_GetActiveOSD() == E_OSD_MENU_CCA_CA_STATUS)
            {
                NGLOG_DEBUG("Active OSD = %ld\n", E_OSD_MENU_CCA_CA_STATUS);
                MApp_ZUI_ACT_NotifyCCACaStatusUpdate(&pu8Data[1]);
                return TRUE;
            }
        }

        return FALSE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_UpdateIconStatus
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/07/15
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_UpdateMailIconStatus(void)
{
    int i = 0;
    UINT16 u16ItemCount = 0;

    u16ItemCount = MApp_DecoderMsg_GetTotalMails();
    NGLOG_DEBUG("count = %d\n", u16ItemCount);
    if (u16ItemCount > 0)
    {
        if (MApp_ZUI_ACT_DetectCCAIrdMailIcon() == FALSE)
        {
            NGLOG_DEBUG("\n");
            for (i = 0; i < u16ItemCount; i++)
            {
                if (MApp_DecoderMsg_GetMailInfoIDNewMailItems(i) == TRUE)
                {
                    // One new item is found
                    MApp_DecoderMsg_TextMsg_ShowIcon(E_CCA_TEXT_MSG_TYPE_MAILBOX, TRUE);
                    break;
                }
            }
        }

        if (i == u16ItemCount)
        {
            // Hide the Mail icon
            if (MApp_ZUI_ACT_DetectCCAIrdMailIcon() == TRUE)
            {
                MApp_DecoderMsg_TextMsg_ShowIcon(E_CCA_TEXT_MSG_TYPE_MAILBOX, FALSE);
            }
        }
    }
}

void MApp_CCAUI_UpdateAnncmntIconStatus(void)
{
    int i = 0;
    UINT16 u16ItemCount = 0;

    u16ItemCount = MApp_DecoderMsg_GetTotalAnncmnts();
    //S3_UI_CTRL_DBG("count = %d\n", u16ItemCount);
    if (u16ItemCount > 0)
    {
        if (MApp_ZUI_ACT_DetectCCAIrdAnncmntIcon() == FALSE)
        {

            for (i = 0; i < u16ItemCount; i++)
            {
                if (MApp_DecoderMsg_GetMailInfoIDNewAnncmntItems(i) == TRUE)
                {
                    // One new item is found
                    MApp_DecoderMsg_TextMsg_ShowIcon(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT, TRUE);
                    break;
                }
            }
        }

        if (i == u16ItemCount)
        {
            // Hide the Mail icon
            if (MApp_ZUI_ACT_DetectCCAIrdAnncmntIcon() == TRUE)
            {
                MApp_DecoderMsg_TextMsg_ShowIcon(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT, FALSE);
            }
        }
    }
}



#if 0

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_ShowPreview
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////

//FIXME: Stream syntax and TMS/Mail data arrangement
BOOL MApp_S3UI_ShowPreview(UINT8 *pu8Data)
{
	MApp_ZUI_ACT_ShowS3IrdPreview(pu8Data[0]);

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_S3UI_UpdatePreviewStatus
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/07/15
///////////////////////////////////////////////////////////////////////////////

void MApp_S3UI_UpdatePreviewStatus(void)
{
    UINT8 u8PreviewTime = 0;

    u8PreviewTime = MApp_DVB_Client_Monitoring_Get_Preview_Time();
    //S3_UI_CTRL_DBG("count = %d\n", u8PreviewTime);
    if (u8PreviewTime > 0)
    {
        if (MApp_ZUI_ACT_DetectS3IrdPreview() == FALSE)
        {
            MApp_DVB_Client_Monitoring_ShowPreview(TRUE);
        }
    }
	else
	{
		if (MApp_ZUI_ACT_DetectS3IrdPreview() == TRUE)
		{
			MApp_DVB_Client_Monitoring_ShowPreview(FALSE);
		}
	}
}


#endif



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_UpdateTmsTexMsg
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/07/15
///////////////////////////////////////////////////////////////////////////////

void MApp_CCAUI_UpdateTmsTexMsg(void)
{
    int i = 0;
    NGL_DATE* pstDelDate;
    UINT16   u16ItemCount = 0;

    u16ItemCount = MApp_DecoderMsg_GetTotalMails();
    //S3_UI_CTRL_DBG("count = %d\n", u16ItemCount);
    for (i = 0; i < u16ItemCount; i++)
    {
        pstDelDate = MApp_DecoderMsg_GetMailDelTime(i);
        if (MApi_ConvertDate2Seconds(pstDelDate) >= MApi_DateTime_Get())
        {
            MApp_DecoderMsg_DeleteMail(i);
        }
    }

    u16ItemCount = MApp_DecoderMsg_GetTotalAnncmnts();
    //S3_UI_CTRL_DBG("count = %d\n", u16ItemCount);
    for (i = 0; i < u16ItemCount; i++)
    {
        pstDelDate = MApp_DecoderMsg_GetAnncmntDelTime(i);

        if (MApi_ConvertDate2Seconds(pstDelDate) >= MApi_DateTime_Get())
        {
            MApp_DecoderMsg_DeleteAnncmnt(i);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_TaskEntry
///
///
///  @param [in]   pu8Data
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/07/15
///////////////////////////////////////////////////////////////////////////////
void MApp_CCAUI_TaskEntry(void)
{
    static BOOL bObtainStartDateTime = FALSE;
    UINT32 u32StartDateTime = 0;
    UINT32 u32CurrDateTime  = MApi_DateTime_Get();

    if (bObtainStartDateTime == FALSE)
    {
        u32StartDateTime = MApi_DateTime_Get();
        bObtainStartDateTime = TRUE;
    }

    if (MApp_CCAUI_Cmd_GetSystemOsdCheck() == TRUE)
    {


		if (((u32CurrDateTime - u32StartDateTime) & 0xf) == 0x0) // Every other 16 seconds. FIXME
        {
            MApp_CCAUI_UpdateTmsTexMsg();
        }

        // Display an icon clearly visible on the screen
        if (gbCheckMailIcon == FALSE)
        {
            gbCheckMailIcon = TRUE;
            MApp_CCAUI_UpdateMailIconStatus();
        }

        if (gbCheckAnncmtIcon == FALSE)
        {
            gbCheckAnncmtIcon = TRUE;
            MApp_CCAUI_UpdateAnncmntIconStatus();
        }


    }
    else
    {
        return;
    }

    {//Only after MApp_S3UI_Cmd_GetSystemOsdCheck is OK(TRUE)
        if (MApp_ZUI_ACT_DetectCCAIrdMailIcon() == TRUE)
        {
// temp to avoid the crash           MApp_ZUI_ACT_ShowS3IrdMailIcon(FALSE);
            gbCheckMailIcon = FALSE;
        }
        if (MApp_ZUI_ACT_DetectCCAIrdAnncmntIcon() == TRUE)
        {
// temp to avoid the crash            MApp_ZUI_ACT_ShowS3IrdAnnouncementIcon(FALSE);
            gbCheckAnncmtIcon = TRUE;
        }
    }

//	if(MApp_ZUI_GetActiveOSD() == E_OSD_TV_PROG)
//	{
//		MApp_S3UI_UpdatePreviewStatus();
//	}

    MApp_ZUI_ACT_MonitorCCAIrdScreenTextDisplay();

}





#undef _MAPP_ZUI_ACTCCAUICONTROL_C

