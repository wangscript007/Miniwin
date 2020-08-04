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

#define MAPP_ZUI_ACTCCAIRD_C


//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------

#include "HbCommon.h"
#include "DataType.h"

#include "MApp_ZUI_Main.h"
#include "MApp_ZUI_APIcommon.h"
#include "MApp_ZUI_APIstrings.h"
#include "MApp_ZUI_APIwindow.h"
#include "MApp_ZUI_APItables.h"
#include "MApp_ZUI_APIgdi.h"
#include "MApp_ZUI_APIcontrols.h"
#include "MApp_ZUI_APIdraw.h"
#include "MApp_ZUI_ACTglobal.h"
#include "ZUI_exefunc.h"
#include "MApp_ZUI_Utility.h"
#include "MApp_SystemInfo.h"
#include "MApp_ZUI_POPsaving.h"
#include "MApp_ZUI_Resource.h"
#include "MApp_ZUI_POP.h"
#include "MApp_ZUI_GDISetup.h"
#include "apiDTVDataManager.h"
#include "MApp_ServiceInfo.h"
#include "MApp_ZUI_ACTtvbanner.h"
#include "appTvMain_dvb.h"
#include "MApp_SaveData.h"
#include "MApp_Pvr.h"
#include "MApp_ZUI_ACTMediaSimpleStart_PVR.h"

#include "apiOsdRender.h"
#include "Memory64.h"
#include "../../../../mw/include/apiMvf_types.h"
#include "../../../../mw/include/apiMvf.h"
#include "apiPNL.h"
#include "apiXC.h"
#include "drvTVEncoder.h"
#include "MVF_en.h"

#include "MApp_ZUI_ACTCCAIrd.h"


/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef ACTCCAIRD_DEBUG
#define ACTCCAIRD_DBG(fmt, arg...)   {printf("\033[0;35m[ACT CCA IRD STATUS]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define ACTCCAIRD_DBG(fmt, arg...)
#endif


typedef struct
{
    RECT prePopMsgRect;
    BOOL flashing;
	UINT8 gau8TempStrMsg[512];
}CCAPopInfo;


enum
{
    E_STATE_HIDE,
    E_STATE_FINGERPRINT,
    E_STATE_TMS,
    E_STATE_FORCED_MSG,
    E_STATE_PIN,
    E_state_IPPV
};



typedef struct
{
    EN_GOP_DST_TYPE GopDest;
    UINT8 u8WinId;
    UINT8 u8FBId;
    UINT16 u16BmpWidth;
    UINT16 u16BmpHeight;
	UINT16 u16BmpPitch;
	UINT16 u16BmpOffset;
	UINT32 u32BmpAddr;
    UINT16 u16BmpFmt;
    UINT16 u16_X;
    UINT16 u16_Y;
}GWinInfo;

#define CAS_PIN_CODE_LENGTH       4
#define CAS_FLASH_TIME            1000

#define BOX_W_GAP                 20
#define BOX_H_GAP                 10
#define BITMAP_W_SIZE             80
#define BITMAP_H_SIZE             34
#define BITMAP_H_GAP              (BOX_H_GAP * 2)
#define H_SIZE_DEFULT             30
#define H_SIZE_GAP_DEFULT         0    //5
#define BG_BOX_GAP                8

#define W_GWIN_SIZE_DEFULT        1280
#define H_GWIN_SIZE_DEFULT        720

#if (GDI_COLOR_FORMAT == GDI_COLOR_I8)
#define GOP_HD 2
#define GOP_SD 1
#else
#define GOP_HD 0
#define GOP_SD 1
#endif
#define FINGERPRINT_GOP             3       //GOP0:for keltic GOP3:for K1, K5C series
#define FINGERPRINT_CHARACTER_WIDTH 50

#define FINGERPRINT_PALETTE_TABLE_SIZE   9


#if((BOARD_TYPE_SEL == BD_MST183B_D01A)||(BOARD_TYPE_SEL == BD_MST124D_D01B))// Keltic,K1: align 8  K5SN:align 16?
#define PITCH_ALIGN			  8
#else
#define PITCH_ALIGN           16
#endif


#define CAS_DETECT_ERROR_MSG_SHOW MApp_ZUI_API_IsWindowVisible(HWND_TV_BANNER_CENTRAL_POPUP_MSG)
#define CAS_DETECT_MAIL_ICON_SHOW MApp_ZUI_API_IsWindowVisible(HWND_TV_BANNER_MAIL_ICON)
#define CAS_DETECT_ANNOUNCEMENT_ICON_SHOW MApp_ZUI_API_IsWindowVisible(HWND_TV_BANNER_ANNOUNCEMENT_ICON)



/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/

extern UINT32 MApi_GetBOX_CSSN(void);


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/


/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/




GWinInfo gWinInfo = {E_GOP_DST_OP0, 0xFF, 0xFF, 300, 200, 0, 0, 0, E_MS_FMT_RGB565, 0, 0};
BOOL  bFingerWin = FALSE;

static GOP_PaletteEntry _gopI8PaletteEntry[FINGERPRINT_PALETTE_TABLE_SIZE] =
{
    // A      R,    G,    B
    {{ 0x00, 0x00, 0x00, 0x00 }},
    {{ 0x00, 0xff, 0xff, 0xff }},   // white
    {{ 0x00, 0x00, 0xff, 0x00 }},   // green
    {{ 0x00, 0x00, 0x00, 0xff }},   // blue
    {{ 0x00, 0x00, 0xff, 0xff }},   // cyan
    {{ 0x00, 0xff, 0x00, 0x00 }},   // red
    {{ 0x00, 0xff, 0xff, 0x00 }},   // yellow
    {{ 0x00, 0xff, 0x00, 0xff }},   // magenta
    {{ 0x00, 0x08, 0x08, 0x08 }},   // black
};




UINT16  gau16StrBuffer[512];
UINT8   gau8StrMsg[512];

UINT32  gu16TMSPosX           = 0;
UINT32  gu16TMSPosY           = 0;
UINT32  gu16TMSWidth          = 0;
UINT32  gu16TMSHeight         = 0;

UINT32  gu16ErrMsgPosX        = 0;
UINT32  gu16ErrMsgPosY        = 0;
UINT32  gu16ErrMsgWidth       = 0;
UINT32  gu16ErrMsgHeight      = 0;

static UINT8   gu8State              = 0;


//static UINT8   gu8KeyCheck           = 0;
static BOOL gbFlashing            = FALSE;
static UINT32  gu32ForceMsgDelTime   = 0;
static UINT32  gu32Duration          = 0;


static BOOL gbPopFlashing         = FALSE;
static UINT32  gu32PopForceMsgDelTime= 0;
static UINT32  gu32PopDuration		 = 0;

BOOL gbCCAIrdPopMsgExist	 = FALSE;
static BOOL gbErrMsgInterrupted	 = FALSE;
EN_CCA_POPUP_TYPE  gu16CCACurrentPopMsgType	 = CCAIRD_POP_TYPE_NONE;



static GOP_GwinFBAttr DstFBInfo;
static UINT8 u8Mvf_font_size = 16;  // 20 18 16
static MVF_Face _mvfFace;
static unsigned long u32MVF_Size = MS_MVF_EN_SIZE;
static unsigned char *pMVF_Data = MS_MVF_EN_DATA;



#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
static BOOL gbFingerprintingOption = FALSE;
static stCCAUiDisplayAttribute gDispAttr;
#endif


static CCAPopInfo gS3PopInfo;
static EN_CCA_DISPLAY_PRIORITY gePriority = E_CCA_DISPLAY_PRIORITY_SPARE;






//-------------------------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_GetRandomNum
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
UINT16 MApp_ZUI_ACT_GetRandomNum(void)
{
    return ((MApi_GetBOX_CSSN() & 0xFFFF) + rand());

}

void MApp_ZUI_ACT_PageClean_Change(void)
{
    gePriority   = E_CCA_DISPLAY_PRIORITY_SPARE;
    gbFlashing   = FALSE;
    gu32Duration = FALSE;
    MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_ShowErrorMSG
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////

void MApp_ZUI_ACT_ShowErrorMSG(BOOL bShow, UINT8* pu8Buff)
{
    if (bShow == TRUE)
    {
        memset(gau8StrMsg, 0, sizeof(gau8StrMsg));    
        snprintf((MS_S8*)gau8StrMsg, sizeof(gau8StrMsg), pu8Buff);
        //FIXME MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_IRD_ERROR_MSG, SW_SHOW);
        MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_CENTRAL_POPUP_MSG, SW_SHOW);
    }
    else
    {
        if (CAS_DETECT_ERROR_MSG_SHOW)
        {
            //FIXME MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_IRD_ERROR_MSG, SW_HIDE);
            MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_CENTRAL_POPUP_MSG, SW_SHOW);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_GetSystemOSDCheck
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_ZUI_ACT_GetSystemOSDCheck(void)
{
    BOOL bCheck = TRUE;

	/* //Change popup appearing timing to tvbanner only
	if (MApp_ZUI_GetActiveOSD() >= E_OSD_MENU_S3IRD_CA_MENU && MApp_ZUI_GetActiveOSD() <= E_OSD_MENU_S3IRD_OTA)
    {
        bCheck = FALSE;
    }
    */
    //fix when main menu or audio volume bar osd pop-up, softcell ui is not monitored
    if ((MApp_ZUI_GetActiveOSD() == E_OSD_MAIN_MENU) || (MApp_ZUI_GetActiveOSD() == E_OSD_TV_AUDIOPOPUP))
    {
        return TRUE;
    }

    if ((MApp_ZUI_GetActiveOSD() != E_OSD_TV_PROG) && (MApp_ZUI_GetActiveOSD() != E_OSD_MENU_MEDIA_PVR_PLAY))
    {
		bCheck = FALSE;
    }
	else
	{
	    if(MApp_ZUI_GetState() == E_ZUI_STATE_RUNNING)
        {
            bCheck = TRUE;
        }
        else
        {
            bCheck = FALSE;
        }
	}

    return bCheck;
}




BOOL MApp_ZUI_ACT_DetectCCAIrdMailIcon(void)
{
    if (CAS_DETECT_MAIL_ICON_SHOW)
    {
        //ACTS3IRD_DBG("CAS_DETECT_MAIL_ICON_SHOW = 1 \n");
        return TRUE;
    }
    else
    {
        //ACTS3IRD_DBG("CAS_DETECT_MAIL_ICON_SHOW = 0 \n");
        return FALSE;
    }
}

BOOL MApp_ZUI_ACT_DetectCCAIrdAnncmntIcon(void)
{
    if (CAS_DETECT_ANNOUNCEMENT_ICON_SHOW)
    {
        //ACTS3IRD_DBG("CAS_DETECT_ANNOUNCEMENT_ICON_SHOW = 1 \n");
        return TRUE;
    }
    else
    {
        //ACTS3IRD_DBG("CAS_DETECT_ANNOUNCEMENT_ICON_SHOW = 0 \n");
        return FALSE;
    }
}

BOOL MApp_ZUI_ACT_DetectCCAIrd(void)
{
    E_OSD_ID eOsdId = MApp_ZUI_GetActiveOSD();

    //ACTS3IRD_DBG("DetectS3Ird. ID = %d\n", eOsdId);
//    if (eOsdId == E_OSD_MENU_S3IRD)
//    {
//        return TRUE;
//    }
//    else
    {
        if ((eOsdId == E_OSD_MAIN_MENU)            ||
            (eOsdId == E_OSD_MENU_CCA_CA_STATUS) ||
            (eOsdId == E_OSD_MENU_CCA_CA_MAIL_INFO))
        {
            gePriority = E_CCA_DISPLAY_PRIORITY_MENU;
        }
        else
        {
            gePriority = E_CCA_DISPLAY_PRIORITY_SPARE;
        }
        return FALSE;
    }
}



static void _SetGfxPalette(void)
{
    GFX_PaletteEntry _gePalette[FINGERPRINT_PALETTE_TABLE_SIZE];
    int i;

    HB_printf("Load I8 palette to GFX\n");

    for (i=0; i<FINGERPRINT_PALETTE_TABLE_SIZE; i++)
    {
        _gePalette[i].RGB.u8A =  _gopI8PaletteEntry[i].RGB.u8A;
        _gePalette[i].RGB.u8R =  _gopI8PaletteEntry[i].RGB.u8R;
        _gePalette[i].RGB.u8G =  _gopI8PaletteEntry[i].RGB.u8G;
        _gePalette[i].RGB.u8B =  _gopI8PaletteEntry[i].RGB.u8B;
    }
    //Set GE palette table when bitblt from I8 to ARGB
    if(GFX_SUCCESS != MApi_GFX_SetPaletteOpt(_gePalette, 0, FINGERPRINT_PALETTE_TABLE_SIZE - 1 ))
    {
        HB_printf("[%s][%d] MApi_GFX_SetPaletteOpt failed\n", __FUNCTION__, __LINE__);
    }

    if(GOP_API_SUCCESS != MApi_GOP_GWIN_SetPaletteOpt(_gopI8PaletteEntry, 0, FINGERPRINT_PALETTE_TABLE_SIZE - 1, E_GOP_PAL_ARGB8888))
    {
        HB_printf("[%s][%d] MApi_GOP_GWIN_SetPaletteOpt failed\n", __FUNCTION__, __LINE__);
    }
}

static void _Init_GOP(UINT8 u8GOP)
{
    
	GOP_InitInfo pGopInit;
    pGopInit.u16PanelWidth = g_IPanel.Width();
    pGopInit.u16PanelHeight = g_IPanel.Height();
    pGopInit.u16PanelHStr = g_IPanel.HStart();
    pGopInit.u32GOPRBAdr = GOP_GWIN_RB_ADR;
    pGopInit.u32GOPRBLen = GOP_GWIN_RB_LEN;
    pGopInit.u32GOPRegdmaAdr = GOP_REGDMABASE_ADR;
    pGopInit.u32GOPRegdmaLen = GOP_REGDMABASE_LEN;
    pGopInit.bEnableVsyncIntFlip = FALSE;
    
    if(GOP_API_SUCCESS == MApi_GOP_InitByGOP(&pGopInit, u8GOP))
	{
		printf("[%s] MApi_GOP_Init GOP %d success!\n", __FUNCTION__,u8GOP);
	}

	MApi_GOP_GWIN_SwitchGOP(u8GOP);
    MApi_GOP_GWIN_SetTransClr(GOPTRANSCLR_FMT0, 0xFF);
    MApi_GOP_GWIN_EnableTransClr(GOPTRANSCLR_FMT0, TRUE);
    MApi_GOP_GWIN_SetGOPDst(u8GOP, E_GOP_DST_OP0);
	

}
static UINT32 _Create_FB(UINT8 *u8FB, UINT16 FBWidth, UINT16 FBHeight, UINT16 FBFmt)
{
    UINT32 clearvalue;
    UINT8  ret;
    // look for available Sdram slot
    *u8FB = MApi_GOP_GWIN_GetFreeFBID();
    if(MAX_GWIN_FB_SUPPORT <= *u8FB)
    {
        return GWIN_NO_AVAILABLE;
    }
    
    if(TRUE == bFingerWin)
    {
        ACTCCAIRD_DBG("[%ld]:previous fingerprint still exist!\n", __LINE__);
        MApi_GOP_GWIN_ReleaseWin(gWinInfo.u8WinId);
        MApi_GOP_GWIN_DestroyFB(gWinInfo.u8FBId);
    }
    
    if ( GWIN_OK != (ret = MApi_GOP_GWIN_CreateFB( *u8FB, gWinInfo.u16_X, gWinInfo.u16_Y, FBWidth, FBHeight, FBFmt)) )
    {
        printf("[%s] MApi_GOP_GWIN_CreateFB: failed to create FB, ret:[%d] \n", __FUNCTION__, ret);
        return GWIN_NO_AVAILABLE;
    }

    MApi_GOP_GWIN_GetFBInfo( *u8FB, &DstFBInfo );

    ACTCCAIRD_DBG("fingerprint FB[%d].gWinId = %d\n", *u8FB, DstFBInfo.gWinId);
    ACTCCAIRD_DBG("fingerprint FB[%d].enable = %d\n", *u8FB, DstFBInfo.enable);
    ACTCCAIRD_DBG("fingerprint FB[%d].allocated = %d\n", *u8FB, DstFBInfo.allocated);
    ACTCCAIRD_DBG("fingerprint FB[%d].x0 = %d\n", *u8FB, DstFBInfo.x0);
    ACTCCAIRD_DBG("fingerprint FB[%d].y0 = %d\n", *u8FB, DstFBInfo.y0);
    ACTCCAIRD_DBG("fingerprint FB[%d].x1 = %d\n", *u8FB, DstFBInfo.x1);
    ACTCCAIRD_DBG("fingerprint FB[%d].y1 = %d\n", *u8FB, DstFBInfo.y1);
    ACTCCAIRD_DBG("fingerprint FB[%d].width = %d\n", *u8FB, DstFBInfo.width);
    ACTCCAIRD_DBG("fingerprint FB[%d].height = %d\n", *u8FB, DstFBInfo.height);
    ACTCCAIRD_DBG("fingerprint FB[%d].pitch = %d\n", *u8FB, DstFBInfo.pitch);
    ACTCCAIRD_DBG("fingerprint FB[%d].fbFmt = %d\n", *u8FB, DstFBInfo.fbFmt);
    ACTCCAIRD_DBG("fingerprint FB[%d].addr = %d\n", *u8FB, DstFBInfo.addr);
    ACTCCAIRD_DBG("fingerprint FB[%d].size = %d\n", *u8FB, DstFBInfo.size);
    ACTCCAIRD_DBG("fingerprint FB[%d].next = %d\n", *u8FB, DstFBInfo.next);
    ACTCCAIRD_DBG("fingerprint FB[%d].prev = %d\n", *u8FB, DstFBInfo.prev);
    ACTCCAIRD_DBG("fingerprint FB[%d].string = %d\n", *u8FB, DstFBInfo.string);
    ACTCCAIRD_DBG("fingerprint FB[%d].s_x = %d\n", *u8FB, DstFBInfo.s_x);
    ACTCCAIRD_DBG("fingerprint FB[%d].s_y = %d\n", *u8FB, DstFBInfo.s_y);
    ACTCCAIRD_DBG("fingerprint FB[%d].dispWidth = %d\n", *u8FB, DstFBInfo.dispWidth);
    ACTCCAIRD_DBG("fingerprint FB[%d].dispHeight = %d\n", *u8FB, DstFBInfo.dispHeight);

    if( (FBFmt == (UINT16)E_MS_FMT_YUV422) || (FBFmt == (UINT16)E_MS_FMT_RGB565) )  // "the color of the background"
    {

        UINT8 RR = 128;
        UINT8 GG = 8;
        UINT8 BB = 128;

		//RGB565 to 24-bit RGB888
		UINT32 R5 = ( RR * 249 + 1000 ) >> 11;  
		UINT32 G6 = ( GG * 253 +  500 ) >> 10;
		UINT32 B5 = ( BB * 249 + 1000 ) >> 11;

        RR = R5;
        GG = G6;
        BB = B5;
        
        clearvalue = RR;
        clearvalue = clearvalue<<6 | GG;
        clearvalue = clearvalue<<5 | BB;

        clearvalue = (clearvalue<<16)|clearvalue;

        //printf(" color 0x%x\n",color);

        MApi_GFX_ClearFrameBufferByWord(DstFBInfo.addr, DstFBInfo.size, clearvalue);
    }
    else
    {
        MApi_GFX_ClearFrameBuffer(DstFBInfo.addr, DstFBInfo.size, 0x7);
    }

    MApi_GFX_FlushQueue();

    return 0;
}

static UINT32 _Create_GWin(UINT8 u8GOP, UINT8 *u8GWin, UINT8 u8FB, UINT16 FBFmt)
{
    
    *u8GWin = MApi_GOP_GWIN_CreateWin_Assign_FB(u8GOP, u8FB, gWinInfo.u16_X, gWinInfo.u16_Y);

    if(*u8GWin == GWIN_NO_AVAILABLE)
    {
        printf("MApi_GOP_GWIN_CreateWin_Assign_FB fail\n");
        return GWIN_NO_AVAILABLE;
    }
#if(BOARD_TYPE_SEL == BD_MST183B_D01A)// Keltic: VE   K5 series,K1:OP
    VE_DrvStatus DrvStatus;
    MDrv_VE_GetStatus(&DrvStatus);

    if(DrvStatus.VideoSystem <= 3)
    {
        MApi_GOP_GWIN_Set_STRETCHWIN(FINGERPRINT_GOP, E_GOP_DST_MIXER2VE, 0, 0, 720, 480);
    }
    else
    {
        MApi_GOP_GWIN_Set_STRETCHWIN(FINGERPRINT_GOP, E_GOP_DST_MIXER2VE, 0, 0, 720, 576);
    }

#endif
   
    //set transparent color
    //0xFF: black and cannot see background video
    //0x80: half transparent
    if( FBFmt == E_MS_FMT_ARGB8888 || FBFmt == E_MS_FMT_ARGB4444)
    {
        MApi_GOP_GWIN_SetBlending( *u8GWin, TRUE, 0x80);
    }
    else
    {
        MApi_GOP_GWIN_SetBlending( *u8GWin, FALSE, 0x80);
    }

    _SetGfxPalette();

    return 0;
}

static BOOL _CreateWin(UINT8 u8GOP, UINT16 u16BmpWidth, UINT16 u16BmpHeight, UINT16 u16BmpPitch, UINT16 u16BmpFmt)
{
    UINT8 u8GWin = 0;
    UINT8 u8FB = 0;
    UINT16 FBWidth = u16BmpWidth;
    UINT16 FBHeight = u16BmpHeight;
    UINT16 FBFmt = u16BmpFmt;

    if( 0 != _Create_FB(&u8FB, FBWidth, FBHeight, FBFmt))
    {
         printf("Create FB fail\n");
         return FALSE;
    }
    gWinInfo.u8FBId = u8FB;

    if( 0 != _Create_GWin(u8GOP, &u8GWin, u8FB, FBFmt))
    {
         printf("Create GWin fail, GOPnum = %d, u8GWin = %d, FBId = %d, FBFmt = %d\n", u8GOP,  u8GWin, u8FB, FBFmt);
         return FALSE;
    }
    gWinInfo.u8WinId = u8GWin;

    printf("[%s] Create GWin ok, GOPnum = %d, u8GWin = %d, FBId = %d, FBFmt = %d,\n", __FUNCTION__, u8GOP,  u8GWin, u8FB, FBFmt);

    return TRUE;
}

BOOL _PrepareDraw(UINT16 u16BmpWidth, UINT16 u16BmpHeight, UINT16 u16BmpPitch, UINT16 u16BmpFmt)
{
    UINT8 u8Gop = FINGERPRINT_GOP;

    _Init_GOP(u8Gop);

	if(FALSE == _CreateWin(u8Gop, u16BmpWidth, u16BmpHeight, u16BmpPitch, u16BmpFmt))
	{
		printf("[%s] _CreateWin ERROR\n", __FUNCTION__);
		return FALSE;
	}
    //set it to default for later decision by coverage percent
    u8Mvf_font_size = 80;
    bFingerWin = TRUE;

    return TRUE;
}


BOOL MApp_ZUI_ACT_FingerprintFont_Init(void)
{
    MVF_Error err;

    err = MVF_New_Face(pMVF_Data, pMVF_Data+u32MVF_Size, &_mvfFace);
    if ( err )
    {
        HB_printf("MApi_VectorFont_Init() -> MVF_New_Face error! and err = %d\n",err);
        return (FALSE);
    }

    return (TRUE);
}

void MApp_ZUI_ACT_SetPopMsgType(EN_CCA_POPUP_TYPE msgType)
{
	gu16CCACurrentPopMsgType = msgType;
}

EN_CCA_POPUP_TYPE MApp_ZUI_ACT_GetPopMsgType(void)
{
	return gu16CCACurrentPopMsgType;
}


BOOL MApp_ZUI_ACT_IfCCAPopMsgExist(void)
{
	if(gbCCAIrdPopMsgExist)
	{
		return TRUE;
	}
	else
	{
		return FALSE;

	}
}

UINT32 MApp_ZUI_ACT_GetCCAPopMsgDuration(void)
{
	return gu32PopDuration;
}




#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
BOOL MApp_ZUI_ACT_StoreEnhancedOvertFingerprint(stIrdFPOptionsTLV* pstFpoTlv)
{
    return TRUE;
}

BOOL MApp_ZUI_ACT_StoreCCAIrdFingerprintingOption(stCCAUiDisplayAttribute* pstAttribute)
{
    memcpy( &gDispAttr, pstAttribute, sizeof(stCCAUiDisplayAttribute));
    gbFingerprintingOption = TRUE;
    return TRUE;
}
#endif

BOOL MApp_ZUI_ACT_ShowCCAIrdFingerPrint(stCCAUiDisplayAttribute* pstAttribute)
{
    int i = 0, index = 0;

    BOOL bCheck = FALSE;
    UINT16  x, y, offset;

    MVF_Bitmap bitmap;
    MVF_Outline outline;
    GFX_BufferInfo srcbuf, dstbuf;
    GFX_DrawRect bltblock;
    GFX_RgbColor color;

    // Set the priority
    gePriority = E_CCA_DISPLAY_PRIORITY_FINGERPRINT;

#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
    if(gbFingerprintingOption == TRUE)
    {
        //use fingerprinting option values
        pstAttribute = &gDispAttr;
        //restore
        gbFingerprintingOption = FALSE;
    }
#endif

    gu32Duration = pstAttribute->u32Duration;
    gbFlashing   = !(pstAttribute->bFlashing);
    gu32ForceMsgDelTime = gu32Duration + MApi_DateTime_Get();

    ACTCCAIRD_DBG("gu32Duration          = %ld\n", gu32Duration);
    ACTCCAIRD_DBG("MApi_DateTime_Get()   = %ld\n", MApi_DateTime_Get());
    ACTCCAIRD_DBG("gu32ForceMsgDelTime   = %ld\n", gu32ForceMsgDelTime);

    gWinInfo.u16_X        = pstAttribute->u16PosX;
    gWinInfo.u16_Y        = pstAttribute->u16PosY;
    gWinInfo.u16BmpWidth  = pstAttribute->u16Width;
    gWinInfo.u16BmpHeight = pstAttribute->u16Height;
    //gWinInfo.u16BmpPitch  = 1000;

    ACTCCAIRD_DBG("gWinInfo.u16_X        = %ld\n", gWinInfo.u16_X);
    ACTCCAIRD_DBG("gWinInfo.u16_Y        = %ld\n", gWinInfo.u16_Y);
    ACTCCAIRD_DBG("gWinInfo.u16BmpWidth  = %ld\n", gWinInfo.u16BmpWidth);
    ACTCCAIRD_DBG("gWinInfo.u16BmpHeight = %ld\n", gWinInfo.u16BmpHeight);
    ACTCCAIRD_DBG("gWinInfo.u16BmpPitch = %ld\n", gWinInfo.u16BmpPitch);

    memset(gau8StrMsg, 0, 512);

    while (pstAttribute->au8StrBuf[i] && (i < sizeof(pstAttribute->au8StrBuf)))
    {
        gau8StrMsg[i] = pstAttribute->au8StrBuf[i];
        ACTCCAIRD_DBG("%c\n", gau8StrMsg[i]);
        i++;
    }

    bCheck = _PrepareDraw(gWinInfo.u16BmpWidth, gWinInfo.u16BmpHeight, gWinInfo.u16BmpPitch, gWinInfo.u16BmpFmt);
    if(bCheck == FALSE)
    {
    	ACTCCAIRD_DBG("Finger Printf PrepareDraw  Fail")
        return bCheck;
    }

    u8Mvf_font_size = (u8Mvf_font_size * pstAttribute->u8Percent)/100;


//	offset = (strlen(gau8StrMsg)/2) * (u8Mvf_font_size);
//    x = gWinInfo.u16BmpWidth/2 - offset + (u8Mvf_font_size/2);
//    y = gWinInfo.u16BmpHeight/2 - (u8Mvf_font_size/2);

	x = y = 0;

	offset = gWinInfo.u16BmpWidth / (strlen(gau8StrMsg)- 1);




    
    color.g = color.b = 0; //background color
    MApi_GFX_SetSrcColorKey(TRUE, CK_OP_EQUAL, GFX_FMT_I8, &color, &color);
    MApi_GFX_SetIntensity(0, GFX_FMT_I8, (UINT32*)&color); //background


	i = 1;	
	#if 1
	while( i <= 15 ) //GFX_FMT_I4 mode		"the color of the word"
	{
		color.r = 0xEE;
		color.g = 0x00;
        color.b = 0x00;
		MApi_GFX_SetIntensity(i, GFX_FMT_RGB565, (UINT32*)&color);
		i++;
	}
	#endif

    while(index < strlen(gau8StrMsg)- 1)
    {
        memset(&outline, 0, sizeof(MVF_Outline));
        memset(&bitmap,0,sizeof(MVF_Bitmap));

        MApi_GFX_BeginDraw();

        if(0 != MVF_Load_Glyph(&_mvfFace, (UINT16)gau8StrMsg[index], &outline))
        {
            HB_printf("[%s][%d] MVF_Load_Glyph failed\n", __FUNCTION__, __LINE__);
        }
        if(0 != MVF_Render(&_mvfFace, &outline, u8Mvf_font_size, (UINT16)gau8StrMsg[index], FALSE, MVF_BITMAP_FORMAT_I4, &bitmap))
        {
            HB_printf("[%s][%d] MVF_Render failed\n", __FUNCTION__, __LINE__);
        }

#if 0
        HB_printf("fingerprint bitmap.rows = %d\n", bitmap.rows);
        HB_printf("fingerprint bitmap.width = %d\n", bitmap.width);
#endif


        srcbuf.u32ColorFmt = GFX_FMT_I4;
        srcbuf.u32Addr = ((UINT32)bitmap.buffer);
        srcbuf.u32Pitch = (UINT32)(bitmap.pitch);
        srcbuf.u32Width = bitmap.width;
        srcbuf.u32Height = bitmap.rows;

        MsOS_Dcache_Flush(srcbuf.u32Addr,(srcbuf.u32Height)*(srcbuf.u32Width)*(srcbuf.u32Pitch));
        MsOS_FlushMemory();

        if( GFX_SUCCESS != MApi_GFX_SetSrcBufferInfo(&srcbuf, 0))
        {
            HB_printf("[%s][%d] MApi_GFX_SetSrcBufferInfo failed\n", __FUNCTION__, __LINE__);
        }

        MApi_GOP_GWIN_SwitchGOP( FINGERPRINT_GOP );
        MApi_GOP_GWIN_Switch2Gwin( gWinInfo.u8WinId );  //set gop gwin address for ge render

        dstbuf.u32ColorFmt = E_MS_FMT_RGB565;
        dstbuf.u32Addr = DstFBInfo.addr;
		// K1 pitch can not pixel aligned but 32 bits aligned
        dstbuf.u32Pitch = ((DstFBInfo.pitch)/(PITCH_ALIGN))*(PITCH_ALIGN);//DstFBInfo.pitch;
        dstbuf.u32Width = bitmap.width;
        dstbuf.u32Height = bitmap.rows;
        if( GFX_SUCCESS != MApi_GFX_SetDstBufferInfo(&dstbuf, 0))
        {
            HB_printf("[%s][%d] MApi_GFX_SetDstBufferInfo failed\n", __FUNCTION__, __LINE__);
        }

		x = (offset - bitmap.width)/2 + index*offset;
		y = (gWinInfo.u16BmpHeight - bitmap.rows)/2;


        bltblock.srcblk.x = 0x0;
        bltblock.srcblk.y = 0x0;
        bltblock.srcblk.width = bitmap.width;
        bltblock.srcblk.height = bitmap.rows;
        //bltblock.dstblk.x = x + (u8Mvf_font_size*2 - bitmap.width)/2;
        //x = bltblock.dstblk.x;
		bltblock.dstblk.x = x;
		bltblock.dstblk.y = y;//fontSize-bitmap->y_shift + offset;
        bltblock.dstblk.width = bitmap.width;
        bltblock.dstblk.height = bitmap.rows;

        //* fire GE bitblt *//
        UINT8 ret;
        ret = MApi_GFX_BitBlt(&bltblock, GFXDRAW_FLAG_SCALE); //GFXDRAW_FLAG_DEFAULT|
        if (ret != GFX_SUCCESS)
        {
            HB_printf("\n MApi_GFX_BitBlt...............%d, ret =%d char_code = %x\n", __LINE__, ret, (UINT16)gau8StrMsg[index]);
        }

        MApi_GFX_EndDraw();

        index++;
    }

    if( GOP_API_SUCCESS != MApi_GOP_GWIN_MapFB2Win(gWinInfo.u8FBId, gWinInfo.u8WinId) )
    {
        HB_printf("[%s][%d] MApi_GOP_GWIN_MapFB2Win failed\n", __FUNCTION__, __LINE__);
    }

    if( GOP_API_SUCCESS != MApi_GOP_GWIN_Enable(gWinInfo.u8WinId, TRUE) )
    {
        HB_printf("[%s] MApi_GOP_GWIN_Enable failed", __FUNCTION__);
    }

    return bCheck;
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_GetS3IrdPriorityDisplayTicket
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_ZUI_ACT_GetCCAIrdPriorityDisplayTicket(EN_CCA_DISPLAY_PRIORITY ePriority)
{
    ACTCCAIRD_DBG("\n Current = %d, New = %d\n", gePriority, ePriority);
    if (ePriority <= gePriority)
    {
        // With higher or equal priority
        gePriority = ePriority;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



UINT8 MApp_ZUI_ACT_GetIrdShowPage(void)
{
    ACTCCAIRD_DBG("\nGetIrdShowPage = %d\n", gu8State);
    return gu8State;
}

void MApp_ZUI_ACT_SetIrdShowPage(UINT8 u8Index)
{
    ACTCCAIRD_DBG("\nSetIrdShowPage = %d\n", u8Index);

    gu8State = u8Index;
}



void MApp_ZUI_ACT_ResetPopMsgStatus(void)
{
	gu32PopForceMsgDelTime = 0;
	gePriority	 = E_CCA_DISPLAY_PRIORITY_SPARE;
	gbPopFlashing	 = FALSE;
	gu32PopDuration = 0;
	gbCCAIrdPopMsgExist = FALSE;
	gbErrMsgInterrupted = FALSE;
	gu16CCACurrentPopMsgType = CCAIRD_POP_TYPE_NONE;
}




BOOL MApp_ZUI_ACT_ShowCCAIrdTMS(stCCAUiDisplayAttribute* pstAttribute, EN_CCA_ERROR_TYPE eErrType, EN_CCA_POPUP_TYPE popMsgType)
{
    int i = 0;
    BOOL bCheck = FALSE;

	ACTCCAIRD_DBG("popMsgType = %d\n", popMsgType);

	if(popMsgType == CCAIRD_POP_TYPE_ERR_MSG_D)
	{
		if(gbCCAIrdPopMsgExist == TRUE && gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_ERR_MSG_E)
		{
			ACTCCAIRD_DBG("E type msg exist, D type msg comes\n");
			MApp_ZUI_POP_Disable();
			MApp_ZUI_ACT_ResetPopMsgStatus();
		}
        else if(gbCCAIrdPopMsgExist == TRUE && gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_TMS)
        {
            ACTCCAIRD_DBG("TMS type msg exist, D type msg comes\n");
        }
        else
        {
            ACTCCAIRD_DBG("Warning! Not Handled Message Type: [%d]\n", popMsgType);
        }
        return TRUE;
	}
	else if(popMsgType == CCAIRD_POP_TYPE_ERR_MSG_E && gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_TMS)
	{
		gu16TMSPosX   = pstAttribute->u16PosX;
		gu16TMSPosY   = pstAttribute->u16PosY;
		gu16TMSWidth  = pstAttribute->u16Width;
		gu16TMSHeight = pstAttribute->u16Height;
		gbPopFlashing = FALSE;

		while (pstAttribute->au8StrBuf[i])
		{
			gau8StrMsg[i] = pstAttribute->au8StrBuf[i];
			i++;
		}

		gbErrMsgInterrupted = TRUE;
		memset(&gS3PopInfo, 0, sizeof(gS3PopInfo));
		gS3PopInfo.prePopMsgRect.left = gu16TMSPosX;
		gS3PopInfo.prePopMsgRect.top = gu16TMSPosY;
		gS3PopInfo.prePopMsgRect.width = gu16TMSWidth;
		gS3PopInfo.prePopMsgRect.height = gu16TMSHeight;
		gS3PopInfo.flashing = gbPopFlashing;
		strncpy(gS3PopInfo.gau8TempStrMsg, gau8StrMsg, strlen(gau8StrMsg));
		return TRUE;

	}
	else if(popMsgType == CCAIRD_POP_TYPE_TMS && gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_ERR_MSG_E)
	{
		gbErrMsgInterrupted = TRUE;
		memset(&gS3PopInfo, 0, sizeof(gS3PopInfo));
		gS3PopInfo.prePopMsgRect.left = gu16TMSPosX;
		gS3PopInfo.prePopMsgRect.top = gu16TMSPosY;
		gS3PopInfo.prePopMsgRect.width = gu16TMSWidth;
		gS3PopInfo.prePopMsgRect.height = gu16TMSHeight;
		gS3PopInfo.flashing = gbPopFlashing;
		strncpy(gS3PopInfo.gau8TempStrMsg, gau8StrMsg, strlen(gau8StrMsg));
	}
    else
	{
        ACTCCAIRD_DBG("Warning! Not Handled Message Type: [%d]\n", popMsgType);
    }

	gu16CCACurrentPopMsgType = popMsgType;

	ACTCCAIRD_DBG("gu16CCACurrentPopMsgType = %d\n", gu16CCACurrentPopMsgType);

    // Set the priority
    gePriority = eErrType + E_CCA_DISPLAY_PRIORITY_GENERAL_ERROR_BANNERS;

    if (MApp_ZUI_ACT_GetCCAIrdPriorityDisplayTicket(gePriority) == FALSE)
    {
        return FALSE;
    }

	if(popMsgType == CCAIRD_POP_TYPE_TMS)
	{
    	gu32PopDuration = pstAttribute->u32Duration;
		gbPopFlashing	= !(pstAttribute->bFlashing); //.0217
	}
    else
    {
		gu32PopDuration = 0; // 'E' msg will last forever util 'D' msg comes.
		gbPopFlashing = FALSE;
    }
    //gbPopFlashing   = pstAttribute->bFlashing;
    gu32PopForceMsgDelTime = gu32PopDuration + MApi_DateTime_Get();
    ACTCCAIRD_DBG("gu32PopForceMsgDelTime = %ld\n", gu32PopForceMsgDelTime);

    MApp_ZUI_ACT_SetIrdShowPage(E_STATE_TMS);

    gu16TMSPosX   = pstAttribute->u16PosX;
    gu16TMSPosY   = pstAttribute->u16PosY;
    gu16TMSWidth  = pstAttribute->u16Width;
    gu16TMSHeight = pstAttribute->u16Height;

    memset(gau8StrMsg, '\0', sizeof(gau8StrMsg));

    while (pstAttribute->au8StrBuf[i])
    {
        gau8StrMsg[i] = pstAttribute->au8StrBuf[i];
        i++;
    }

	gbCCAIrdPopMsgExist = TRUE;
	RECT rect = {gu16TMSPosX, gu16TMSPosY, gu16TMSWidth, gu16TMSHeight};
    MApi_U8StringToU16String(gau8StrMsg, gau16StrBuffer, strlen(gau8StrMsg));
	MApp_ZUI_POP_SetPos(rect);
    MApp_ZUI_POP_Enable((UINT8*)gau16StrBuffer, wcslen(gau16StrBuffer) * 2, 0);
    bCheck = TRUE;

    return bCheck;
}

BOOL MApp_ZUI_ACT_ShowCCAIrdForcedMSG(stCCAUiDisplayAttribute* pstAttribute)
{
    int i = 0;
    BOOL bCheck = FALSE;

    if (MApp_ZUI_ACT_GetCCAIrdPriorityDisplayTicket(E_CCA_DISPLAY_PRIORITY_TEXT_MSG) == FALSE)
    {
        return FALSE;
    }

    gu32PopDuration = pstAttribute->u32Duration;
    gbPopFlashing   = pstAttribute->bFlashing;
    gu32PopForceMsgDelTime = gu32PopDuration + MApi_DateTime_Get();
    ACTCCAIRD_DBG("gu32PopForceMsgDelTime = %ld\n", gu32PopForceMsgDelTime);
    ACTCCAIRD_DBG("gu16CCACurrentPopMsgType = %ld\n", gu16CCACurrentPopMsgType);


	if(gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_ERR_MSG_E)
	{
		gbErrMsgInterrupted = TRUE;
		memset(&gS3PopInfo, 0, sizeof(gS3PopInfo));
		gS3PopInfo.prePopMsgRect.left = gu16TMSPosX;
		gS3PopInfo.prePopMsgRect.top = gu16TMSPosY;
		gS3PopInfo.prePopMsgRect.width = gu16TMSWidth;
		gS3PopInfo.prePopMsgRect.height = gu16TMSHeight;
		gS3PopInfo.flashing = gbPopFlashing;
		strncpy(gS3PopInfo.gau8TempStrMsg, gau8StrMsg, strlen(gau8StrMsg));
	
	}


    MApp_ZUI_ACT_SetIrdShowPage(E_STATE_FORCED_MSG);

    gu16ErrMsgPosX   = pstAttribute->u16PosX;
    gu16ErrMsgPosY   = pstAttribute->u16PosY;
    gu16ErrMsgWidth  = pstAttribute->u16Width;
    gu16ErrMsgHeight = pstAttribute->u16Height;

    memset(gau8StrMsg, '\0', sizeof(gau8StrMsg));

    while (pstAttribute->au8StrBuf[i])
    {
        gau8StrMsg[i] = pstAttribute->au8StrBuf[i];
        i++;
    }

    ACTCCAIRD_DBG("\nString length = %d\n", i);    
    
    {
		#if 0 // Replace S3IRD windows by using Popup msgbox to show msg
        MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_S3IRD);
		#else
		gbCCAIrdPopMsgExist = TRUE;
		gu16CCACurrentPopMsgType = CCAIRD_POP_TYPE_FORCE_MSG;
		RECT rect = {gu16ErrMsgPosX, gu16ErrMsgPosY, gu16ErrMsgWidth, gu16ErrMsgHeight};
        MApi_U8StringToU16String(gau8StrMsg, gau16StrBuffer, strlen(gau8StrMsg));
		MApp_ZUI_POP_SetPos(rect);
        MApp_ZUI_POP_Enable((UINT8*)gau16StrBuffer, wcslen(gau16StrBuffer) * 2, 0);
		#endif
        bCheck = TRUE;
    }


    return bCheck;
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_ShowCCAIrdMailIcon
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////

void MApp_ZUI_ACT_ShowCCAIrdMailIcon(BOOL bShow)
{

	if(MApp_ZUI_GetActiveOSD() == E_OSD_MAIN_MENU)
	{
		return;  // in main menu, not deal with the mail or annc icon
	}
		


    if (bShow == TRUE)
    {
        if (CAS_DETECT_MAIL_ICON_SHOW == FALSE)
        {
            MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_MAIL, SW_SHOW);
        }
    }
    else
    {
        if (CAS_DETECT_MAIL_ICON_SHOW)
        {

            MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_MAIL, SW_HIDE);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_ShowCCAIrdAnnouncementIcon
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////

void MApp_ZUI_ACT_ShowCCAIrdAnnouncementIcon(BOOL bShow)
{
	if(MApp_ZUI_GetActiveOSD() == E_OSD_MAIN_MENU)
	{
		return;  // in main menu, not deal with the mail or annc icon
	}


    if (bShow == TRUE)
    {
        if (CAS_DETECT_ANNOUNCEMENT_ICON_SHOW == FALSE)
        {
            MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_ANNOUNCEMENT, SW_SHOW);
        }
    }
    else
    {
        if (CAS_DETECT_ANNOUNCEMENT_ICON_SHOW)
        {
            MApp_ZUI_API_ShowWindow(HWND_TV_BANNER_ANNOUNCEMENT, SW_HIDE);
        }
    }
}




///////////////////////////////////////////////////////////////////////////////
///  global  MApp_CCAUI_GetSystemOsdCheck
///
///
///  @param [in]
///
///  @return EN_CCA_UI_CONDITION
///
///  @author MStarSemi @date 2013/5/21
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_CCAUI_GetSystemOsdCheck(void)
{
    BOOL bCheck = TRUE;

	/* //Change popup appearing timing to tvbanner only
	if (MApp_ZUI_GetActiveOSD() >= E_OSD_MENU_S3IRD_CA_MENU && MApp_ZUI_GetActiveOSD() <= E_OSD_MENU_S3IRD_OTA)
    {
        bCheck = FALSE;
    }
    */
    //fix when main menu or audio volume bar osd pop-up, softcell ui is not monitored
    if ((MApp_ZUI_GetActiveOSD() == E_OSD_MAIN_MENU) || (MApp_ZUI_GetActiveOSD() == E_OSD_TV_AUDIOPOPUP))
    {
        return TRUE;
    }

    if ((MApp_ZUI_GetActiveOSD() != E_OSD_TV_PROG) && (MApp_ZUI_GetActiveOSD() != E_OSD_MENU_MEDIA_PVR_PLAY))
    {
		bCheck = FALSE;
    }
	else
	{
	    if(MApp_ZUI_GetState() == E_ZUI_STATE_RUNNING)
        {
            bCheck = TRUE;
        }
        else
        {
            bCheck = FALSE;
        }
	}

    return bCheck;
}


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_MonitorCCAIrdScreenTextDisplay
///
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_ZUI_ACT_MonitorCCAIrdScreenTextDisplay(void)
{
    static UINT32 u32ShowCount = 0;

#if 0
    ACTS3IRD_DBG("gu32Duration = %d\n", gu32Duration);
    ACTS3IRD_DBG("gbFlashing   = %d\n", gbFlashing);
    ACTS3IRD_DBG("gu32ForceMsgDelTime   = %d\n", gu32ForceMsgDelTime);
#endif

	if (gbCCAIrdPopMsgExist == TRUE) // Check popup msg first
	{
		if (gu32PopDuration != 0)
        {
            if (MApi_DateTime_Get() >= gu32PopForceMsgDelTime)
            {
                ACTCCAIRD_DBG("\nS3Ird POP Disable gu16S3CurrentPopMsgType 0x%x gbErrMsgInterrupted x%x\n",gu16CCACurrentPopMsgType,gbErrMsgInterrupted);
				MApp_ZUI_POP_Disable();
//				MApp_ZUI_ACT_ResetPopMsgStatus();
				u32ShowCount = 0;

				if((gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_TMS || gu16CCACurrentPopMsgType == CCAIRD_POP_TYPE_FORCE_MSG) && gbErrMsgInterrupted == TRUE)
				{
					ACTCCAIRD_DBG("\nS3Ird POP Rusme E type error msg\n");
					gbCCAIrdPopMsgExist = TRUE;
					gbPopFlashing = gS3PopInfo.flashing;
					gu32PopDuration = 0; // 'E' type msg won't disappear util 'D' type msg comes.
					gu32PopForceMsgDelTime = 0;
					gbErrMsgInterrupted = FALSE;
					gu16CCACurrentPopMsgType = CCAIRD_POP_TYPE_ERR_MSG_E;
					memset(gau8StrMsg,0,sizeof(gau8StrMsg));
					strncpy(gau8StrMsg,gS3PopInfo.gau8TempStrMsg, strlen(gS3PopInfo.gau8TempStrMsg));
					MApi_U8StringToU16String(gS3PopInfo.gau8TempStrMsg, gau16StrBuffer, strlen(gS3PopInfo.gau8TempStrMsg));
					MApp_ZUI_POP_SetPos(gS3PopInfo.prePopMsgRect);
			        MApp_ZUI_POP_Enable((UINT8*)gau16StrBuffer, wcslen(gau16StrBuffer) * 2, 0);
				}
				else
				{
					MApp_ZUI_ACT_ResetPopMsgStatus();
				}
            }

            // Check if the OSD display is off
            if (gu32PopForceMsgDelTime == 0)
            {
               gbPopFlashing   = FALSE;
               gu32PopDuration = 0;
            }
        }

        if (gbPopFlashing == TRUE)
        {
			if ((u32ShowCount % 12) == 0)
			{
				MApp_ZUI_POP_Hide();
			}
			else
			{
				MApp_ZUI_POP_Show();
			}
			u32ShowCount++;
		}
	}

    if (MApp_ZUI_ACT_DetectCCAIrd() == TRUE || bFingerWin == TRUE)
    {
        if (gu32Duration != 0)
        {
            if (MApp_ZUI_ACT_DetectCCAIrd() == TRUE || bFingerWin == TRUE)
            {
                if (MApi_DateTime_Get() >= gu32ForceMsgDelTime)
                {
                    ACTCCAIRD_DBG("\nMApp_ZUI_ACT_PageClean_Change begins\n");
                    gu32ForceMsgDelTime = 0;
                    //MApp_ZUI_ACT_Hide();
                    MApp_ZUI_ACT_PageClean_Change();

                    if(bFingerWin == TRUE)
                    {
                        MApi_GOP_GWIN_Enable(gWinInfo.u8WinId, FALSE);
                        MApi_GOP_GWIN_ReleaseWin(gWinInfo.u8WinId);
                        MApi_GOP_GWIN_DestroyFB(gWinInfo.u8FBId);
                        bFingerWin = FALSE;
                    }
                }
            }

            // Check if the OSD display is off
            if (gu32ForceMsgDelTime == 0)
            {
               gbFlashing   = FALSE;
               gu32Duration = 0;
            }
        }

        if (gbFlashing == TRUE) // Every other second. FIXME
        {
			if ((u32ShowCount % 12) == 0)
			{
			    MApi_GOP_GWIN_Enable(gWinInfo.u8WinId, FALSE);
            }
			else
			{
			    MApi_GOP_GWIN_Enable(gWinInfo.u8WinId, TRUE);
         	}

			u32ShowCount++;
		}
	}

    return TRUE;
}




#undef MAPP_ZUI_ACTCCAIRD_C

