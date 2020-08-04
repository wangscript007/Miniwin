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

#define MAPP_ZUI_ACTCCACASTATUS_C


//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------
#include "MApp_ZUI_ACTCCACaStatus.h"
#include "UniversalClient_API.h"
#include "MApp_ZUI_ACTCCACasCmd.h"
#include <ngl_log.h>

namespace ntvplus{

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#define SERVICE_STATUS_MENU_ITEM_NUM     8
#define SERVICE_STATUS_MENU_ITEM_OFFSET  2

#define STATUS_ITEM_MAX_LINES            64  // Lines
#define STATUS_ITEM_MAX_LINE_LENGTH      254  // 80 bytes

#define PRODUCT_ITEM_MAX_LINES            16  // Lines


/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/
extern UINT16 Current_SMC_Resource_Id;


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static UINT16 u16UnicodeStr[512];
static UINT8  gau8Services[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
static UINT8  gau8Product[PRODUCT_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
static UINT8  gau8Client[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];

static UINT32  gu32ProductIdx;

static HWND _boxlistservices_lines[] =
{
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV1,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV2,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV3,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV4,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV5,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV6,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV7,
    HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV8,
};

static ZUICTLSolidVScrollBarData _boxlistservicesvscroll =
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
    .lines_per_view  = SERVICE_STATUS_MENU_ITEM_NUM,
};


static ZUIUTL_LBNav _lbnavboxlistservices =
{
    .isInited        = FALSE,
    .lines_per_view  = SERVICE_STATUS_MENU_ITEM_NUM,
    .total_lines     = 0,
    .cur_line        = 0,
    .view_start_line = 0,
    .hWnds           = _boxlistservices_lines,
    .flag            = ZUIUTL_LBNAV_CONTINUOUS_NAV_ENABLE,
    .scrolldata      = &_boxlistservicesvscroll,
    .scrollbar       = HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SCRO,
};


static HWND _boxlistproduct_lines[] =
{
    HWND_MENU_CCA_PRODUCT_STATUS_LINE1,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE2,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE3,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE4,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE5,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE6,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE7,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE8,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE9,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE10,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE11,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE12,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE13,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE14,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE15,
    HWND_MENU_CCA_PRODUCT_STATUS_LINE16,
};

static ZUICTLSolidVScrollBarData _boxlistproductvscroll =
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
    .lines_per_view  = 16,
};


static ZUIUTL_LBNav _lbnavboxlistproduct =
{
    .isInited        = FALSE,
    .lines_per_view  = PRODUCT_ITEM_MAX_LINES,
    .total_lines     = 0,
    .cur_line        = 0,
    .view_start_line = 0,
    .hWnds           = _boxlistproduct_lines,
    .flag            = ZUIUTL_LBNAV_CONTINUOUS_NAV_ENABLE,
    .scrolldata      = &_boxlistproductvscroll,
    .scrollbar       = HWND_MENU_CCA_PRODUCT_STATUS_SCRO,
};


static HWND _boxlistclient_lines[] =
{
    HWND_MENU_CCA_CLIENT_STATUS_LINE1,
    HWND_MENU_CCA_CLIENT_STATUS_LINE2,
    HWND_MENU_CCA_CLIENT_STATUS_LINE3,
    HWND_MENU_CCA_CLIENT_STATUS_LINE4,
    HWND_MENU_CCA_CLIENT_STATUS_LINE5,
    HWND_MENU_CCA_CLIENT_STATUS_LINE6,
    HWND_MENU_CCA_CLIENT_STATUS_LINE7,
    HWND_MENU_CCA_CLIENT_STATUS_LINE8,
    HWND_MENU_CCA_CLIENT_STATUS_LINE9,
    HWND_MENU_CCA_CLIENT_STATUS_LINE10,
};

static ZUICTLSolidVScrollBarData _boxlistclientvscroll =
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
    .lines_per_view  = 10,
};


static ZUIUTL_LBNav _lbnavboxlistclient =
{
    .isInited        = FALSE,
    .lines_per_view  = 10,
    .total_lines     = 0,
    .cur_line        = 0,
    .view_start_line = 0,
    .hWnds           = _boxlistclient_lines,
    .flag            = ZUIUTL_LBNAV_CONTINUOUS_NAV_ENABLE,
    .scrolldata      = &_boxlistclientvscroll,
    .scrollbar       = HWND_MENU_CCA_CLIENT_STATUS_SCRO,
};

BOOL Read_Status = FALSE;

/********************************************************************************/
/*                            Global Variables                                   */
/********************************************************************************/


/********************************************************************************/
/*                            Extern Function                                   */
/********************************************************************************/

extern  BOOL LoaderStauts_result;

extern BOOLEAN _MApp_ZUI_API_AllocateVarData(void);
extern void _MApp_FONT_ascii_2_unicode(UINT16 *out, UINT8 *in);
BOOL CCA_Status_Get_Device_Status(void);

/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  private  _Update_DVB_Client_Services_init
///
///
///  @param [in]
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2013/07/03
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///  private  _Update_DVB_Client_Product_init
///
///
///  @param [in]
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2013/07/03
///////////////////////////////////////////////////////////////////////////////
bool IrdccaMenuWindow::CreateIrdCcacommonStatus(UINT8 menuflag)  
//static BOOL _Update_DVB_Client_Product_init(void)
{
	U16 u16ArraySize  = PRODUCT_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH;
	if(inflv) inflv->clearAllItems();
	else{
		inflv = new ListView(800,480);
		addChildView(inflv)->setPos(420,120);
	}
	inflv->setFlag(View::FOCUSED);
	
	switch (menuflag)
	{
		case E_APP_CMD_CCA_CA_CLIENT_STATUS:{
			U16    u16ClientNum = 0;
			if(inftitle) inftitle->setText("Client status");
			else {
				inftitle=new TextView("Client status",800,40);
				addChildView(inftitle)->setPos(420, 80);
			}
			memset(gau8Client, 0, u16ArraySize);
			u16ClientNum = MApp_CCAUI_CasCmd_GetClientStatus(gau8Client, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH);
			for ( int i=0; i<u16ClientNum; i++ ){
				inflv->addItem(new ListView::ListItem(gau8Client[i * STATUS_ITEM_MAX_LINE_LENGTH],i));
			}
			inflv->setIndex(0);
			menuflag = E_APP_CMD_CCA_CA_CLIENT_STATUS; 	
			return TRUE;
		}break;
		case E_APP_CMD_CCA_CA_PRODUCT_STATUS:{
			if(inftitle) inftitle->setText("Product status");
			else {
				inftitle=new TextView("Product status",800,40);
				addChildView(inftitle)->setPos(420, 80);
			}
			UINT32	  u32ProductNum = 0;
			gu32ProductIdx = 0;
			u32ProductNum = MApp_CCAUI_CasCmd_GetProductCount();
			memset(gau8Product, 0, u16ArraySize);
			MApp_CCAUI_CasCmd_GetProductStatus(gau8Product, PRODUCT_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH, 0);
			for ( int i=0; i<u32ProductNum+2; i++ ){
				inflv->addItem(new ListView::ListItem(gau8Product[i * STATUS_ITEM_MAX_LINE_LENGTH],i));
			}
			inflv->setIndex(2);//count and title
			menuflag = E_APP_CMD_CCA_CA_PRODUCT_STATUS;		
			return TRUE;
		}break;	
		case E_APP_CMD_CCA_CA_SERVICE_STATUS_ITEM:{
			inftitle->setText("");
		}break;
		default: break;
	}


    //at least show product count and title line
    //if (u32ProductNum > 0) 
    {
        MApp_ZUI_UTL_LBNav_Init(&_lbnavboxlistproduct);
        //MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistproduct);
        //_lbnavboxlistproduct.cur_line             = 0;
        _lbnavboxlistproduct.total_lines          = u32ProductNum + 2; //add product count and title line
        //_lbnavboxlistproduct.scrolldata->cur_line = _lbnavboxlistproduct.cur_line;
        GETWNDDATA(HWND_MENU_CCA_PRODUCT_STATUS_SCRO) = (void *) &_boxlistproductvscroll;
        MApp_ZUI_UTL_LBNav_ReInit(&_lbnavboxlistproduct);
        MApp_ZUI_UTL_LBNav_UpdateList(&_lbnavboxlistproduct);
        MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_SHOW);
        return TRUE;
    }

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_HIDE);
//    MApp_ZUI_API_SetFocus(prevfoces);

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  _Update_DVB_Client_Client_init
///
///
///  @param [in]
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2013/07/03
///////////////////////////////////////////////////////////////////////////////
  
static BOOL _Update_DVB_Client_Client_init(void)
{
    U16    u16ClientNum = 0;
    UINT16 u16ArraySize  = STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH;

    memset(gau8Client, 0, u16ArraySize);

    //Get the number from high level APP    
    u16ClientNum = MApp_CCAUI_CasCmd_GetClientStatus(gau8Client, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH);
    //NGLOG_DEBUG("%s", gau8Client);

    //FIXME: check the total lines of client status.
    if (u16ClientNum > 0)
    {
        MApp_ZUI_UTL_LBNav_Init(&_lbnavboxlistclient);
        //keep in same page when refresh data
        //MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistclient);
        //_lbnavboxlistclient.cur_line             = 0;
        _lbnavboxlistclient.total_lines          = u16ClientNum;
        //_lbnavboxlistclient.scrolldata->cur_line = _lbnavboxlistclient.cur_line;
        GETWNDDATA(HWND_MENU_CCA_CLIENT_STATUS_SCRO) = (void *) &_boxlistclientvscroll;
        MApp_ZUI_UTL_LBNav_ReInit(&_lbnavboxlistclient);
        MApp_ZUI_UTL_LBNav_UpdateList(&_lbnavboxlistclient);
        MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CLIENT_STATUS, SW_SHOW);
        return TRUE;
    }

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_HIDE);
//    MApp_ZUI_API_SetFocus(prevfoces);

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_ZUI_ACT_AppShowCCACaStatus
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_AppShowCCACaStatus(void)
{
    NGLOG_DEBUG("AppShowCCACaStatus\n");
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CA_STATUS_PAGE, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_SERVICE_STATUS_MENU);

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_LOAD_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CLIENT_STATUS, SW_HIDE);

#if 0//FIXME
    if (MApp_General_Msg_GetVersionInfo() == NULL)
    {
        MApp_General_Msg_UpdateVersionInfo();
        MsOS_DelayTask(100);
    }
#endif
    
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_AppShowCCACaStatus_SoftcellServicetatus
///  [OSD page handler] provide for showing MENU application
///
///  @return
///
///  @author MStarSemi @date 2013/7/1
///////////////////////////////////////////////////////////////////////////////
BOOL _MApp_ZUI_ACT_AppShowCCACaStatus_SoftcellServicetatus(void)
{
    
    NGLOG_DEBUG("_MApp_ZUI_ACT_AppShowCCACaStatus_SoftcellServicetatus\n");
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV1);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SCRO, SW_SHOW);

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CA_STATUS_PAGE, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_LOAD_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CLIENT_STATUS, SW_HIDE);

    if (_Update_DVB_Client_Services_init() == FALSE)
    {
        MApp_ZUI_ACT_AppShowCCACaStatus();
    //    return FALSE;
    }
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_AppShowCCACaStatus_LoadStatus
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/1
///////////////////////////////////////////////////////////////////////////////
void _MApp_ZUI_ACT_AppShowCCACaStatus_LoadStatus(void)
{
    NGLOG_DEBUG("AppShowCCACaStatus_LoadStatus\n");
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_LOAD_STATUS, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_IRDETO_LOAD_STATUS_LIST);

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CA_STATUS_PAGE, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CLIENT_STATUS, SW_HIDE);

#if 0//FIXME
    if (MApp_General_Msg_GetRegistrationParams() == NULL)
    {
        MApp_General_Msg_UpdateRegistrationParams();
        MsOS_DelayTask(100);
    }
#endif
    if( Read_Status == TRUE )
    {
        LoaderStauts_result = CCA_Status_Get_Device_Status();
        if(LoaderStauts_result == TRUE)
        { 
            printf("\033[1;35m[ACT S3 CA STATUS]%s(%d): ", __FUNCTION__, __LINE__); 
            printf("Get LoaderStatus Success!!");
            printf("\033[m");
        }
        else
        {
            printf("\033[1;35m[ACT S3 CA STATUS]%s(%d): ", __FUNCTION__, __LINE__); 
            printf("Get LoaderStatus Fail!!");
            printf("\033[m");     
        }
        Read_Status = FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  _MApp_ZUI_ACT_AppShowCCACaStatus_ProductStatus
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/1
///////////////////////////////////////////////////////////////////////////////
BOOL _MApp_ZUI_ACT_AppShowCCACaStatus_ProductStatus(void)
{
    NGLOG_DEBUG("AppShowCCACaStatus_ProductStatus\n");
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_PRODUCT_STATUS_LIST);

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CA_STATUS_PAGE, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_LOAD_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CLIENT_STATUS, SW_HIDE);

    if (_Update_DVB_Client_Product_init() == FALSE)
    {
        MApp_ZUI_ACT_AppShowCCACaStatus();
    //    return FALSE;
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  _MApp_ZUI_ACT_AppShowCCACaStatus_ClientStatus
///  [OSD page handler] provide for showing MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2013/7/1
///////////////////////////////////////////////////////////////////////////////
BOOL _MApp_ZUI_ACT_AppShowCCACaStatus_ClientStatus(void)
{
    NGLOG_DEBUG("AppShowCCACaStatus_ProductStatus\n");
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CLIENT_STATUS, SW_SHOW);
    MApp_ZUI_API_SetFocus(HWND_MENU_CCA_CLIENT_STATUS_LIST);

    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_CA_STATUS_PAGE, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_LOAD_STATUS, SW_HIDE);
    MApp_ZUI_API_ShowWindow(HWND_MENU_CCA_PRODUCT_STATUS, SW_HIDE);

    if (_Update_DVB_Client_Client_init() == FALSE)
    {
        MApp_ZUI_ACT_AppShowCCACaStatus();
    //    return FALSE;
    }
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCACaStatusKey_CaStatus
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOL _MApp_ZUI_ACT_HandleCCACaStatusKey_CaStatus(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state
    BOOL bResult = FALSE;

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    
    switch(key)
    {
        case VK_UP:
        case VK_DOWN:
        {
            HWND TempFocus = MApp_ZUI_API_GetFocus();

            NGLOG_DEBUG("HandleCCACaStatusKey_CaStatus. Focus = %ld\n", TempFocus);
            switch (TempFocus)
            {
                case HWND_MENU_CCA_SERVICE_STATUS_MENU:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_LOAD_STATUS_MENU : HWND_MENU_CCA_CLIENT_STATUS_MENU;
                    break;
                }
                case HWND_MENU_CCA_LOAD_STATUS_MENU:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_PRODUCT_STATUS_MENU : HWND_MENU_CCA_SERVICE_STATUS_MENU;
                    break;
                }
                case HWND_MENU_CCA_PRODUCT_STATUS_MENU:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_CLIENT_STATUS_MENU : HWND_MENU_CCA_LOAD_STATUS_MENU;
                    break;
                }
                case HWND_MENU_CCA_CLIENT_STATUS_MENU:
                {
                    TempFocus = (key == VK_DOWN) ? HWND_MENU_CCA_SERVICE_STATUS_MENU : HWND_MENU_CCA_PRODUCT_STATUS_MENU;
                    break;
                }
            }

            NGLOG_DEBUG("HandleCCACaStatusKey_CaStatus. Target focus = %ld\n", TempFocus);
            MApp_ZUI_API_SetFocus(TempFocus);
            return TRUE;
        }

        case VK_SELECT:
            switch (MApp_ZUI_API_GetFocus())
            {
                case HWND_MENU_CCA_SERVICE_STATUS_MENU:
                    bResult = _MApp_ZUI_ACT_AppShowCCACaStatus_SoftcellServicetatus();
                    return bResult;
                    //return TRUE;
                case HWND_MENU_CCA_LOAD_STATUS_MENU:
                    Read_Status = TRUE;
                    _MApp_ZUI_ACT_AppShowCCACaStatus_LoadStatus();
                    return TRUE;
                case HWND_MENU_CCA_PRODUCT_STATUS_MENU:
                    bResult =_MApp_ZUI_ACT_AppShowCCACaStatus_ProductStatus();
                    return bResult;
                case HWND_MENU_CCA_CLIENT_STATUS_MENU:
                    bResult =_MApp_ZUI_ACT_AppShowCCACaStatus_ClientStatus();
                    return bResult;
                default:
                    return FALSE;
            }
            return FALSE;

        case VK_EXIT:
            MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_MENU);
            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCACaStatusAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCACaStatusKey_ServiceStatus
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOL _MApp_ZUI_ACT_HandleCCACaStatusKey_ServiceStatus(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    NGLOG_DEBUG("_MApp_ZUI_ACT_HandleCCACaStatusKey_ServiceStatus.\n");

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
            NGLOG_DEBUG("_MApp_ZUI_ACT_HandleCCACaStatusKey_ServiceStatus. Navigate listbox\n");
            if (VK_LEFT == key)
            {
                key = VK_PAGE_UP;
            }

            if (VK_RIGHT == key)
            {
                key = VK_PAGE_DOWN;
            }
            MApp_ZUI_UTL_LBNav_Action(&_lbnavboxlistservices, key);
            return TRUE;
        }
        case VK_SELECT:
        {
            NGLOG_DEBUG("_MApp_ZUI_ACT_HandleCCACaStatusKey_ServiceStatus.\n");
            if (_lbnavboxlistservices.total_lines != 0)
            {
                MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_CCA_CA_SERVICE_STATUS);
            }
            return TRUE;
        }
        case VK_EXIT:
        case VK_MENU:
            // back to the parent
            MApp_ZUI_ACT_AppShowCCACaStatus();
            MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistservices);
            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCACaStatusAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}



///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCACaStatusKey_LoadStatus
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCACaStatusKey_LoadStatus(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    switch(key)
    {
        case VK_SELECT:
            // back to the parent
            Read_Status = TRUE;
            MApp_ZUI_ACT_AppShowCCACaStatus();
        case VK_EXIT:
            // back to the parent
            MApp_ZUI_ACT_AppShowCCACaStatus();
            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCACaStatusAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCACaStatusKey_ProductStatus
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCACaStatusKey_ProductStatus(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    switch(key)
    {
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_PAGE_UP:
        case VK_PAGE_DOWN:
        {
            NGLOG_DEBUG("_MApp_ZUI_ACT_HandleCCACaStatusKey_LoadStatus. Navigate listbox\n");
            if ((VK_LEFT == key) || (VK_UP == key))
            {
                key = VK_PAGE_UP;
            }

            if ((VK_RIGHT == key) || (VK_DOWN == key))
            {
                key = VK_PAGE_DOWN;
            }
            MApp_ZUI_UTL_LBNav_Action(&_lbnavboxlistproduct, key);
            return TRUE;
        }
        case VK_SELECT:
        case VK_EXIT:
            // back to the parent
            MApp_ZUI_ACT_AppShowCCACaStatus();
            MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistproduct);
            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCACaStatusAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_HandleCCACaStatusKey_ClientStatus
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOLEAN    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN _MApp_ZUI_ACT_HandleCCACaStatusKey_ClientStatus(VIRTUAL_KEY_CODE key)
{
    //note: this function will be called in running state

    //reset timer if any key
    MApp_ZUI_API_ResetTimer(HWND_MAINFRAME, 0);

    switch(key)
    {
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_PAGE_UP:
        case VK_PAGE_DOWN:
        {
            NGLOG_DEBUG("_MApp_ZUI_ACT_HandleCCACaStatusKey_LoadStatus. Navigate listbox\n");
            if ((VK_LEFT == key) || (VK_UP == key))
            {
                key = VK_PAGE_UP;
            }

            if ((VK_RIGHT == key) || (VK_DOWN == key))
            {
                key = VK_PAGE_DOWN;
            }
            MApp_ZUI_UTL_LBNav_Action(&_lbnavboxlistclient, key);
            return TRUE;
        }
        case VK_SELECT:
        case VK_EXIT:
            // back to the parent
            MApp_ZUI_ACT_AppShowCCACaStatus();
            MApp_ZUI_UTL_LBNav_Reset(&_lbnavboxlistclient);
            return TRUE;
        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCACaStatusAction(EN_EXE_POWEROFF);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_HandleCCACaStatusKey
///  [OSD page handler] global key handler for MENU application
///
///  @param [in]       key VIRTUAL_KEY_CODE      key code
///
///  @return BOOL    true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_ZUI_ACT_HandleCCACaStatusKey(VIRTUAL_KEY_CODE key)
{
    BOOL bResult = FALSE;

    //note: this function will be called in running state
    NGLOG_DEBUG("HandleCCACaStatusKey. Key = %ld\n", key);
    switch(key)
    {
        default:
        {
            switch (MApp_ZUI_API_GetParent(MApp_ZUI_API_GetFocus()))
            {
                case HWND_MENU_CCA_CA_STATUS_LIST:
                    NGLOG_DEBUG("HWND_MENU_CCA_CA_STATUS_LIST.\n");
                    bResult = _MApp_ZUI_ACT_HandleCCACaStatusKey_CaStatus(key);
                    break;
                case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_LIST:
                    NGLOG_DEBUG("HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS.\n");
                    bResult = _MApp_ZUI_ACT_HandleCCACaStatusKey_ServiceStatus(key);
                    break;
                case HWND_MENU_CCA_LOAD_STATUS:
                    NGLOG_DEBUG("HWND_MENU_CCA_LOAD_STATUS.\n");
                    bResult = _MApp_ZUI_ACT_HandleCCACaStatusKey_LoadStatus(key);
                    break;
                case HWND_MENU_CCA_PRODUCT_STATUS_LIST:
                    NGLOG_DEBUG("HWND_MENU_CCA_PRODUCT_STATUS.\n");
                    bResult = _MApp_ZUI_ACT_HandleCCACaStatusKey_ProductStatus(key);
                    break;
                case HWND_MENU_CCA_CLIENT_STATUS_LIST:
                    NGLOG_DEBUG("HWND_MENU_CCA_CLIENT_STATUS.\n");
                    bResult = _MApp_ZUI_ACT_HandleCCACaStatusKey_ClientStatus(key);
                    break;
                default:
                   return FALSE;
                //ASSERT(0);
            }
            return bResult;
        }

        case VK_POWER:
            MApp_ZUI_ACT_ExecuteCCACaStatusAction(EN_EXE_POWEROFF);
            return TRUE;

    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_TerminateCCACaStatus
///  [OSD page handler] terminate MENU application
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_TerminateCCACaStatus(void)
{
    NGLOG_DEBUG("\n");
    
    //MApp_SaveGenSetting();
    //enCAmenuState = _enTargetMenuState;
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_ExecuteCCACaStatusAction
///  [OSD page handler] execute a specific action in MENU application
///
///  @param [in]       act U16      action ID
///
///  @return BOOLEAN     true for accept, false for ignore
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
BOOLEAN MApp_ZUI_ACT_ExecuteCCACaStatusAction(U16 act)
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
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_CaStatusTitle
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_CaStatusTitle(void)
{
    UINT8 au8Buf[128] = "Irdeto CA Status";
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
#if 0//FIXME    
    UINT8 au8Buf[128] = "DVB Client";

    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_S3UI_CasCmd_GetClientId(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));

    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_S3UI_CasCmd_GetClientVersion(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr + wcslen(u16UnicodeStr), strlen(au8Buf));

    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_S3UI_CasCmd_GetClientBuild(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr + wcslen(u16UnicodeStr), strlen(au8Buf));
#endif
    return ((LPTSTR)u16UnicodeStr);
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderDevice
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderDevice(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemDeviceValue(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderKey
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderKey(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemKey(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderCssn
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderCssn(void)
{
    UINT8 au8Buf[128];
    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));   
    MApp_CCAUI_CasCmd_GetLoaderItemCssn(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderDownloadID
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderDownloadID(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf(au8Buf, sizeof(au8Buf), "L-                          0x%04lx", MApp_CCAUI_CasCmd_GetLoaderItemDoadloadId());
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderFwVer
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderFwVer(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderFirmwareVersion(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderLoaderVer
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderLoaderVer(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    snprintf(au8Buf, sizeof(au8Buf), "Loader version        0x%04x", MApp_CCAUI_CasCmd_GetLoaderItemLoaderVersion());
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderSerialNo
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderSerialNo(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemSerialNo(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderSpecificInfo
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderSpecificInfo(void)
{
    UINT8 au8Buf[128];

    NGLOG_DEBUG("\n");
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderSpecificInfo(au8Buf, sizeof(au8Buf));
    MApi_U8StringToU16String(au8Buf, u16UnicodeStr, strlen(au8Buf));
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_ProductStatus
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_ProductStatus(UINT8 u8Index)
{   
    //UINT16 u16CurrLine  = 0;
    UINT16 u16ArraySize  = PRODUCT_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH;
    UINT32 u32Index = 0;

    NGLOG_DEBUG("\n");

    //get data for each page
    if(u8Index == 0)
    {        
        NGLOG_DEBUG("\n");
        memset(gau8Product, 0, u16ArraySize);

        if(_lbnavboxlistproduct.view_start_line == 0)
        {
            u32Index = 0;
        }
        else
        {
            // -2: because of product count and title
            u32Index = _lbnavboxlistproduct.view_start_line - 2;
        }
        MApp_CCAUI_CasCmd_GetProductStatus(gau8Product, PRODUCT_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH, u32Index);
    }
    
    //u16CurrLine = u8Index + _lbnavboxlistproduct.view_start_line;   
    //MApi_U8StringToU16String(&gau8Product[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH], u16UnicodeStr, STATUS_ITEM_MAX_LINE_LENGTH);
    MApi_U8StringToU16String(&gau8Product[u8Index * STATUS_ITEM_MAX_LINE_LENGTH], u16UnicodeStr, STATUS_ITEM_MAX_LINE_LENGTH);

    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_ClientStatus
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_ClientStatus(UINT8 u8Index)
{   
    UINT16 u16CurrLine  = 0;

    NGLOG_DEBUG("\n");

    u16CurrLine = u8Index + _lbnavboxlistclient.view_start_line;
    MApi_U8StringToU16String(&gau8Client[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH], u16UnicodeStr, STATUS_ITEM_MAX_LINE_LENGTH);

    return ((LPTSTR)u16UnicodeStr);
}


///////////////////////////////////////////////////////////////////////////////
///  private  _MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerives
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
LPTSTR _MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerives(UINT8 u8Index)
{   
    UINT16 u16CurrLine  = 0;

    NGLOG_DEBUG("\n u8Index = %d\n", u8Index);

    u16CurrLine = u8Index + _lbnavboxlistservices.view_start_line;
    MApi_U8StringToU16String(&gau8Services[u16CurrLine * STATUS_ITEM_MAX_LINE_LENGTH], u16UnicodeStr, STATUS_ITEM_MAX_LINE_LENGTH);
    
    return ((LPTSTR)u16UnicodeStr);
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerivesCurrentLineStartAddr
///
///  @param [in]
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2013/7/4
///////////////////////////////////////////////////////////////////////////////
UINT8* MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerivesCurrentLine(void)
{
    UINT32 u32CurrLineStartAddr = _lbnavboxlistservices.cur_line * STATUS_ITEM_MAX_LINE_LENGTH;

    return &gau8Services[u32CurrLineStartAddr];
}

///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_GetCCACaStatusDynamicText
///  [OSD page handler] dynamic text content provider in MENU application
///
///  @param [in]       hwnd HWND     window handle we are processing
///
///  @return LPCTSTR     string content
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
LPTSTR MApp_ZUI_ACT_GetCCACaStatusDynamicText(HWND hwnd)
{
    NGLOG_DEBUG("%s",__FUNCTION__);
    switch (hwnd)
    {
        case HWND_MENU_CCA_CA_STATUS_MENU_TEXT:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_CaStatusTitle());

        // Load status
        case HWND_MENU_CCA_IRDETO_LOAD_STATUS_LIST_DEVICE:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderDevice());
        case HWND_MENU_CCA_IRDETO_LOAD_STATUS_LIST_KEY:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderKey());
        case HWND_MENU_CCA_IRDETO_LOAD_STATUS_LIST_CSSN:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderCssn());
        case HWND_MENU_CCA_IRDETO_LOAD_STATUS_LIST_L:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderDownloadID());
        case HWND_MENU_CCA_MANUFACTURER_LOAD_STATUS_LIST_FW_VER:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderFwVer());
        case HWND_MENU_CCA_MANUFACTURER_LOAD_STATUS_LIST_LOADER_VER:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderLoaderVer());
        case HWND_MENU_CCA_MANUFACTURER_LOAD_STATUS_LIST_SERIAL_NUM:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderSerialNo());
        case HWND_MENU_CCA_MANUFACTURER_LOAD_STATUS_LIST_SPECIFIC_INFO:
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_LoaderSpecificInfo());

        // Product status
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE1:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE2:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE3:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE4:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE5:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE6:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE7:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE8:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE9:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE10:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE11:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE12:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE13:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE14:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE15:
        case HWND_MENU_CCA_PRODUCT_STATUS_LINE16:
        {
            UINT8 u8Index = (hwnd - HWND_MENU_CCA_PRODUCT_STATUS_LINE1);

            if (HWND_MENU_CCA_PRODUCT_STATUS_LINE1 == hwnd)
            {
                if (_lbnavboxlistproduct.total_lines == 0)
                {
                    return MApp_ZUI_API_GetString(en_str_PleaseWait);
                }
            }
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_ProductStatus(u8Index));
        }

        // Client status
        case HWND_MENU_CCA_CLIENT_STATUS_LINE1:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE2:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE3:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE4:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE5:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE6:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE7:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE8:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE9:
        case HWND_MENU_CCA_CLIENT_STATUS_LINE10:
        {
            UINT8 u8Index = (hwnd - HWND_MENU_CCA_CLIENT_STATUS_LINE1);

            if (HWND_MENU_CCA_CLIENT_STATUS_LINE1 == hwnd)
            {
                if (_lbnavboxlistclient.total_lines == 0)
                {
                    return MApp_ZUI_API_GetString(en_str_PleaseWait);
                }
            }
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_ClientStatus(u8Index));
        }

        // DVB Client services
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV1_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV2_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV3_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV4_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV5_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV6_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV7_TEXT:
        case HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV8_TEXT:
        {
            UINT8 u8Index = ((hwnd - HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV1_TEXT) / 2);

            if (HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV1_TEXT == hwnd)
            {
                if (_lbnavboxlistservices.total_lines == 0)
                {
                    return MApp_ZUI_API_GetString(en_str_NoInformation);
                }
            }
            return ((LPTSTR)_MApp_ZUI_ACT_GetCCACaStatusDynamicText_DvbClientSerives(u8Index));
        }
        default:
            return NULL;
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///  private  MApp_ZUI_ACT_NotifyCCACaStatusUpdate
///  
///
///  @return no return value
///
///  @author MStarSemi @date 2007/1/25
///////////////////////////////////////////////////////////////////////////////
void MApp_ZUI_ACT_NotifyCCACaStatusUpdate(UINT8* pu8Data)
{
    HWND hwndTemp = MApp_ZUI_API_GetFocus();

    NGLOG_DEBUG("Focus = %d. hwnd = %ld", pu8Data[0], hwndTemp);

    if (pu8Data[0] == E_APP_CMD_CCA_CA_SERVICE_STATUS)
    {
        if ((hwndTemp >= HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV1) &&
            (hwndTemp <= HWND_MENU_CCA_DVB_CLIENT_SERVICES_STATUS_SRV8))
        {
            _MApp_ZUI_ACT_AppShowCCACaStatus_SoftcellServicetatus();
        }
    }
    else if (pu8Data[0] == E_APP_CMD_CCA_CA_LOADER_STATUS)
    {
        if (hwndTemp == HWND_MENU_CCA_IRDETO_LOAD_STATUS_LIST)
        {
            Read_Status = TRUE;
            _MApp_ZUI_ACT_AppShowCCACaStatus_LoadStatus();
        }
    }
    else if (pu8Data[0] == E_APP_CMD_CCA_CA_PRODUCT_STATUS)
    {
        if ((hwndTemp >= HWND_MENU_CCA_PRODUCT_STATUS_LIST) &&
            (hwndTemp <= HWND_MENU_CCA_PRODUCT_STATUS_LINE16))
        {
            _MApp_ZUI_ACT_AppShowCCACaStatus_ProductStatus();
        }
    }
    else if (pu8Data[0] == E_APP_CMD_CCA_CA_CLIENT_STATUS)
    {
        if ((hwndTemp >= HWND_MENU_CCA_CLIENT_STATUS_LIST) &&
            (hwndTemp <= HWND_MENU_CCA_CLIENT_STATUS_LINE10))
        {
            _MApp_ZUI_ACT_AppShowCCACaStatus_ClientStatus();
        }
    }
}
}
#undef MAPP_ZUI_ACTCCACASTATUS_C

