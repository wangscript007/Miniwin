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
/// file    appCCADecoderMsgAttribtuedDisplay.c
/// @brief  Parser and handle of CCA Decoder Destination "Attributed Display"
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define _APP_CCA_DECODERMSG_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>


// api
#include "appMain.h"
#include "appCCADecoderMsg.h"
#include "appCCADecoderMsgTextMsg.h"
#include "appCCADecoderMsgAttribtuedDisplay.h"
#include "CCA_UCMsgProc.h"

#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif

//#define CCA_DECMSG_ATTRDISPLAY_DEBUG

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef CCA_DECMSG_ATTRDISPLAY_DEBUG
#define CCA_DECMSG_ATTRDISPLAY_DBG(fmt, arg...)   {printf("\033[0;35m[ATTR DISPLAY]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define CCA_DECMSG_ATTRDISPLAY_DBG(fmt, arg...)
#endif

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/


/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static stIrdMsgAttributedDisplayPayload gstAttrDisplay;
static UINT16 gu16MailIndex = IRD_INVALID_MAIL_ANNCMNT_INDEX;  // The index of the massage in the Mail buffer

/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/


/********************************************************************************/
/*                            extern Function                                */
/********************************************************************************/


/********************************************************************************/
/*                            Local Function                                */
/********************************************************************************/

///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_StoreMails
///  Store the mail messages
///
///  @param [in]    bFlush  The flag to indicate buffer flushing requested by IRD message
///  @param [in]    pstMsg  The pointer to the message
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static void _MApp_DecoderMsg_AttributedDisplay_PrintCSSN(void)
{
#ifdef  CCA_DECMSG_ATTRDISPLAY_DEBUG
    if (gstAttrDisplay.u16TextLength > 1)  // Not a null string
    {
        printf("\n");
        printf("Fingerprint Text (CSSN) = %s\n", gstAttrDisplay.au8CSSNASCII);
        printf("\n");
    }
    else
    {
        printf("\n");
        printf("Empty Fingerprint Text\n");
        printf("\n");
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_IrdAttributedDisplay_GetCoveragPercentage
///  Convert the Coverage code to the percentage
///
///  @param [in]
///
///  @return percentage
///
///  @author MStarSemi @date 2013/5/22
///////////////////////////////////////////////////////////////////////////////

UINT8 MApp_IrdAttributedDisplay_GetCoveragPercentage(UINT8 u8CoverageCode)
{
    if (gstAttrDisplay.u8MessageType >= E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT)
    {
        if (u8CoverageCode <= 20)
        {
            return 45; //sqrt(20 * 100);
        }
        else if (u8CoverageCode == 63)
        {
            return 100; //sqrt(100 * 00);
        }
        else
        {
            return sqrt((21 + (UINT8)(((u8CoverageCode - 21) * (99 - 21)) / (62 - 21))) * 100);
        }
    }

    return 45;  // Default 20% coverage
}

///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_AttributedDisplay_StoreMsg
///  Store normal and forced text messages
///
///  @param [in] u16Crc
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

static BOOL _MApp_DecoderMsg_AttributedDisplay_StoreMsg(UINT16 u16Crc)
{
    stIrdTextMsg stMsg;

    memset(&stMsg, 0x0, sizeof(stIrdTextMsg));

    // FIXME: This needs futhur verification with decoder test.

    // From the spec, it seems the duration means the display duration.
    // Then it has no need to save duration with the message.
#if 1
    if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_NORMAL)
    {
        stMsg.u8Class = E_CCA_TEXT_MSG_CLASS_NORMAL;

    }
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT)
    {
        stMsg.u8Class = E_CCA_TEXT_MSG_CLASS_FORCE;
    }
#else
    if (gstAttrDisplay.u16Duration == 0x0)  // 0 = INDEFINITELY
    {
        if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_NORMAL)
        {
            stMsg.u8Class = E_CCA_TEXT_MSG_CLASS_NORMAL;

        }
        else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT)
        {
            stMsg.u8Class = E_CCA_TEXT_MSG_CLASS_FORCE;
        }
        else
        {
            CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Wrong message type = %d!\n", gstAttrDisplay.u8MessageType);
            return FALSE;
        }
    }
    else
    {
        UINT32 u32DelTime = 0;
        stMsg.u8Class = E_CCA_TEXT_MSG_CLASS_TIMED;

        // Convert the duration to date for deletion
        u32DelTime  = MsOS_GetSystemTime(); // Get current system time
        u32DelTime += gstAttrDisplay.u16Duration;

        MApp_Convert2LocalDateTime(u32DelTime, &stMsg.stDelTime);
    }
#endif

    stMsg.u16Length = gstAttrDisplay.u16TextLength;
    stMsg.u16Crc    = u16Crc;
    memcpy(stMsg.au8MsgByte, gstAttrDisplay.pu8TextByte, gstAttrDisplay.u16TextLength);

    // Store the message
#if 1
    // Save to Mail Buffer because the CRC will recorded and checked for the items at this buffer.
    gu16MailIndex = MApp_DecoderMsg_ProcessLongTextMsg(E_CCA_IRD_DEST_BUFFER_MAIL, FALSE, &stMsg);
#else
    if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_NORMAL)
    {

    }
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT)
    {
        gu16MailIndex = MApp_DecoderMsg_ProcessLongTextMsg(E_CCA_IRD_DEST_BUFFER_ANNCMNT, FALSE, &stMsg);
    }
#endif
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_AttributedDisplay_GetDisplayMethod
///  Parser of Destination Attributed Display Message in IRD message
///
///  @param [in]
///
///  @return Pointer of the TLV data if u8FPTag=0, NULL for others.
///
///  @author MStarSemi @date 2013/5/29
///////////////////////////////////////////////////////////////////////////////

stIrdAttributedDisplayMethod* MApp_DecoderMsg_AttributedDisplay_GetDisplayMethod(void)
{
    return (&gstAttrDisplay.stDisplayMethod);
}


///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_AttributedDisplay_GetFingerprintOptionTLV
///
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/7/22
///////////////////////////////////////////////////////////////////////////////
#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
BOOL _MApp_DecoderMsg_AttributedDisplay_GetFingerprintOptionTLV(const UINT8* pu8Data, stIrdFPOptionsTLV* pstFpoTlv)
{
    // TLV processing
    pstFpoTlv->u8Tag       = pu8Data[0];
    pstFpoTlv->u8Length    = pu8Data[1];


    //FIXME: where to store the options or which module should use them
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] TLV Tag = %d\n", pstFpoTlv->u8Tag);

    if (pstFpoTlv->u8Tag == TAG_ENHANCED_OVERT_FINGERPRINTING_OPTIONS)
    {
        if (pstFpoTlv->u8Length == 0)
        {
            CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Wrong TLV length = %d\n", pstFpoTlv->u8Length);
            return FALSE;
        }
        else
        {
            CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] TLV length = %d\n", pstFpoTlv->u8Length);
        }
        pstFpoTlv->stEofVar.u8LocationXFactor             = pu8Data[2];
        pstFpoTlv->stEofVar.u8LocationYFactor             = pu8Data[3];
        pstFpoTlv->stEofVar.u8BgTransparencyAlphaFactor   = pu8Data[4];

        // 24 bits
        pstFpoTlv->stEofVar.u32BgColourRGB                = ((UINT32)pu8Data[5] << 16) |
                                                            ((UINT32)pu8Data[6] << 8)  |
                                                            ((UINT32)pu8Data[7]);
        pstFpoTlv->stEofVar.u8FontTransparencyAlphaFactor = pu8Data[8];

        // 24 bits
        pstFpoTlv->stEofVar.u32FontColourRGB              = ((UINT32)pu8Data[9] << 16) |
                                                            ((UINT32)pu8Data[10] << 8)  |
                                                            ((UINT32)pu8Data[11]);
        pstFpoTlv->stEofVar.u8FontTypeIndex               = pu8Data[12];

        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u8LocationXFactor = 0x%2x\n", pstFpoTlv->stEofVar.u8LocationXFactor);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u8LocationYFactor = 0x%2x\n", pstFpoTlv->stEofVar.u8LocationYFactor);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u8BgTransparencyAlphaFactor = 0x%2x\n", pstFpoTlv->stEofVar.u8BgTransparencyAlphaFactor);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u32BgColourRGB = 0x%0.6x\n", pstFpoTlv->stEofVar.u32BgColourRGB);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u8FontTransparencyAlphaFactor = 0x%2x\n", pstFpoTlv->stEofVar.u8FontTransparencyAlphaFactor);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u32FontColourRGB = 0x%0.6x\n", pstFpoTlv->stEofVar.u32FontColourRGB);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] u8FontTypeIndex = 0x%2x\n", pstFpoTlv->stEofVar.u8FontTypeIndex);
    }
    else
    {
        // Store the data
        memcpy(pstFpoTlv->au8Variable, &pu8Data[2], pstFpoTlv->u8Length);
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Unknow options = %d\n", pstFpoTlv->u8Tag);
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_AttributedDisplay_GetFPTLV
///  Get the TLV of the Fingerprinting Option
///
///  @param [in]    u8FPTag
///
///  @return Pointer of the TLV data if u8FPTag=0, NULL for others.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

stIrdFPOptionsTLV* MApp_DecoderMsg_AttributedDisplay_GetFPTLV(UINT8 u8FPTag)
{
    if (u8FPTag == TAG_ENHANCED_OVERT_FINGERPRINTING_OPTIONS)
    {
        return (&gstAttrDisplay.stFPTLV);
    }
    else
    {
        return NULL;
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_AttributedDisplay_Parser
///  Parser of Destination Attributed Display Message in IRD message
///
///  @param [in]      U16Length
///  @param [in]      pu8Data
///  @param [in]      u16Crc      CRC value obtained at  CA IRD message
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_AttributedDisplay_Parser(const UINT8 *pu8Data, UINT16 u16Length, UINT16 u16Crc)
{
#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
    stIrdFPOptionsTLV* pstTLV = &gstAttrDisplay.stFPTLV;
#endif
    EN_CCA_CRC_STATUS eCrcSts  = E_CCA_CRC_NEW;


    if (u16Length < IRDMSG_ATTR_HEADER_LENGTH)
    {
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] The total length is smaller than expected!\n");
        return FALSE;
    }

    // Zero out the structure
    memset((void *)&gstAttrDisplay, 0x0, sizeof(stIrdMsgAttributedDisplayPayload));

    // Get the message type
    gstAttrDisplay.u8MessageType = (UINT8)pu8Data[0];
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Message type = %d\n", gstAttrDisplay.u8MessageType);

    if (gstAttrDisplay.u8MessageType > E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINTING_OPTIONS)
    {
         CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Unknown message type (Not supported)!\n");
         return FALSE;
    }

    // Shall check support of unicode character sets
    if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_NORMAL)
    {
        eCrcSts = MApp_DecoderMsg_CheckCrcList(E_CCA_TEXT_MSG_TYPE_MAILBOX, E_CCA_CRC_ADM_NORMAL, u16Crc);
    }
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT)
    {
        eCrcSts = MApp_DecoderMsg_CheckCrcList(E_CCA_TEXT_MSG_TYPE_MAILBOX, E_CCA_CRC_ADM_FORCED, u16Crc);
    }

    if (eCrcSts != E_CCA_CRC_NEW)
    {
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Duplicated message received!\n");
        return TRUE; // Terminate the parsing of the current decoder message
    }

    // Get the duration
    gstAttrDisplay.u16Duration = ((UINT16)pu8Data[1] << 8) | ((UINT16)pu8Data[2]);
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Message duration = %d\n", gstAttrDisplay.u16Duration);

    // Get the display method - 0, normal; 1, banner
    gstAttrDisplay.stDisplayMethod.bFlashing      = (pu8Data[3] >> 0) & 0x1;
    gstAttrDisplay.stDisplayMethod.bBanner        = (pu8Data[3] >> 1) & 0x1;
    gstAttrDisplay.stDisplayMethod.u8CoverageCode = (pu8Data[3] >> 2) & 0x3f;
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Flashing = %d\n", gstAttrDisplay.stDisplayMethod.bFlashing);
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Banner = %d\n", gstAttrDisplay.stDisplayMethod.bBanner);
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Coverage = %d\n", gstAttrDisplay.stDisplayMethod.u8CoverageCode);

    gstAttrDisplay.bFingerprintType = (pu8Data[4] >> 7) & 0x1;
    gstAttrDisplay.u16TextLength    = (((UINT16)pu8Data[4] & 0xf) << 8) | ((UINT16)pu8Data[5]);
    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] FP Type = %d\n", gstAttrDisplay.bFingerprintType);

    //  The first (MSB) three bits shall be = 3b'000. Max length = 511
    if (gstAttrDisplay.u16TextLength > IRDMSG_ATTR_MAX_TEXT_LENGTH)
    {
         CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Syntax error on Text Length!\n");
         return FALSE;
    }
    else if (gstAttrDisplay.u16TextLength > (u16Length - IRDMSG_ATTR_HEADER_LENGTH))
    {
         CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Value of Text Length = %d is smaller than expected!\n", gstAttrDisplay.u16TextLength);
         return FALSE;
    }
    else
    {
         CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Text Length = %d \n", gstAttrDisplay.u16TextLength);
    }

    // Shall check support of unicode character sets
    // Proess the data bytes
    if ((gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_NORMAL) ||
        (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT))
    {
        // Store messages
        // Point to the starting address of Text Byte field
        gstAttrDisplay.pu8TextByte = &pu8Data[6];
        _MApp_DecoderMsg_AttributedDisplay_StoreMsg(u16Crc);
    }
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINT)
    {
        // Follow the specified display method,
        // <3.14 Text_Byte shall contain Hashed Smartcard Serial Number (in ASCII) inserted by the SoftCell
        // >=3.14 hashed secure chipset serial number (CSSN), or hashed full unique address of smartcard (5
        // FIXME

        if ((u16Length - 6) == gstAttrDisplay.u16TextLength)
        {
            memcpy(gstAttrDisplay.au8CSSNASCII, &pu8Data[6], gstAttrDisplay.u16TextLength);
            gstAttrDisplay.au8CSSNASCII[gstAttrDisplay.u16TextLength] = '\n'; // End of string
            gstAttrDisplay.u16TextLength++;
            // Print the received number
            _MApp_DecoderMsg_AttributedDisplay_PrintCSSN();
        }

    }
#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINTING_OPTIONS)
    {
        pstTLV = &gstAttrDisplay.stFPTLV;

        if (gstAttrDisplay.u16TextLength > 0)
        {
            _MApp_DecoderMsg_AttributedDisplay_GetFingerprintOptionTLV(&pu8Data[6], pstTLV);
        }
    }
#endif
    else
    {
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Unknown Message Type\n");
    }

    CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Start UI control\n");

    if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FORCED_TEXT)
    {
        // UI Control: follow the specified display method - Consider UI control task for ths timing event.
        // FIXME
        AppCommand appCmd;

        memset(&appCmd.u8Data[0], '\0', APP_CMD_DATA_LEN);

        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display]Show Content\n");
        appCmd.eCmd        = E_APP_CMD_IRD_CCA;
        appCmd.eFlg        = E_APP_CMDFLG_NONE;//E_APP_CMDFLG_SYNC;//E_APP_CMDFLG_NONE;
        appCmd.eSrcAppType = E_APP_TYPE_ANY;
        appCmd.eDstAppType = E_APP_TYPE_MENU;
        appCmd.u8Data[0]   = E_APP_CMD_CCAIRD_TMS;
        appCmd.u8Data[1]   = E_CCA_TEXT_MSG_CLASS_FORCE;
        appCmd.u8Data[2]   = (gu16MailIndex >> 8) & 0xff;                           // Index (H)
        appCmd.u8Data[3]   = (gu16MailIndex & 0xff);                                // Index (L)
        appCmd.u8Data[4]   = (gstAttrDisplay.u16Duration >> 8) & 0xff;              // Duration(H)
        appCmd.u8Data[5]   = (gstAttrDisplay.u16Duration & 0xff);                   // Duration(L)
        appCmd.u8Data[6]   = pu8Data[3];                                            // Coverage code, banner, flash
        appMain_RecvNotify(&appCmd);
    }
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINT)
    {
        // Follow the specified display method
        // if not support COVERT, then turn it into OVERT fingerprint display
#if (CA_CCA_FINGERPRINT_COVERT == 0)
        if (gstAttrDisplay.bFingerprintType == FINGERPRINT_TYPE_COVERT)
        {
            gstAttrDisplay.bFingerprintType = FINGERPRINT_TYPE_OVERT;
        }
#endif
        if (gstAttrDisplay.bFingerprintType == FINGERPRINT_TYPE_OVERT)
        {
            AppCommand appCmd;

            // Overt FP
            // Client shall ignore the bit, and display fingerprint always as Banner.
            gstAttrDisplay.stDisplayMethod.bBanner = 0;

            memset(&appCmd.u8Data[0], '\0', APP_CMD_DATA_LEN);

            CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Fingerprint OVERT\n");
            appCmd.eCmd        = E_APP_CMD_IRD_CCA;
            appCmd.eFlg        = E_APP_CMDFLG_NONE;//E_APP_CMDFLG_SYNC;//E_APP_CMDFLG_NONE;
            appCmd.eSrcAppType = E_APP_TYPE_ANY;
            appCmd.eDstAppType = E_APP_TYPE_MENU;
            appCmd.u8Data[0]   = E_APP_CMD_CCAIRD_FINGERPRINT;
            appCmd.u8Data[1]   = (gstAttrDisplay.u16Duration >> 8) & 0xff;           // Duration(H)
            appCmd.u8Data[2]   = (gstAttrDisplay.u16Duration & 0xff);                // Duration(L)
            appCmd.u8Data[3]   = (pu8Data[3]);                                       // Coverage code, banner, flash
            appCmd.u8Data[4]   = ((gstAttrDisplay.u16TextLength + 1) >> 8) & 0x0f;   // Length(H)
            appCmd.u8Data[5]   = (gstAttrDisplay.u16TextLength + 1) & 0xff;          // Length(L)

            // FIXME: how to send the data along with the command
            if (gstAttrDisplay.u16TextLength < (APP_CMD_DATA_LEN - 6 - 1))
            {
                memcpy(&appCmd.u8Data[6], gstAttrDisplay.au8CSSNASCII, gstAttrDisplay.u16TextLength); // Including '\n'
                appCmd.u8Data[gstAttrDisplay.u16TextLength + 6 - 1] = 0xD; // For ZUI OSD display, when LF detected, replace it with CR
            }
            else
            {
                CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display]Hashed key too longer\n");
                appCmd.u8Data[4] = ((APP_CMD_DATA_LEN - 2) >> 8) & 0xff;   // FIXME, Length(H)
                appCmd.u8Data[5] = (APP_CMD_DATA_LEN - 2) & 0xff;          // FIXME, Lenght(L)
                memcpy(&appCmd.u8Data[6], gstAttrDisplay.au8CSSNASCII, (APP_CMD_DATA_LEN - 6));
                appCmd.u8Data[APP_CMD_DATA_LEN - 1] = 0xD; // For ZUI OSD display, when LF detected, replace it with CR
            }
            appMain_RecvNotify(&appCmd);
        }
        else  //FINGERPRINT_TYPE_COVERT
        {
            // Overt FP
            // Client shall ignore the bit, and display fingerprint always as Banner.
            gstAttrDisplay.stDisplayMethod.bBanner = 0;
            CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Fingerprint COVERT\n");
            // FIXME: there's no spec. Pending
        }
    }
#if (CA_CCA_FINGERPRINT_ENHANCED_OVERT == 1)
    else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINTING_OPTIONS)
    {
        // Follow the specified display method
#if (CA_CCA_FINGERPRINT_COVERT == 0)
        if (gstAttrDisplay.bFingerprintType == FINGERPRINT_TYPE_COVERT)
        {
            gstAttrDisplay.bFingerprintType = FINGERPRINT_TYPE_OVERT;
        }
#endif
        if (gstAttrDisplay.bFingerprintType == FINGERPRINT_TYPE_OVERT)
        {
            // Overt FP
            // FIXME
            CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Fingerprinting Options OVERT\n");

            // Client shall ignore the bit, and display fingerprint always as Banner.
            gstAttrDisplay.stDisplayMethod.bBanner = 0;

            if (pstTLV->u8Tag == TAG_ENHANCED_OVERT_FINGERPRINTING_OPTIONS)
            {
                // FIXME: how to send the data along with the command
                AppCommand appCmd;

                memset(&appCmd.u8Data[0], '\0', APP_CMD_DATA_LEN);

                appCmd.eCmd        = E_APP_CMD_IRD_CCA;
                appCmd.eFlg        = E_APP_CMDFLG_NONE;//E_APP_CMDFLG_SYNC;//E_APP_CMDFLG_NONE;
                appCmd.eSrcAppType = E_APP_TYPE_ANY;
                appCmd.eDstAppType = E_APP_TYPE_MENU;
                appCmd.u8Data[0]   = E_APP_CMD_CCAIRD_FINGERPRINTING_OPTION;
                appCmd.u8Data[1]   = (gstAttrDisplay.u16Duration >> 8) & 0xff;           // Duration(H)
                appCmd.u8Data[2]   = (gstAttrDisplay.u16Duration & 0xff);                // Duration(L)
                appCmd.u8Data[3]   = (pu8Data[3]);                                       // Coverage code, banner, flash
                appCmd.u8Data[4]   = pstTLV->u8Tag;

                appMain_RecvNotify(&appCmd);
            }
        }
        else  //FINGERPRINT_TYPE_COVERT
        {
            // Covert FP
            // FIXME: there's no spec. Pending
        }
    }//else if (gstAttrDisplay.u8MessageType == E_CCA_ATTR_MESSAGE_TYPE_FRIGERPRINTING_OPTIONS)
#endif
    else
    {
        CCA_DECMSG_ATTRDISPLAY_DBG("[Attributed Display] Unknown Message Type\n");
    }
    return TRUE;
}

#undef _APP_CCA_DECODERMSG_C

