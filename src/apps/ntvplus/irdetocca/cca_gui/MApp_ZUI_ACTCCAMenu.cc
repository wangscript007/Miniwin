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
// (MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#define MAPP_ZUI_ACTCCAMENU_C


//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------
#include "DataType.h"
#include "MApp_ZUI_Main.h"
#include "MApp_ZUI_APIcommon.h"
#include "MApp_ZUI_APIstrings.h"
#include "MApp_ZUI_APIwindow.h"
#include "MApp_ZUI_APItables.h"
#include "MApp_ZUI_APIgdi.h"
#include "MApp_ZUI_APIcontrols.h"
#include "MApp_ZUI_ACTCCAMenu.h"
#include "MApp_ZUI_ACTeffect.h"
#include "MApp_ZUI_APIdraw.h"
#include "MApp_ZUI_ACTglobal.h"
#include "ZUI_exefunc.h"
#include "MApp_ZUI_Utility.h"
#include "MApp_ZUI_Resource.h"


#if (CA_IRD_CCA == 1)
#include "appCCADecoderMsgDecControl.h"
#endif


/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef ACTCCAMENU_DEBUG
#define ACTCCAMENU_DBG(fmt, arg...)   {printf("\033[0;35m[ACT CCA MENU]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define ACTCCAMENU_DBG(fmt, arg...)
#endif


//static UINT16           u16UnicodeStr[250];

#if (CA_IRD_CCA == 1)
static BOOL ShowOTA = FALSE;
#endif
/////////////////////////////////////////////////////////////////////

extern BOOLEAN _MApp_ZUI_API_AllocateVarData(void);
extern void _MApp_FONT_ascii_2_unicode(UINT16 *out, UINT8 *in);

/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_AppShowCCAMenu
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_AppShowCCAMenu(void)
{

    //MApp_ZUI_API_SetFocus(HWND_MENU_CCA_CA_STATUS_PAGE);
    #if  (CA_IRD_CCA == 1)
    ShowOTA = MApp_MenuPreDownloadControl();
    if (ShowOTA == FALSE)
    {
        MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CA_OTA, SW_HIDE);
    }
    #endif

    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT);

#if !defined(ACTCCATEST)
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CA_TEST, SW_HIDE);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_HandleCCAMenuKey
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_HandleCCAMenuKey(VIRTUAL_KEY_CODE key)
{
    ACTCCAMENU_DBG("HandleCCAMenuKey. key =%ld\n", key);

    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    switch (key)
    {
        case VK_UP:
        case VK_DOWN:
        {
            HWND TempFocus = MApp_ZUI_API_GetFocus();

            switch (TempFocus)
            {
                #if (CA_IRD_CCA == 1)
                case HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT:
                {
                    if (ShowOTA)
                    {
                        TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_EMAIL_INFO : HWND_MENU_CCA_DVB_CA_OTA;
                        break;
                    }
                    else
                    {
                        TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_EMAIL_INFO : HWND_MENU_CCA_DVB_CA_EMAIL_INFO;
                        break;
                    }
                }
                case HWND_MENU_CCA_DVB_CA_EMAIL_INFO:
                {
                    if (ShowOTA)
                    {
                        TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_OTA : HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT;
                        break;
                    }
                    else
                    {
                        TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT : HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT;
                        break;
                    }                    
                }
                #else
                case HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_EMAIL_INFO : HWND_MENU_CCA_DVB_CA_OTA;
                    break;
                }
                case HWND_MENU_CCA_DVB_CA_EMAIL_INFO:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_OTA : HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT;
                    break;
                }
                #endif
#if !defined(ACTCCATEST)
                case HWND_MENU_CCA_DVB_CA_OTA:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT : HWND_MENU_CCA_DVB_CA_EMAIL_INFO;
                    break;
                }
#else
                case HWND_MENU_CCA_DVB_CA_OTA:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_TEST : HWND_MENU_CCA_DVB_CA_EMAIL_INFO;
                    break;
                }
                case HWND_MENU_CCA_DVB_CA_TEST:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT : HWND_MENU_CCA_DVB_CA_EMAIL_INFO;
                    break;
                }
#endif
            }

            MApp_ZUI_API_SetFocus(TempFocus);
            return TRUE;
        }
        case VK_SELECT:
            switch (MApp_ZUI_API_GetFocus())
            {
                case HWND_MENU_CCA_DVB_CA_CA_STATUS_TEXT:
                    MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_STATUS);
                    return TRUE;

                case HWND_MENU_CCA_DVB_CA_EMAIL_INFO:
                    MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_MAIL_INFO);
                    return TRUE;

                case HWND_MENU_CCA_DVB_CA_OTA:
                    #if (CA_IRD_CCA == 1)
                    MApp_MenuDownloadControl();
                    #else
                    MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_OTA);
                    #endif
                    return TRUE;

#if defined(ACTCCATEST)
                case HWND_MENU_CCA_DVB_CA_TEST:
                    MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA);
                    return TRUE;
#endif
                default:
                    return FALSE;
            }
            return TRUE;

        case VK_EXIT:
        case VK_MENU:
            MApp_ZUI_UTL_PageSwitch(E_OSD_MAIN_MENU);

            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCAMenuAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_TerminateCCAMenu
///  [OSD page handler] terminate MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_TerminateCCAMenu(void)
{
    ACTCCAMENU_DBG("TerminateCCAMenu\n");
    //MApp_SaveGenSetting();
    //enCAmenuState = _enTargetMenuState;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_ExecuteCCAMenuAction
///  [OSD page handler] execute a specific action in MENU application
///
///  @param [in]       act U16      action ID
///
///  @return BOOLEAN     true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_ExecuteCCAMenuAction(U16 act)
{
    switch(act)
    {
        case EN_EXE_POWEROFF:
            return TRUE;

        case EN_EXE_CLOSE_CURRENT_OSD:
        {
            //we use auto-close widget to realize this delayed task.
            //beware to kill timer when leaving this page.
            //timer triggered will send this action. do task here.
            //examine the key-drawing response if any improvement?
            //MApp_ZUI_API_ShowWindow(HWND_MENU_SYSTEM_PARENT_CTL_STATIC_ITEM, SW_HIDE);
            //ACTPMSBAR_RESET;
            //MApp_ZUI_API_KillTimer (HWND_MENU_SYSTEM_PARENT_CTL_STATIC_ITEM, 1);
            return TRUE;
        }

        default:
            return FALSE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAMenuDynamicText
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]       hwnd HWND     window handle we are processing
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
LPTSTR MApp_ZUI_ACT_GetCCAMenuDynamicText(HWND hwnd)
{
    switch(hwnd)
    {
        default:
            return NULL;
    }

    return NULL;
}

/////////////////////////////////////////////////////////
// Customize Window Procedures

#undef MAPP_ZUI_ACTCCAMENU_C
