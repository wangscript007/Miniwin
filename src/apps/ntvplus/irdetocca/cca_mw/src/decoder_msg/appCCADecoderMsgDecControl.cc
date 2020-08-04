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
/// file    appCCADecoderMsgDecControl.c
/// @brief  Parser and handle of CCA Decoder Destination "Decoder Control"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define _APP_CCA_DECODERMSG_C


/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
#include <ngl_log.h>
#include <ngl_types.h>
#include "appCCADecoderMsg.h"
#include "appCCADecoderMsgDecControl.h"
#include "appCCADecoderMsgLoaderPt.h"
#include "CCA_UCMsgProc.h"

#include "MApp_ZUI_ACTCCAUIControl.h"

#if(LOADER_V5 == 1)
#include "MApp_Ird_loader_status.h"
#include "LoaderCoreAPI_ApplicationHelper.h"
#include "LoaderCore_Types.h"
#include "LoaderCorePrivate.h"
#endif

#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif
NGL_MODULE(MSGDECCTR);

namespace cca{
extern void MApp_ResetSetting(void);

//#define CCA_DECMSG_DECCTRL_DEBUG
#define CCA_DECMSG_DECCTRL_OTA

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/


//#define SCAN_NIT_TIMEOUT            20500  // Millisecs
#define DEFAULT_DVBT_NIT_TIMEOUT    30000  // Millisecs
#define DEFAULT_DVBC_NIT_TIMEOUT    30000  // Millisecs
#define DEFAULT_BAT_TIMEOUT         15000  // Millisecs
#define PRIVATE_LINK_DESC           0x3622

// Bouquet
#define CCA_DECMSG_DECCTRL_ALLALLOWEDBOUQUETS 0xffff
#define CCA_DECMSG_DECCTRL_CURRENTBOUQUET     0x0000

#define E_SI_DESC_STLK              0x80 // Standard linkage descriptor
#define E_SI_DESC_ENLK              0x82 // Enhanced linkage descriptor

#define CROSS_TRANSPONDER_NUM       2

#if(LOADER_V3_2 == 1)
#ifdef CCA_DECMSG_DECCTRL_OTA
static UINT16   TS_ID      = 0;
static UINT16   ON_ID      = 0;
#endif
#endif

static UINT16   SERV_ID    = 0;
static UINT16   ES_OTA_PID = 0;
#if(LOADER_V3_2 == 1)
static UINT16   TABLE_ID   = 0;
#endif
#define DOWNLOAD_TS_TIMEOUT          30000

//Get download control section : define and static variables
#define SEC_BUF_SIZE_DCS            (1024)
#define TABLE_ID_DCS                (0xFE)

static BOOL bDecCtrlInitDone = FALSE;
#if(LOADER_V3_2 == 1)
static UINT8*  _pu8DcsBuf       = NULL;
static UINT8   _u8DmxIdDcs      = DMX_DMXID_NULL;
static UINT8   _u8CrcErrorCnt;
#endif

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/
// Add to revert to last viewing status
extern MS_FE_CARRIER_PARAM* _ptunerParam;
//extern UINT16 Current_SMC_Resource_Id;
//extern S3_SMC_MSG_Card_Info SmartCard_Info[SMC_MAX_SUPPORT_NUM];

BOOL gbEMMTriggered = FALSE;
#if(DTV_SYSTEM_STYLE == DVBS_STYLE)
extern MS_SAT_PARAM* _pstSATParam;
#endif

#if (defined(DVBS_STYLE) || defined(DVBC_STYLE))
#define _MENU_CSCAN_PAT_BASE              FALSE // FALSE means SDT base
#endif

/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static stIrdMsgDecControlPayload gstDecCtrl = {0};

BOOL gbMenuTriggered = FALSE;

// Add to revert to last viewing status
static MS_FE_CARRIER_PARAM gstOrigParam;

#if (CA_IRD_CCA == 1)
static EN_OTA_MENU_TRIGGER_STATE_TYPE gbMenuDownloadReady = EN_OTA_MENU_TRIGGER_STATE_NONE;
static UINT16 u16str[128] = {0};
static UINT8 pStr[128] = {0};
#endif
/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/


/********************************************************************************/
/*                            extern Function                                */
/********************************************************************************/
#ifdef CCA_DECMSG_DECCTRL_OTA

//extern msg_registration_parameter_reply_data_st Msg_reg_para;  //From appS3GeneralMsg.c

typedef struct
{
    BOOL bSetBBCB;
    UINT8   u8ManufactureCode; //manufacturer code
    UINT8   u8HWVersion;       //hardware code

    BOOL bSetEEPROM;
    UINT16  u16Variant;

    BOOL bSetSPCB;
    UINT8 u8LoadSequenceNumber;
} stCCASystem;
stCCASystem gstSystem;

#endif

/********************************************************************************/
/*                            Local Function                                */
/********************************************************************************/

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_ResetDecoder
///  Reset the decoder /system
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static void MApp_DecoderMsg_ResetDecoder(void)
{
    NGLOG_DEBUG("\n");
    MDrv_WDT_SetTimer_ms(E_WDT_DBGLV_ALL, 1);

    while(1);
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_SwitchChannel
///  Switch channel with specified Service Type and position
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_SwitchChannel(eSERVICETYPE eSrvType, UINT16 u16Pos)
{
    eOP_RESULT eOpResult     = oprFAILURE;
    BOOL bResult          = FALSE;

    NGLOG_DEBUG("[DecCtrl] Swithc to the service %d and position %d\n", eSrvType, u16Pos);

    //eSrvType = MApi_DB_CM_GetCurrentServiceType();
    eOpResult = MApi_DB_CM_SetCurrentServiceType(eSrvType);
    if (eOpResult != oprSUCCESS)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to set the current service type. OpResult = %d\n", eOpResult);
        return FALSE;
    }

    eOpResult = MApi_DB_CM_SetCurrentPosition(eSrvType, u16Pos);
    if (eOpResult != oprSUCCESS)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to set the current position. OpResult = %d\n", eOpResult);
        return FALSE;
    }

    tvview_ShowBlackScreen();

    bResult = _appTV_Cmd_ChangProg(u16Pos + 1);
    if (bResult == FALSE)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to change program\n");
        return FALSE;
    }
    MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_ResetDecoder
///  Reset the decoder /system
///
///  @param [in]    u16BouquetID  Bouquet ID
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_ResetChannelOrder(UINT16 u16BouquetID)
{
    SI_BAT_Info const *pstSiBat = NULL;
    UINT16 u16BIDMask           = 0xFFFF;
    UINT8 u8BouquetIndex        = 0;
    UINT8 u8BouquetListIndex    = 0;
    UINT8 u8ServiceIndex        = 0;
    BOOL bResult             = FALSE;

    /*        0xffff = All allowed bouquets;
                        0 = current bouquet;
           1 to 0xfffe = actual bouquet ID
       */

#if 0
    {
        pstSiBat = MApi_SI_Get_BAT_Info();
        if (pstSiBat == NULL)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to get BAT info\n");
            return FALSE;
        }

        for (u8BouquetIndex = 0; u8BouquetIndex < pstSiBat->u8BouquetNum; u8BouquetIndex++)
        {
            for (u8BouquetListIndex = 0; u8BouquetListIndex < pstSiBat->BouquetGroup[u8BouquetIndex].u8BouquetListNum; u8BouquetListIndex++)
            {
                for (u8ServiceIndex = 0; u8ServiceIndex < SI_SERVICE_NUM; u8ServiceIndex++)
                {
                    NGLOG_DEBUG("[DecCtrl] service id=0x%04X, type=%d\n",
                                           pstSiBat->BouquetGroup[u8BouquetIndex].BouquetList[u8BouquetListIndex].BatSLInfo[u8ServiceIndex].wService_ID,
                                           pstSiBat->BouquetGroup[u8BouquetIndex].BouquetList[u8BouquetListIndex].BatSLInfo[u8ServiceIndex].u8Service_Type);
                }
            }
        }
    }
#endif

    if (u16BouquetID == CCA_DECMSG_DECCTRL_CURRENTBOUQUET)
	{
		//need to find out the current BOUQUET
		pstSiBat = MApi_SI_Get_BAT_Info();
        if (pstSiBat == NULL)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to get BAT info\n");
            return FALSE;
        }
        else
        {
            NGLOG_DEBUG("[DecCtrl] SI BAT info:u8BouquetNum = %d \n", pstSiBat->u8BouquetNum);
            NGLOG_DEBUG("[DecCtrl] SI BAT info:BouquetGroup[0].u8BouquetListNum = %d \n", pstSiBat->BouquetGroup[0].u8BouquetListNum);
            NGLOG_DEBUG("[DecCtrl] SI BAT info:BouquetGroup[0].u16BouquetID = %d \n", pstSiBat->BouquetGroup[0].u16BouquetID);
        }
	}
	else 
	{
	
		if(u16BouquetID == CCA_DECMSG_DECCTRL_ALLALLOWEDBOUQUETS)
		{
			u16BIDMask		= 0x0;
			u16BouquetID	= 0x1;
		}

		
    	bResult = MApi_SI_Clear_BAT_Info();
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to clear BAT\n");
            return FALSE;
        }

        //bResult = MApi_SI_Parser_BAT_Start(PRIVATE_LINK_DESC, u16BIDMask);
        bResult = MApi_SI_Parser_BAT_Start(u16BouquetID, u16BIDMask);
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to parse BAT\n");
            return FALSE;
        }

        MsOS_DelayTask(1000);

        bResult = MApi_SI_Parser_BAT_Stop();
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to stop BAT\n");
            return FALSE;
        }

        pstSiBat = MApi_SI_Get_BAT_Info();
        if (pstSiBat == NULL)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to get BAT info\n");
            return FALSE;
        }
        else
        {
            NGLOG_DEBUG("[DecCtrl] SI BAT info:u8BouquetNum = %d \n", pstSiBat->u8BouquetNum);
            NGLOG_DEBUG("[DecCtrl] SI BAT info:BouquetGroup[0].u8BouquetListNum = %d \n", pstSiBat->BouquetGroup[0].u8BouquetListNum);
            NGLOG_DEBUG("[DecCtrl] SI BAT info:BouquetGroup[0].u16BouquetID = %d \n", pstSiBat->BouquetGroup[0].u16BouquetID);
        }
    }
    // Reset Channel Test
    // BAT: Service List Descriptor, Channel List
    // The decoder should return to the first channel of the first bouquet.

    // FIXME: How to reset channel? How decoder can return to the first channel of the first bouquet?
    for (u8BouquetIndex = 0; u8BouquetIndex < pstSiBat->u8BouquetNum; u8BouquetIndex++)
	{
    	for (u8BouquetListIndex = 0; u8BouquetListIndex < pstSiBat->BouquetGroup[u8BouquetIndex].u8BouquetListNum; u8BouquetListIndex++)
        {
            if (pstSiBat->BouquetGroup[u8BouquetIndex].u16BouquetID == u16BouquetID)
            {
                for (u8ServiceIndex = 0; u8ServiceIndex < SI_SERVICE_NUM; u8ServiceIndex++)
                {
                    NGLOG_DEBUG("[DecCtrl] service id=0x%04X, type=%d\n",
                                           pstSiBat->BouquetGroup[u8BouquetIndex].BouquetList[u8BouquetListIndex].BatSLInfo[u8ServiceIndex].wService_ID,
                                           pstSiBat->BouquetGroup[u8BouquetIndex].BouquetList[u8BouquetListIndex].BatSLInfo[u8ServiceIndex].u8Service_Type);
                }
                break;
            }
        }
    }

    NGLOG_DEBUG("[DecCtrl] u8BouquetIndex=%d, u8BouquetListIndex=%d\n", u8BouquetIndex, u8BouquetListIndex);
    // Return to the first channel of the first bouquet
    if (pstSiBat->u8BouquetNum > 0)
    {
        MApp_DecoderMsg_SwitchChannel(pstSiBat->BouquetGroup[0].BouquetList[0].BatSLInfo[0].u8Service_Type, 0);
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_RescanSiResetChannelOrder
///  Re-Scan Service Information and Reset Channel Order
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_RescanSiResetChannelOrder(void)
{
    UINT16 u16Pos            = 0;
    eSERVICETYPE eSrvType    = svtINVALID;
    UINT16 u16CurService_ID;
    UINT16 u16NewService_ID;

    // Remember the current service ID
    eSrvType = MApi_DB_CM_GetCurrentServiceType();
    if (eSrvType == svtINVALID)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to get the current service type = %d\n", eSrvType);
        return FALSE;
    }
    u16Pos= MApi_DB_CM_GetCurrentPosition(eSrvType);

    u16CurService_ID = MApi_DB_CM_GetService_ID(eSrvType, u16Pos);

    // Should post event to notify UI to display warning message, then trigger Scan APP

    // FIXME: this relates to UI control.
    
#if(DTV_SYSTEM_STYLE == DVBS_STYLE )
	MApp_ZUI_UTL_PageSwitch(E_OSD_MENU_DISHSETUP); //E_OSD_MENU_AUTOSCAN
#else
	MApp_ZUI_UTL_PageSwitch(E_OSD_MANUAL_SCAN); //E_OSD_MENU_AUTOSCAN
#endif

    // Under scanning
    MApp_CCAUI_Rescan(9000); // 9 seconds

	
    // Ideally the decoder should return to its previous channel number
    u16NewService_ID = MApi_DB_CM_GetService_ID(eSrvType, u16Pos);

    if (u16NewService_ID == u16CurService_ID)
    {
        MApp_DecoderMsg_SwitchChannel(eSrvType, u16Pos);
    }
    else
    {
        SI_BAT_Info const *pstSiBat = NULL;

        pstSiBat = MApi_SI_Get_BAT_Info();
        if (pstSiBat == NULL)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to get BAT info\n");
            return FALSE;
        }
        else
        {
            NGLOG_DEBUG("[DecCtrl] SI BAT info:u8BouquetNum = %d\n", pstSiBat->u8BouquetNum);
            NGLOG_DEBUG("[DecCtrl] SI BAT info:BouquetGroup[0].u8BouquetListNum = %d\n", pstSiBat->BouquetGroup[0].u8BouquetListNum);
            NGLOG_DEBUG("[DecCtrl] SI BAT info:BouquetGroup[0].u16BouquetID = %d\n", pstSiBat->BouquetGroup[0].u16BouquetID);
        }
        if (pstSiBat->u8BouquetNum > 0)
        {
            MApp_DecoderMsg_SwitchChannel(pstSiBat->BouquetGroup[0].BouquetList[0].BatSLInfo[0].u8Service_Type, 0);
        }
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_EraseServiceLists
///  Erase service lists RESET Re-Tune from Home Tune Partition Data, Re-Scan Service Information and Reset Channel Order
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static void MApp_DecoderMsg_EraseServiceLists(void)
{
    // Should post event to notify UI to display warning message

    // Read the home tune partition values from the Smartcard and use them to re-tune

    // Should first erase all saved service lists including those for other networks

    // FIXME: Wil it be like a reset to factory setting??
    tvview_Disable();
    MApp_ZUI_CloseDisplayGDI();
    MApp_SystemSetting_RestoreToDefault();
    MApp_ResetSetting();

    // Perform a reset
    // FIXME: Shall save some info onto flash before reset
    MApp_DecoderMsg_ResetDecoder();  // After here, the system will be reset.

    // On start-up,  re-scan, reconnect
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DCS_Parse
///  Force tune to the specific service
///
///  @param [in]    u16OriginalNetworkId   Original Network ID
///  @param [in]    u16TransportStreamId  TS ID
///  @param [in]    u16ServiceId                Service ID
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_Recovery_ForceTune(UINT16 u16OriginalNetworkId, UINT16 u16TransportStreamId, UINT16 u16ServiceId)
{
    UINT16 u16Pos         = 0;
    eSERVICETYPE eSrvType = svtINVALID;
    eOP_RESULT eOpResult  = oprFAILURE;
    BOOL bResult       = FALSE;

    eSrvType = MApi_DB_CM_GetCurrentServiceType();
    if (eSrvType == svtINVALID)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to get the current service type = %d\n", eSrvType);
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[DecCtrl] Current service type = %d\n", eSrvType);
    }


    u16Pos = MApi_DB_CM_GetPositionWithTSONServiceId(eSrvType,
             gstDecCtrl.stRecoverData.stForceTune.u16TransportStreamId,
             gstDecCtrl.stRecoverData.stForceTune.u16OriginalNetworkId,
             gstDecCtrl.stRecoverData.stForceTune.u16ServiceId
#if( (DTV_SYSTEM_STYLE == DVBS_STYLE))
             ,
             MApi_DB_CM_GetPhysicalChannelNumber(eSrvType, MApi_DB_CM_GetCurrentPosition(eSrvType))
#endif

                                                    );
    NGLOG_DEBUG("[DecCtrl] MApi_DB_CM_GetPositionWithTSONServiceId pos = %d\n", u16Pos);

    eOpResult = MApi_DB_CM_SetCurrentPosition(eSrvType, u16Pos);
    if (eOpResult != oprSUCCESS)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to set the current position = %d\n", eOpResult);
        return FALSE;
    }

    tvview_ShowBlackScreen();

    if ((u16Pos + 1) < 65536)
    {
        bResult = _appTV_Cmd_ChangProg(u16Pos + 1);
    }
    else
    {
        NGLOG_DEBUG("[DecCtrl] Wrong u16Pos = %d\n", u16Pos);
    }

    if (bResult == FALSE)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to change program\n");
        return FALSE;
    }

    MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_Recovery_Reconnect
///  Reconnect to the current service
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_Recovery_Reconnect(void)
{
    UINT16 u16Pos         = 0;
    eSERVICETYPE eSrvType = svtINVALID;
    eOP_RESULT eOpResult  = oprFAILURE;
    BOOL bResult       = FALSE;

    eSrvType  = MApi_DB_CM_GetCurrentServiceType();
    if (eSrvType == svtINVALID)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to get the current service type = %d\n", eSrvType);
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[DecCtrl] Current service type = %d\n", eSrvType);
    }

    u16Pos    = MApi_DB_CM_GetCurrentPosition(eSrvType);
    eOpResult = MApi_DB_CM_SetCurrentPosition(eSrvType, u16Pos);
    if (eOpResult != oprSUCCESS)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to set the current position = %d\n", eOpResult);
        return FALSE;
    }

    tvview_ShowBlackScreen();

    bResult = _appTV_Cmd_ChangProg(u16Pos + 1);
    if (bResult == FALSE)
    {
        NGLOG_DEBUG("[DecCtrl] Failed to change program\n");
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[DecCtrl] Succeed to change program\n");
    }

    MApp_ZUI_UTL_PageSwitch(E_OSD_TV_PROG);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DCS_Parse
///  Get download control section : parser
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
#if(LOADER_V3_2 == 1)
static BOOL MApp_DecoderMsg_DCS_Parse(void)
{
    UINT16  u16SecLen         = 0;
    UINT16  u16SecLenMask     = 0xFFF;
#ifdef CCA_DECMSG_DECCTRL_OTA //FIXME: will hook up with system driver
    UINT16  u16DLTime         = 0;
#endif
    UINT8   au8Read[1024];
    UINT8   *pu8LoopPosition, *pu8LoopPositionEnd;
    BOOL bRet              = FALSE;
    UINT32  u32ActualSize     = 0;
    UINT32  u32RmnSize        = 0;
    DMX_FILTER_STATUS eStatus = DMX_FILTER_STATUS_ERROR;
    DMX_EVENT eEvent          = DMX_EVENT_DATA_READY;

    if (_u8DmxIdDcs == DMX_DMXID_NULL)
    {
        NGLOG_DEBUG("[DecCtrl] invalid dmx id\n");
        return FALSE;
    }

    memset(au8Read, 0x00, sizeof(au8Read));

    eStatus = MApi_DMX_Proc(_u8DmxIdDcs, &eEvent) ;

    if (eStatus == DMX_FILTER_STATUS_OK)
    {
        if (eEvent == DMX_EVENT_DATA_READY)
        {
            eStatus = MApi_DMX_CopyData(_u8DmxIdDcs, au8Read, SEC_BUF_SIZE_DCS, &u32ActualSize, &u32RmnSize, NULL);
            if (eStatus == DMX_FILTER_STATUS_ERROR)
            {
                if ((u32ActualSize == 0) && (u32RmnSize > 0))
                {
                    MApi_DMX_SectReset(_u8DmxIdDcs);
                }
                return bRet;  // FALSE
            }
            else if (u32ActualSize == 0)
            {
                return bRet; // FALSE
            }
        }
        else
        {
            if (eEvent == DMX_EVENT_SEC_CRCERROR)
            {
                NGLOG_DEBUG("\n[DecCtrl] ===CRC Error===\n");

                //Fix step 06 problem
                _u8CrcErrorCnt++;

                return bRet; // FALSE

            } // if (eEvent == DMX_EVENT_SEC_CRCERROR)

            MApi_DMX_SectReset(_u8DmxIdDcs);

            return bRet; // FALSE
        }
    }
    else
    {
        return bRet;
    }

    /*
           int i;
           NGLOG_DEBUG("\n===");
           for(i = 0; i < 24; i = i + 8)
          {
                 printf("[%x] [%x] [%x] [%x] [%x] [%x] [%x] [%x] ",
                 au8Read[i + 0], au8Read[i + 1], au8Read[i + 2], au8Read[i + 3],
                 au8Read[i + 4], au8Read[i + 5], au8Read[i + 6], au8Read[i + 7]);
          }
          NGLOG_DEBUG("\n===");
    */
    u16SecLen = (((UINT16)au8Read[1] << 8) | ((UINT16)au8Read[2])) & u16SecLenMask;

    pu8LoopPosition    = au8Read + 8;
    pu8LoopPositionEnd = pu8LoopPosition + (u16SecLen - 9);
    //NGLOG_DEBUG("u16SecLen = 0x%02x\n", u16SecLen);

    while (pu8LoopPosition < pu8LoopPositionEnd)
    {
        // Fix step 04 problem
        // FIXME: This relates to system parameter

#ifdef CCA_DECMSG_DECCTRL_OTA //FIXME: will hook up with system driver
        if (gstSystem.u8LoadSequenceNumber < pu8LoopPosition[0])
        {
            TABLE_ID = pu8LoopPosition[1];
            //NGLOG_DEBUG("[DecCtrl] TABLE_ID = 0x%02x\n", TABLE_ID);
            u16DLTime = (pu8LoopPosition[2] << 8) | pu8LoopPosition[3];

#if (CA_IRD_CCA == 1)
            if(gbMenuTriggered == FALSE)
            {
                //diag_printf("Download should take approximately %d seconds. Download commencing in 5 seconds\n", u16DLTime);
                diag_sprintf((char*)pStr, "%s%d%s", "Download should take approximately ", u16DLTime , "seconds. Download commencing in 5 seconds");
                MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
                MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
                //MApp_ZUI_SyncUI();
                MsOS_DelayTask(5000);
            }
            else
            {
                //diag_printf("Download should take approximately %d seconds. Download commencing in 5 seconds. Press OK to start download. Press EXIT to abort download\n",u16DLTime);
                diag_sprintf((char*)pStr, "%s%d%s", "Download should take approximately ", u16DLTime , " seconds. Download commencing in 5 seconds");
                MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
                ComposeMsgData('\r', en_str_OKYes, u16str + wcslen(u16str));
                ComposeMsgData(' ', en_str_ExitCancel, u16str + wcslen(u16str));
                MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
                MApp_ZUI_SyncUI();
            }
#endif

            bRet = TRUE;
            break;
        }
#endif
        pu8LoopPosition += 4;
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DCS_Stop
///  Get download control section : stop filter
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_DCS_Stop(void)
{
    BOOL bError = FALSE;
    if (_u8DmxIdDcs != DMX_DMXID_NULL)
    {
        MApi_DMX_Stop(_u8DmxIdDcs);
        MApi_DMX_Close(_u8DmxIdDcs);
        _u8DmxIdDcs = DMX_DMXID_NULL;
    }

    if (_pu8DcsBuf != NULL)
    {
        bError = IRD_FREE(_pu8DcsBuf);
        if (bError == FALSE)
        {
            NGLOG_DEBUG("[DecCtrl] Free memory failed \n");
            return bError;
        }
        _pu8DcsBuf = NULL;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg__DCS_Start
///  Get download control section : start filter
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_DCS_Start(void)
{
    UINT32* pu32DcsBufSec = NULL;
    UINT16 u16DcsPid      = ES_OTA_PID;
    DMX_Flt_info stDmxInfoDcs;
    DMX_FILTER_STATUS eFltStatus = DMX_FILTER_STATUS_ERROR;
    DMX_FILTER_TYPE eFltType     = DMX_FILTER_SOURCE_TYPE_LIVE | DMX_FILTER_TYPE_SECTION;
    UINT8 au8Pattern[16], au8Mask[16], au8NMask[16];

    // Prepare section buffer
    HB_ASSERT(_pu8DcsBuf == NULL);

    _pu8DcsBuf = IRD_MALLOC((UINT32)SEC_BUF_SIZE_DCS);
    if (_pu8DcsBuf == NULL)
    {
        NGLOG_DEBUG("[DecCtrl] MApp_DecoderMsg_DCS_Start failed!\n");
        MApp_DecoderMsg_DCS_Stop();
        return FALSE;
    }

    pu32DcsBufSec = (UINT32 *)(ALIGN_8((UINT32)_pu8DcsBuf));
    NGLOG_DEBUG("[DecCtrl] DcsBufSec = 0x%08x\n", pu32DcsBufSec);

    // Allocate filter
    eFltStatus = MApi_DMX_Open(eFltType, &_u8DmxIdDcs);
    if (eFltStatus != DMX_FILTER_STATUS_OK)
    {
        NGLOG_DEBUG("[DecCtrl] MApp_DecoderMsg_DCS_Start failed!\n");
        MApp_DecoderMsg_DCS_Stop();
        return FALSE;
    }

    NGLOG_DEBUG("\n[DecCtrl] ===DCS Pid = 0x%03x===\n", u16DcsPid);

    // set PID
    MApi_DMX_Pid(_u8DmxIdDcs, &u16DcsPid, TRUE);

    // set filter property and buffer
    stDmxInfoDcs.Info.SectInfo.SectBufAddr  = (MS_PHYADDR)MsOS_VA2PA((UINT32)pu32DcsBufSec);
    stDmxInfoDcs.Info.SectInfo.SectBufSize  = SEC_BUF_SIZE_DCS;
    stDmxInfoDcs.Info.SectInfo.SectMode     = DMX_SECT_MODE_CONTI | DMX_SECT_MODE_CRCCHK;
    stDmxInfoDcs.Info.SectInfo.Event        = DMX_EVENT_DATA_READY | DMX_EVENT_SEC_CRCERROR | DMX_EVENT_CB_POLL;;
    stDmxInfoDcs.Info.SectInfo.pNotify      = NULL;
    stDmxInfoDcs.Info.SectInfo.pType2Notify = NULL;
    stDmxInfoDcs.Info.SectInfo.Type2NotifyParam1 = 0;

    eFltStatus = MApi_DMX_Info(_u8DmxIdDcs, &stDmxInfoDcs, &eFltType, TRUE);
    if (eFltStatus != DMX_FILTER_STATUS_OK)
    {
        NGLOG_DEBUG("[DecCtrl] MApi_DMX_Info failed!\n");
        MApp_DecoderMsg_DCS_Stop();
        return FALSE;
    }

    // set match pattern
    memset(au8Pattern, 0x00, sizeof(au8Pattern));
    memset(au8Mask,    0x00, sizeof(au8Mask));
    memset(au8NMask,   0x00, sizeof(au8NMask));
    au8Pattern[0] = TABLE_ID_DCS; //DCS table ID

    // FIXME: System parameters
#ifdef CCA_DECMSG_DECCTRL_OTA
    au8Pattern[3] = gstSystem.u8ManufactureCode; //manufacturer code
    au8Pattern[4] = gstSystem.u8HWVersion; //hardware code
#endif
    au8Pattern[6] = 0; //DCS section number is zero
    au8Mask[0] = au8Mask[3] = au8Mask[4] = au8Mask[6] = 0xff;

    eFltStatus = MApi_DMX_SectPatternSet(_u8DmxIdDcs, au8Pattern, au8Mask, au8NMask, sizeof(au8Pattern));
    if (eFltStatus != DMX_FILTER_STATUS_OK)
    {
        NGLOG_DEBUG("[DecCtrl] MApi_DMX_SectPatternSet failed!\n");
        MApp_DecoderMsg_DCS_Stop();
        return FALSE;
    }

    eFltStatus = MApi_DMX_SectReset(_u8DmxIdDcs);
    if (eFltStatus != DMX_FILTER_STATUS_OK)
    {
        NGLOG_DEBUG("[DecCtrl] MApi_DMX_SectReset failed!\n");
        MApp_DecoderMsg_DCS_Stop();
        return FALSE;
    }
    // enable filter
    eFltStatus = MApi_DMX_Start(_u8DmxIdDcs);
    if (eFltStatus != DMX_FILTER_STATUS_OK)
    {
        NGLOG_DEBUG("[DecCtrl] MApi_DMX_Start failed!\n");
        MApp_DecoderMsg_DCS_Stop();
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderGetBAT
///  Get BAT for Decoder Control
///
///  @param [in]    u8OldLoadSeqNum   Old load sequence number
///  @param [in]    u8NewLoadSeqNum  New load sequence number
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
#ifdef CCA_DECMSG_DECCTRL_OTA
static BOOL MApp_DecoderMsg_VerifyLoaderSeqNum(UINT8 u8OldLoadSeqNum, UINT8 u8NewLoadSeqNum )
{
    /* values 0 and 255 are never receivable. */
    if ((u8NewLoadSeqNum == 0x0) || (u8NewLoadSeqNum == 0xFF))
    {
        NGLOG_DEBUG("\n[DecCtrl] Incorrect SW Version %d, %d\n", u8OldLoadSeqNum, u8NewLoadSeqNum);
        return FALSE /* ERROR_DLH_INCORRECT_SW_VERSION */;
    }

    /* If the load sequence number of the STB cannot be discerned due to blank
           or corrupt flash, then the value shall be assumed to be 255 and the
           download window shall run from 1 to 254 inclusive. */
    if (u8NewLoadSeqNum != 0xFF)
    {
        if (u8NewLoadSeqNum > u8OldLoadSeqNum)
        {
            // The window runs from 1 above the current value to 99 above the current value inclusive.
            if ((u8NewLoadSeqNum - u8OldLoadSeqNum ) > 99) // 99 = LOAD_SEQ_NUM_WINDOW
            {
                NGLOG_DEBUG("\n[DecCtrl] Incorrect SW Version %d, %d\n", u8OldLoadSeqNum, u8NewLoadSeqNum);
                return FALSE; // ERROR_INCORRECT_SW_VERSION
            }
        }
        else if (u8NewLoadSeqNum < u8OldLoadSeqNum)
        {
            // If the window extends beyond 256 then modulus 256 should be applied to calculate the limit of the window
            if ((u8NewLoadSeqNum + 256 - u8OldLoadSeqNum) > 99 ) // 99 = LOAD_SEQ_NUM_WINDOW
            {
                NGLOG_DEBUG("\n[DecCtrl] Incorrect SW Version %d, %d\n", u8OldLoadSeqNum, u8NewLoadSeqNum);
                return FALSE; // ERROR_INCORRECT_SW_VERSION
            }
        }
        else
        {
            NGLOG_DEBUG("\n[DecCtrl] Same SW Version %d, %d\n", u8OldLoadSeqNum, u8NewLoadSeqNum);
            return FALSE; // ERROR_SAME_SW_VERSION
        }
    }

    return TRUE; // NO_ERROR
}
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_GetBAT
///  Get BAT for Decoder Control
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
#if(LOADER_V3_2 == 1)
#ifdef CCA_DECMSG_DECCTRL_OTA //FIXME: will hook up with system driver
static BOOL MApp_DecoderMsg_GetBAT(void)
{
    SI_BAT_Info const *pstSiBat;
    UINT32     u32StartTime   = MsOS_GetSystemTime();
    UINT16     u16BIDMask     = 0xFFFF;
    UINT8      u8LinkageIndex = 0;
    EN_LINKAGE eLinkageType   = E_LINKAGE_NO_MATCH;
    BOOL    bCheckloader   = FALSE;
    UINT8      u8ControlByte  = 0x00;

    MApi_SI_Clear_BAT_Info();
    MApi_SI_Parser_BAT_Start(PRIVATE_LINK_DESC, u16BIDMask);
    MsOS_DelayTask(1000);
    MApi_SI_Parser_BAT_Stop();

    pstSiBat = MApi_SI_Get_BAT_Info();




    if(pstSiBat == NULL)
    {
        NGLOG_DEBUG("[DecCtrl]Get BAT Info Fail!\n");
        return FALSE;
    }


    //Get BAT timeout mechanism
    if ((MsOS_GetSystemTime() - u32StartTime) > DEFAULT_BAT_TIMEOUT)
    {
        NGLOG_DEBUG("[DecCtrl]Get BAT timeout!\n");
        return FALSE;
    }

    // FIXME: they are hooked up with System layer
    if(gstSystem.bSetBBCB != TRUE)
    {
        NGLOG_DEBUG("[DecCtrl] set BBCB!\n");
        //ucStatus_setBBCB();
    }

    if(gstSystem.bSetEEPROM != TRUE)
    {
        NGLOG_DEBUG("[DecCtrl] set EEPROM!\n");
        //ucStatus_setEEPROM();
    }

    if(gstSystem.bSetSPCB != TRUE)
    {
        NGLOG_DEBUG("[DecCtrl] set SPCB!\n");
        //ucStatus_setSPCB();
    }

    // Linkage type and control byte decision
    // FIXME: they are hooked up with System layer
    for (u8LinkageIndex = 0; u8LinkageIndex < SI_MAX_IRDETO_LINKAGE; u8LinkageIndex++)
    {
        //NGLOG_DEBUG("\n[DecCtrl] === u8LinkageType = 0x%02x ===  SI_MAX_IRDETO_LINKAGE %d\n", pstSiBat->IrdInfo[u8LinkageIndex].u8LinkageType,SI_MAX_IRDETO_LINKAGE);

        if (pstSiBat->IrdInfo[u8LinkageIndex].u8LinkageType == E_SI_DESC_ENLK) //E_SI_DESC_ENLK = 0x82
        {
            // Modify linkage type descriptor matching flow:
            // if matching manufacture ID is found in 0x82, don't search linkage type 0x80

            NGLOG_DEBUG("\n[DecCtrl] === u8ManufactureCode = 0x%02x ===\n", pstSiBat->IrdInfo[u8LinkageIndex].u8ManufactureCode);
            if (pstSiBat->IrdInfo[u8LinkageIndex].u8ManufactureCode == gstSystem.u8ManufactureCode)
            {
                eLinkageType = E_LINKAGE_MATCH_MAN_ID;
                bCheckloader = MApp_DecoderMsg_VerifyLoaderSeqNum(gstSystem.u8LoadSequenceNumber,
                               pstSiBat->IrdInfo[u8LinkageIndex].u8LoadSeqNum);

                if ((gstSystem.u8HWVersion == pstSiBat->IrdInfo[u8LinkageIndex].u8HardwareCode)    &&
                        (gstSystem.u16Variant == pstSiBat->IrdInfo[u8LinkageIndex].u16OperatorVariant) &&
                        (bCheckloader == TRUE))
                {
                    eLinkageType = E_LINKAGE_MATCH;
                    TS_ID   = pstSiBat->IrdInfo[u8LinkageIndex].u16TsID;
                    ON_ID   = pstSiBat->IrdInfo[u8LinkageIndex].u16OriNID;
                    SERV_ID = pstSiBat->IrdInfo[u8LinkageIndex].u16ServiceID;

                    u8ControlByte = pstSiBat->IrdInfo[u8LinkageIndex].u8CtrlByte;

                    NGLOG_DEBUG("\n[DecCtrl] === TS_ID:%d ===\n", TS_ID);
                    NGLOG_DEBUG("\n[DecCtrl] === ON_ID:%d ===\n", ON_ID);
                    NGLOG_DEBUG("\n[DecCtrl] === SERV_ID:%d ===\n", SERV_ID);
                    NGLOG_DEBUG("\n[DecCtrl] === Control Byte:0x%02X ===\n", u8ControlByte);

                    break;
                }
            }
        }
    }//for(u8LinkageIndex = 0; u8LinkageIndex < SI_MAX_IRDETO_LINKAGE; u8LinkageIndex++)

    /* 0x82 doesn't have any match */
    if (eLinkageType == E_LINKAGE_NO_MATCH)
    {
        for (u8LinkageIndex = 0; u8LinkageIndex < SI_MAX_IRDETO_LINKAGE; u8LinkageIndex++)
        {
            //0x80 only takes first match
            // FIXME: they are hooked up with System layer
            if (pstSiBat->IrdInfo[u8LinkageIndex].u8LinkageType == E_SI_DESC_STLK)
            {
                if (pstSiBat->IrdInfo[u8LinkageIndex].u8ManufactureCode == gstSystem.u8ManufactureCode)
                {
                    eLinkageType = E_LINKAGE_MATCH;
                    TS_ID   = pstSiBat->IrdInfo[u8LinkageIndex].u16TsID;
                    ON_ID   = pstSiBat->IrdInfo[u8LinkageIndex].u16OriNID;
                    SERV_ID = pstSiBat->IrdInfo[u8LinkageIndex].u16ServiceID;

                    u8ControlByte = pstSiBat->IrdInfo[u8LinkageIndex].u8CtrlByte;
                    NGLOG_DEBUG("\n[DecCtrl] === TS_ID:%d ===\n", TS_ID);
                    NGLOG_DEBUG("\n[DecCtrl] === ON_ID:%d ===\n", ON_ID);
                    NGLOG_DEBUG("\n[DecCtrl] === SERV_ID:%d ===\n", SERV_ID);
                    NGLOG_DEBUG("\n[DecCtrl] === Control Byte:0x%02X ===\n", u8ControlByte);
                    break;
                }
            }

        }
    }

    // No match
    if (eLinkageType != E_LINKAGE_MATCH)
    {
        NGLOG_DEBUG("\n[DecCtrl] Incorrect Linkage type %d\n", eLinkageType);
        return FALSE;
    }

    // If any match, check control bytes
    if (u8ControlByte == 0x00)
    {
        // menu disabled, emm disabled
        // Only disrupted downloads may use the download stream
        gbMenuTriggered = FALSE;
        NGLOG_DEBUG("\n[DecCtrl] u8ControlByte = %d\n", u8ControlByte);
        return FALSE;
    }
    else if (u8ControlByte == 0x01)
    {
        // menu disabled, emm enabled
        // No STB menu invoked downloads.
        // Download may be invoked from the front panel or disrupted downloads or download EMM
        if (gbMenuTriggered == TRUE)
        {
            NGLOG_DEBUG("\n[DecCtrl] u8ControlByte = %d\n", u8ControlByte);
            return FALSE;
        }
        //else emm triggered
    }
    else if (u8ControlByte == 0x02)
    {
        // menu enabled, emm enabled
        // Downloads from the STB Menu, the front panel, or disrupted downloads and download EMM permitted
        if (gbMenuTriggered == TRUE)
        {
            // Download flag is set?
            if (gstDecCtrl.stDownload.bDownloadAllowed == FALSE)
            {
                NGLOG_DEBUG("\n[DecCtrl] u8ControlByte = %d. Download allowed %d\n", u8ControlByte, gstDecCtrl.stDownload.bDownloadAllowed);
                return FALSE;
            }
        }
    }
    else if (u8ControlByte == 0x03)
    {
        // not required?
        // Downloads from the STB Menu, the front panel, or disrupted downloads, download EMM permitted
        // and Active Announcement by OSD allowed based on automatic detection by the STB
        NGLOG_DEBUG("\n[DecCtrl] u8ControlByte = %d\n", u8ControlByte);
        return TRUE;
    }
    else
    {
        NGLOG_DEBUG("\n[DecCtrl] Unknown u8ControlByte = %d\n", u8ControlByte);
        return FALSE;
    }

    return TRUE;

}
#endif
#endif

#ifdef CCA_DECMSG_DECCTRL_OTA //FIXME: will hook up with system driver.

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_GetNIT_Sat
///  Get NIT for Decoder Control
///
///  @param [in]    pstParam  Pointer to the struct of Carrier parameters
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

#if(FRONTEND_TYPE_SEL==DVBS)
#ifndef BCD2Dec4Byte
#define BCD2Dec4Byte(x)      ((((x)>>24&0x0F) * 1000000) + (((x)>>20&0x0F) * 100000) + (((x)>>16&0x0F) * 10000) \
                              + (((x)>>12&0x0F) * 1000) + (((x)>>8&0x0F) * 100) + (((x)>>4&0x0F) * 10) + ((x)&0x0F))
#endif

static BOOL MApp_DecoderMsg_GetNIT_Sat(MS_FE_CARRIER_PARAM* pstParam)
{
    BOOL Result1;
    BOOL Result2;
    UINT32     u32StartTime = 0;
    UINT16 		RemainFreq = 1;
    DESC_SATEL_DEL_SYS  astNitInfo[CROSS_TRANSPONDER_NUM];
    DESC_SATEL_DEL_SYS* pstNitInfo = NULL;


    Result1 = Result2 = FALSE;

    memset(&astNitInfo, 0x00, sizeof(DESC_SATEL_DEL_SYS) * CROSS_TRANSPONDER_NUM);

    u32StartTime = MsOS_GetSystemTime();

    while(1)
    {
        //[TODO] check EXIT key

        // FIXME: It needs to think more if it already got by the other process
        Result1 = MApi_SI_GetNextSatelParameter(&astNitInfo[0], &RemainFreq);

        //MApi_SI_GetNextSatelParameter(DESC_SATEL_DEL_SYS *desc,&RemainFreq )
        if (Result1 == FALSE)
        {
            if ((MsOS_GetSystemTime() - u32StartTime) > DEFAULT_DVBT_NIT_TIMEOUT)
            {
                break;
            }

            MsOS_DelayTask(50);
        }
        else
        {
            // try to get the next one
            Result2 = MApi_SI_GetNextSatelParameter(&astNitInfo[1], &RemainFreq);
            NGLOG_DEBUG("[DecCtrl] SI is ready.\n");
            break;
        }
    }

    if (Result1 != TRUE)
    {
        NGLOG_DEBUG("[DecCtrl] Get NIT timeout.\n");
        return FALSE;
    }
    else
    {
        // Check if cross transponder exist
        if (Result2 == TRUE)
        {
            NGLOG_DEBUG("[DecCtrl] Cross transponder exist!\n");
            pstNitInfo = &astNitInfo[1];
        }
        else
        {
            NGLOG_DEBUG("[DecCtrl] Cross transponder not exist!\n");
            pstNitInfo = &astNitInfo[0];
        }


        pstParam->u32Frequency = BCD2Dec4Byte(pstNitInfo->u32CentreFreq)/100;
        pstParam->SatParam.u16SymbolRate = BCD2Dec4Byte((pstNitInfo->u32SymbolRate)&0xfffffff)/10;
        pstParam->SatParam.u8Polarity = pstNitInfo->u8Polarization;
//		pstParam->SatParam.u8SatID = _lnb_satelliteID[0];

        return TRUE;
    }
}
#elif (FRONTEND_TYPE_SEL==DVBC)
///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_GetNIT_Cable
///  Get NIT for Decoder Control
///
///  @param [in]    pstParam  Pointer to the struct of Carrier parameters
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_GetNIT_Cable(MS_FE_CARRIER_PARAM* pstParam)
{
    SI_NIT_CDS_Info     astNitInfo[CROSS_TRANSPONDER_NUM];
    SI_NIT_CDS_Info*    pstNitInfo      = NULL;
    SI_Result           eResult         = E_SI_NOT_READY;
    UINT32              u32StartTime    = 0;
    UINT8               u8TableNum      = CROSS_TRANSPONDER_NUM;

    memset(&astNitInfo, 0x00, sizeof(SI_NIT_CDS_Info) * CROSS_TRANSPONDER_NUM);

    u32StartTime = MsOS_GetSystemTime();

    while(1)
    {
        //[TODO] check EXIT key
        eResult = MApi_SI_Parser_Get_NIT_FreqTableInfo(&u8TableNum, &astNitInfo[0]);

        if (eResult == E_SI_NOT_READY)
        {
            if ((MsOS_GetSystemTime() - u32StartTime) > DEFAULT_DVBC_NIT_TIMEOUT)
            {
                NGLOG_DEBUG("[DecCtrl] Get NIT timeout.\n");
                break;
            }

            MsOS_DelayTask(50);
        }
        else
        {
            NGLOG_DEBUG("[DecCtrl] SI is ready.\n");
            break;
        }
    }

    if (eResult != E_SI_OK)
    {
        NGLOG_DEBUG("[DecCtrl] Get NIT failed:[%d]\n", eResult);
        return FALSE;
    }
    else
    {
        // Check if cross transponder exist
        NGLOG_DEBUG("[DecCtrl] Check if cross transponder exist\n");
        if (u8TableNum == 1)
        {
            pstNitInfo = &astNitInfo[0];
        }
        else
        {
            pstNitInfo = &astNitInfo[1];
        }

        pstParam->u32Frequency              = (pstNitInfo->u32Frequency)/1000;
        pstParam->CabParam.eConstellation   = (pstNitInfo->eModulation) - 1;
        pstParam->CabParam.u16SymbolRate    = (pstNitInfo->u32SymbolRate)/1000;

        NGLOG_DEBUG("\n[DecCtrl] === C Frequency     :%ld ===\n", pstParam->u32Frequency);
        NGLOG_DEBUG("\n[DecCtrl] === C Modulation    :%d ===\n",  pstParam->CabParam.eConstellation);
        NGLOG_DEBUG("\n[DecCtrl] === C Symbol Rate   :%d ===\n",  pstParam->CabParam.u16SymbolRate);
        return TRUE;
    }
}

#else
///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_GetNIT
///  Get NIT for Decoder Control
///
///  @param [in]    pstParam  Pointer to the struct of Carrier parameters
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_GetNIT(MS_FE_CARRIER_PARAM* pstParam)
{
    SI_Result  eResult      = E_SI_NOT_READY;
    UINT32     u32StartTime = 0;
    DESC_TERR_DEL_SYS  astNitInfo[CROSS_TRANSPONDER_NUM];
    DESC_TERR_DEL_SYS* pstNitInfo = NULL;
    DESC_TERR_DEL_TYPE eType;

    memset(&astNitInfo, 0x00, sizeof(DESC_TERR_DEL_SYS) * CROSS_TRANSPONDER_NUM);

    u32StartTime = MsOS_GetSystemTime();

    while(1)
    {
        //[TODO] check EXIT key

        // FIXME: MApi_SI_Parser_Get_NIT_DVBT()
        eResult = MApi_SI_Parser_Get_NIT_DVBT(&astNitInfo[0], &eType);

        if (eResult == E_SI_NOT_READY)
        {
            if ((MsOS_GetSystemTime() - u32StartTime) > DEFAULT_DVBT_NIT_TIMEOUT)
            {
                break;
            }

            MsOS_DelayTask(50);
        }
        else
        {
            NGLOG_DEBUG("[DecCtrl] SI is ready.\n");
            break;
        }
    }

    if (eResult != E_SI_OK)
    {
        NGLOG_DEBUG("[DecCtrl] Get NIT timeout.\n");
        return FALSE;
    }
    else
    {
        // Check if cross transponder exist
        NGLOG_DEBUG("[DecCtrl] Check if cross transponder exist\n");
        if (astNitInfo[0].u8fOtherFreq == 0)
        {
            pstNitInfo = &astNitInfo[0];
        }
        else
        {
            pstNitInfo = &astNitInfo[1];
        }

        // Store NIT info
        if (eType == DESC_TERR_DEL_T2)
        {
            pstParam->TerParam.u8PlpID = pstNitInfo->u8PlpID;
            pstParam->u32Frequency     = (pstNitInfo->u32CentreFreq) / 1000;

            NGLOG_DEBUG("\n[DecCtrl] === T2 Frequency:%ld ===\n", pstParam->u32Frequency);
            NGLOG_DEBUG("\n[DecCtrl] === T2 BandWidth:%d ===\n", pstNitInfo->u16BW);

            pstParam->TerParam.eBandWidth = pstNitInfo->u16BW - 6;
            pstParam->TerParam.u8ScanType = E_DEMOD_TYPE_T2;
        }
        else
        {
        #if(LOADER_V3_2 == 1)
            pstParam->u32Frequency = (pstNitInfo->u32CentreFreq) / 100;
        #else
            pstParam->u32Frequency = (pstNitInfo->u32CentreFreq)*1000;
        #endif

            NGLOG_DEBUG("\n[DecCtrl] === T Frequency:%ld ===\n", pstParam->u32Frequency);
            NGLOG_DEBUG("\n[DecCtrl] === T BandWidth:%d ===\n", pstNitInfo->u8BW);

            if ((pstNitInfo->u8BW == 6) || (pstNitInfo->u8BW == 7))
            {
                pstParam->TerParam.eBandWidth = (pstNitInfo->u8BW) - 6;
            }
            else
            {
                pstParam->TerParam.eBandWidth = 2;//8 MHz
            }
            pstParam->TerParam.u8PlpID = pstNitInfo->u8PlpID;
            pstParam->TerParam.u8ScanType = DESC_TERR_DEL_T;
        }

        return TRUE;
    }
}


#endif



#endif
static WORD pid_num = 0xFFF;
extern UINT16	u16DSMCC_PID;
lc_result LoaderCoreAPI_WriteDVBSSUParameters( const lc_dvb_ssu_parameters *pDvbssuParameters, 
    const lc_buffer_st *pPrivateData);

lc_dvb_ssu_parameters lc_dvb_ssu_parameters_DVTapp;
lc_buffer_st  pPrivateData_DVTapp = {LC_NULL, 0};
static BOOL MApp_LC_TuningParamToLoaderV5(MS_FE_CARRIER_PARAM* pstParam)
{
    lc_result result;
    LC_TUNING_PARAMETER lc_tuning_parameter;
	lc_tuning_parameter.FE_Para.TerParam.eBandWidth = BW_MODE_8MHZ;
	pid_num = u16DSMCC_PID;
	lc_dvb_ssu_parameters_DVTapp.pid = pid_num;
	if(LC_MANUFACTURER_PRIVATE_INFO_LENGTH <= sizeof(LC_TUNING_PARAMETER))
	{
		pPrivateData_DVTapp.length = sizeof(LC_TUNING_PARAMETER); //
	}
	else
	{
		pPrivateData_DVTapp.length = LC_MANUFACTURER_PRIVATE_INFO_LENGTH; //	
	}
	memset(&lc_tuning_parameter,0,sizeof(lc_tuning_parameter));
	pPrivateData_DVTapp.bytes = (lc_uchar *)&lc_tuning_parameter;
	lc_tuning_parameter.FE_Para.u32Frequency = pstParam->u32Frequency;
	lc_tuning_parameter.FE_Para.TerParam.eLevelSel = pstParam->TerParam.eLevelSel;
	lc_tuning_parameter.FE_Para.TerParam.u8PlpID = pstParam->TerParam.u8PlpID;
	lc_tuning_parameter.FE_Para.TerParam.eConstellation= pstParam->TerParam.eConstellation;
    lc_tuning_parameter.FE_Para.TerParam.u8ScanType= pstParam->TerParam.u8ScanType;
    lc_tuning_parameter.FE_Para.TerParam.eBandWidth= pstParam->TerParam.eBandWidth;
    NGLOG_DEBUG("demodParam.TerParam.eBandWidth %lu\n",lc_tuning_parameter.FE_Para.TerParam.eBandWidth);
	NGLOG_DEBUG("demodParam.u32Frequency %lu\n",lc_tuning_parameter.FE_Para.u32Frequency);
	NGLOG_DEBUG("demodParam.TerParam.u8ScanType %lu\n",lc_tuning_parameter.FE_Para.TerParam.u8ScanType);
	NGLOG_DEBUG("demodParam.u8PlpID %lu\n",lc_tuning_parameter.FE_Para.TerParam.u8PlpID);
	NGLOG_DEBUG("demodParam.TerParam.eLevelSel %lu\n",lc_tuning_parameter.FE_Para.TerParam.eLevelSel);
	NGLOG_DEBUG("demodParam.eConstellation %lu\n",lc_tuning_parameter.FE_Para.TerParam.eConstellation);
	result = LoaderCoreAPI_WriteDVBSSUParameters( &lc_dvb_ssu_parameters_DVTapp, &pPrivateData_DVTapp);
	if( result != LC_SUCCESS)
	{
	    return FALSE;
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_Tune2Connect
///  Tune and connnect for Decoder Control
///
///  @param [in]    pstParam  Pointer to the struct of tunning parameters
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
extern BOOL OTA_trigger;
static BOOL MApp_DecoderMsg_Tune2Connect(MS_FE_CARRIER_PARAM* pstParam)
{
    SI_PAT_Info stSI_PAT_Info;
    SI_PMT_Info stSI_PMT_Info;
    UINT32  u32StartTime    = MsOS_GetSystemTime();
    UINT8   u8Loop          = 0;
    BOOL bIsValidService = FALSE;
    BOOL bRet            = FALSE;

#if (CA_IRD_CCA == 1)
    //diag_printf("connecting to download stream(30 seconds remaining)\n");
    diag_sprintf((char*)pStr, "%s", "connecting to download stream\r (30 seconds remaining)");
    MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
    MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
    if(gbMenuTriggered == TRUE)
    {
        MApp_ZUI_SyncUI();
    }
#endif

    //Tune to RF channel
#if(DTV_SYSTEM_STYLE == DVBS_STYLE)
    if (MApi_DigiTuner_Tune2RfCh(_pstSATParam, pstParam, FE_TUNE_MANUAL) == FALSE)
#else
    if (MApi_DigiTuner_Tune2RfCh(pstParam, FE_TUNE_MANUAL) == FALSE)
#endif
    {
        NGLOG_DEBUG("\n[DecCtrl] === Failed to tune to RF channel ===\n");
        return FALSE;
    }

    while ((MsOS_GetSystemTime() - u32StartTime) < DOWNLOAD_TS_TIMEOUT)
    {
#if(CA_IRD_CCA == 1)
        //diag_printf("connecting to download stream(%d seconds remaining)\n", (DOWNLOAD_TS_TIMEOUT -(MsOS_GetSystemTime() - u32StartTime))/1000 );
        diag_sprintf((char*)pStr, "%s%d%s", "connecting to download stream\r(", (DOWNLOAD_TS_TIMEOUT -(MsOS_GetSystemTime() - u32StartTime))/1000 ," seconds remaining)");
        MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
        MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
        if(gbMenuTriggered == TRUE)
        {
            MApp_ZUI_SyncUI();
        }
#endif

        MsOS_DelayTask(100);

        // Check if demod locks or not
        if (E_DEMOD_LOCK == MApi_DigiTuner_TPSGetLock())
        {
            NGLOG_DEBUG("[DecCtrl] TPS get lock Done\n");
            if (MApi_SI_Monitor_Get_PAT_Info(&stSI_PAT_Info) == TRUE)
            {
                NGLOG_DEBUG("[DecCtrl] TPS get PAT info Done\n");
                for (u8Loop = 0; u8Loop < stSI_PAT_Info.u8NumOfSrv; u8Loop++)
                {
                    NGLOG_DEBUG("[DecCtrl] stSI_PAT_Info.stServiceIDInfo[u8Loop].u16ServiceID = 0x%x\n",stSI_PAT_Info.stServiceIDInfo[u8Loop].u16ServiceID);
                    NGLOG_DEBUG("[DecCtrl] SERV_ID = 0x%x\n",SERV_ID);
                    #if(LOADER_V5 == 1)
                        if( OTA_trigger ==  TRUE )
                        {
                            extern UINT16 NIT_service_ID;
                            SERV_ID = NIT_service_ID;
                        }
                    #endif
                    if (stSI_PAT_Info.stServiceIDInfo[u8Loop].u16ServiceID == SERV_ID)
                    {
                        NGLOG_DEBUG("\n[DecCtrl] === PAT program number: [%d] ===\n", SERV_ID);
                        NGLOG_DEBUG("\n[DecCtrl] === PMT pid: [%d] ===\n", stSI_PAT_Info.stServiceIDInfo[u8Loop].u16PmtPID);
                        bIsValidService = TRUE;
                        break;
                    }
                }
            }
            else
            {
                NGLOG_DEBUG("\n[DecCtrl] TPS get PAT info failed\n");
            }

            if (bIsValidService == TRUE)
            {
                NGLOG_DEBUG("\n[DecCtrl] Valid service\n");
                //Get PMT
                // FIXME: MApi_SI_Monitor_Get_PMT_InfoWithServID() from  SIV2
                //if (MApi_SI_Monitor_Get_PMT_InfoWithServID(&stSI_PMT_Info, SERV_ID, stSI_PAT_Info.stServiceIDInfo[u8Loop].u16PmtPID) == TRUE)
                //mark by darien
                {
                    NGLOG_DEBUG("\n[DecCtrl] === ES pid: [0x%04x] ===\n", stSI_PMT_Info.u16OTASectionPID);
                    ES_OTA_PID = stSI_PMT_Info.u16OTASectionPID;
                }

                bRet = TRUE;
                break;
            }
            else
            {
                NGLOG_DEBUG("\n[DecCtrl] Invalid service\n");
                MsOS_DelayTask(100);
            }
        }
        else
        {
            NGLOG_DEBUG("\n[DecCtrl] TPS get lock failed\n");
            MsOS_DelayTask(100);
        }
    }
#if(LOADER_V5 == 1)
    MApp_LC_TuningParamToLoaderV5(pstParam);
#endif

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_RevertViewingStatus
///  Revert viewing status for Decoder Control
///
///  @param [in]    pstParam  Pointer to the stuct of download parameters
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_RevertViewingStatus(MS_FE_CARRIER_PARAM* pstParam)
{
    // Tune to RF channel
#if(DTV_SYSTEM_STYLE == DVBS_STYLE)
    if (MApi_DigiTuner_Tune2RfCh(_pstSATParam, pstParam, FE_TUNE_MANUAL) == FALSE)
#else
    if (MApi_DigiTuner_Tune2RfCh(pstParam, FE_TUNE_MANUAL) == FALSE)
#endif
    {
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_ProcessDownloadParam
///  Propcess Downlad parameters for Decoder Control
///
///  @param [in]    pstParam  Pointer to the stuct of download parameters
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
#if(LOADER_V3_2 == 1)
static BOOL MApp_DecoderMsg_ProcessDownloadParam(MS_FE_CARRIER_PARAM* pstParam)
{
    UINT32 u32StartTime = MsOS_GetSystemTime();

#if (CA_IRD_CCA == 1)
    //diag_printf("Analysing download stream (30 seconds remaining).\n");
    diag_sprintf((char*)pStr, "%s", "Analysing download stream \r (30 seconds remaining)");
    MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
    MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
    if(gbMenuTriggered == TRUE)
    {
        MApp_ZUI_SyncUI();
    }
#endif

    // get download control section : filter start
    if (MApp_DecoderMsg_DCS_Start() == FALSE)
    {
        return FALSE;
    }

    _u8CrcErrorCnt = 0;

    // Get download control section : parse
    while ((MsOS_GetSystemTime() - u32StartTime) < DOWNLOAD_TS_TIMEOUT)
    {
#if (CA_IRD_CCA == 1)
        //diag_printf("Analysing download stream (%d seconds remaining).\n", (DOWNLOAD_TS_TIMEOUT- (MsOS_GetSystemTime() - u32StartTime))/1000);
        diag_sprintf((char*)pStr, "%s%d%s", "Analysing download stream \r(", (DOWNLOAD_TS_TIMEOUT- (MsOS_GetSystemTime() - u32StartTime))/1000, " seconds remaining)");
        MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
        MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
        if(gbMenuTriggered == TRUE)
        {
            MApp_ZUI_SyncUI();
        }
#endif

        MsOS_DelayTask(100);

        // Fix step 06 problem
        // Crc error check
        if (_u8CrcErrorCnt >= 3)
        {
            // Stop the DCS filter
            MApp_DecoderMsg_DCS_Stop();
            return FALSE;
        }

        if (MApp_DecoderMsg_DCS_Parse() == TRUE)
        {
            break;
        }
    }

    // Get download control section : filter stop
    MApp_DecoderMsg_DCS_Stop();

    // Check 30secs timeout
    if ((MsOS_GetSystemTime() - u32StartTime) > DOWNLOAD_TS_TIMEOUT)
    {
        return FALSE;
    }

    // Set NVM download status to EEPROM_DECLDR_START
    // FIXME: EEPROM processing
#if ( (MS_DVB_TYPE_SEL == DVBS) || (MS_DVB_TYPE_SEL == DVBS2) )
    if (FALSE == MApp_DecoderMsg_WriteLoaderpt(pstParam, _pstSATParam, ES_OTA_PID, TABLE_ID))
#else
    if (FALSE == MApp_DecoderMsg_WriteLoaderpt(pstParam, ES_OTA_PID, TABLE_ID))
#endif
    {
        return FALSE;
    }

    NGLOG_DEBUG("[DecCtrl]\n");

    //Check 30secs timeout
    if ((MsOS_GetSystemTime() - u32StartTime) > DOWNLOAD_TS_TIMEOUT)
    {
        return FALSE;
    }

    // FIXME: EEPROM processing
    if (MApp_DecoderMsg_CheckLoaderPtTSParam() == FALSE)
    {
        return FALSE;
    }

    // Check 30secs timeout
    if ((MsOS_GetSystemTime() - u32StartTime) > DOWNLOAD_TS_TIMEOUT)
    {
        return FALSE;
    }

    return TRUE;
}
#endif
///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_ProcessOTAFlag
///  Process OTA flag for Decoder Control
///
///  @param [in]    bFlg  Flag to enable/disable OTA write
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_ProcessOTAFlag(BOOL bFlg)
{
    NGLOG_DEBUG("[DecCtrl]\n");

    // Processing of Loader data
    if(FALSE == MApp_DecoderMsg_WriteLoaderPtOTAFlag(bFlg))
    {
        return FALSE;
    }

    // Reset the decoder/system
    MApp_DecoderMsg_ResetDecoder();

    return TRUE;
}

#if (CA_IRD_CCA == 1)
BOOL MApp_DecoderMsg_MenuProcessOTAFlag(BOOL bFlg)
{
    return MApp_DecoderMsg_ProcessOTAFlag(bFlg);
}
#endif
///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderDownloadControl
///  Download process for Decoder Control
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL MApp_DecoderMsg_DecoderDownloadControl(void)
{
    NGLOG_DEBUG("MApp_DecoderMsg_DecoderDownloadControl\n");
    MS_FE_CARRIER_PARAM stDmdParam;

    SI_ParseParm            siParam;

    siParam.bDsmccSync      = FALSE;
    siParam.bNITFreqTable   = TRUE;

    // 1. BAT
#if (CA_IRD_CCA == 1)
    gbMenuDownloadReady = EN_OTA_MENU_TRIGGER_STATE_START;
#else
    gbMenuTriggered = FALSE;
#endif

#ifdef CCA_DECMSG_DECCTRL_OTA
#if ( LOADER_V3_2 == 1)
    if (MApp_DecoderMsg_GetBAT() == FALSE)
    {
        // 1. Get BAT timeout
        // 2. No matching linkage descriptors
        // 3. Menu-triggered but menu diasbled
        // 4. Emm & menu both disabled
        // 5. flag download flag is not set
        NGLOG_DEBUG("[DecCtrl] Failed to get BAT\n");
        return FALSE;
    }
#endif

#if (CA_IRD_CCA == 1)
    if (gbMenuTriggered == TRUE)
    {
        NGLOG_DEBUG("MApp_DecoderMsg_DecoderDownloadControl\n");
        //diag_printf("Looking for tuning parameters of the download service\n");
        diag_sprintf((char*)pStr, "%s", "Looking for tuning parameters of the download service");
        MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
        MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
        MApp_ZUI_SyncUI();
        MsOS_DelayTask(2000);//slime
    }
#endif

    // 2. NIT
    memset(&stDmdParam, 0, sizeof(MS_FE_CARRIER_PARAM));

#if ((FRONTEND_TYPE_SEL==DVBS) || (FRONTEND_TYPE_SEL==DVBC))
    if (MApi_SI_Parser_Monitor_IsStart(0))
    {
        MApi_SI_Parser_Monitor_Stop(0);
        while (MApi_SI_Parser_Monitor_IsStart(0))
        {
            MsOS_DelayTask(10);
        }
    }
    MApi_SI_Parser_CScan_Start(0, &siParam, _MENU_CSCAN_PAT_BASE);
#endif
#if(FRONTEND_TYPE_SEL==DVBS)
    if (MApp_DecoderMsg_GetNIT_Sat(&stDmdParam) == FALSE)
#elif (FRONTEND_TYPE_SEL==DVBC)
    if (MApp_DecoderMsg_GetNIT_Cable(&stDmdParam) == FALSE)
#else
    if (MApp_DecoderMsg_GetNIT(&stDmdParam) == FALSE)
#endif
    {
        //Get NIT timeout
        NGLOG_DEBUG("[DecCtrl] Failed to get NIT\n");
#if  (CA_IRD_CCA == 1)
        if (gbMenuTriggered == TRUE)
        {
            //diag_printf("Looking for tuning parameters failed. Aborting download.\n");
            diag_sprintf((char*)pStr, "%s", "Looking for tuning parameters failed.\r Aborting download.");
            MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
            ComposeMsgData('\r', en_str_OKYes, u16str + wcslen(u16str));
            MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
            MApp_ZUI_SyncUI();
        }
#endif

        return FALSE;
    }

#if ((FRONTEND_TYPE_SEL==DVBS) || (FRONTEND_TYPE_SEL==DVBC))
    MApi_SI_Parser_CScan_Stop(0);
    MsOS_DelayTask(300);

    SI_ParseParm SI_Parm;
    //stop Current monitor
    MApi_SI_Parser_Monitor_Stop(0);
    memset(&SI_Parm, 0x0, sizeof(SI_ParseParm));
    MApi_SI_Parser_Monitor_Start(0, &SI_Parm);
#endif

#endif

    // 3. Start to Tune
    // Start to tune to TS and connect to the download service (including process PAT,PMT,etc) with a 30-second countdown

    memcpy(&gstOrigParam, &_ptunerParam, sizeof(MS_FE_CARRIER_PARAM));
    if (MApp_DecoderMsg_Tune2Connect(&stDmdParam) == FALSE)
    {
        MApp_DecoderMsg_RevertViewingStatus(&gstOrigParam);
#if (CA_IRD_CCA == 1)
        if(gbMenuTriggered == FALSE)
        {
            //diag_printf("Connection to download stream failed. Aborting download.\n");
            diag_sprintf((char*)pStr, "%s", "Connection to download stream failed.\r Aborting download.");
            MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
            MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
            //MApp_ZUI_SyncUI();
            MsOS_DelayTask(5000);
            MApp_ZUI_POP_Disable();
        }
        else
        {
            //diag_printf("Connection to download stream failed. Aborting download. Press OK to acknowledge.\n");
            diag_sprintf((char*)pStr, "%s", "Connection to download stream failed.\r Aborting download.\r Press OK to acknowledge.");
            MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
            ComposeMsgData('\r', en_str_OKYes, u16str + wcslen(u16str));
            MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
            MApp_ZUI_SyncUI();
        }
#endif

        NGLOG_DEBUG("[DecCtrl] Failed to tune to TS\n");
        return FALSE;
    }

#if(LOADER_V3_2 == 1)
    // 4. Process Download Parameter
    if (MApp_DecoderMsg_ProcessDownloadParam(&stDmdParam) == FALSE)
    {
        // 5. B status
        MApp_DecoderMsg_RevertViewingStatus(&stDmdParam);
#if (CA_IRD_CCA == 1)
        if(gbMenuTriggered == FALSE)
        {
            //diag_printf("Download stream notpresent. Aborting download.\n");
            diag_sprintf((char*)pStr, "%s", "Download stream not present.\r Aborting download.");
            MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
            MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
            //MApp_ZUI_SyncUI();
            MsOS_DelayTask(5000);
            MApp_ZUI_POP_Disable();
        }
        else
        {
            //diag_printf("Download stream not present.Aborting download. Press OK to acknowledge.\n");
            diag_sprintf((char*)pStr, "%s", "Download stream not present.\r Aborting download.\r Press OK to acknowledge.");
            MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
            ComposeMsgData('\r', en_str_OKYes, u16str + wcslen(u16str));
            MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
            MApp_ZUI_SyncUI();
        }
#endif
        NGLOG_DEBUG("[DecCtrl] Failed to Process Download Parameter\n");
        return FALSE;
    }
#else
    // 6. Reboot and then Process OTA flag
#if (CA_IRD_CCA == 1)
            if(gbMenuTriggered == TRUE)
            {
                printf("\033[1;34m[%s:%d]  \n\033[m",__FUNCTION__,__LINE__);
                diag_sprintf((char*)pStr, "%s","Download commencing in 5 seconds");
                MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
                MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
				if(gbMenuTriggered == TRUE)
				{
					MApp_ZUI_SyncUI();
				}
                MsOS_DelayTask(5000);
            }
            else
            {
                diag_sprintf((char*)pStr, "%s", "Press OK to start download. Press EXIT to abort download" );
                MApi_U8StringToU16String(pStr, u16str, (strlen(pStr)));
                ComposeMsgData('\r', en_str_OKYes, u16str + wcslen(u16str));
                ComposeMsgData(' ', en_str_ExitCancel, u16str + wcslen(u16str));
                MApp_ZUI_POP_Enable((UINT8*)u16str, wcslen(u16str) * 2, 0);
                MApp_ZUI_SyncUI();
            }
        #endif
#endif
#if ((CA_IRD_CCA == 1)&&(LOADER_V3_2 == 1))
    if(gbMenuTriggered == FALSE)
    {
        MApp_DecoderMsg_ProcessOTAFlag(TRUE);
    }
    else
    {
        gbMenuDownloadReady = EN_OTA_MENU_TRIGGER_STATE_READY;
    }
#else
    gbEMMTriggered = FALSE;
    MApp_DecoderMsg_ProcessOTAFlag(TRUE);
#endif
    return TRUE;
}

#if (CA_IRD_CCA == 1)
///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderDownloadControl
///  Download process for Decoder Control
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/12/10
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_MenuDownloadReady(void)
{
    if((gbMenuTriggered==TRUE) && (gbMenuDownloadReady==EN_OTA_MENU_TRIGGER_STATE_READY))
    {
        return TRUE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderDownloadControl
///  Download process for Decoder Control
///
///  @param [in]
///
///  @return menu state.
///
///  @author MStarSemi @date 2013/12/10
///////////////////////////////////////////////////////////////////////////////
EN_OTA_MENU_TRIGGER_STATE_TYPE MApp_DecoderMsg_MenuStateGet(void)
{
    return (EN_OTA_MENU_TRIGGER_STATE_TYPE)gbMenuDownloadReady;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderDownloadControl
///  Download process for Decoder Control
///
///  @param [in]    state       menu state
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/12/10
///////////////////////////////////////////////////////////////////////////////
void MApp_DecoderMsg_MenuStateSet(EN_OTA_MENU_TRIGGER_STATE_TYPE state)
{
    gbMenuDownloadReady = state;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderDownloadControl
///  Download process for Decoder Control
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/12/10
///////////////////////////////////////////////////////////////////////////////
extern UINT16 	NIT_service_ID;
static void Get_NIT_Info_LoaderV5(void)
{
    SERV_ID = NIT_service_ID;
}
extern BOOL OTA_trigger;
BOOL MApp_MenuPreDownloadControl(void) //slime
{
    gbMenuTriggered = TRUE;
    MApp_DecoderMsg_DecControl_InitParser();
#if(LOADER_V3_2 == 1)
    return MApp_DecoderMsg_GetBAT();
#else
    if((OTA_trigger == TRUE)&&(gbMenuTriggered == TRUE))
    {
        Get_NIT_Info_LoaderV5();
        return TRUE; //for loaderV5 temporarily
    }else
    {
        return FALSE;
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_DecoderDownloadControl
///  Download process for Decoder Control
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/12/10
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_MenuDownloadControl(void) //slime
{
    gbMenuTriggered = TRUE;
    MApp_DecoderMsg_DecControl_InitParser();
    return MApp_DecoderMsg_DecoderDownloadControl();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///  _MApp_DecControl_Recovery
///  Processing of recovery for Decoder Control
///
///  @param [in]    pu8Data     Pointer to the message data
///  @param [in]    U16Length  Length of the data
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL _MApp_DecControl_Recovery(const UINT8 *pu8Data, UINT16 u16Length)
{
    BOOL bResult = FALSE;
    UINT8 u8Type    = gstDecCtrl.stRecoverData.u8Recovery_Type;

    if (u8Type == E_CCA_RECOVERY_TYPE_RESET_IRD)
    {
        NGLOG_DEBUG("[DecCtrl] Reset the decoder/system\n");
        // Reset the decoder/system
        MApp_DecoderMsg_ResetDecoder();
    }
    else if (u8Type == E_CCA_RECOVERY_TYPE_RESET_CHANNEL_ORDER)
    {
        // Reset the channel order
        gstDecCtrl.stRecoverData.u16BouquetID = ((UINT16)pu8Data[0] << 8) | ((UINT16)pu8Data[1]);
        NGLOG_DEBUG("[DecCtrl] BouquetID=0x%04X\n", gstDecCtrl.stRecoverData.u16BouquetID);

        if (MApp_DecoderMsg_ResetChannelOrder(gstDecCtrl.stRecoverData.u16BouquetID) == FALSE)
        {
            return FALSE;
        }
    }
    else if (u8Type == E_CCA_RECOVERY_TYPE_RESCAN_RESET)
    {
        NGLOG_DEBUG("[DecCtrl] Re-Scan Service Information and Reset Channel Order\n");
        // Re-Scan Service Information and Reset Channel Order
        MApp_DecoderMsg_RescanSiResetChannelOrder();
    }
    else if (u8Type == E_CCA_RECOVERY_TYPE_ERASE_SERVICE_LISTS)
    {
        NGLOG_DEBUG("[DecCtrl] RErase service lists RESET Re-Tune from Home Tune Partition Data, Re-Scan Service Information and Reset Channel Order\n");
        // Erase service lists RESET Re-Tune from Home Tune Partition Data, Re-Scan Service Information
        // and Reset Channel Order
        MApp_DecoderMsg_EraseServiceLists();
    }
    else if (u8Type == E_CCA_RECOVERY_TYPE_RECONNECT)
    {
        // Reconnect to current service
        NGLOG_DEBUG("[DecCtrl] Reconnect to current service\n");
        bResult = MApp_DecoderMsg_Recovery_Reconnect();

        if (bResult == FALSE)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to reconnect to the curent service\n");
            return FALSE;
        }
    }
    else if (u8Type == E_CCA_RECOVERY_TYPE_FORCE_TUNE)
    {
        // Force tune to specific service
        NGLOG_DEBUG("[DecCtrl] Force tune to specific service\n");
        gstDecCtrl.stRecoverData.stForceTune.u16OriginalNetworkId = ((UINT16)pu8Data[0] << 8) |
                ((UINT16)pu8Data[1]);
        gstDecCtrl.stRecoverData.stForceTune.u16TransportStreamId = ((UINT16)pu8Data[2] << 8) |
                ((UINT16)pu8Data[3]);
        gstDecCtrl.stRecoverData.stForceTune.u16ServiceId         = ((UINT16)pu8Data[4] << 8) |
                ((UINT16)pu8Data[5]);

        NGLOG_DEBUG("[DecCtrl] usNetworkID=%d\n", gstDecCtrl.stRecoverData.stForceTune.u16OriginalNetworkId);
        NGLOG_DEBUG("[DecCtrl] usTransportID=%d\n", gstDecCtrl.stRecoverData.stForceTune.u16TransportStreamId);
        NGLOG_DEBUG("[DecCtrl] usServiceID=%d\n", gstDecCtrl.stRecoverData.stForceTune.u16ServiceId);

        bResult = MApp_DecoderMsg_Recovery_ForceTune(gstDecCtrl.stRecoverData.stForceTune.u16OriginalNetworkId,
                  gstDecCtrl.stRecoverData.stForceTune.u16TransportStreamId,
                  gstDecCtrl.stRecoverData.stForceTune.u16ServiceId);
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("[DecCtrl] Failed to force tune\n");
            return FALSE;
        }

    }
    else if (u8Type == E_CCA_RECOVERY_TYPE_FORCE_SET_INSTALLER_PIN_CODE)
    {

		AppCommand appCmd;
		 
        // Force set installer PIN code
        NGLOG_DEBUG("[DecCtrl] Force set installer PIN code\n");
        gstDecCtrl.stRecoverData.u16InstallerPINCode = ((UINT16)pu8Data[0] << 8) | ((UINT16)pu8Data[1]);

        // Reset the pin code stored on the flash
        //MApp_SetPWD(gstDecCtrl.stRecoverData.u16InstallerPINCode);
        // Protocol: Need to change the Installer PIN to '0000'
        // Reset the pin code stored on the flash
        MApp_SetInstallerlPinCode(0x0000);

				// UI control
		appCmd.eCmd 	   = E_APP_CMD_IRD_CCA;
		appCmd.eFlg 	   = E_APP_CMDFLG_NONE; //E_APP_CMDFLG_SYNC;//E_APP_CMDFLG_NONE;
		appCmd.eSrcAppType = E_APP_TYPE_ANY;
		appCmd.eDstAppType = E_APP_TYPE_MENU;
		appCmd.u8Data[0] = E_APP_CMD_CCAIRD_PIN_CHANGE;
		appCmd.u8Data[1] = E_CCA_CONTROL_MESSAGE_PIN_CODE_INSTALLER;
        appCmd.u8Data[2] = pu8Data[0];  // PIN (H)
        appCmd.u8Data[3] = pu8Data[1];  // PIN (L)
		appMain_RecvNotify(&appCmd);



        NGLOG_DEBUG("[DecCtrl] InstallerPinCode = 0x%x\n", gstDecCtrl.stRecoverData.u16InstallerPINCode);
    }

    return TRUE;
}

#if 0
///////////////////////////////////////////////////////////////////////////////
///  _MApp_DecoderMsg_DecControl_ReadSMC
///  Read Smartcard messages including user data and region
///
///  @param [in]
///
///  @return BOOLEAN TRUE - succeed, FALSE - fail.
///
///  @author MStarSemi @date 2013/5/17
///////////////////////////////////////////////////////////////////////////////
static BOOL _MApp_DecoderMsg_DecControl_ReadSMC(void)
{
    int i;
    BOOL bResult = FALSE;
    msg_smartcard_query_data_st* pstQueryUserData = NULL;
    msg_region_query_st* pstQueryRegion           = NULL;
    UINT8  SmartCard_ID = 0; // FIXME, more than 1 Smartcard

    if (SmartCard_Info[SmartCard_ID].Sector_Num > 0)
    {
        // Smartcard messages - Get CAM user data
        pstQueryUserData = (msg_smartcard_query_data_st *)IRD_MALLOC(sizeof(msg_smartcard_query_data_st));
        if (pstQueryUserData == NULL)
        {
            NGLOG_DEBUG("fail to allocate memory\n");
            return FALSE;
        }
        memset(pstQueryUserData, 0x0, sizeof(msg_smartcard_query_data_st));

        pstQueryUserData->wResourceId = Current_SMC_Resource_Id; // From CA task: wResourceId
        bResult = CA_Task_Query(SMC_MSG_QUERY, MSG_SMARTCARD_USER_DATA_QUERY, (void *)pstQueryUserData, sizeof(msg_smartcard_query_data_st));
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("Fail to query User Data SMC[%ld]\n", pstQueryUserData->wResourceId);
            IRD_FREE(pstQueryUserData);
            return FALSE;
        }
        else
        {
            NGLOG_DEBUG("Query User Data SMC[%ld]\n", pstQueryUserData->wResourceId);
        }

        // Smartcard messages - Get Region
        pstQueryRegion = (msg_region_query_st *)IRD_MALLOC(sizeof(msg_region_query_st));
        if (pstQueryRegion == NULL)
        {
            NGLOG_DEBUG("Fail to allocate memory\n");
            return FALSE;
        }

        memset(pstQueryRegion, 0x0, sizeof(msg_region_query_st));

        // Query active sectors
        pstQueryRegion->wResourceId = Current_SMC_Resource_Id; // From CA task: wResourceId

        pstQueryRegion->bSector = 0; // By default, use 0
        for (i = 0; i < SmartCard_Info[SmartCard_ID].Sector_Num; i++)
        {
            if  (SmartCard_Info[SmartCard_ID].astSector[i].fActive == IA_TRUE)  //FIXME
            {
                pstQueryRegion->bSector = SmartCard_Info[SmartCard_ID].astSector[i].bSector; // FIXME: from CA task, FIRST ACTIVE sector number??
                bResult = CA_Task_Query(SMC_MSG_QUERY, MSG_REGION_QUERY, (void *)pstQueryRegion, sizeof(msg_region_query_st));
                if (bResult == FALSE)
                {
                    NGLOG_DEBUG("Fail to query Region data, SMC[%ld]\n", pstQueryRegion->wResourceId);
                    IRD_FREE(pstQueryRegion);
                    return FALSE;
                }
                else
                {
                    NGLOG_DEBUG("Query Region Data SMC[%ld] Sector[%d]\n", pstQueryUserData->wResourceId, pstQueryRegion->bSector);
                }
            }
            else
            {
                NGLOG_DEBUG("Query Region Data SMC[%ld] Sector[%d] fActive[%d]\n", SmartCard_ID,
                                       SmartCard_Info[SmartCard_ID].astSector[i].bSector,
                                       SmartCard_Info[SmartCard_ID].astSector[i].fActive);
            }
        }

    }
    else
    {
        NGLOG_DEBUG("No active SmartCard. Something wrong\n");
        return FALSE;
    }

    return bResult;
}
#endif
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_DecControl_Parser
///  Parser of Destination Decoder Control in IRD message
///
///  @param [in]    pu8Data     Pointer to the message data
///  @param [in]    U16Length  Length of the data
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_DecControl_Parser(const UINT8 *pu8Data, UINT16 u16Length)
{
    BOOL bHandled = FALSE;
    
    // Initialize the parser the first time it's called
    if (bDecCtrlInitDone == FALSE)
    {
        bDecCtrlInitDone = TRUE;
        MApp_DecoderMsg_DecControl_InitParser();
    }

    // Zero out the structure
    memset(&gstDecCtrl, 0x0, sizeof(stIrdMsgDecControlPayload));

    gstDecCtrl.u8DecControl = (pu8Data[0] >> 4) & 0x0f;   // 4 bits

    NGLOG_DEBUG("[DecCtrl]Control = %d\n", gstDecCtrl.u8DecControl);

    if (gstDecCtrl.u8DecControl >= E_CCA_CONTROL_MESSAGE_TYPE_SPARE)
    {
        NGLOG_DEBUG("[DecCtrl] unsupported Control message types!\n");
        return FALSE;
    }

    if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_DOWNLOAD) ///< Download control
    {
        // FIXME: deal with DownloadAllowed
        gstDecCtrl.stDownload.bDownloadAllowed       = (BOOL)((pu8Data[1] >> 7) & 0x01);     // 1 bit

        gstDecCtrl.stDownload.bForcedDownload        = (BOOL)((pu8Data[1] >> 6) & 0x01);     // 1 bit
        gstDecCtrl.stDownload.bProfdecForcedDownload = (BOOL)((pu8Data[1] >> 5) & 0x01);     // 1 bit
        
        gbEMMTriggered = TRUE;
        // FIXME: the download request depends on what decoder is now..
        if (gstDecCtrl.stDownload.bForcedDownload == 1)
        {
#if (CA_IRD_CCA == 1)
            gbMenuTriggered = FALSE;
#endif
            MApp_DecoderMsg_DecoderDownloadControl();
        }

        if (gstDecCtrl.stDownload.bProfdecForcedDownload == 1)
        {
#if (CA_IRD_CCA == 1)
            gbMenuTriggered = FALSE;
#endif
            MApp_DecoderMsg_DecoderDownloadControl();
        }

    }
    else if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_CALLBACK)
    {
        gstDecCtrl.pu8Callback = &pu8Data[1];
        //FIXME: Callback data, To be defined

        bHandled = TRUE;
    }
    else if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_MONITOR)
    {
        gstDecCtrl.pu8Monitor = &pu8Data[1];
        //FIXME: Monitor data, To be defined

        bHandled = TRUE;
    }
    else if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_READ_SMARTCARD)
    {
        //TODO:bHandled = _MApp_DecoderMsg_DecControl_ReadSMC();
    }
    else if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_CHANGE_PINCODE)
    {
        gstDecCtrl.stPinCode.u8NVRAMParentalPINIndex = pu8Data[1];
        gstDecCtrl.stPinCode.u16NVRAMParentalPINCode = ((UINT16)pu8Data[2] << 8) | ((UINT16)pu8Data[3]);

        //FIXME
        if (gstDecCtrl.stPinCode.u8NVRAMParentalPINIndex > 0)
        {
            return FALSE;
        }

        // if not the same, reset the pin code stored on the flash
        if(MApp_ChkParentalPinCode(gstDecCtrl.stPinCode.u16NVRAMParentalPINCode) == FALSE)
        {

			// Protocol: Need to change the Parental PIN to '4321'
        	// Reset the pin code stored on the flash
        	MApp_SetParentalPinCode(4321);
            //MApp_SetParentalPinCode(gstDecCtrl.stPinCode.u16NVRAMParentalPINCode);
			AppCommand appCmd;
			        // UI control
			appCmd.eCmd 	   = E_APP_CMD_IRD_CCA;
			appCmd.eFlg 	   = E_APP_CMDFLG_NONE; //E_APP_CMDFLG_SYNC;//E_APP_CMDFLG_NONE;
			appCmd.eSrcAppType = E_APP_TYPE_ANY;
			appCmd.eDstAppType = E_APP_TYPE_MENU;
        	appCmd.u8Data[0] = E_APP_CMD_CCAIRD_PIN_CHANGE;
        	appCmd.u8Data[1] = E_CCA_CONTROL_MESSAGE_PIN_CODE_PARENTAL;
        	appCmd.u8Data[2] = pu8Data[1];  // Pin Index
        	appCmd.u8Data[3] = pu8Data[2];  // Pin code (H)
        	appCmd.u8Data[4] = pu8Data[3];  // Pin code (L)
        	appMain_RecvNotify(&appCmd);

		}

        NGLOG_DEBUG("Parental PIN Code:: PinIndex = %d, PinCode = 0x%x\n", gstDecCtrl.stPinCode.u8NVRAMParentalPINIndex,
                               gstDecCtrl.stPinCode.u16NVRAMParentalPINCode);

        bHandled = TRUE;
    }
    else if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_RECOVERY)
    {
        gstDecCtrl.stRecoverData.u8Recovery_Type = (UINT8)((pu8Data[1] >> 4) & 0x0f);     // 4 bits

        if (gstDecCtrl.stRecoverData.u8Recovery_Type >= E_CCA_RECOVERY_TYPE_SPARE)
        {
            NGLOG_DEBUG("[DecCtrl] unsupported Recovery types!\n");
            return FALSE;
        }

        bHandled = _MApp_DecControl_Recovery(&pu8Data[2], u16Length - 2);
    }
    else if (gstDecCtrl.u8DecControl == E_CCA_CONTROL_MESSAGE_TYPE_USER)
    {
        gstDecCtrl.pu8User = &pu8Data[1];
        //FIXME: user data for Irdeto internal access

        bHandled = TRUE;
    }

    return bHandled;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_DecControl_InitParser
///  Initialize the parser of Destination Decoder Control in IRD message
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_DecControl_InitParser(void)
{
    gstDecCtrl.stDownload.bDownloadAllowed = TRUE; // Allow the download by default

#ifdef CCA_DECMSG_DECCTRL_OTA
    #if(LOADER_V3_2 == 1)
        extern UINT16 MApi_GetHW_CODE(void);
        extern UINT16 MApi_GetMANUF_CODE(void);
        extern UINT16 MApi_GetVARIANT(void);
        extern UINT16 MApi_GetLOAD_SEQ_NR(void);

        gstSystem.u8HWVersion          = MApi_GetHW_CODE();
        gstSystem.u8ManufactureCode    = MApi_GetMANUF_CODE();
        gstSystem.u16Variant           = MApi_GetVARIANT();
        gstSystem.u8LoadSequenceNumber = MApi_GetLOAD_SEQ_NR();
    #else
       
        extern lc_bbcb_st	stBBCBFlashLoaderData;
        extern Variant_info variant_infomation;
        extern lc_uint16    pDownloadSequenceNumber;

        gstSystem.u8HWVersion          = stBBCBFlashLoaderData.hardware_version;
        gstSystem.u8ManufactureCode    = stBBCBFlashLoaderData.manufacturer_id;
        gstSystem.u16Variant           = variant_infomation.pVariant;
        gstSystem.u8LoadSequenceNumber = pDownloadSequenceNumber;
    #endif
    NGLOG_DEBUG("[DecCtrl] u8ManufactureCode      = [0x%x]\n", gstSystem.u8ManufactureCode);
    NGLOG_DEBUG("[DecCtrl] u8HWVersion            = [0x%x]\n", gstSystem.u8HWVersion);
    NGLOG_DEBUG("[DecCtrl] u16Variant             = [0x%x]\n", gstSystem.u16Variant);
    NGLOG_DEBUG("[DecCtrl] u8LoadSequenceNumber   = [0x%x]\n", gstSystem.u8LoadSequenceNumber);
#endif

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_DecControl_TerminateParser
///  Terminate the parser of Destination Decoder Control in IRD message
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/15
///////////////////////////////////////////////////////////////////////////////
#if(LOADER_V3_2 == 1)
BOOL MApp_DecoderMsg_DecControl_TerminateParser(void)
{
    MApp_DecoderMsg_DCS_Stop();
    bDecCtrlInitDone = FALSE;

    return TRUE;
}
#endif
}
#undef _APP_CCA_DECODERMSG_C

