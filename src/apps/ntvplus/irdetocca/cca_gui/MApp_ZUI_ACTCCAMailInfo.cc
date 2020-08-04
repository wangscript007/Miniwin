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

#define MAPP_ZUI_ACTCCAMAILINFO_C


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
#include "MApp_ZUI_ACTCCAMailInfo.h"
#include "MApp_ZUI_ACTeffect.h"
#include "MApp_ZUI_APIdraw.h"
#include "MApp_ZUI_ACTglobal.h"
#include "ZUI_exefunc.h"
#include "apiGlobalFunction.h"
#include "MApp_ZUI_Utility.h"
#include "MApp_ZUI_Resource.h"

#include "MApp_ZUI_ACTCCACasCmd.h"

//#define ACTCCAMAILINFO_DEBUG

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef ACTCCAMAILINFO_DEBUG
#define ACTCCAMAILINFO_DBG(fmt, arg...)   {printf("\033[0;35m[ACT CCA MailInfo]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define ACTCCAMAILINFO_DBG(fmt, arg...)
#endif

#define CCA_MAIL_SIZE_PER_PAGE   512   //FIXME

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/

//FIXME
static UINT16 gu16UnicodeStr[512];
static UINT8  gau8Item[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
//static UINT16  gau8Mail[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
static UINT8  gau8Mail[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
static UINT8  gu8MailTotalPage   = 0;
static UINT8  gu8MailCurrentPage = 0;

static HWND _boxlistmailitem_lines[] =
{
    HWND_MENU_CCA_EMAIL_INFO_FOCUS1,
    HWND_MENU_CCA_EMAIL_INFO_FOCUS2,
    HWND_MENU_CCA_EMAIL_INFO_FOCUS3,
    HWND_MENU_CCA_EMAIL_INFO_FOCUS4,
    HWND_MENU_CCA_EMAIL_INFO_FOCUS5,
    HWND_MENU_CCA_EMAIL_INFO_FOCUS6,
    HWND_MENU_CCA_EMAIL_INFO_FOCUS7,
};

static ZUICTLSolidVScrollBarData _boxlistmailitemvscroll =
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
    .lines_per_view  = 7,
};


static ZUIUTL_LBNav _lbnavboxlistmailitem =
{
    .isInited        = FALSE,
    .lines_per_view  = 7,
    .total_lines     = 0,
    .cur_line        = 0,
    .view_start_line = 0,
    .hWnds           = _boxlistmailitem_lines,
    .flag            = ZUIUTL_LBNAV_CONTINUOUS_NAV_ENABLE,
    .scrolldata      = &_boxlistmailitemvscroll,
    .scrollbar       = HWND_MENU_CCA_EMAIL_INFO_MAIL_SCRO,
};

/////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_SerNo
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_SerNo(UINT8 u8Index)
{
    UINT8 au8Buf[32];

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "0x%04x", MApp_CCAUI_CasCmd_GetMailMaxItem()); //FIXME
    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_Flag
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_Flag(UINT8 u8Index)
{
    UINT8 au8Buf[32];

    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetMailType(u8Index, au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_Level
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_Level(UINT8 u8Index)
{
    UINT8 au8Buf[32];

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "5678"); //FIXME
    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_CreateTime
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_CreateTime(UINT8 u8Index)
{
    UINT8 au8Buf[32];

    memset(au8Buf, 0, 32);

    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetMailItem
///
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/6/21
///////////////////////////////////////////////////////////////////////////////
UINT8 _MApp_ZUI_ACT_GetMailItem(UINT8 *pu8Buf, UINT8 u8LineCount, UINT8 u8LineLength)
{
    int i = 0;
    UINT8*   pu8Str  = pu8Buf;
    UINT8    u8Count = MApp_CCAUI_CasCmd_GetMailMaxItem();
    UINT8    au8Buf[4][32];

    ACTCCAMAILINFO_DBG("\n");


    if (u8Count > u8LineCount)
    {
        u8Count = u8LineCount;  // Set to the max number of mail items
    }

    for (i = 0; i < u8Count; i++)
    {
        memset(au8Buf, '\0', sizeof(au8Buf));

        MApp_CCAUI_CasCmd_GetMailId(i, au8Buf[0], 32);
        MApp_CCAUI_CasCmd_GetMailType(i, au8Buf[1], 32);

        if (MApp_CCAUI_CasCmd_CheckNewMailItem(i) == TRUE)
        {
            snprintf((char*)au8Buf[2], 32, "(N)0"); //FIXME. Level
        }
        else
        {
            snprintf((char*)au8Buf[2], 32, "(R)0"); //FIXME. Level
        }
        MApp_CCAUI_CasCmd_GetMailCreateTime(i, au8Buf[3], 32);

        snprintf(pu8Str, u8LineLength, "%-10s%-15s%-12s%-s", au8Buf[0], au8Buf[1], au8Buf[2], au8Buf[3]);
        pu8Str += u8LineLength;
    }

    return u8Count;
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
static BOOL _Update_DVB_Client_MailItem_init(void)
{
    U16    u16LineCount = 0;
    UINT16 u16ArraySize  = STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH;

    ACTCCAMAILINFO_DBG("\n");
    memset(gau8Item, 0, u16ArraySize);

    //Get the number from high level APP
    u16LineCount = _MApp_ZUI_ACT_GetMailItem(gau8Item, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH);
    //ACTS3IRDCASTATUS_DBG("%s", gau8Product);

    //FIXME: check the total lines of product status.
    if (u16LineCount > 0)
    {
        MApp_ZUI_UTL_LBNav_Init(&_lbnavboxlistmailitem);
        MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistmailitem);
        _lbnavboxlistmailitem.cur_line             = 0;
        _lbnavboxlistmailitem.total_lines          = u16LineCount;
        _lbnavboxlistmailitem.scrolldata->cur_line = _lbnavboxlistmailitem.cur_line;
        GETWNDDATA(HWND_MENU_CCA_SERVICES_STATUS_PAGE_SCRO) = (void *) &_boxlistmailitemvscroll;

		MApp_ZUI_UTL_LBNav_ReInit(&_lbnavboxlistmailitem);
        MApp_ZUI_UTL_LBNav_UpdateList(&_lbnavboxlistmailitem);
        MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_BG, SW_SHOW);
        return TRUE;
    }

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_BG, SW_HIDE);
//    MApp_ZUI_API_SetFocus(prevfoces);
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetComponentsMaxItem
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_ZUI_ACT_GetComponentsMaxItem(void)
{
    return MApp_CCAUI_CasCmd_GetComponentsMaxString();
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetComponentsText
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_ZUI_ACT_GetComponentsText(UINT8 u8Index, UINT8* pu8Buf, UINT16 u16BufSize)
{
    if (u8Index <= MApp_ZUI_ACT_GetComponentsMaxItem())
    {
        MApp_CCAUI_CasCmd_GetComponentsIndexString(u8Index, pu8Buf, u16BufSize);
        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_SetCCAIrdMailData
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////

void MApp_ZUI_ACT_SetCCAIrdMailData(void)
{
    MApp_CCAUI_CasCmd_SetMailData();
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_SaveCCAIrdMailData
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////

void MApp_ZUI_ACT_SaveCCAIrdMailData(void)
{
    MApp_CCAUI_CasCmd_SaveMailData();
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailBoxMaxItem
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////

UINT8 MApp_ZUI_ACT_GetCCAIrdMailBoxMaxItem(void)
{
    return MApp_CCAUI_CasCmd_GetMailMaxItem();
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailContentText
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_ZUI_ACT_GetCCAIrdMailContentText(UINT8 u8Index, UINT8* pu8Buf, UINT16 u16BufSize)
{
    if (u8Index < MApp_ZUI_ACT_GetCCAIrdMailBoxMaxItem())
    {
        MApp_CCAUI_CasCmd_GetMailContent(u8Index, pu8Buf, u16BufSize);

        //FIXME, calculate the total page of this mail
        gu8MailTotalPage   = (strlen(pu8Buf) / CCA_MAIL_SIZE_PER_PAGE) + 1;
        gu8MailCurrentPage = 0;

        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetMailTotalPage
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_ZUI_ACT_GetMailTotalPage(void)
{
    return gu8MailTotalPage;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetMailCurrentPageValue
///
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
UINT8 MApp_ZUI_ACT_GetMailCurrentPageValue(void)
{
    return (gu8MailCurrentPage + 1);
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAIrdMailTypeText
///
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_ZUI_ACT_GetCCAIrdMailTypeText(UINT8 u8Index, UINT8* pu8Buf, UINT16 u16BufSize)
{
    if (u8Index <= MApp_ZUI_ACT_GetCCAIrdMailBoxMaxItem())
    {
        MApp_CCAUI_CasCmd_GetMailType(u8Index, pu8Buf, u16BufSize);
        return TRUE;
    }

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_AppShowCCAIrdMailInfo_MailContent
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_AppShowCCAIrdMailInfo_MailContent(void)
{
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_NOEMAIL_BG, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_BG, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_DYNTXT);
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_AppShowCCAIrdMailInfo_NoMail
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_AppShowCCAIrdMailInfo_NoMail(void)
{
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_NOEMAIL_BG, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_EMAIL_INFO_NOEMAIL_TEXT);
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_AppShowCCAMailInfo
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_AppShowCCAMailInfo(void)
{
    //FIXME
    UINT8 u8Count = MApp_CCAUI_CasCmd_GetMailMaxItem();
    //UINT8 u8Count = 1; //ui test
    
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_MSGBOX, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_BG, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_BG, SW_SHOW);
    
    if (u8Count > 0)
    {
        MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_NOEMAIL_BG, SW_HIDE);
        MApp_ZUI_API_SetFocus(HWND_MENU_CCA_EMAIL_INFO_FOCUS1);

        if (_Update_DVB_Client_MailItem_init() == FALSE)
        {
            MApp_ZUI_ACT_AppShowCCAIrdMailInfo_NoMail();
        }
    }
    else
    {
        _lbnavboxlistmailitem.total_lines = 0;
        MApp_ZUI_ACT_AppShowCCAIrdMailInfo_NoMail();
    }
}




///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_AppShowCCAIrdMailInfo
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_UpdateCCAIrdMailInfo(void)
{
    if (MApp_ZUI_API_GetFocus() != HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_DYNTXT)
    {
        MApp_ZUI_ACT_AppShowCCAMailInfo();
    }
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelCurrMail
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelCurrMail(void)
{
    MApp_CCAUI_CasCmd_DelMailData(_lbnavboxlistmailitem.cur_line);
    memset(gau8Mail, '\0', STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH);
    MApp_ZUI_ACT_AppShowCCAMailInfo();

    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelAllMails
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelAllMails(void)
{
    MApp_CCAUI_CasCmd_DelAllMailData();
    memset(gau8Item, '\0', STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH);
    memset(gau8Mail, '\0', STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH);

    MApp_ZUI_ACT_AppShowCCAMailInfo();
    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Info
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Info(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    ACTCCAMAILINFO_DBG("Parent = %ld. Handle = %ld\n", MApp_ZUI_API_GetParent(MApp_ZUI_API_GetFocus()), MApp_ZUI_API_GetFocus());

    switch (key)
    {
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_PAGE_UP:
        case VK_PAGE_DOWN:
        {
            ACTCCAMAILINFO_DBG("_MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Info. Navigate listbox\n");
            if (VK_LEFT == key)
            {
                key = VK_PAGE_UP;
            }

            if (VK_RIGHT == key)
            {
                key = VK_PAGE_DOWN;
            }
            MApp_ZUI_UTL_LBNav_Action(&_lbnavboxlistmailitem, key);
            return TRUE;
        }
		case VK_RED:
		{
            return (_MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelCurrMail());
        }
        case VK_YELLOW:
		{
            return (_MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelAllMails());
        }
        case VK_SELECT:
		{
			// Retrieve the selected email and show the content
			memset(gau8Mail, '\0', STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH);
			MApp_ZUI_ACT_GetCCAIrdMailContentText(_lbnavboxlistmailitem.cur_line, gau8Mail, sizeof(gau8Mail));
			MApp_ZUI_ACT_AppShowCCAIrdMailInfo_MailContent();
			return TRUE;
		}
        case VK_EXIT:
        {
            MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_MENU);
            return TRUE;
        }			
        case VK_MENU:
            MApp_ZUI_UTL_PageSwitch(E_OSD_MAIN_MENU);
            return TRUE;

        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCAMailInfoAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Content
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Content(VIRTUAL_KEY_CODE key)
{
    ACTCCAMAILINFO_DBG("_MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Content. Key = %ld\n", key);

    switch (key)
    {
        case VK_DOWN:
        case VK_RIGHT:
        case VK_PAGE_DOWN:
        {
            //Get the next selected page of the current email
            if ((gu8MailCurrentPage + 1) < gu8MailTotalPage)
            {
                gu8MailCurrentPage++;
            }
            else
            {
                // Do nothing
                return TRUE;
            }

            MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_BG, SW_HIDE);
            MApp_ZUI_ACT_AppShowCCAIrdMailInfo_MailContent();
            return TRUE;
        }
        case VK_UP:
        case VK_LEFT:
        case VK_PAGE_UP:
        {
            //Get the preview selected page of the current email
            if (gu8MailCurrentPage > 0)
            {
                gu8MailCurrentPage--;
            }
            else
            {
                // Do nothing
                return TRUE;
            }

            MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_BG, SW_HIDE);
            MApp_ZUI_ACT_AppShowCCAIrdMailInfo_MailContent();

            return TRUE;
        }

        case VK_RED:
        {
            return (_MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelCurrMail());
        }
        case VK_YELLOW:
        {
            return (_MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_DelAllMails());
        }
        case VK_EXIT:
        {
            MApp_ZUI_ACT_AppShowCCAMailInfo();
            return TRUE;
        }
        default:
            return FALSE;
    }
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_HandleCCAIrdMailInfoKey
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_HandleCCAMailInfoKey(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    ACTCCAMAILINFO_DBG("Parent = %ld. Handle = %ld\n", MApp_ZUI_API_GetParent(MApp_ZUI_API_GetFocus()), MApp_ZUI_API_GetFocus());

    switch (key)
    {
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_PAGE_UP:
        case VK_PAGE_DOWN:
        case VK_RED:
        case VK_YELLOW:
        case VK_SELECT:
        case VK_EXIT:
            switch (MApp_ZUI_API_GetParent(MApp_ZUI_API_GetFocus()))
            {
                case HWND_MENU_CCA_EMAIL_INFO_MAIL1:
                case HWND_MENU_CCA_EMAIL_INFO_MAIL2:
                case HWND_MENU_CCA_EMAIL_INFO_MAIL3:
                case HWND_MENU_CCA_EMAIL_INFO_MAIL4:
                case HWND_MENU_CCA_EMAIL_INFO_MAIL5:
                case HWND_MENU_CCA_EMAIL_INFO_MAIL6:
                case HWND_MENU_CCA_EMAIL_INFO_MAIL7:
                     return _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Info(key);
                case HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_BG:
                     return _MApp_ZUI_ACT_HandleCCAIrdMailInfoKey_Content(key);
                case HWND_MENU_CCA_EMAIL_INFO_NOEMAIL_BG:
                     MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_MENU);
                     return TRUE;
                default:
                     ACTCCAMAILINFO_DBG("Unexpected Handle\n");
                     return FALSE;
            }
            return FALSE;
        case VK_MENU:
            MApp_ZUI_UTL_PageSwitch(E_OSD_MAIN_MENU);
            return TRUE;

        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCAMailInfoAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}





///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_TerminateCCAMailInfo
///  [OSD page handler] terminate MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_TerminateCCAMailInfo(void)
{
    ACTCCAMAILINFO_DBG("Terminate CCA MailInfo\n");

    //FIXME: MApp_SaveGenSetting();
    //enCAmenuState = _enTargetMenuState;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_ExecuteCCAMailInfoAction
///  [OSD page handler] execute a specific action in MENU application
///
///  @param [in]       act U16      action ID
///
///  @return BOOLEAN     true for accept, false for ignore
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_ExecuteCCAMailInfoAction(U16 act)
{
    switch (act)
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
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_DtSum
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_DtSum(void)
{
    UINT8 au8Buf[32];
    UINT8 u8Sum = MApp_CCAUI_CasCmd_GetMailMaxItem();

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "%d", u8Sum);
    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_NewMail
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_NewMail(void)
{
    UINT8 au8Buf[32];
    UINT8 u8Sum = MApp_CCAUI_CasCmd_GetNewMailItemNumber();  //FIXME: get the correct number of new emails

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "%d", u8Sum);
    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_Info
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_Info(void)
{
    UINT8 au8Buf[32];

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "Info");
    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_MailItem
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_MailItem(UINT8 u8Index)
{
    UINT16 u16CurrLine = u8Index + _lbnavboxlistmailitem.view_start_line;

    MApi_U8StringToU16String(&gau8Item[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH], gu16UnicodeStr, strlen(&gau8Item[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH]));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_MailContent
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_MailContent(void)
{
    UINT8 au8Buf[64];
    UINT32 u32PageStartAddr = 0;

    if (gu8MailCurrentPage >= gu8MailTotalPage)
    {
        snprintf((char*)au8Buf, sizeof(au8Buf), "Wrong email page is set!");
        MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));
    }
    else
    {
        ACTCCAMAILINFO_DBG("_MApp_ZUI_ACT_GetCCAIrdMailInfoDynamicText_MailContent\n");
        u32PageStartAddr = gu8MailCurrentPage * CCA_MAIL_SIZE_PER_PAGE;
        MApi_U8StringToU16String(&gau8Mail[u32PageStartAddr], gu16UnicodeStr, CCA_MAIL_SIZE_PER_PAGE);
    }

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_CurrPage
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_CurrPage(void)
{
    UINT8 au8Buf[8];
    UINT8 u8CurrPage = MApp_ZUI_ACT_GetMailCurrentPageValue();  //FIXME: get the correct number of new emails

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "%d", u8CurrPage);

    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_TotalPage
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_TotalPage(void)
{
    UINT8 au8Buf[8];
    UINT8 u8TotalPage = MApp_ZUI_ACT_GetMailTotalPage();  //FIXME: get the correct number of new emails

    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((char*)au8Buf, sizeof(au8Buf), "%d", u8TotalPage);

    MApi_U8StringToU16String(au8Buf, gu16UnicodeStr, strlen(au8Buf));

    return ((LPTSTR)gu16UnicodeStr);
}



///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCAMailInfoDynamicText
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]       hwnd HWND     window handle we are processing
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/8
///////////////////////////////////////////////////////////////////////////////
LPTSTR MApp_ZUI_ACT_GetCCAMailInfoDynamicText(HWND hwnd)
{
    switch(hwnd)
    {
        case HWND_MENU_CCA_EMAIL_INFO_DT_SUM:
        {
            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_DtSum();
        }
        case HWND_MENU_CCA_EMAIL_INFO_DT_NEWEMAIL:
        {
            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_NewMail();
        }
        case HWND_MENU_CCA_EMAIL_INFO_INFO0_BG1:
        {
            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_Info();
        }

        case HWND_MENU_CCA_EMAIL_INFO_MAIL1_TEXT:
        case HWND_MENU_CCA_EMAIL_INFO_MAIL2_TEXT:
        case HWND_MENU_CCA_EMAIL_INFO_MAIL3_TEXT:
        case HWND_MENU_CCA_EMAIL_INFO_MAIL4_TEXT:
        case HWND_MENU_CCA_EMAIL_INFO_MAIL5_TEXT:
        case HWND_MENU_CCA_EMAIL_INFO_MAIL6_TEXT:
        case HWND_MENU_CCA_EMAIL_INFO_MAIL7_TEXT:
        {
            UINT8 u8PageIndex = (hwnd - HWND_MENU_CCA_EMAIL_INFO_MAIL1_TEXT) / 5;

            if (HWND_MENU_CCA_EMAIL_INFO_MAIL1_TEXT == hwnd)
            {
                if (_lbnavboxlistmailitem.total_lines == 0)
                {
                    return MApp_ZUI_API_GetString(en_str_NoInformation);
                }
            }

            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_MailItem(u8PageIndex);
        }
        case HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_DYNTXT:
        {
            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_MailContent();
        }
        case HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_CURPAGE_VALUE:
        {
            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_CurrPage();
        }
        case HWND_MENU_CCA_EMAIL_INFO_EMAIL_CONTENT_TOTALPAGE_VALUE:
        {
            return _MApp_ZUI_ACT_GetCCAMailInfoDynamicText_TotalPage();
        }

        default:
            return NULL;
    }

    return NULL;
}

/////////////////////////////////////////////////////////
// Customize Window Procedures

#undef MAPP_ZUI_ACTCCAMAILINFO_C

