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

#define MAPP_ZUI_ACTCCASERVICESTATUS_C

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
#include "MApp_ZUI_ACTCCAServiceStatus.h"
#include "MApp_ZUI_ACTeffect.h"
#include "MApp_ZUI_APIdraw.h"
#include "MApp_ZUI_ACTglobal.h"
#include "ZUI_exefunc.h"
#include "MApp_ZUI_Utility.h"
#include "MApp_ZUI_Resource.h"
#include "apiGlobalFunction.h"

#include "UniversalClient_API.h"
#include "MApp_ZUI_ACTCCACasCmd.h"
#include "MApp_ZUI_ACTCCACaStatus.h"

//#define CCA_SERVICESTATUS_DEBUG

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------

#ifdef CCA_SERVICESTATUS_DEBUG
#define CCA_SERVICESTATUS_DBG(fmt, arg...)   {printf("\033[0;35m[S3 Service Status]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define CCA_SERVICESTATUS_DBG(fmt, arg...)
#endif

#define STATUS_ITEM_MAX_LINES            64  // Lines
#define STATUS_ITEM_MAX_LINE_LENGTH      254  // 80 bytes

#define S3_IRD_SERVICE_STATUS_SCREEN_TEXT_LINE_LENGTH  32
#define S3_IRD_SERVIEE_TEXT_BUF_SIZE                   (STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH)


#define TOTAL_SERVICES_STATUS_PAGE_LINE_EXTEND 1

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/
extern UINT8* MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerivesCurrentLine(void);


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static UINT16 gu16UnicodeStr[512];
//static UINT8  gau8Str[S3_IRD_SERVIEE_TEXT_BUF_SIZE];

static HWND _boxlistsrvstatus_lines[] =
{
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE1,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE2,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE3,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE4,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE5,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE6,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE7,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE8,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE9,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE10,
#if TOTAL_SERVICES_STATUS_PAGE_LINE_EXTEND
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE11,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE12,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE13,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE14,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE15,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE16,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE17,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE18,
    HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE19,
#endif
};

static ZUICTLSolidVScrollBarData _boxlistsrvstatusvscroll =
{
    .border          = 1,
#if (GDI_COLOR_FORMAT == GDI_COLOR_I8)
    .bg_color        = 0xFFF8F8F8,
    .bar_color       = 0xFFF6F6F6,
#else
    .bg_color        = 0xFF2B81D7,
    .bar_color       = 0xFFABCDEF,
#endif
    .cur_line        = 0,
    .total_lines     = 0,
#if !TOTAL_SERVICES_STATUS_PAGE_LINE_EXTEND
    .lines_per_view  = 10,
#else
    .lines_per_view  = 19,
#endif
};


static ZUIUTL_LBNav _lbnavboxlistsrvstatus =
{
    .isInited        = FALSE,
#if !TOTAL_SERVICES_STATUS_PAGE_LINE_EXTEND
    .lines_per_view  = 10,
#else
    .lines_per_view  = 19,
#endif
    .total_lines     = 0,
    .cur_line        = 0,
    .view_start_line = 0,
    .hWnds           = _boxlistsrvstatus_lines,
    .flag            = ZUIUTL_LBNAV_CONTINUOUS_NAV_ENABLE,
    .scrolldata      = &_boxlistsrvstatusvscroll,
    .scrollbar       = HWND_MENU_CCA_SERVICES_STATUS_PAGE_SCRO,
};

///////////////////////////////////////////////////////////////////////////////
///  private  _atohex
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////

static UINT32 _atohex(UINT8 u8Char)
{
    if (u8Char >= 'a')
    {
        return  ((u8Char - 'a') + 10);
    }
    else if (u8Char >= 'A')
    {
        return  ((u8Char - 'A') + 10);
    }
    else if (u8Char >= '0')
    {
        return  (u8Char - '0');
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_SerivesStatusName
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
UINT16 _MApp_ZUI_ACT_GetCCASerivesStatus_CurrentHandle(UINT8 index)
{
    UINT8* pCurrLine = gau8Services+index*STATUS_ITEM_MAX_LINE_LENGTH;//MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerivesCurrentLine();
    UINT16 u16Handle = 0;

    CCA_SERVICESTATUS_DBG("\n");
    
    // Note:  the first 10 digits of the current line is like "0x00000000"
    //u16Handle = u16Handle + (_atohex(pCurrLine[2]) << 28);
    //u16Handle = u16Handle + (_atohex(pCurrLine[3]) << 24);
    //u16Handle = u16Handle + (_atohex(pCurrLine[4]) << 20);
    //u16Handle = u16Handle + (_atohex(pCurrLine[5]) << 16);
    u16Handle = u16Handle + (_atohex(pCurrLine[6]) << 12);
    u16Handle = u16Handle + (_atohex(pCurrLine[7]) << 8);
    u16Handle = u16Handle + (_atohex(pCurrLine[8]) << 4);
    u16Handle = u16Handle + (_atohex(pCurrLine[9]));

    CCA_SERVICESTATUS_DBG("u16Handle = 0x%08x\n", u16Handle);

    return u16Handle;
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_S3UI_CasCmd_GetServiceStatus
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT16 _MApp_ZUI_ACT_GetServiceStatus(UINT8 *pu8Text, UINT8 u8LineCount, UINT16 u16LineLength,UINT8 index)
{
    UINT8  u8TotalLines = 0;
    UINT8  u8SplitLines = 0;
    UINT8* pu8Str       = pu8Text;
    UINT32 u32Handle    = _MApp_ZUI_ACT_GetCCASerivesStatus_CurrentHandle();
    UINT32 u32Index     = MApp_CCAUI_CasCmd_GetServiceIndexByHandle(u32Handle);
    UINT8  au8Buf[STATUS_ITEM_MAX_LINE_LENGTH];

    if (u32Index == 0xfffffff)
    {
        CCA_SERVICESTATUS_DBG("This handle is not found = %d\n", u32Handle);
        return 0;
    }
    else
    {
        CCA_SERVICESTATUS_DBG("Handle = %d, Index = %ld\n", u32Handle, u32Index);
    }

    //enable Monitor
    if(MApp_CCAUI_CasCmd_EnableServiceMonitorStatus(u32Index, TRUE) == FALSE)
    {
        CCA_SERVICESTATUS_DBG("Enable Monitor for handle %d FAIL!!\n", u32Handle);
    }

    // Service name
    memset(au8Buf, '\0', sizeof(au8Buf));
    if (MApp_CCAUI_CasCmd_GetServiceItemName(u32Index, au8Buf, sizeof(au8Buf)) == TRUE)
    {
        snprintf((char *)pu8Str, u16LineLength, "%s", au8Buf);
    }
    else
    {
        snprintf((char *)pu8Str, u16LineLength, "%s", "FAIL to get the service name!");
    }
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    // Handle
    snprintf((char *)pu8Str, u16LineLength, "  Service Handle:  0x%08lX", u32Handle);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Service status
    memset(au8Buf, '\0', sizeof(au8Buf));
    if (MApp_CCAUI_CasCmd_GetServiceItemStatus(u32Index, au8Buf, u16LineLength) == TRUE)
    {
        //FIXME next line : if(strlen(au8Buf)>34)
        snprintf((char *)pu8Str, u16LineLength, "  Service Status:   %s", au8Buf);
    }
    else
    {
        snprintf((char *)pu8Str, u16LineLength, "%s", "FAIL to get the service status!");
    }
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Service Pids status
    u8SplitLines = MApp_CCAUI_CasCmd_GetServiceItemCaStatus(u32Index, pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;

    //Monitoring
    u8SplitLines = MApp_CCAUI_CasCmd_GetServiceItemMonitorStatus(u32Index, pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;
    
    return u8TotalLines;
}


///////////////////////////////////////////////////////////////////////////////
///  private  _Update_DVB_Client_SrvStatus_init
///
///
///  @param [in]
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2013/07/03
///////////////////////////////////////////////////////////////////////////////
static BOOL _Update_DVB_Client_SrvStatus_init(void)
{
    UINT16 u16LineCount = 0;

    CCA_SERVICESTATUS_DBG("\n");

    memset(gau8Str, 0, S3_IRD_SERVIEE_TEXT_BUF_SIZE);

    //Get the number from high level APP
    u16LineCount = _MApp_ZUI_ACT_GetServiceStatus(gau8Str, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH);
    //ACTCCACASTATUS_DBG("%s", gau8Product);

    //FIXME: check the total lines of product status.
    if (u16LineCount > 0)
    {
        MApp_ZUI_UTL_LBNav_Init(&_lbnavboxlistsrvstatus);
        MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistsrvstatus);
        _lbnavboxlistsrvstatus.cur_line             = 0;
        _lbnavboxlistsrvstatus.total_lines          = u16LineCount;
        _lbnavboxlistsrvstatus.scrolldata->cur_line = _lbnavboxlistsrvstatus.cur_line;
        GETWNDDATA(HWND_MENU_CCA_SERVICES_STATUS_PAGE_SCRO) = (void *) &_boxlistsrvstatusvscroll;
        MApp_ZUI_UTL_LBNav_ReInit(&_lbnavboxlistsrvstatus);
        MApp_ZUI_UTL_LBNav_UpdateList(&_lbnavboxlistsrvstatus);
        MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_SERVICES_STATUS_PAGE, SW_SHOW);
        return TRUE;
    }

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_SERVICES_STATUS_PAGE, SW_HIDE);
//    MApp_ZUI_API_SetFocus(prevfoces);
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_AppShowCCAServiceStatus
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_AppShowCCAServiceStatus(void)
{
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_SERVICES_STATUS_PAGE, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_SERVICES_STATUS_PAGE_LIST);

    if (_Update_DVB_Client_SrvStatus_init() == FALSE)
    {
        MApp_ZUI_ACT_AppShowCCACaStatus();
        //    return FALSE;
    }

}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_HandleCCAServiceStatusKey
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_HandleCCAServiceStatusKey(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    switch (key)
    {
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_PAGE_UP:
        case VK_PAGE_DOWN:
        {
            CCA_SERVICESTATUS_DBG("MApp_ZUI_ACT_HandleCCAServiceStatusKey. Navigate listbox\n");
            if ((VK_LEFT == key) || (VK_UP == key))
            {
                key = VK_PAGE_UP;
            }

            if ((VK_RIGHT == key) || (VK_DOWN == key))
            {
                key = VK_PAGE_DOWN;
            }
            MApp_ZUI_UTL_LBNav_Action(&_lbnavboxlistsrvstatus, key);
            return TRUE;
        }
        case VK_EXIT:
            MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_MENU);
            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCAServiceStatusAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_TerminateCCAServiceStatus
///  [OSD page handler] terminate MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_TerminateCCAServiceStatus(void)
{
    CCA_SERVICESTATUS_DBG("[]term:menu\n");

    //disable Monitor
    UINT32 u32Handle    = _MApp_ZUI_ACT_GetCCASerivesStatus_CurrentHandle();
    UINT32 u32Index     = MApp_CCAUI_CasCmd_GetServiceIndexByHandle(u32Handle);
    if(MApp_CCAUI_CasCmd_EnableServiceMonitorStatus(u32Index, FALSE) == FALSE)
    {
        CCA_SERVICESTATUS_DBG("Enable Monitor for handle %d FAIL!!\n", u32Handle);
    }
    
    //MApp_SaveGenSetting();
    //enCAmenuState = _enTargetMenuState;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_ExecuteCCAServiceStatusAction
///  [OSD page handler] execute a specific action in MENU application
///
///  @param [in]       act U16      action ID
///
///  @return BOOLEAN     true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_ExecuteCCAServiceStatusAction(U16 u16Act)
{
    switch (u16Act)
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
///  private  _MApp_ZUI_ACT_GetCCAServiceStatusDynamicTextLines
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAServiceStatusDynamicTextLines(UINT8 u8Index)
{
    UINT16 u16CurrLine = u8Index + _lbnavboxlistsrvstatus.view_start_line;

    MApi_U8StringToU16String(&gau8Str[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH], gu16UnicodeStr, strlen(&gau8Str[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH]));
    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAServiceStatusDynamicText
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]       hwnd HWND     window handle we are processing
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
LPTSTR MApp_ZUI_ACT_GetCCAServiceStatusDynamicText(HWND hwnd)
{
    switch (hwnd)
    {
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE1:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE2:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE3:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE4:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE5:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE6:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE7:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE8:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE9:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE10:
#if TOTAL_SERVICES_STATUS_PAGE_LINE_EXTEND
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE11:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE12:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE13:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE14:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE15:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE16:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE17:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE18:
        case HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE19:
#endif


            {
                UINT8 u8Index = (hwnd - HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE1);

                if (HWND_MENU_CCA_SERVICES_STATUS_PAGE_LINE1 == hwnd)
                {
                    if (_lbnavboxlistsrvstatus.total_lines == 0)
                    {
                        return MApp_ZUI_API_GetString(en_str_NoInformation);
                    }
                }
                return ((LPTSTR)_MApp_ZUI_ACT_GetCCAServiceStatusDynamicTextLines(u8Index));
            }
        default:
            return NULL;
    }

    return NULL;
}

/////////////////////////////////////////////////////////
// Customize Window Procedures

#undef MAPP_ZUI_ACTCCASERVICESTATUS_C

