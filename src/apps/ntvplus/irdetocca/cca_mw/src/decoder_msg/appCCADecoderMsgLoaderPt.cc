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
/// file    appCCADecoderMsgLoaderPt.c
/// @brief  CCA Decoder Message parser and dispatcher
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////



#define _APP_CCA_DECODERMSGPAODERPT_C

/********************************************************************************/
/*                            Header Files                                      */
/********************************************************************************/
// common
#include <ngl_types.h>


// api

#include "appCCADecoderMsg.h"
#include "appCCADecoderMsgLoaderPt.h"

// driver

#if defined (MSOS_TYPE_LINUX)
#include <linux/string.h>
#endif

#include "../../../platform/api/ird_ldr_sup/irdeto.h"

#if(LOADER_V5 == 1)
#include "LC_apiDigiTuner.h"
#include "LoaderCoreAPI_ApplicationHelper.h"
#include "LoaderCore_Types.h"
#include "LoaderCorePrivate.h"
#endif
//#define CCA_LOADERPT_DEBUG

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#ifdef CCA_LOADERPT_DEBUG
#define CCA_LOADERPT_DBG(fmt, arg...)   {printf("\033[0;35m[LoadPt]%s(%d): ", __FUNCTION__, __LINE__); printf((char *)fmt, ##arg); printf("\033[m");}
#else
#define CCA_LOADERPT_DBG(fmt, arg...)
#endif

#define _M_DS_DATA_COUNT    (_M_DS_DATA_COUNT_)                          ///<  defined by the manufacturers
#if ( (MS_DVB_TYPE_SEL == DVBS) || (MS_DVB_TYPE_SEL == DVBS2) )
#define _M_DS_DATA_COUNT_   (sizeof(MS_FE_CARRIER_PARAM)+sizeof(MS_SAT_PARAM)+4)
#else
#define _M_DS_DATA_COUNT_   (sizeof(MS_FE_CARRIER_PARAM))
#endif
#define DL_TS_DATA_COUNT    (_M_DS_DATA_COUNT + 6)
#define LDR_AREA_COUNT      (14)        ///< data count of Loader PT, exclusive of DL_TS_STRUCT.
#define LDR_AREA_OFFSET     (DL_TS_DATA_COUNT * 2) /* relative offset. */

//#define LOADER_PT_SIZE    ( DL_TS_DATA_COUNT * 2 + LDR_AREA_COUNT )
#if 0//(NVM_device == 1)
#define IRDETO_PT_INDEX   (0x400)
#elif(CA_CCA_WITH_IRD_LDR == 1)
#define IRDETO_PT_INDEX   (0x140000)//0x130000 for loader
#else
#define IRDETO_PT_INDEX   (0x20000)//0x20000 for mboot test
#endif
//#define IRDETO_PT_SIZE    (512)
#define LOADER_PT_INDEX     (0x150000)//(IRDETO_PT_INDEX + IRDETO_PT_SIZE)
#define LOADER_PT_SIZE      sizeof(LOADER_PT_STRUCT)//(DL_TS_DATA_COUNT * 2 + LDR_AREA_COUNT)

#define IRDETO_PT_INDEX2    (0x180000)/* relative address */
#define LOADER_PT_INDEX2    (0x181000)

#define BACKUP_LOADERPT     (1)

/******************************************************************************/
/*                                 External                                   */
/******************************************************************************/
/********************************************************************************/
/*                            Global Variables                                  */
/********************************************************************************/
/********************************************************************************/
/*                            Local Variables                                   */
/********************************************************************************/
typedef struct
{
    BYTE  abDS[_M_DS_DATA_COUNT]; /* delivery system physical parameters */

    WORD  wPID; /* code download ES PID */

    BYTE  bTable_ID;

    BYTE  reserved;		// Add reserved

    WORD  wCRC16; /* it contains the CRC16 of all above data, exclusive of itself. */
} DL_TS_STRUCT;

typedef struct
{
    DL_TS_STRUCT  astTS[2]; /* Download TS parameters written by Highlevel code.  */

    /* Note: When being ready to trigger a OTA dl, Highlevel
       code must set fEnableOtaDL=TRUE and fFlashCorrupted=FALSE. */

    BOOL   fEnableOtaDL; /* only for OTA_DOWNLOAD. */

    BOOL   fFlashCorrupted; /* for OTA_DOWNLOAD and OTA_FP_DOWNLOAD.
	                           TRUE - some of data in flash has been modified during dl. */

    BOOL   fSleepMode;

    BYTE   fTestMode;

    ULONG  fResult;

    BYTE   bMenuLang;

    BYTE   bBootType;

    BYTE   bReserved;

    BYTE   bReserved1;

    WORD   wCRC16; /* it contains the CRC16 of all above data, exclusive of DL_TS_STRUCT and itself. */
} LOADER_PT_STRUCT;


static BYTE  abLdrPtBuf[LOADER_PT_SIZE];
static LOADER_PT_STRUCT stLdrPt;

//Static variable to store EEPROM memory
static BYTE origLoaderPT[LOADER_PT_SIZE];


/********************************************************************************/
/*                            extern Function                                */
/********************************************************************************/
// Irdeto control APIs
extern INT NVM_DRV_Write(PBYTE pbBuffer, ULONG wOffset, WORD wLength );
extern INT NVM_DRV_Read(PBYTE pbBuffer, ULONG wOffset, WORD wLength );

extern INT FLSH_DRV_Read(  PBYTE pbBuffer, ULONG uDest,    ULONG uLength );
extern INT FLSH_DRV_Write( PBYTE pbBuffer, ULONG uDest,    ULONG uLength );
extern INT FLSH_DRV_Erase( ULONG uDest,    ULONG uLength );

/********************************************************************************/
/*                            Local Function                                */
/********************************************************************************/

static UINT16 LE2BE(UINT16 u16Value)
{
    return ((u16Value >> 8) & 0xff) + ((u16Value << 8) & 0xff00);
}

static INT8 NVM_MNG_ReadLoaderPt(UINT8* pu8Buf )
{
    if (NVM_DRV_Read((PBYTE)pu8Buf, (ULONG)LOADER_PT_INDEX, (ULONG)LOADER_PT_SIZE) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static VOID NVM_MNG_ParseLoaderPt( PBYTE pbBuf, LOADER_PT_STRUCT *pstBuf )
{
    BYTE  i;
    WORD  j, wCnt;

    wCnt = 0;
    for( i=0; i<2; i++ )
    {
        for( j=0; j<_M_DS_DATA_COUNT; j++ )
        {
            pstBuf->astTS[i].abDS[j] = pbBuf[wCnt++];
        }

        pstBuf->astTS[i].wPID  = (WORD)((WORD)pbBuf[wCnt++] << 8);
        pstBuf->astTS[i].wPID |= (WORD)pbBuf[wCnt++];

        pstBuf->astTS[i].bTable_ID = pbBuf[wCnt++];
        wCnt++; /* skip reserved byte. */

        pstBuf->astTS[i].wCRC16  = (WORD)((WORD)pbBuf[wCnt++] << 8);
        pstBuf->astTS[i].wCRC16 |= (WORD)pbBuf[wCnt++];
    }

    pstBuf->fEnableOtaDL = (BOOL)pbBuf[wCnt++];
    printf("OTAAA:%x \n",pstBuf->fEnableOtaDL);
    pstBuf->fFlashCorrupted = (BOOL)pbBuf[wCnt++];
    pstBuf->fSleepMode = (BOOL)pbBuf[wCnt++];
    pstBuf->fTestMode = (BYTE)pbBuf[wCnt++];
    pstBuf->fResult = ((ULONG)pbBuf[wCnt++]);
    pstBuf->fResult += (((ULONG)pbBuf[wCnt++])<<8);
    pstBuf->fResult += (((ULONG)pbBuf[wCnt++])<<16);
    pstBuf->fResult += (((ULONG)pbBuf[wCnt++])<<24);
    pstBuf->bMenuLang = (BYTE)pbBuf[wCnt++];
    pstBuf->bBootType = (BYTE)pbBuf[wCnt++];
    wCnt++; // skip reserved
    wCnt++; // skip reserved1
    pstBuf->wCRC16  = (WORD)((WORD)pbBuf[wCnt++] << 8);
    pstBuf->wCRC16 |= (WORD)pbBuf[wCnt];
}

/*
 * Description: Verifies the integrity of the tuning parameter area within the
 *              Loader NVRAM Partition.
 *
 * Parameters : pbBuf  - pointer to data buffer containing the image of Loader
 *              NVRAM Partition.
 *              pstBuf - pointer to the structure holding the contents of Loader
 *              NVRAM Partition.
 *
 * Returns    : -1 - Tuning parameters in the Loader Pt were corrupted.
 *               0 - Two sets of tuning parameters are all correct.
 *               1 - Only the 1st set is OK.
 *               2 - Only the 2nd set is OK.
 */
static INT NVM_MNG_VerifyTsParameters( PBYTE pbBuf, LOADER_PT_STRUCT *pstBuf )
{
    INT  sRet;
    WORD  wCRC_16;
    ULONG uSize;
#if (BACKUP_LOADERPT == 1)
    BYTE tmp[DL_TS_DATA_COUNT*2];
    INT sRet2;
    WORD loaderpt_crc;
    BYTE  i;
    WORD  j, wCnt;
    PBYTE pbBuf2 = pbBuf;
#endif

    sRet = -1;

    /* the 1st set of tuning parameters. */
    uSize = DL_TS_DATA_COUNT - 2;
    wCRC_16 = MApp_DecoderMsg_CalCrc16( (PVOID)pbBuf, uSize, 0 );
    if( (wCRC_16 == pstBuf->astTS[0].wCRC16) && (pstBuf->astTS[0].wCRC16 != 0) )
    {
        sRet = 1; /* only the 1st is OK. */
    }

    pbBuf += DL_TS_DATA_COUNT;

    /* the 2nd set of tuning parameters. */
    wCRC_16 = MApp_DecoderMsg_CalCrc16( (PVOID)pbBuf, uSize, 0 );
    if( (wCRC_16 == pstBuf->astTS[1].wCRC16) && (pstBuf->astTS[1].wCRC16 != 0) )
    {
        if( sRet == 1 )
            sRet = 0; /* both of them are correct. */
        else
            sRet = 2; /* only the 2nd is OK. */
    }

#if (BACKUP_LOADERPT == 1)
    sRet2 = -1;

    /* the 1st set of tuning parameters. */
    uSize = DL_TS_DATA_COUNT - 2;
    if(FLSH_DRV_Read(tmp, (ULONG)LOADER_PT_INDEX2, (ULONG)(DL_TS_DATA_COUNT*2)))
    {
        wCRC_16 = MApp_DecoderMsg_CalCrc16( (PVOID)tmp, uSize, 0 );
        loaderpt_crc = tmp[DL_TS_DATA_COUNT-2] << 8;
        loaderpt_crc |= tmp[DL_TS_DATA_COUNT-1];
        //printf("1 wCRC_16 = 0x%x\n", wCRC_16);
        //printf("1 loaderpt_crc = 0x%x\n", loaderpt_crc);
        if( (wCRC_16 == loaderpt_crc) && (loaderpt_crc != 0) )
        {
            sRet2 = 1; /* only the 1st is OK. */
        }

        /* the 2nd set of tuning parameters. */
        wCRC_16 = MApp_DecoderMsg_CalCrc16( (PVOID)(tmp+DL_TS_DATA_COUNT), uSize, 0 );
        loaderpt_crc = tmp[DL_TS_DATA_COUNT*2-2] << 8;
        loaderpt_crc |= tmp[DL_TS_DATA_COUNT*2-1];
        //printf("2 wCRC_16 = 0x%x\n", wCRC_16);
        //printf("2 loaderpt_crc = 0x%x\n", loaderpt_crc);
        if( (wCRC_16 == loaderpt_crc) && (loaderpt_crc != 0) )
        {
            if( sRet2 == 1 )
                sRet2 = 0; /* both of them are correct. */
            else
                sRet2 = 2; /* only the 2nd is OK. */
        }
    }
    else
    {
        CCA_LOADERPT_DBG("[NVM_MNG] FLSH_DRV_Read LOADERPT2 are Fail.\n");
        sRet = -1;
    }
    //printf("sRet = %d\n", sRet);
    //printf("sRet2 = %d\n", sRet2);

    if( sRet < 0 )
    {
        if(sRet2>=0)    //Recover PT0
        {
            FLSH_DRV_Erase(LOADER_PT_INDEX, DL_TS_DATA_COUNT*2);
            FLSH_DRV_Write(tmp, LOADER_PT_INDEX, DL_TS_DATA_COUNT*2);

            wCnt = 0;
            for( i=0; i<2; i++ )
            {
                for( j=0; j<_M_DS_DATA_COUNT; j++ )
                {
                    pstBuf->astTS[i].abDS[j] = tmp[wCnt++];
                }

                pstBuf->astTS[i].wPID  = (WORD)((WORD)tmp[wCnt++] << 8);
                pstBuf->astTS[i].wPID |= (WORD)tmp[wCnt++];

                pstBuf->astTS[i].bTable_ID = tmp[wCnt++];
                wCnt++; /* skip reserved byte. */

                pstBuf->astTS[i].wCRC16  = (WORD)((WORD)tmp[wCnt++] << 8);
                pstBuf->astTS[i].wCRC16 |= (WORD)tmp[wCnt++];
            }
            sRet = sRet2;
        }
        else
        {
            ;
        }
    }
    else
    {
        if(sRet2>=0)
        {
            ;
        }
        else    //Recover PT1
        {
            FLSH_DRV_Erase(LOADER_PT_INDEX2, DL_TS_DATA_COUNT*2);
            FLSH_DRV_Write(pbBuf2, LOADER_PT_INDEX2, DL_TS_DATA_COUNT*2);
        }
    }
#endif

    if( sRet < 0 )
    {
        CCA_LOADERPT_DBG("[NVM_MNG] ERROR: TS parameters within NVRAM were corrupted.\n");
    }
    else /* it may keep working as long as one of two sets is correct. */
    {
        CCA_LOADERPT_DBG("[NVM_MNG] TS parameters within NVRAM are OK.\n");
    }

    return sRet;
}

///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_RestoreLoaderpt
///  Verifies the integrity of the tuning parameter area within the Loader NVRAM Partition.
///
///  @param [in]
///
///  @return
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static void _MApp_DecoderMsg_RestoreLoaderpt(void)
{
    NVM_DRV_Write(&origLoaderPT[0], LOADER_PT_INDEX, sizeof(LOADER_PT_STRUCT));
}

///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_RestoreLoaderpt
///  Verifies the integrity of the tuning parameter area within the Loader NVRAM Partition.
///
///  @param [in] pstLdrPt
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL _MApp_DecoderMsg_ChkTSParam(LOADER_PT_STRUCT *pstLdrPt)
{
    if (NVM_MNG_ReadLoaderPt(&abLdrPtBuf[0]) == 0)
    {
        NVM_MNG_ParseLoaderPt(&abLdrPtBuf[0], pstLdrPt);
        if (NVM_MNG_VerifyTsParameters(&abLdrPtBuf[0], pstLdrPt) >= 0)
        {
            CCA_LOADERPT_DBG("[HL] Forced download required.\n");
            CCA_LOADERPT_DBG("[HL] TS parameters Verify done!\n");
            return TRUE;
        }
        CCA_LOADERPT_DBG("[HL] TS parameters Verify fail!\n");
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///  private _MApp_DecoderMsg_RestoreLoaderpt
///  Verifies the integrity of the tuning parameter area within the Loader NVRAM Partition.
///
///  @param [in]    pstDemodParam  Pointer to the struct of Demod parameters
///  @param [in]    u32Pid                PID
///  @param [in]    u32TableId          Table ID
///
///  @return
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
#if ( (MS_DVB_TYPE_SEL == DVBS) || (MS_DVB_TYPE_SEL == DVBS2) )
static BOOL _MApp_DecoderMsg_WriteLoaderpt(MS_FE_CARRIER_PARAM* pstDemodParam, MS_SAT_PARAM* stSATParam, UINT16 u16Pid, UINT16 u16TableId)
{
    LOADER_PT_STRUCT loaderPT;
    MS_FE_CARRIER_PARAM tunesetting;

    memset(&loaderPT, 0x00, sizeof(loaderPT));
    memset(&tunesetting, 0x00, sizeof(MS_FE_CARRIER_PARAM));

    // Preserve original loader partition
    // Modify check value
    // FIXME:
    if (NVM_MNG_ReadLoaderPt(&origLoaderPT[0]) == 1)
    {
        CCA_LOADERPT_DBG("Read NVRAM error \n");
        return FALSE;
    }

    tunesetting.SatParam.eConstellation     = pstDemodParam->SatParam.eConstellation;
    tunesetting.SatParam.eRollOff           = pstDemodParam->SatParam.eRollOff;
    tunesetting.SatParam.eIQ_Mode           = pstDemodParam->SatParam.eIQ_Mode;
    tunesetting.SatParam.eCodeRate          = pstDemodParam->SatParam.eCodeRate;
    tunesetting.SatParam.u16SymbolRate      = pstDemodParam->SatParam.u16SymbolRate;
    tunesetting.SatParam.u8Polarity         = pstDemodParam->SatParam.u8Polarity;
    tunesetting.SatParam.u8SatID            = pstDemodParam->SatParam.u8SatID;
    tunesetting.u32Frequency                = pstDemodParam->u32Frequency;

    memcpy(&loaderPT.astTS[0].abDS[0], &tunesetting, sizeof(MS_FE_CARRIER_PARAM));
    memcpy(&loaderPT.astTS[1].abDS[0], &tunesetting, sizeof(MS_FE_CARRIER_PARAM));

    memcpy((&loaderPT.astTS[0].abDS[0]) + (sizeof(MS_FE_CARRIER_PARAM)) + 4, (PBYTE)stSATParam, sizeof(MS_SAT_PARAM));
    memcpy((&loaderPT.astTS[1].abDS[0]) + (sizeof(MS_FE_CARRIER_PARAM)) + 4, (PBYTE)stSATParam, sizeof(MS_SAT_PARAM));

    loaderPT.fEnableOtaDL    = FALSE;
    loaderPT.fFlashCorrupted = FALSE;

    loaderPT.astTS[0].wPID      = LE2BE(u16Pid);
    loaderPT.astTS[0].bTable_ID = u16TableId;
    loaderPT.astTS[0].wCRC16    = (WORD)LE2BE(MApp_DecoderMsg_CalCrc16(&loaderPT.astTS[0], sizeof(DL_TS_STRUCT) - sizeof(WORD), 0));

    loaderPT.astTS[1].wPID      = LE2BE(u16Pid);
    loaderPT.astTS[1].bTable_ID = u16TableId;
    loaderPT.astTS[1].wCRC16    = (WORD)LE2BE(MApp_DecoderMsg_CalCrc16(&loaderPT.astTS[1], sizeof(DL_TS_STRUCT) - sizeof(WORD), 0));

    loaderPT.wCRC16             = (WORD)LE2BE(MApp_DecoderMsg_CalCrc16(&loaderPT.fEnableOtaDL, (LDR_AREA_COUNT - 2), 0));

    NVM_DRV_Write((PBYTE)&loaderPT, LOADER_PT_INDEX, sizeof(LOADER_PT_STRUCT));
    NVM_DRV_Write((PBYTE)&loaderPT, LOADER_PT_INDEX2, sizeof(LOADER_PT_STRUCT));

    return TRUE;
}

#else
static BOOL _MApp_DecoderMsg_WriteLoaderpt(MS_FE_CARRIER_PARAM* pstDemodParam, UINT16 u16Pid, UINT16 u16TableId)
{
    LOADER_PT_STRUCT loaderPT;
    MS_FE_CARRIER_PARAM tunesetting;

    memset(&loaderPT, 0x00, sizeof(loaderPT));
    memset(&tunesetting, 0x00, sizeof(MS_FE_CARRIER_PARAM));

    // Preserve original loader partition
    // Modify check value
    // FIXME:
    if (NVM_MNG_ReadLoaderPt(&origLoaderPT[0]) == 1)
    {
        CCA_LOADERPT_DBG("Read NVRAM error \n");
        return FALSE;
    }

#if (defined(DVBC_STYLE))
    tunesetting.CabParam.eConstellation     = pstDemodParam->CabParam.eConstellation;    // CAB_QAM64
    tunesetting.CabParam.u16SymbolRate      = pstDemodParam->CabParam.u16SymbolRate;     // 6875
    tunesetting.CabParam.eIQMode            = pstDemodParam->CabParam.eIQMode;           // CAB_IQ_NORMAL
    tunesetting.CabParam.u8TapAssign        = pstDemodParam->CabParam.u8TapAssign;       // 1
    tunesetting.CabParam.u32FreqOffset      = pstDemodParam->CabParam.u32FreqOffset;     // 0
    tunesetting.CabParam.u8TuneFreqOffset   = pstDemodParam->CabParam.u8TuneFreqOffset;  // 0
#endif

#if (MS_DVB_TYPE_SEL == DVBT2)
    tunesetting.TerParam.eBandWidth         = pstDemodParam->TerParam.eBandWidth;
    tunesetting.TerParam.u8ScanType         = pstDemodParam->TerParam.u8ScanType;
    tunesetting.TerParam.eConstellation     = 2;
#endif
    tunesetting.u32Frequency                = pstDemodParam->u32Frequency;

    memcpy(&loaderPT.astTS[0].abDS[0], &tunesetting, sizeof(MS_FE_CARRIER_PARAM));
    memcpy(&loaderPT.astTS[1].abDS[0], &tunesetting, sizeof(MS_FE_CARRIER_PARAM));

    loaderPT.fEnableOtaDL    = FALSE;
    loaderPT.fFlashCorrupted = FALSE;

    loaderPT.astTS[0].wPID      = LE2BE(u16Pid);
    loaderPT.astTS[0].bTable_ID = u16TableId;
    loaderPT.astTS[0].wCRC16    = (WORD)LE2BE(MApp_DecoderMsg_CalCrc16(&loaderPT.astTS[0], sizeof(DL_TS_STRUCT) - sizeof(WORD), 0));

    loaderPT.astTS[1].wPID      = LE2BE(u16Pid);
    loaderPT.astTS[1].bTable_ID = u16TableId;
    loaderPT.astTS[1].wCRC16    = (WORD)LE2BE(MApp_DecoderMsg_CalCrc16(&loaderPT.astTS[1], sizeof(DL_TS_STRUCT) - sizeof(WORD), 0));

    loaderPT.wCRC16 = (WORD)LE2BE(MApp_DecoderMsg_CalCrc16(&loaderPT.fEnableOtaDL, (LDR_AREA_COUNT - 2), 0));

    NVM_DRV_Write((PBYTE)&loaderPT, LOADER_PT_INDEX, sizeof(LOADER_PT_STRUCT));

    return TRUE;
}
#endif

void WriteFlag_loaderpt(LOADER_PT_STRUCT _stLdrPt)
{
    BYTE pbBuf[LDR_AREA_COUNT];
    memset(pbBuf , 0x00, sizeof(pbBuf));
    int idx = 0;

    pbBuf[idx++] = _stLdrPt.fEnableOtaDL; //        BOOL   fEnableOtaDL; /* only for OTA_DOWNLOAD. */
    pbBuf[idx++] = _stLdrPt.fFlashCorrupted; // BOOL   fFlashCorrupted; /* for OTA_DOWNLOAD and OTA_FP_DOWNLOAD.
    pbBuf[idx++] = _stLdrPt.fSleepMode; //     BOOL   fSleepMode;
    pbBuf[idx++] = _stLdrPt.fTestMode; //     BYTE   fTestMode;
    pbBuf[idx++] = _stLdrPt.fResult & 0xFF; //     ULONG  fResult;
    pbBuf[idx++] = (_stLdrPt.fResult>>8) & 0xFF;; //     ULONG  fResult;
    pbBuf[idx++] = (_stLdrPt.fResult>>16) & 0xFF; //     ULONG  fResult;
    pbBuf[idx++] = (_stLdrPt.fResult>>24) & 0xFF; //     ULONG  fResult;
    pbBuf[idx++] = _stLdrPt.bMenuLang; //     BYTE   bMenuLang;
    pbBuf[idx++] = _stLdrPt.bBootType; //     BYTE   bBootType;
    idx++;    //    BYTE   bReserved;
    idx++;      //    BYTE   bReserved;

    WORD _crc = MApp_DecoderMsg_CalCrc16( (PVOID)(pbBuf), (LDR_AREA_COUNT - 2), 0 );

    pbBuf[idx++] = _crc >> 8;

    pbBuf[idx] = _crc;

    NVM_DRV_Write( (PBYTE )pbBuf, (LOADER_PT_INDEX + LDR_AREA_OFFSET), LDR_AREA_COUNT);
}


///////////////////////////////////////////////////////////////////////////////
///  private MApp_DecoderMsg_ProcessLoaderPt
///
///  @param [in]    bFlg  Flag to enable/disable OTA write
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
static BOOL _MApp_DecoderMsg_WriteOTAFlag(BOOL bFlg)
{
#if(LOADER_V3_2 == 1)
    BYTE  pBuf[LOADER_PT_SIZE];
    LOADER_PT_STRUCT _tmp_pt;

    if( NVM_MNG_ReadLoaderPt( &pBuf[0] ) == 0 )
    {
        NVM_MNG_ParseLoaderPt( &pBuf[0], &_tmp_pt );
        _tmp_pt.fEnableOtaDL = bFlg;
        WriteFlag_loaderpt(_tmp_pt);
    }
#else// loaderV5
    LoaderCoreAPI_SetDownloadFlag( LC_TRUE, OTA_DOWNLOAD);
#endif
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_RestoreLoaderpt
///  Verifies the integrity of the tuning parameter area within the Loader NVRAM Partition.
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///  Note: The app flow shall call MApp_DecoderMsg_RevertViewingStatus(&gstOrigParam)
///            to revert viewing status after restoration.
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_RestoreLoaderpt(void)
{
    _MApp_DecoderMsg_RestoreLoaderpt();
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_RestoreLoaderpt
///  Verifies the integrity of the tuning parameter area within the Loader NVRAM Partition.
///
///  @param [in]    pstDemodParam  Pointer to the struct of Demod parameters
///  @param [in]    u16Pid              PID
///  @param [in]    u16TableId          Table ID
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
#if ( (MS_DVB_TYPE_SEL == DVBS) || (MS_DVB_TYPE_SEL == DVBS2) )
BOOL MApp_DecoderMsg_WriteLoaderpt(MS_FE_CARRIER_PARAM* pstDemodParam, MS_SAT_PARAM *stSATParam, UINT16 u16Pid, UINT16 u16TableId)
{
    return _MApp_DecoderMsg_WriteLoaderpt(pstDemodParam, stSATParam, u16Pid, u16TableId);
}
#else
BOOL MApp_DecoderMsg_WriteLoaderpt(MS_FE_CARRIER_PARAM* pstDemodParam, UINT16 u16Pid, UINT16 u16TableId)
{
    return _MApp_DecoderMsg_WriteLoaderpt(pstDemodParam, u16Pid, u16TableId);
}
#endif
///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_RestoreLoaderpt
///  Verifies the integrity of the tuning parameter area within the Loader NVRAM Partition.
///
///  @param [in]
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_CheckLoaderPtTSParam(void)
{
    return _MApp_DecoderMsg_ChkTSParam(&stLdrPt);
}

///////////////////////////////////////////////////////////////////////////////
///  global MApp_DecoderMsg_ProcessLoaderPt
///
///  @param [in]    bFlg  Flag to enable/disable OTA write
///
///  @return BOOLEAN true if success, false if failed.
///
///  @author MStarSemi @date 2013/5/2
///////////////////////////////////////////////////////////////////////////////
BOOL MApp_DecoderMsg_WriteLoaderPtOTAFlag(BOOL bFlg)
{
    return _MApp_DecoderMsg_WriteOTAFlag(bFlg);
}

#undef _APP_CCA_DECODERMSGPAODERPT_C

