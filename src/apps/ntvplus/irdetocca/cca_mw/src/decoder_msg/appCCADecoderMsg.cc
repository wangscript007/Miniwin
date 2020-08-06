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
/// file    appCCADecoderMsg.c
/// @brief  CCA Decoder Message parser and dispatcher
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////



#define _APP_CCA_DECODERMSG_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>
#include <ngl_timer.h>
#include <string.h>
#include <appCCADecoderMsg.h>
#include <appCCADecoderMsgTextMsg.h>
#include <appCCADecoderMsgAttribtuedDisplay.h>
#include <appCCADecoderMsgCIPlusCam.h>
#include <appCCADecoderMsgDecControl.h>
#include <appCCADecoderMsgProfDec.h>
#include <CCA_UCMsgProc.h>
//#include "CCA_UCSaveData.h"


#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif
NGL_MODULE(IRDCCADECMSG);
namespace cca{

#define CCA_MAIL_FLASH_SIZE	0x008000
#define CCA_ANNC_FLASH_SIZE	0x008000
/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/

/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
static BOOL gbParserInitDone   = FALSE;
static BOOL gbMailInfoInitDone = FALSE;

//stCrcEntry gstCrcList[IRD_MESSAGE_RECORDED_CRC_ENTIIES];
stCrcEntry gstCrcList_Mail[IRD_MAX_NUM_MAILS];
stCrcEntry gstCrcList_Annm[IRD_MAX_NUM_ANNCMNTS];


// Storage for mail and Anouncement messages
stIrdSysSetting* gpstIrdSysSetting         = NULL;

static UINT16 gu16FreeSpaceMailArchive     = CCA_MAIL_FLASH_SIZE;
static UINT16 gu16FreeSpaceAnncmntArchive  = CCA_ANNC_FLASH_SIZE;

static stIrdTextMsg gstMailBuf;                     ///< Buffer with lastly received data
static stIrdTextMsg gstAnncmntBuf;                  ///< Buffer with lastly received data
static UINT16 gu16TotalNumMails            = 0;     ///< Buffer size
static UINT16 gu16TotalNumAnncmnts         = 0;     ///< Buffer size
static UINT16 gu16MailCount                = 0;     ///< Number of Mails in the Mail Buffer
static UINT16 gu16AnncmntCount             = 0;     ///< Number of Announcements in the Announcement Buffer

// A rule of thumb is: for broadcasting environment 2 long messages in the FIFO should be sufficient.
static stIrdLongTextMsg* gpstLongMsgBuf[E_CCA_LONGMSG_BUFSIZE] = {0};  ///< 2 Temp buffer of long messages
static stIrdLongTextMsg** gpLongMsgs[E_CCA_LONGMSG_BUFSIZE]    = {0};  ///< 2 arrays of pointers to temp buffer of long messages
static UINT16 gu16NumLongMsgs[E_CCA_LONGMSG_BUFSIZE]           = {0};  ///< Number of Long messages in the Buffer. Value 0 means no message

static stIrdTLongTextMsgSegment gstCurrentLongMsgSeg;
static UINT16 gu16TextMsgId = 0;

/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/
static const UINT16 u16TableCrc16[256] =
{
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

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
#if 0
static void _MApp_DecoderMsg_PrintRawTextMsg(const void* pstMsg)
{
#ifdef  CCA_DECMSG_DEBUG

    int i, j;
    UINT16 u16Width  = 16;
    UINT16 u16Height = sizeof(stIrdTextMsg) >> 4;
    const UINT8* pU8Text;

    pU8Text = (const UINT8*) pstMsg;

    for (i = 0; i < u16Height; i++)
    {
        for (j = 0; j < u16Width; j++)
        {
            printf("%.2x ", pU8Text[i * u16Width + j]);
        }
        printf("\n");
    }

    for (j = u16Height * u16Width; j < sizeof(stIrdTextMsg); j++)
    {
        printf("%.2x ", pU8Text[j]);
    }
    printf("\n");
#endif
}
#endif

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
#ifdef  CCA_DECMSG_DEBUG
static void _MApp_DecoderMsg_PrintTextMsg(const stIrdTextMsg* pstMsg)
{
    int i;

    printf("\n");
    printf("Text msg length = %d\n", pstMsg->u16Length);
    printf("Text msg class = %d\n ", pstMsg->u8Class);

    for (i = 0; i <  pstMsg->u16Length; i++)
    {
       printf("%c", pstMsg->au8MsgByte[i]);
    }
    printf("\n");
}
#endif

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
static void _MApp_DecoderMsg_ResetToInitialSetting(EN_CCA_IRD_DEST_BUFFER_ID eBufId)
{
    if (eBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
    {
        memset(gpstIrdSysSetting->stMails, 0x0, sizeof(stIrdTextArchiveInfo) * gu16TotalNumMails);
        gu16FreeSpaceMailArchive  = CCA_MAIL_FLASH_SIZE;
        gu16MailCount             = 0;

    }
    else if (eBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
    {
        memset(gpstIrdSysSetting->stAnncmnts, 0x0, sizeof(stIrdTextArchiveInfo) * gu16TotalNumAnncmnts);
        gu16FreeSpaceAnncmntArchive = CCA_ANNC_FLASH_SIZE;
        gu16AnncmntCount            = 0;
    }
    else
    {
        NGLOG_DEBUG("Wrong buffer ID %d\n", eBufId);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// private _MApp_DecoderMsg_GetSysSetting
/// Obtain the Sys Setting from default setting or by loading it from the flash
///
///  @param [in]  void
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////
static void _MApp_DecoderMsg_GetSysSetting(void)
{
    int i = 0;

    //NGLOG_DEBUG("\n%d, %d\n", gu16TotalNumMails, gu16TotalNumAnncmnts);
    //App_LoadIrdSysSetting();
   // App_GetIrdSysSetting(gpstIrdSysSetting);  need  fix MX

    // Find the first empty buffer for mails
    for (i = 0; i < gu16TotalNumMails; i++)
    {
        if (gpstIrdSysSetting->stMails[i].u16Length == 0)
        {
             break;
        }
        else
        {
             //NGLOG_DEBUG("\n Length = %d\n", gpstIrdSysSetting->stMails[i].u16Length);
             //NGLOG_DEBUG("\ncrc %d\n", gpstIrdSysSetting->stMails[i].u16Crc);
             gstCrcList_Mail[i].eType  = E_CCA_CRC_NORMAL_MAIL; // FIXME
             gstCrcList_Mail[i].u16Crc = gpstIrdSysSetting->stMails[i].u16Crc;

             if (gpstIrdSysSetting->stMails[i].u16Id > gu16TextMsgId)
             {
                 gu16TextMsgId = gpstIrdSysSetting->stMails[i].u16Id;
             }

             gu16FreeSpaceMailArchive = gu16FreeSpaceMailArchive - gpstIrdSysSetting->stMails[i].u16Length;
        }
    }
    gu16MailCount = i;

    // Find the first empty buffer for announcements
    for (i = 0; i < gu16TotalNumAnncmnts; i++)
    {
        //NGLOG_DEBUG("\n%d\n", gpstIrdSysSetting->stAnncmnts[i].u16Length);
        if (gpstIrdSysSetting->stAnncmnts[i].u16Length == 0)
        {
             break;
        }
        else
        {
			gstCrcList_Annm[i].eType  = E_CCA_CRC_NORMAL_MAIL; // FIXME
            gstCrcList_Annm[i].u16Crc = gpstIrdSysSetting->stAnncmnts[i].u16Crc;
			if (gpstIrdSysSetting->stAnncmnts[i].u16Id > gu16TextMsgId)
			{
				gu16TextMsgId = gpstIrdSysSetting->stAnncmnts[i].u16Id;
			}

			gu16FreeSpaceAnncmntArchive  = gu16FreeSpaceAnncmntArchive - gpstIrdSysSetting->stAnncmnts[i].u16Length;
        }
    }
    gu16AnncmntCount = i;
    gu16TextMsgId++;

    NGLOG_DEBUG("# of Mail items = %d, free space = %d\n", gu16MailCount, gu16FreeSpaceMailArchive);
    NGLOG_DEBUG("# of Anncmnt items = %d, free space = %d\n", gu16AnncmntCount, gu16FreeSpaceAnncmntArchive);
}

//////////////////////////////////////////////////////////////////////////////
/// private _MApp_DecoderMsg_SetSysSetting
/// Save the Sys Setting to the flash
///
///  @param [in]  void
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

static void _MApp_DecoderMsg_SetSysSetting(void)
{
    //App_SetIrdSysSetting(gpstIrdSysSetting);

    //App_SaveIrdSysSetting();

    // Write the setting to the spi flash
   // MApp_SaveGenSetting(FALSE);

}

#define CONFIG_FOR_NGL_CCA_MSGPATH  "/home/gx/msgdata.bin"
BOOL UcMsgPs_Initialize(void)
{
	FILE *file=nullptr;	
	file=fopen(CONFIG_FOR_NGL_CCA_MSGPATH,"ab+");
	NGLOG_DEBUG("file:(%p) errno:(%d)",file,errno);
	if(file){
		fclose(file);
    	return UC_ERROR_SUCCESS;
	}
	return UC_ERROR_OS_FAILURE;
}

BOOL UC_Load_DataFromSerialFlash(EN_CCA_IRD_DEST_BUFFER_ID eDestBufId, void* pBuf, UINT32 u32Size, UINT16 u16Offset)
{
    INT8*  pReadBin     = NULL;
    UINT32  u32StartAddr = 0;
    BOOL bResult      = FALSE;

    if (pBuf == NULL)
    {
        NGLOG_DEBUG("[CCA SerialFlash] Buffer point is NULL\n");
        return FALSE;
    }
    else
    {
        if (eDestBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
        {
            u32StartAddr = 0; // FIXME
        }
        else if (eDestBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
        {
            u32StartAddr = CCA_MAIL_FLASH_SIZE; // FIXME
        }

        pReadBin = (INT8 *) pBuf;

        u32StartAddr = u32StartAddr + u16Offset; // FIXME
		FILE* f = fopen(CONFIG_FOR_NGL_CCA_MSGPATH,"rb");
		fseek(f,u32StartAddr,SEEK_SET);
        bResult  = fread(pReadBin, u32Size,1,f);
		fclose(f);
		return TRUE;
    }

}

BOOL UC_Save_DataToSerialFlash(EN_CCA_IRD_DEST_BUFFER_ID eDestBufId, void* pBuf, UINT32 u32Size, UINT16 u16Offset)
{
    INT8*  pWriteBin    = NULL;
    UINT32  u32StartAddr = 0;
    BOOL bResult      = FALSE;

    pWriteBin = (INT8 *) pBuf;

    if (pBuf == NULL)
    {
        NGLOG_DEBUG("[CCA SerialFlash] Buffer point is NULL\n");
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[CCA SerialFlash] pBuf = 0x%x, u32Size = %ld, u16Offset = %d\n", pBuf, u32Size, u16Offset);

        if (eDestBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
        {
            u32StartAddr = 0; // FIXME
        }
        else if (eDestBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
        {
            u32StartAddr = CCA_MAIL_FLASH_SIZE; // FIXME
        }

        u32StartAddr = u32StartAddr + u16Offset; // FIXME
		FILE *f = fopen(CONFIG_FOR_NGL_CCA_MSGPATH,"rb+");
		fseek(f,u32StartAddr,SEEK_SET);
		fwrite(pWriteBin, u32Size, 1,f);
		fclose(f);
		return TRUE;
    }

}

///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_RemoveDataFromBuffer
///  Remove the specified data which are stored on the serial flash
///
///  @param [in]    eBufId
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL _MApp_DecoderMsg_RemoveDataFromBuffer(EN_CCA_IRD_DEST_BUFFER_ID eBufId, UINT16 u16StartIndex, UINT16 u16EndIndex)
{
    int i = 0;
    UINT16  u16Src  = 0;
    UINT16  u16Dest = 0;
    BOOL bResult = FALSE;
    static  stIrdTextArchiveInfo* gpInfo = NULL;
    UINT16  u16Index      = 0;
    UINT16* pu16FreeSpace = 0;
    INT8*  pu8Buf        = NULL;
    UINT16  u16Offset     = 0;
    UINT32  u32Length     = 0;
    UINT16  u16SrcOffset  = 0;

    if (u16StartIndex > u16EndIndex)
    {
        NGLOG_DEBUG("Wrong indices: start = %d, end =%d\n", u16StartIndex, u16EndIndex);
    }

    NGLOG_DEBUG("Free space = %d, %d\n", gu16FreeSpaceMailArchive, gu16FreeSpaceAnncmntArchive);
    if (eBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
    {
         gpInfo        = gpstIrdSysSetting->stMails;
         u16Index      = gu16MailCount;
         pu16FreeSpace = &gu16FreeSpaceMailArchive;
         gu16MailCount = gu16MailCount - (u16EndIndex + 1 - u16StartIndex);
    }
    else
    {
         gpInfo           = gpstIrdSysSetting->stAnncmnts;
         u16Index         = gu16AnncmntCount;
         pu16FreeSpace    = &gu16FreeSpaceAnncmntArchive;
         gu16AnncmntCount = gu16AnncmntCount - (u16EndIndex + 1 - u16StartIndex);
    }

    NGLOG_DEBUG("Overwrite the info of deleted buffers: Start = %d, end = %d\n", u16StartIndex, u16EndIndex);

    // Return the free space to the pool
    for (i = u16StartIndex; i <= u16EndIndex; i++)
    {
        *pu16FreeSpace = *pu16FreeSpace + gpInfo[i].u16Length;
        NGLOG_DEBUG("Free space = %d\n", *pu16FreeSpace);
    }

    // Overwrite the info of deleted buffers
    u16Src       = u16EndIndex + 1;
    u16Dest      = u16StartIndex;

    u16Offset    = gpInfo[u16Dest].u16Offset;
    u16SrcOffset = gpInfo[u16Src].u16Offset;

    for (i = u16Src; i < u16Index; i++)
    {
         // Update the info
         memcpy(&gpInfo[u16Dest], &gpInfo[i], sizeof(stIrdTextArchiveInfo));
         gpInfo[u16Dest].u16Offset = u16Offset;

         // Calculate the offset of the next buffer
         u16Offset = u16Offset + gpInfo[i].u16Length;

         // Calculate the length of all data to move
         u32Length = u32Length + gpInfo[i].u16Length;
         u16Dest++;
    }

    if (u32Length != 0)  // If there are valid data after the deleted block of data
    {
        // Allocate temp buffer
        pu8Buf = (char*)IRD_MALLOC_NONCACHED(u32Length);
        if (pu8Buf == NULL)
        {
            NGLOG_DEBUG("malloc pu8Buf(%x)\n", u32Length);
            return FALSE;
        }

        // Locate all valid data on the serial flash. From the block next to the block of index "u16EndIndex"
        bResult = UC_Load_DataFromSerialFlash(eBufId, pu8Buf, u32Length, u16SrcOffset);
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("Load data from serail flash failed\n");
            return FALSE;
        }

        // Save back all valid data to the starting address of the block of index "u16StartIndex"
        bResult = UC_Save_DataToSerialFlash(eBufId, pu8Buf, u32Length, gpInfo[u16StartIndex].u16Offset);
        if (bResult == FALSE)
        {
            NGLOG_DEBUG("Save data to serail flash failed\n");
            return FALSE;
        }

        if (pu8Buf != NULL)
        {
            IRD_FREE_NONCACHED(pu8Buf);
            pu8Buf = NULL;
        }

        for (i = u16Index - (u16EndIndex + 1 - u16StartIndex); i < u16Index; i++)
        {
             // Zero out the info to indicate the empty buffers
             memset(&gpInfo[i], 0x0, sizeof(stIrdTextArchiveInfo));
        }
    }
    else
    {
        for (i = u16StartIndex; i <= u16EndIndex; i++)
        {
             // Zero out the info to indicate the empty buffers
             memset(&gpInfo[i], 0x0, sizeof(stIrdTextArchiveInfo));
        }
    }

    // Update the Crc list
    if (eBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
    {
        memset(&gstCrcList_Mail, 0x0, sizeof(gstCrcList_Mail));

        for (i = 0; i < gu16MailCount; i++)
        {
            gstCrcList_Mail[i].eType  = E_CCA_CRC_NORMAL_MAIL; // FIXME
            gstCrcList_Mail[i].u16Crc = gpInfo[i].u16Crc;
        }
    }

	if (eBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
    {
        memset(&gstCrcList_Annm, 0x0, sizeof(gstCrcList_Annm));

        for (i = 0; i < gu16AnncmntCount; i++)
        {
            gstCrcList_Annm[i].eType  = E_CCA_CRC_NORMAL_MAIL; // FIXME
            gstCrcList_Annm[i].u16Crc = gpInfo[i].u16Crc;
        }
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailInfoType
///  Get the type
///
///  @param [in] u8Index
///
///  @return The total number of Mail info
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

static void _MApp_DecoderMsg_NotifyMailAnncmntUpdate(EN_CCA_TEXT_MSG_TYPE eType)
{
	tCCAIRD_UI_Event e;
	e.u8CmdData[0]=E_APP_CMD_CCAIRD_MAIL;
	//e.u8CmdData[1]=E_APP_CMD_CCA_CA_SERVICE_STATUS;
	CCAIRDEventSource::getInstance()->sendIrdCcaCmd(e);
}

///////////////////////////////////////////////////////////////////////////////
///  global _MApp_DecoderMsg_PreProcForOSD
///  Replace (1)LF with CR (2)TAB with 4 spaces for ZUI OSD display
///
///  @param [in]    pstMsg - block of source data
///  @param [in]    pu8Data - block of destination data
///
///  @return the length of the update string
///
///  @author MStarSemi @date 2013/7/18
///////////////////////////////////////////////////////////////////////////////
static UINT16 _MApp_DecoderMsg_PreProcForOSD(const stIrdTextMsg* pstMsg, UINT8* pu8Data)
{
    int i = 0, j = 0, k = 0;

//    NGLOG_DEBUG("\n");

    for (i = 0; i < pstMsg->u16Length; i++)
    {
#if 0        
        printf("%c%2d ", pstMsg->au8MsgByte[i], pstMsg->au8MsgByte[i]);
#endif
        if (pstMsg->au8MsgByte[i] == 0xA)
        {
            // LF detected, replace it with CR
            pu8Data[j] = 0xD;
            j++;
        }
        else if (pstMsg->au8MsgByte[i] == 0x09)
        {
            // TAB detected, replace it with 4 spaces
            for (k = 0; k < 4; k++)
            {
                pu8Data[j] = 0x20;
                j++;
            }
        }
        else
        {
            pu8Data[j] = pstMsg->au8MsgByte[i];
            j++;
        }

        if (j >= IRD_MAX_TEXT_LENGTH_IN_BYTE)
        {
            return j;
        }
    }
    
#if 0
    NGLOG_DEBUG("\n");
    for (i = 0; i < j; i++)
    {
        printf("%c%2d ", pu8Data[i], pu8Data[i]);
    }
    NGLOG_DEBUG("\n");
#endif

    // Return the length of the update string.

    return j;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_DeleteMails
///  Delete the mail message
///
///  @param [in]    u16MailIndex
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_DeleteMail(UINT16 u16MailIndex)
{
    if (u16MailIndex >= gu16TotalNumMails)
    {
        NGLOG_DEBUG("Wrong Index value = %d\n", u16MailIndex);
        return FALSE;
    }
    else
    {
        //MApp_CCA_EraseDataOnSerialFlash(E_CCA_IRD_DEST_BUFFER_MAIL, gpstIrdSysSetting->gpstMails[u16MailIndex].u16Length, u16MailIndex);
        _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_MAIL, u16MailIndex, u16MailIndex);

        // Save the System setting
        NGLOG_DEBUG("[SysSetting] Save the setting!\n");
        _MApp_DecoderMsg_SetSysSetting();
    }

    _MApp_DecoderMsg_NotifyMailAnncmntUpdate(E_CCA_TEXT_MSG_TYPE_MAILBOX);
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_DeleteAnncmnt
///  Delete the mail message
///
///  @param [in]    u16MailIndex
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_DeleteAnncmnt(UINT16 u16AnncmntIndex)
{
    if (u16AnncmntIndex >= gu16TotalNumAnncmnts)
    {
        NGLOG_DEBUG("Wrong Index value = %d\n", u16AnncmntIndex);
        return FALSE;
    }
    else
    {
        //MApp_CCA_EraseDataOnSerialFlash(E_CCA_IRD_DEST_BUFFER_ANNCMNT, gpstIrdSysSetting->stAnncmnts[u16AnncmntIndex].u16Length, u16AnncmntIndex);
        _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_ANNCMNT, u16AnncmntIndex, u16AnncmntIndex);

        // Save the System setting
        NGLOG_DEBUG("[SysSetting] Save the setting!\n");
        _MApp_DecoderMsg_SetSysSetting();
    }

    _MApp_DecoderMsg_NotifyMailAnncmntUpdate(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT);
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_DeleteMails
///  Delete the mail message
///
///  @param [in]    u16MailIndex
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_DeleteAllMails(void)
{
    BOOL bResult = FALSE;

    //MApp_CCA_EraseDataOnSerialFlash(E_CCA_IRD_DEST_BUFFER_MAIL, gpstIrdSysSetting->gpstMails[u16MailIndex].u16Length, u16MailIndex);
    _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_MAIL, 0, gu16MailCount - 1);

    // Save the System setting
    NGLOG_DEBUG("[SysSetting] Save the setting!\n");
    _MApp_DecoderMsg_SetSysSetting();

    _MApp_DecoderMsg_NotifyMailAnncmntUpdate(E_CCA_TEXT_MSG_TYPE_MAILBOX);
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_DeleteAllAnncmnts
///  Delete the mail message
///
///  @param [in]    u16MailIndex
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_DeleteAllAnncmnts(void)
{
    BOOL bResult = FALSE;

    //MApp_CCA_EraseDataOnSerialFlash(E_CCA_IRD_DEST_BUFFER_ANNCMNT, gpstIrdSysSetting->stAnncmnts[u16AnncmntIndex].u16Length, u16AnncmntIndex);
    bResult = _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_ANNCMNT, 0, gu16AnncmntCount - 1);

    // Save the System setting
    NGLOG_DEBUG("[SysSetting] Save the setting!\n");
    _MApp_DecoderMsg_SetSysSetting();

    _MApp_DecoderMsg_NotifyMailAnncmntUpdate(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT);
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailMsg
///  Get a Mail from the buffer
///
///  @param [in]    u16MailIndex Index of the requested mail
///
///  @return The pointer to the Mail Buffer if the requested Mail exists.
///              Otherwise, NULL pointer is returned.
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

stIrdTextMsg* MApp_DecoderMsg_GetMailMsg(UINT16 u16MailIndex)
{
    BOOL bResult = FALSE;
    stIrdTextArchiveInfo* pstArchive = NULL;

    NGLOG_DEBUG("[Mail buffer] Get Mail Msg %d\n", u16MailIndex);

    if (gbParserInitDone == FALSE)
    {
        return NULL;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    if (u16MailIndex < gu16TotalNumMails)
    {
        memset(gstMailBuf.au8MsgByte, '\0', sizeof(gstMailBuf.au8MsgByte));

        pstArchive = &gpstIrdSysSetting->stMails[u16MailIndex];

        bResult = UC_Load_DataFromSerialFlash(E_CCA_IRD_DEST_BUFFER_MAIL,
                                                  (void *)gstMailBuf.au8MsgByte,
                                                  pstArchive->u16Length,
                                                  pstArchive->u16Offset);

        if (bResult != FALSE)
        {
            memcpy(&gstMailBuf.stDelTime, &pstArchive->stDelTime, sizeof(NGL_DATE));
            gstMailBuf.u16Length = pstArchive->u16Length;
            gstMailBuf.u8Class   = pstArchive->u8Class;
            pstArchive->bNewItem = FALSE; // Remark it as "read"
            _MApp_DecoderMsg_SetSysSetting();

#ifdef  CCA_DECMSG_DEBUG
            NGLOG_DEBUG("[Mail buffer] Load data:\n");
            _MApp_DecoderMsg_PrintTextMsg(&gstMailBuf);
#endif

            return &gstMailBuf;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL; // Note: The caller shall always check the returned value
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetAnncmentMsg
///  Get an annoucement from the buffer
///
///  @param [in]    u16AnncmntIndex Index of the requested announcement
///
///  @return The pointer to the Announcement Buffer if the requested Announcement exists.
///              Otherwise, NULL pointer is returned.
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

stIrdTextMsg* MApp_DecoderMsg_GetAnncmentMsg(UINT16 u16AnncmntIndex)
{
    BOOL bResult = FALSE;
    stIrdTextArchiveInfo* pstArchive = NULL;

    NGLOG_DEBUG("[Announcement buffer] Get Anncment Msg %d\n", u16AnncmntIndex);

    if (gbParserInitDone == FALSE)
    {
        return NULL;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    if (u16AnncmntIndex < gu16TotalNumAnncmnts)
    {
        memset(gstAnncmntBuf.au8MsgByte, '\0', sizeof(gstAnncmntBuf.au8MsgByte));
        pstArchive = &gpstIrdSysSetting->stAnncmnts[u16AnncmntIndex];
        bResult = UC_Load_DataFromSerialFlash(E_CCA_IRD_DEST_BUFFER_ANNCMNT,
                                                  (void *)gstAnncmntBuf.au8MsgByte,
                                                  pstArchive->u16Length,
                                                  pstArchive->u16Offset);

        if (bResult != FALSE)
        {
            memcpy(&gstAnncmntBuf.stDelTime, &pstArchive->stDelTime, sizeof(NGL_DATE));
            gstAnncmntBuf.u16Length = pstArchive->u16Length;
            gstAnncmntBuf.u8Class   = pstArchive->u8Class;
            pstArchive->bNewItem    = FALSE; // Remark it as "read"

            _MApp_DecoderMsg_SetSysSetting();

#ifdef  CCA_DECMSG_DEBUG
            NGLOG_DEBUG("[Announcement buffer] Load data:\n");
            _MApp_DecoderMsg_PrintTextMsg(&gstAnncmntBuf);
#endif

            return &gstAnncmntBuf;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL; // Note: The caller shall always check the returned value
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetTotalMails
///  Get a Mail from the buffer
///
///  @param [in]
///
///  @return The total number of Mail info
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

UINT16 MApp_DecoderMsg_GetTotalMails(void)
{
    if (gbParserInitDone == FALSE)
    {
        NGLOG_DEBUG("\n");
        return 0;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    return (gu16MailCount);

}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetTotalAnncmnts
///  Get a Mail from the buffer
///
///  @param [in]
///
///  @return The total number of Mail info
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

UINT16 MApp_DecoderMsg_GetTotalAnncmnts(void)
{
    if (gbParserInitDone == FALSE)
    {
        NGLOG_DEBUG("\n");
        return 0;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    return (gu16AnncmntCount);
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailInfoType
///  Get the type
///
///  @param [in] u8Index
///
///  @return The total number of Mail info
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

EN_CCA_TEXT_MSG_TYPE MApp_DecoderMsg_GetMailInfoType(UINT16 u16Index)
{
    if (gbParserInitDone == FALSE)
    {
        NGLOG_DEBUG("\n");

        return E_CCA_TEXT_MSG_TYPE_SPARE;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    if (u16Index < gu16MailCount)
    {
        return E_CCA_TEXT_MSG_TYPE_MAILBOX;
    }
    else if (u16Index < (gu16MailCount + gu16AnncmntCount))
    {
        return E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT;
    }
    else
    {
        return E_CCA_TEXT_MSG_TYPE_SPARE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailInfoID
///  Get the type
///
///  @param [in] u8Index
///
///  @return The ID of Mail info
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////
UINT16 MApp_DecoderMsg_GetMailInfoID(UINT16 u16Index)
{
//   NGLOG_DEBUG("Index = %d\n", u8Index);
    if (gbParserInitDone == FALSE)
    {
        NGLOG_DEBUG("\n");

        return 0xffff;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    if (u16Index < gu16MailCount)
    {
        return gpstIrdSysSetting->stMails[u16Index].u16Id;
    }
    else if (u16Index < (gu16MailCount + gu16AnncmntCount))
    {
        return gpstIrdSysSetting->stAnncmnts[u16Index - gu16MailCount].u16Id;
    }
    else
    {
        return 0xffff;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_CheckNewMailItems
///  Check existence of new mails and announcements
///
///  @param [in]    u16MailIndex
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_GetMailInfoIDNewMailItems(UINT16 u16Index)
{
    if (gbParserInitDone == FALSE)
    {
        NGLOG_DEBUG("\n");

        return FALSE;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    if (u16Index < gu16MailCount)
    {
        return gpstIrdSysSetting->stMails[u16Index].bNewItem;
    }
    else
    {
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_GetMailInfoIDNewAnncmntItems
///  Check existence of new mails and announcements
///
///  @param [in]    u16MailIndex
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_GetMailInfoIDNewAnncmntItems(UINT16 u16Index)
{
    if (gbParserInitDone == FALSE)
    {
        NGLOG_DEBUG("\n");

        return FALSE;
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    if (u16Index < gu16AnncmntCount)
    {
        return gpstIrdSysSetting->stAnncmnts[u16Index].bNewItem;
    }
    else
    {
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailInfoMailCreateTime
///  Get the type
///
///  @param [in] u8Index
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////
UINT32 MApp_DecoderMsg_GetMailInfoMailCreateTime(UINT16 u16Index)
{
//   NGLOG_DEBUG("Index = %d\n", u8Index);
   if (gbParserInitDone == FALSE)
   {
       NGLOG_DEBUG("\n");
       return 0;
   }

   if (gbMailInfoInitDone == FALSE)  //FIXME
   {
       _MApp_DecoderMsg_GetSysSetting();
       gbMailInfoInitDone = TRUE;
   }

   if (u16Index < gu16MailCount)
   {
       return (gpstIrdSysSetting->stMails[u16Index].u32CreateTime);
   }
   else
   {
       return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailInfoAnncmntCreateTime
///  Get the type
///
///  @param [in] u8Index
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////
UINT32 MApp_DecoderMsg_GetMailInfoAnncmntCreateTime(UINT16 u16Index)
{
//   NGLOG_DEBUG("Index = %d\n", u8Index);
   if (gbParserInitDone == FALSE)
   {
       NGLOG_DEBUG("\n");
       return 0;
   }

   if (gbMailInfoInitDone == FALSE)  //FIXME
   {
       _MApp_DecoderMsg_GetSysSetting();
       gbMailInfoInitDone = TRUE;
   }

   if (u16Index < gu16AnncmntCount)
   {
       return (gpstIrdSysSetting->stAnncmnts[u16Index].u32CreateTime);
   }
   else
   {
       return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetMailDelTime
///  Get the type
///
///  @param [in] u8Index
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////
NGL_DATE* MApp_DecoderMsg_GetMailDelTime(UINT16 u16Index)
{
//   NGLOG_DEBUG("Index = %d\n", u8Index);
   if (gbParserInitDone == FALSE)
   {
       NGLOG_DEBUG("\n");
       return 0;
   }

   if (gbMailInfoInitDone == FALSE)  //FIXME
   {
       _MApp_DecoderMsg_GetSysSetting();
       gbMailInfoInitDone = TRUE;
   }

   if (u16Index < gu16MailCount)
   {
       return (&gpstIrdSysSetting->stMails[u16Index].stDelTime);
   }
   else
   {
       return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_GetAnncmntDelTime
///  Get the type
///
///  @param [in] u8Index
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////
NGL_DATE* MApp_DecoderMsg_GetAnncmntDelTime(UINT16 u16Index)
{
//   NGLOG_DEBUG("Index = %d\n", u8Index);
   if (gbParserInitDone == FALSE)
   {
       NGLOG_DEBUG("\n");
       return 0;
   }

   if (gbMailInfoInitDone == FALSE)  //FIXME
   {
       _MApp_DecoderMsg_GetSysSetting();
       gbMailInfoInitDone = TRUE;
   }

   if (u16Index < gu16AnncmntCount)
   {
       return (&gpstIrdSysSetting->stAnncmnts[u16Index].stDelTime);
   }
   else
   {
       return NULL;
   }
}

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
static UINT16 _MApp_DecoderMsg_StoreMails(BOOL bFlush, const stIrdTextMsg* pstMsg)
{
    int i = 0;
    UINT8  u8BufToOverwritten = 0;
    UINT16 u16FreeSpace       = gu16FreeSpaceMailArchive;
    UINT16 u16CurrentIndex    = IRD_INVALID_MAIL_ANNCMNT_INDEX;
    stIrdTextArchiveInfo* pstArchive;
    UINT8 au8Temp[IRD_MAX_TEXT_LENGTH_IN_BYTE + 1];
    UINT16 u16Length;

    if (pstMsg == NULL)
    {
        NGLOG_DEBUG("[Mail buffer] Wrong source\n");
        return IRD_INVALID_MAIL_ANNCMNT_INDEX;
    }

    if (bFlush == TRUE)
    {
        // Flush the buffer
        _MApp_DecoderMsg_ResetToInitialSetting(E_CCA_IRD_DEST_BUFFER_MAIL);
    }

    u16Length = _MApp_DecoderMsg_PreProcForOSD(pstMsg, au8Temp);

    // Save to the serial flash
    NGLOG_DEBUG("[Mail buffer] Save to the serial flash at gpstIrdSysSetting->stMails[%d]\n", gu16MailCount);
    if (u16Length < gu16FreeSpaceMailArchive)
    {
        // Check if it reaches the max number of mails
        if (gu16MailCount >= gu16TotalNumMails)
        {
            NGLOG_DEBUG("[Mail buffer] Replace the oldest one with the new comer\n");

            // FIXME, flush the first (in fact, just overwrite the data on the serial flash
            // MApp_CCA_EraseDataOnSerialFlash(E_CCA_IRD_DEST_BUFFER_MAIL, gpstIrdSysSetting->stMails[0].u16Length, 0);

            // FIXME,  Left aligned on the serial flash and update the archive info

            _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_MAIL, 0, 0);
        }
    }
    else
    {
        NGLOG_DEBUG("[Mail buffer] Delete the oldest ones and save the new comer\n");
        // Delete the oldest ones and save the new comer
        for (i = 0; i < gu16MailCount; i++)
        {
            u16FreeSpace = u16FreeSpace + gpstIrdSysSetting->stMails[i].u16Length;
            if (u16FreeSpace >= u16Length)
            {
                 u8BufToOverwritten = i;
                 break;
            }
        }

        // FIXME,  Left aligned on the serial flash and update the archive info
        _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_MAIL, 0, u8BufToOverwritten);
    }

    // Point to the first empty buffer
    u16CurrentIndex = gu16MailCount;

    gu16MailCount++;   // Next buffer

    NGLOG_DEBUG("[Mail buffer] Append the data to save\n");

    pstArchive = &gpstIrdSysSetting->stMails[u16CurrentIndex];
    memcpy(&pstArchive->stDelTime, &pstMsg->stDelTime, sizeof(NGL_DATE));

    pstArchive->u16Length = u16Length;
    pstArchive->u8Class   = pstMsg->u8Class;
    pstArchive->u16Crc    = pstMsg->u16Crc;
    NGLOG_DEBUG("pstArchive->u16Crc = %d\n", pstArchive->u16Crc);
    pstArchive->u16Id     = gu16TextMsgId; //FIXME
    gu16TextMsgId++;
    gu16TextMsgId         = gu16TextMsgId & 0xffff; // To avoid the overrun.

   // pstArchive->u32CreateTime = //MApi_DateTime_Get(); //MsOS_GetSystemTime();
    nglGetTime((NGL_TIME*)pstArchive->u32CreateTime);
    pstArchive->bNewItem      = TRUE; // Remark it as "new item"

    gu16FreeSpaceMailArchive = gu16FreeSpaceMailArchive - u16Length;
    NGLOG_DEBUG("gu16FreeSpaceMailArchive = %d\n", gu16FreeSpaceMailArchive);

    if (u16CurrentIndex > 0)
    {
        pstArchive->u16Offset = gpstIrdSysSetting->stMails[u16CurrentIndex - 1].u16Offset +
                                gpstIrdSysSetting->stMails[u16CurrentIndex - 1].u16Length;
    }
    else
    {
        pstArchive->u16Offset = 0;
    }

    // Write data to the serial flash
    NGLOG_DEBUG("[Mail buffer] Write data to the serial flash physically\n");
    NGLOG_DEBUG("[Mail buffer] gpstIrdSysSetting->stMails[%d].u16Length = %d\n", u16CurrentIndex, pstArchive->u16Length);
    UC_Save_DataToSerialFlash(E_CCA_IRD_DEST_BUFFER_MAIL,
                                  (void *)au8Temp,
                                  pstArchive->u16Length,
                                  pstArchive->u16Offset);

    // Save the System setting
    NGLOG_DEBUG("[SysSetting] Save the setting!\n");
    _MApp_DecoderMsg_SetSysSetting();

#ifdef  CCA_DECMSG_TEST
    MApp_DecoderMsg_GetMailMsg(u16CurrentIndex);
    _MApp_DecoderMsg_PrintTextMsg(pstMsg);
#endif

    _MApp_DecoderMsg_NotifyMailAnncmntUpdate(E_CCA_TEXT_MSG_TYPE_MAILBOX);
    return u16CurrentIndex;
}


///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_StoreAnnouncements
///  Store the mail messages
///
///  @param [in]    bFlush   The flag to indicate buffer flushing requested by IRD message
///  @param [in]    pstMsg  The pointer to the message
///
///  @return Data index in the buffer
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static UINT16 _MApp_DecoderMsg_StoreAnnouncements(BOOL bFlush, const stIrdTextMsg* pstMsg)
{
    int i = 0;
    UINT8  u8BufToOverwritten = 0;
    UINT16 u16FreeSpace       = gu16FreeSpaceAnncmntArchive;
    UINT16 u16CurrentIndex    = IRD_INVALID_MAIL_ANNCMNT_INDEX;
    stIrdTextArchiveInfo* pstArchive;
    UINT8 au8Temp[IRD_MAX_TEXT_LENGTH_IN_BYTE + 1];
    UINT16 u16Length;

    if (pstMsg == NULL)
    {
        NGLOG_DEBUG("[Announcement buffer] Wrong source\n");
        return IRD_INVALID_MAIL_ANNCMNT_INDEX;
    }

    if (bFlush == TRUE)
    {
        // Flush the buffer
        _MApp_DecoderMsg_ResetToInitialSetting(E_CCA_IRD_DEST_BUFFER_ANNCMNT);
    }

    u16Length = _MApp_DecoderMsg_PreProcForOSD(pstMsg, au8Temp);

    // Save to the serial flash
    NGLOG_DEBUG("[Announcement buffer] Save to the serial flash at gpstIrdSysSetting->stAnncmnts[%d]\n", gu16AnncmntCount);

    if (u16Length < gu16FreeSpaceAnncmntArchive)
    {
        // Check if it reaches the max number of announcements
        if (gu16AnncmntCount >= gu16TotalNumAnncmnts)
        {
            NGLOG_DEBUG("[Announcement buffer] Replace the oldest one with the new comer\n");

            // FIXME, flush the first (in fact, just overwrite the data on the serial flash
            // MApp_CCA_EraseDataOnSerialFlash(E_CCA_IRD_DEST_BUFFER_ANNCMNT, gpstIrdSysSetting->stAnncmnts[0].u16Length, 0);

            // FIXME,  Left aligned on the serial flash and update the archive info
            _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_ANNCMNT, 0, 0);
        }
    }
    else
    {
        NGLOG_DEBUG("[Announcement buffer] Delete the oldest ones and save the new comer\n");
        // Delete the oldest ones and save the new comer
        for (i = 0; i < gu16AnncmntCount; i++)
        {
            u16FreeSpace = u16FreeSpace + gpstIrdSysSetting->stAnncmnts[i].u16Length;
            if (u16FreeSpace >= u16Length)
            {
                 u8BufToOverwritten = i;
                 break;
            }
        }
        NGLOG_DEBUG("[Announcement buffer] Free space = %d\n", u16FreeSpace);

        // Left aligned on the serial flash and update the archive info
        _MApp_DecoderMsg_RemoveDataFromBuffer(E_CCA_IRD_DEST_BUFFER_ANNCMNT, 0, u8BufToOverwritten);
    }

    // Point to the first empty buffer
    u16CurrentIndex = gu16AnncmntCount;

    gu16AnncmntCount++; // Next buffer

    NGLOG_DEBUG("[Announcement buffer] Append the data %d to save\n", u16CurrentIndex);

    pstArchive = &gpstIrdSysSetting->stAnncmnts[u16CurrentIndex];
    memcpy(&pstArchive->stDelTime, &pstMsg->stDelTime, sizeof(NGL_DATE));
    pstArchive->u16Length = u16Length;
    pstArchive->u8Class   = pstMsg->u8Class;
    pstArchive->u16Crc    = pstMsg->u16Crc;
    pstArchive->u16Id     = gu16TextMsgId; //FIXME
    gu16TextMsgId++;
    gu16TextMsgId         = gu16TextMsgId & 0xffff; // To avoid the overrun.

    nglGetTime((NGL_TIME*)pstArchive->u32CreateTime);
    pstArchive->bNewItem      = TRUE; // Remark it as "new item"

    gu16FreeSpaceAnncmntArchive = gu16FreeSpaceAnncmntArchive - u16Length;
    NGLOG_DEBUG("gu16FreeSpaceAnncmntArchive = %d\n", gu16FreeSpaceAnncmntArchive);

    if (u16CurrentIndex > 0)
    {
        pstArchive->u16Offset = gpstIrdSysSetting->stAnncmnts[u16CurrentIndex - 1].u16Offset +
                                gpstIrdSysSetting->stAnncmnts[u16CurrentIndex - 1].u16Length;
    }
    else
    {
        pstArchive->u16Offset = 0;
    }

    // Write data to the serial flash
    NGLOG_DEBUG("[Announcement buffer] Write data to the serial flash physically\n");
    NGLOG_DEBUG("[Announcement buffer] gpstIrdSysSetting->stAnncmnts[%d].u16Length = %d\n", u16CurrentIndex, pstArchive->u16Length);
    UC_Save_DataToSerialFlash(E_CCA_IRD_DEST_BUFFER_ANNCMNT,
                                  (void *)au8Temp,
                                  pstArchive->u16Length,
                                  pstArchive->u16Offset);

    // Save the System setting
    NGLOG_DEBUG("[SysSetting] Save the setting!\n");
    _MApp_DecoderMsg_SetSysSetting();
    NGLOG_DEBUG("\n");

#ifdef  CCA_DECMSG_TEST
    MApp_DecoderMsg_GetAnncmentMsg(u16CurrentIndex);
    _MApp_DecoderMsg_PrintTextMsg(pstMsg);
#endif

    _MApp_DecoderMsg_NotifyMailAnncmntUpdate(E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT);
    NGLOG_DEBUG("\n");
    return u16CurrentIndex;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_DetectLongTextMsg
///  Check the existence of long text messages and attributed display by seeking the defined segment header
///
///  @param [in]    pstMsg  the pointer to the message
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/22
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_DetectLongTextMsg(const stIrdTextMsg* pstMsg)
{
    // <msg id>P<total pages>-<page num><space> in ASCII
    if (pstMsg->u16Length <= IRD_SEGMENT_HEADER_MIN_LENGTH)
    {
        NGLOG_DEBUG("[Long Msg] This is never possilbe a long message\n");
        return FALSE;
    }

    memset(&gstCurrentLongMsgSeg, 0x0, sizeof(stIrdTLongTextMsgSegment));

    if ((pstMsg->au8MsgByte[2] == ':') &&   // msg id <mm:ss>
        (pstMsg->au8MsgByte[5] == 'P'))     // Character 'P'
    {
        if ((pstMsg->au8MsgByte[7] == '-') &&       // Character '-'
            (pstMsg->au8MsgByte[9] == ' '))         // Space
        {
            // Total length  = 10, ,  total pager <= 9

            gstCurrentLongMsgSeg.u8Min         = ((pstMsg->au8MsgByte[0] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[1] - '0');
            gstCurrentLongMsgSeg.u8Sec         = ((pstMsg->au8MsgByte[3] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[4] - '0');
            gstCurrentLongMsgSeg.u8TotalPage   =  (pstMsg->au8MsgByte[6] - '0');
            gstCurrentLongMsgSeg.u8CurrentPage =  (pstMsg->au8MsgByte[8] - '0');
        }
        else if ((pstMsg->au8MsgByte[8] == '-') &&  // Character '-'
                 (pstMsg->au8MsgByte[10] == ' '))   // Space
        {
            // Total length  = 11,  total pager >= 10, page 1 to 9
            gstCurrentLongMsgSeg.u8Min         = ((pstMsg->au8MsgByte[0] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[1] - '0');
            gstCurrentLongMsgSeg.u8Sec         = ((pstMsg->au8MsgByte[3] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[4] - '0');
            gstCurrentLongMsgSeg.u8TotalPage   = ((pstMsg->au8MsgByte[6] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[7] - '0');
            gstCurrentLongMsgSeg.u8CurrentPage =  (pstMsg->au8MsgByte[9] - '0');
        }
        else if ((pstMsg->au8MsgByte[8] == '-') &&  // Character '-'
                 (pstMsg->au8MsgByte[11] == ' '))   // Space
        {
            // Total length  = 12,  total pager >= 10, page number >= 10
            gstCurrentLongMsgSeg.u8Min         = ((pstMsg->au8MsgByte[0] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[1] - '0');
            gstCurrentLongMsgSeg.u8Sec         = ((pstMsg->au8MsgByte[3] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[4] - '0');
            gstCurrentLongMsgSeg.u8TotalPage   = ((pstMsg->au8MsgByte[6] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[7] - '0');
            gstCurrentLongMsgSeg.u8CurrentPage = ((pstMsg->au8MsgByte[9] - '0') * 10) +
                                                  (pstMsg->au8MsgByte[10] - '0');
        }
        else
        {
            NGLOG_DEBUG("[Long Msg] This is NOT a long message\n");
            return FALSE;
        }
    }
    else
    {
        NGLOG_DEBUG("[Long Msg] This is NOT a long message\n");
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_CompareLongTextMsgId
///  Compare the segment header of new and existing long text messages and attributed display
///
///  @param [in] eBufId ID of the selected long message buffer
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/22
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_CompareLongTextMsgId(EN_CCA_LONGMSG_BUFFER_ID eBufId)
{
    // FIXME: Need to cnosider the disordering
    // Assuming the segment of page 1 will be arrive first and can always be found in the buffer in case of multiple pages
    if ((gstCurrentLongMsgSeg.u8Min == gpLongMsgs[eBufId][0]->stSegment.u8Min) &&
        (gstCurrentLongMsgSeg.u8Sec == gpLongMsgs[eBufId][0]->stSegment.u8Sec) &&
        (gstCurrentLongMsgSeg.u8TotalPage == gpLongMsgs[eBufId][0]->stSegment.u8TotalPage))
    {
        NGLOG_DEBUG("[Long Msg buffer] The segment of existing long message [%d] is found\n", eBufId);
        return TRUE;
    }
    else
    {
        NGLOG_DEBUG("[Long Msg buffer] The segment of new long message is found\n");
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_GetLongTextMsgIdSeconds
///  Obtain the total page number
///
///  @param [in] eBufId
///
///  @return u32MsgIdSeconds  The seconds obtained from the segement header
///
///  @author MStarSemi @date 2013/5/22
///////////////////////////////////////////////////////////////////////////////

inline UINT16 MApp_DecoderMsg_GetLongTextMsgIdSeconds(EN_CCA_LONGMSG_BUFFER_ID eBufId)
{
    // FIXME: Need to cnosider the disordering
    // Assuming the segment of page 1 will be arrive first and can always be found in the buffer in case of multiple pages
    UINT32 u32MsgIdSeconds = (gpLongMsgs[eBufId][0]->stSegment.u8Min * 60) +
                              gpLongMsgs[eBufId][0]->stSegment.u8Sec;

    return u32MsgIdSeconds;
}


///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_SearchLongTextMsgBuf
///  Compare the segment header of new and existing long text messages and attributed display
///
///  @param [in] eBufId
///
///  @return eBufId ID of the long message buffer.
///              The value E_CCA_LONGMSG_NEW means a new long message is received so that
///              a buffer is required to store it
///
///  @author MStarSemi @date 2013/5/22
///////////////////////////////////////////////////////////////////////////////

EN_CCA_LONGMSG_BUFFER_ID MApp_DecoderMsg_SearchLongTextMsgBuf(void)
{
    INT8 eBufId       = E_CCA_LONGMSG_BUF0;
    EN_CCA_LONGMSG_BUFFER_ID eTargetBufId = E_CCA_LONGMSG_BUF0;
    UINT16 u16MsgIdSeconds[E_CCA_LONGMSG_BUFSIZE];
    UINT16 u16MinMsgIdSeconds = 0xffff;

    // Check if the long message exists at one of the buffers
    for (eBufId ; eBufId < E_CCA_LONGMSG_BUFSIZE; eBufId++)
    {
        if (MApp_DecoderMsg_CompareLongTextMsgId((EN_CCA_LONGMSG_BUFFER_ID) eBufId) == TRUE)
        {
             break;
        }
    }

    // This long message not exist, find a buffer to save
    if (eBufId == E_CCA_LONGMSG_BUFSIZE)
    {
        // Search an empty buffer
        for (eBufId = 0; eBufId < E_CCA_LONGMSG_BUFSIZE; eBufId++)
        {
            if (gu16NumLongMsgs[eBufId] == 0)
            {
                // Long msg buffer is empty
                NGLOG_DEBUG("[Long Msg buffer] Long message buffer [%d] is empty!!\n", eBufId);
                break;
            }
        }

        if (eBufId == E_CCA_LONGMSG_BUFSIZE)
        {
            // All buffers are not empty, find an existing buffer to store
            for (eBufId = 0; eBufId < E_CCA_LONGMSG_BUFSIZE; eBufId++)
            {
                // Get the seconds represented with msgid <mm:ss>
                u16MsgIdSeconds[eBufId] = MApp_DecoderMsg_GetLongTextMsgIdSeconds((EN_CCA_LONGMSG_BUFFER_ID)eBufId);
                if (u16MsgIdSeconds[eBufId] <= u16MinMsgIdSeconds)
                {
                    eTargetBufId       = (EN_CCA_LONGMSG_BUFFER_ID)eBufId;
                    u16MinMsgIdSeconds = u16MsgIdSeconds[eBufId];
                }
            }
            NGLOG_DEBUG("[Long Msg buffer] All buffers are not empty. Choose a buffer to store [%d]!!\n", eTargetBufId);

            // New long message arrives, which implies the existing long message is incomplete.
            // Throw existing ones in the LongMsg buffer
            gu16NumLongMsgs[eTargetBufId] = 0;
            memset(gpstLongMsgBuf[eTargetBufId], 0x0, sizeof(stIrdLongTextMsg) * IRD_MAX_NUM_LONGMSG_PAGES);
        }
        else
        {
            // Use the empty buffer found
            eTargetBufId = (EN_CCA_LONGMSG_BUFFER_ID)eBufId;
            NGLOG_DEBUG("[Long Msg buffer] Found an empty buffer [%d]!!\n", eTargetBufId);
         }
     }
     else
     {
            // Found the target buffer for coming long message
            NGLOG_DEBUG("[Long Msg buffer] Found the target buffer [%d] for coming long message!!\n", eTargetBufId);
     }

    return eTargetBufId;
}


///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_DoAllLongTextMsgPageArrive
///  Check if the final page is received
///
///  @param [in] eBufId
///
///  @return BOOLEAN true if success, false if fail.
///
///  @author MStarSemi @date 2013/5/23
///////////////////////////////////////////////////////////////////////////////

inline BOOL MApp_DecoderMsg_DoAllLongTextMsgPageArrive(EN_CCA_LONGMSG_BUFFER_ID eBufId)
{
    // Assuming the segment of page 1 will be arrive first and can always be found in the buffer in case of multiple pages
    if (gstCurrentLongMsgSeg.u8TotalPage == gpLongMsgs[eBufId][0]->stSegment.u8TotalPage)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_ProcessLongTextMsg
///  Check the existence of long text messages and attributed display by seeking the defined segment header
///  If the received message is not a long message, store it directly to the mail and announcement buffers.
///
///  @param [in]    eDestBufId  The desitination buffer ID for message saving
///  @param [in]    bFlush         The flag to indicate buffer flushing requested by IRD message
///  @param [in]    pstMsg        The pointer to the message
///
///  @return Index  Data index in the buffer
///
///  @author MStarSemi @date 2013/5/22
///////////////////////////////////////////////////////////////////////////////

UINT16 MApp_DecoderMsg_ProcessLongTextMsg(EN_CCA_IRD_DEST_BUFFER_ID eDestBufId, BOOL bFlush, const stIrdTextMsg* pstMsg)
{
    int i;
    UINT16 u16Index     = IRD_INVALID_MAIL_ANNCMNT_INDEX;
    UINT8 u8CurrentPage = 0;
    EN_CCA_LONGMSG_BUFFER_ID eLongMsgBufId = E_CCA_LONGMSG_BUF0;

    NGLOG_DEBUG("[Long Msg buffer] Buffer ID = %d\n", eDestBufId);

    if (pstMsg == NULL)
    {
        NGLOG_DEBUG("[Long Msg buffer] Wrong source\n");
        return IRD_INVALID_MAIL_ANNCMNT_INDEX;
    }

    if (eDestBufId > E_CCA_IRD_DEST_BUFFER_ANNCMNT)
    {
        NGLOG_DEBUG("[Long Msg buffer] Wrong destination\n");
        return IRD_INVALID_MAIL_ANNCMNT_INDEX;
    }

    if (bFlush == TRUE)
    {
         NGLOG_DEBUG("[Long Msg buffer] flushing\n");

         // Flush the Mail and Announcement buffers as well
         _MApp_DecoderMsg_ResetToInitialSetting(eDestBufId);

         // New long message, which implies the existing long message is incomplete.
         // Throw existing ones in the LongMsg buffer
         for (i = 0; i < E_CCA_LONGMSG_BUFSIZE; i++)
         {
             gu16NumLongMsgs[i] = 0;
             memset(gpstLongMsgBuf[i], 0x0, sizeof(stIrdLongTextMsg) * IRD_MAX_NUM_LONGMSG_PAGES);
         }
    }

    NGLOG_DEBUG("[Long Msg buffer] \n");
    if (MApp_DecoderMsg_DetectLongTextMsg(pstMsg) == FALSE)
    {
        // Not a long message
        if (eDestBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
        {
            u16Index = _MApp_DecoderMsg_StoreMails(bFlush, pstMsg);
        }
        else if (eDestBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
        {
            u16Index = _MApp_DecoderMsg_StoreAnnouncements(bFlush, pstMsg);
        }

        NGLOG_DEBUG("[Long Msg buffer]\n");
        return u16Index;
    }
    else
    {
        NGLOG_DEBUG("[Long Msg buffer] SearchLongTextMsgBuf\n");

        // Search the destination buffer by checking the msgid at the segment header
        eLongMsgBufId = MApp_DecoderMsg_SearchLongTextMsgBuf();
    }

    NGLOG_DEBUG("[Long Msg buffer]\n");
    // Store the Mails/Announcements
    if (gstCurrentLongMsgSeg.u8TotalPage == 1)
    {
        NGLOG_DEBUG("[Long Msg buffer]\n");
        // Copy the message to Mail/announcement buffer directly
        if (eDestBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
        {
             u16Index = _MApp_DecoderMsg_StoreMails(bFlush, pstMsg);
        }
        else if (eDestBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
        {
             u16Index = _MApp_DecoderMsg_StoreAnnouncements(bFlush, pstMsg);
        }
    }
    else
    {
        NGLOG_DEBUG("[Long Msg buffer]\n");

        // Store the message to Long Msg buffer
        u8CurrentPage = gstCurrentLongMsgSeg.u8CurrentPage;
        memcpy(&gpLongMsgs[eLongMsgBufId][u8CurrentPage]->stMsg, pstMsg, sizeof(stIrdTextMsg));
        memcpy(&gpLongMsgs[eLongMsgBufId][u8CurrentPage]->stSegment, &gstCurrentLongMsgSeg, sizeof(stIrdTLongTextMsgSegment));

        gu16NumLongMsgs[eLongMsgBufId]++; // Go to the next

        if (MApp_DecoderMsg_DoAllLongTextMsgPageArrive(eLongMsgBufId) == TRUE)
        {
            // All pages are received and the long message is compete
            // Move the complete message/pages to Mail/announcements buffer sequentially
            if (eDestBufId == E_CCA_IRD_DEST_BUFFER_MAIL)
            {
                for (i = 0; i < gu16NumLongMsgs[eLongMsgBufId]; i++)
                {
                     if (i == 0)
                     {
                         _MApp_DecoderMsg_StoreMails(FALSE, &gpLongMsgs[eLongMsgBufId][i]->stMsg);
                     }
                     else // Use the index of first page
                     {
                         u16Index = _MApp_DecoderMsg_StoreMails(FALSE, &gpLongMsgs[eLongMsgBufId][i]->stMsg);
                     }
                }
            }
            else if (eDestBufId == E_CCA_IRD_DEST_BUFFER_ANNCMNT)
            {
                for (i = 0; i < gu16NumLongMsgs[eLongMsgBufId]; i++)
                {
                     if (i > 0)
                     {
                         _MApp_DecoderMsg_StoreAnnouncements(FALSE, &gpLongMsgs[eLongMsgBufId][i]->stMsg);
                     }
                     else // Use the index of first page
                     {
                         u16Index = _MApp_DecoderMsg_StoreAnnouncements(FALSE, &gpLongMsgs[eLongMsgBufId][i]->stMsg);
                     }
                }
            }

            gu16NumLongMsgs[eLongMsgBufId] = 0;
            memset(gpstLongMsgBuf[eLongMsgBufId], 0x0, sizeof(stIrdLongTextMsg) * IRD_MAX_NUM_LONGMSG_PAGES);
        }


        if (gu16NumLongMsgs[eLongMsgBufId] == IRD_MAX_NUM_LONGMSG_PAGES)
        {
            // Long msg buffer is full
            // Theoretically, it shall not be here unless the long message has more than 5 pages!
            // The spare buffer is being taken.
            NGLOG_DEBUG("[Long Msg buffer] Buffer full. Might need to enlarge the buffer!!\n");
            return IRD_INVALID_MAIL_ANNCMNT_INDEX;
        }
    }

    return u16Index;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_CheckCrcList
///  Check if the receive messasge is duplicationed with CRC values in database
///
///  @param [in]    enType  CRC type
///  @param [in]    u16Crc  CRC16 value to check
///
///  @return BOOLEAN true for handled (don't dispatch continully)
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

EN_CCA_CRC_STATUS MApp_DecoderMsg_CheckCrcList(EN_CCA_TEXT_MSG_TYPE msgType, EN_CCA_CRC_MESSAGE_TYPE enType, UINT16 u16Crc)
{
    static UINT8 u8MailHead = 0;
	static UINT8 u8AnnmHead = 0;

    int i = 0;
	UINT8 *head = NULL;
	stCrcEntry *pCrcList = NULL;
	UINT32 maxNum = 0;

	if(msgType == E_CCA_TEXT_MSG_TYPE_ANNOUNCEMENT)
	{
		NGLOG_DEBUG("[CRC] Check ANNM CRC!\n");
		pCrcList = gstCrcList_Annm;
		maxNum = IRD_MAX_NUM_ANNCMNTS;
		head = &u8AnnmHead;
	}
	else
	{
		NGLOG_DEBUG("[CRC] Check MAIL CRC!\n");
		pCrcList = gstCrcList_Mail;
		maxNum = IRD_MAX_NUM_MAILS;
		head = &u8MailHead;
	}

    // Scan the list
    for (i = 0; i < maxNum; i++)
    {
        // Compare the CRC values
        if (pCrcList[i].u16Crc == u16Crc)
        {
            NGLOG_DEBUG("[CRC] Duplicated message received!\n");
            return E_CCA_CRC_DUPLICATED;
        }

        // Try to find an empty slot
        if (pCrcList[i].eType == E_CCA_CRC_EMPTY)
        {
            NGLOG_DEBUG("[CRC] New message received! i = %d\n", i);
            pCrcList[i].eType  = enType;
            pCrcList[i].u16Crc = u16Crc;
            break;
        }
    }

    // FIFO full, OVERWRITE the oldest one indexed by u8Head
    if (i == maxNum)
    {
        NGLOG_DEBUG("[CRC] CRC List FIFO full!\n");

        pCrcList[*head].eType  = enType;
        pCrcList[*head].u16Crc = u16Crc;
        *head = (*head + 1) & 0x3f;
    }

    return E_CCA_CRC_NEW;
}

///////////////////////////////////////////////////////////////////////////////
///  MApp_DecoderMsg_CalCrc16
///  Calculate the CRC16 value of data in the message payload
///
///  @param [in]  pStartAddr        Starting address of the data
///  @param [in]  u32SizeInBytes  Length of the data
///  @param [in]  u16InitCrc         Initial value of CRC
///
///  @return CRC16 value
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////

UINT16 MApp_DecoderMsg_CalCrc16(const void* pStartAddr, UINT32 u32SizeInBytes, UINT16 u16InitCrc)
{
    UINT16 u16Crc;
    UINT8* pu8Data;    /* Init the start value */

    u16Crc  = u16InitCrc;     /* Calculate CRC16 */
    pu8Data = (UINT8 *)pStartAddr;

    while (u32SizeInBytes--)
    {
         u16Crc = (UINT16)(((u16Crc >> 8) & 0x00ff) ^ u16TableCrc16[((u16Crc ^ (*pu8Data)) & 0x00ff)]);
         pu8Data++;
    }

    return (u16Crc);
} // end of MApp_Crc16


///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_Parser
///  IRD message parser
///
///  @param [in]    hMessage   Message handle
///  @param [in]    CaMsgInfo  pointer to the received message in CaTask
///
///  @return
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_Parser(uc_raw_ird_msg *Raw_Ird_Msg)
{
    stIrdMessage stIrdMsg = {0};
    BOOL bHandled  = FALSE;
    UINT16  u16Crc    = 0;
    UINT32  u32Length = 0;
    UINT8   au8DecMsgBuf[IRD_MESSAGE_MAX_LENGTH];
    UINT8*  pu8DecMsg = NULL;

    if (gbParserInitDone != TRUE)
    {
        NGLOG_DEBUG("[IRD Message] Parser is not initialized. Call MApp_DecoderMsg_InitParser(). \n");
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[IRD Message] Enter the parser\n");
    }

    if (gbMailInfoInitDone == FALSE)  //FIXME
    {
        _MApp_DecoderMsg_GetSysSetting();
        gbMailInfoInitDone = TRUE;
    }

    memset(&au8DecMsgBuf[0], 0x0, IRD_MESSAGE_MAX_LENGTH);

    u32Length = Raw_Ird_Msg->length;
    if (u32Length == 0)
    {
        NGLOG_DEBUG("[IRD Message] Wrong CA message length = %ld\n", u32Length);
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[IRD Message] CA message length = %ld\n", u32Length);
    }

    memcpy(au8DecMsgBuf, Raw_Ird_Msg->rawIrdMsg, Raw_Ird_Msg->length);

    // Debug purpose
    NGLOG_DEBUG("[IRD Message] CA System ID %d\n", Raw_Ird_Msg->caSystemID);

    pu8DecMsg = &au8DecMsgBuf[0];

    // Get the destination ID of the Decoder Message (4 bits)
    stIrdMsg.u8DestinationId = (UINT8)((pu8DecMsg[0] >> 4) & 0x0f);

    // Validate the destination ID first
    if (stIrdMsg.u8DestinationId >= E_CCA_IRD_DEST_ID_SPARE)
    {
        NGLOG_DEBUG("[IRD Message] Invalid or Spare Destination ID!\n");
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("[IRD Message] Destination ID = %d \n", stIrdMsg.u8DestinationId);
    }

    // Get the length of the Decoder Message - 12 bits, max = 4095 bytes
    // Exclude the destination and length fields
    stIrdMsg.u32Length = (UINT16)((((UINT16)(pu8DecMsg[0] & 0x0f) << 8) & 0x0f00) |
                                  (((UINT16)(pu8DecMsg[1] & 0xff))));

    if ((stIrdMsg.u32Length == 0) || (stIrdMsg.u32Length != (u32Length - 4))) // The length excludes 2 bytes header and 2 bytes CRC
    {
         NGLOG_DEBUG("[IRD Message] Zero or wrong message length = %d!\n", stIrdMsg.u32Length);
         return FALSE;
    }
    else
    {
         NGLOG_DEBUG("[IRD Message] Required message length = %d!\n", stIrdMsg.u32Length);
    }

    // Get CRC16 (16 bits), check all text data except the 2 CRC bytes
    stIrdMsg.u16CRC = (UINT16)(((((UINT16)pu8DecMsg[(stIrdMsg.u32Length + 2)]) << 8) & 0xff00) |
                                (((UINT16)pu8DecMsg[(stIrdMsg.u32Length + 3)]) & 0x00ff));

    u16Crc = MApp_DecoderMsg_CalCrc16(&pu8DecMsg[0], stIrdMsg.u32Length + 2L, 0);

    // Compare calculated CRC value with the received value
    if (u16Crc != stIrdMsg.u16CRC)
    {
         NGLOG_DEBUG("[IRD Message] CRC value error! %d, IRD = %d\n", u16Crc, stIrdMsg.u16CRC);
         return FALSE;
    }

    // In following cases, the length (2 bytes) of CRC fields will be excluded
    switch (stIrdMsg.u8DestinationId)
    {
        case E_CCA_IRD_DEST_TEXT:
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_TEXT!\n");
            bHandled = MApp_DecoderMsg_TextMsg_Parser(&pu8DecMsg[2], stIrdMsg.u32Length, stIrdMsg.u16CRC);
            break;
        }
        case E_CCA_IRD_DEST_DECODER_CONTROL:
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_DECODER_CONTROL!\n");
            //bHandled = MApp_DecoderMsg_DecControl_Parser(&pu8DecMsg[2], stIrdMsg.u32Length);  MX
            break;
        }
        case E_CCA_IRD_DEST_CAM: //Not used
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_CAM (Not used)!\n");
            bHandled = FALSE;
            break;
        }
        case E_CCA_IRD_DEST_PROF_DEC:
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_PROF_DEC!\n");
            bHandled = MApp_DecoderMsg_ProfDecMsg_Parser(&pu8DecMsg[2], stIrdMsg.u32Length);
            break;
        }
        case E_CCA_IRD_DEST_ATTRIBUTED_DISPLAY:
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_ATTRIBUTED_DISPLAY!\n");
            //bHandled = MApp_DecoderMsg_AttributedDisplay_Parser(&pu8DecMsg[2], stIrdMsg.u32Length, stIrdMsg.u16CRC); MX
            break;
        }
        case E_CCA_IRD_DEST_OPEN_CABLE: // Not implemented
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_OPEN_CABLE (Not supported)!\n");
            bHandled = FALSE;
            break;
        }
        case E_CCA_IRD_DEST_CIPLUS_CAM:
        {
            NGLOG_DEBUG("[IRD Message] Destination ID = E_CCA_IRD_DEST_CIPLUS_CAM!\n");
           // bHandled = MApp_DecoderMsg_CIPlusCam_Parser(&pu8DecMsg[2], stIrdMsg.u32Length);
            break;
        }
        default:
            // Shall never go here
            NGLOG_DEBUG("[IRD Message] Invalid Destination ID|!\n");
            return FALSE;
    }

    return bHandled;
}



///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsgr_Init_Paser
///  Initialize IRD message parser
///
///  @param [in]      u16MaxNumGroups  Max number of groups
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

BOOL MApp_DecoderMsg_InitParser(UINT16 u16MaxNumGroups)
{
    int i = 0, j = 0;

    if (u16MaxNumGroups == 0)
    {
        NGLOG_DEBUG("[IRD Message] Wrong group number!\n");
        return FALSE;
    }
    else if (u16MaxNumGroups > IRD_MAX_NUM_GROUPS)
    {
        NGLOG_DEBUG("[IRD Message] Over the max group number. Increase the number!\n");
        return FALSE;
    }

    gu16TotalNumMails    = IRD_MAX_NUM_MAILS * u16MaxNumGroups;
    gu16TotalNumAnncmnts = IRD_MAX_NUM_ANNCMNTS * u16MaxNumGroups;

    memset(&gstMailBuf, 0x0, sizeof(stIrdTextMsg));
    memset(&gstAnncmntBuf, 0x0, sizeof(stIrdTextMsg));

    if (gpstIrdSysSetting != NULL)
    {
        IRD_FREE(gpstIrdSysSetting);
    }

    gpstIrdSysSetting = (stIrdSysSetting *)IRD_MALLOC(sizeof(stIrdSysSetting));
    if (gpstIrdSysSetting == NULL)
    {
        NGLOG_DEBUG("Memory allocation failed!\n");
        return FALSE;
    }
    else
    {
        NGLOG_DEBUG("Memory allocation Done!\n");
    }

    // Initialise the pointers
    _MApp_DecoderMsg_ResetToInitialSetting(E_CCA_IRD_DEST_BUFFER_MAIL);
    _MApp_DecoderMsg_ResetToInitialSetting(E_CCA_IRD_DEST_BUFFER_ANNCMNT);

    // Allocate temp buffer of long messages
    // Free the buffer if it's allocated
    for (i = 0; i < E_CCA_LONGMSG_BUFSIZE; i++)
    {
        if (gpstLongMsgBuf[i] != NULL)
        {
            IRD_FREE(gpstLongMsgBuf[i]);
        }

        gpstLongMsgBuf[i] = (stIrdLongTextMsg *)IRD_MALLOC(sizeof(stIrdLongTextMsg) * IRD_MAX_NUM_LONGMSG_PAGES);
        if (gpstLongMsgBuf[i] == NULL)
        {
            NGLOG_DEBUG("[Long Msg buffer] Memory allocation failed!\n");
            return FALSE;
        }

        // Array of pointers to the Mail Buffer
        if (gpLongMsgs[i] != NULL)
        {
            IRD_FREE(gpLongMsgs[i]);
        }

        gpLongMsgs[i] = (stIrdLongTextMsg **)IRD_MALLOC(sizeof(stIrdLongTextMsg *) * IRD_MAX_NUM_LONGMSG_PAGES);
        if (gpLongMsgs[i] == NULL)
        {
            NGLOG_DEBUG("[LOng Msg buffer] Memory allocation of pointers failed!\n");
            return FALSE;
        }

        // Initialise the pointers
        for (j = 0; j < IRD_MAX_NUM_LONGMSG_PAGES; j++)
        {
            gpLongMsgs[i][j] = &gpstLongMsgBuf[i][j];
        }

        gu16NumLongMsgs[i] = 0;         ///< Number of Long messages in the Buffer
    }

    // Init Text Message parser
    MApp_DecoderMsg_TextMsg_InitParser(u16MaxNumGroups);

    gbParserInitDone = TRUE;
	UcMsgPs_Initialize();

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global  MApp_DecoderMsg_TerminateParser
/// Terminate IRD message parser
///
///  @param [in]  void
///
///  @return
///
///  @author MStarSemi @date 2013/5/9
///////////////////////////////////////////////////////////////////////////////

void MApp_DecoderMsg_TerminateParser(void)
{
    int i = 0;

    gbMailInfoInitDone = FALSE;
    gbParserInitDone   = FALSE;

    MApp_DecoderMsg_TextMsg_TerminateParser();

    for (i = 0; i < E_CCA_LONGMSG_BUFSIZE; i++)
    {
        if (gpLongMsgs[i] != NULL)
        {
            IRD_FREE((void *)gpLongMsgs[i]);
            gpLongMsgs[i] = NULL;
        }
    }

    for (i = 0; i < E_CCA_LONGMSG_BUFSIZE; i++)
    {
        if (gpstLongMsgBuf[i] != NULL)
        {
            IRD_FREE((void *)gpstLongMsgBuf[i]);
            gpstLongMsgBuf[i] = NULL;
        }
    }

    if (gpstIrdSysSetting != NULL)
    {
        IRD_FREE(gpstIrdSysSetting);
    }
}
}
#undef _APP_CCA_DECODERMSG_C

