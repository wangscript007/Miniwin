/**
 * @file UniversalClient.h
 */
#ifndef UNIVERSALCLIENT_H__INCLUDED__
#define UNIVERSALCLIENT_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_Types.h"
#include "UniversalClient_Common_API.h"
#include "gxav_demux_propertytypes.h"

#define CCA_SECTOR_NUMBER					(8)
#define CCA_BLOCK_SIZE						(0x10000)

#define MAX_ECM					     	    (5)////dead code for compile

#define MAX_EMM_STREAM_NUM                  (2)
#define MAX_ECM_STREAM_NUM                  (5)
#define MAX_EMM_FLT_NUM                     (20)
#define MAX_ECM_FLT_NUM                     MAX_ECM_STREAM_NUM  // 1 to1 mapping 
#define MAX_ECM_COMPONENT_NUM               (5)
#define EMM_FLT_BUF_SIZE                    (0x2000)//(8192)//(4096)
#define ECM_FLT_BUF_SIZE                    (0x400)//256)


#define MAX_PRODUCT						    (500)
#define MAX_METADATA						(64)
#define MAX_SESSION							(64)

#define CSSN_DATA_SIZE						(8)
#define PRIVATE_DATA_SIZE					(16)
#define CSSK_DATA_SIZE						(16*4)
#define SESSION_DATA_SIZE					(16)
#define TMS_DATA_SIZE						(32)
#define NATIONALITY_DATA_SIZE				(3)
#define SERIALNUMBER_DATA_SIZE			    (4)
#define PERSONALIZED_DATA_SIZE              (44)

#define ECM_QUEUE_SIZE						(1024)
#define EMM_QUEUE_SIZE						(4096+8)


#define CCA_INVALID_PID	                0x1FFF
/// Descrambler null filter ID
#define DRV_DSCMB_FLT_NULL          	0xFFFFFFFF

#define STREAM_TYPE_MASK                (uc_stream_handle)0xFFFF0000

#define MYEMMSERVICECONTEXT             (void *)0x00010001
#define MYEMMMESSAGEPROCDATA            (void *)0x00020001
#define EMM_STREAM_HANDLE               (void *)0x00030001

#define MYECMSERVICECONTEXT             (void *)0x00040001
#define MYECMMESSAGEPROCDATA            (void *)0x00050001
#define ECM_STREAM_HANDLE               (void *)0x00060001

#define PVR_PLAYBACK_SERVICE_CONTEXT    (void *)0x00070001
#define PVR_PLAYBACK_SERVICE_PROC_DATA  (void *)0x00080001
#define PVR_PLAYBACK_SERVICE_HANDLE     (void *)0x00090001
#define PVR_PLAYBACK_STREAM_HANDLE		(void *)0x000A0001

#define PVR_RECORD_SERVICE_CONTEXT      (void *)0x000B0001
#define PVR_RECORD_SERVICE_PROC_DATA    (void *)0x000C0001
#define PVR_RECORD_SERVICE_HANDLE       (void *)0x000D0001
#define PVR_RECORD_STREAM_HANDLE		(void *)0x000E0001

typedef struct _uc_emm_st
{
	uc_service_handle		serviceHandle;
	uc_stream_handle		streamHandle[MAX_EMM_STREAM_NUM];
	uc_uint16				pid[MAX_EMM_STREAM_NUM];
	uc_connection_handle	connectionHandle[MAX_EMM_STREAM_NUM];
	uc_notify_callback		notifyCallback[MAX_EMM_STREAM_NUM];
    
	uc_filter_handle		filterHandle[MAX_EMM_FLT_NUM];
    uc_uint32		        filterToStream[MAX_EMM_FLT_NUM]; //filter belongs to which stream  one stream will open more than one filters
    uc_sint32				slotID[MAX_EMM_FLT_NUM];
	uc_byte				    filterBuffer[MAX_EMM_FLT_NUM][EMM_FLT_BUF_SIZE];
    uc_buffer_st            byteBufferStruct[MAX_EMM_FLT_NUM];    
	GxDemuxProperty_Slot    dmx_slot_prop[MAX_EMM_FLT_NUM]; 
	GxDemuxProperty_Filter  dmx_filter_prop[MAX_EMM_FLT_NUM]; 
} uc_emm_st;


typedef struct _uc_ecm_key_set_st
{

	uc_byte Odd_Key_Set_Status;
	uc_byte Even_Key_Set_Status;	

}uc_ecm_key_set_st;

typedef struct _uc_ecm_st
{
	uc_service_handle		serviceHandle;
	uc_stream_handle		streamHandle[MAX_ECM_STREAM_NUM];
	uc_uint16				pid[MAX_ECM_STREAM_NUM];
	uc_connection_handle	connectionHandle[MAX_ECM_STREAM_NUM];
	uc_notify_callback		notifyCallback[MAX_ECM_STREAM_NUM];
	uc_filter_handle		filterHandle[MAX_ECM_FLT_NUM];
	uc_sint32				slotID[MAX_EMM_FLT_NUM];
	uc_byte				    filterBuffer[MAX_ECM_FLT_NUM][ECM_FLT_BUF_SIZE];
    uc_buffer_st            byteBufferStruct[MAX_ECM_FLT_NUM];
	uc_uint16				componentPid[MAX_ECM_STREAM_NUM][MAX_ECM_COMPONENT_NUM];
	uc_uint32				dscmbHandle[MAX_ECM_STREAM_NUM][MAX_ECM_COMPONENT_NUM];
	uc_ecm_key_set_st		key_status[MAX_ECM_STREAM_NUM][MAX_ECM_COMPONENT_NUM];	
	uc_byte				    even_key[16];  // org is 8 but for cavid = 4, it needs to extend to 16 bytes
	uc_byte				    odd_key[16];   // org is 8 but for cavid = 4, it needs to extend to 16 bytes
	GxDemuxProperty_Slot    dmx_slot_prop[MAX_ECM_FLT_NUM]; 
	GxDemuxProperty_Filter  dmx_filter_prop[MAX_ECM_FLT_NUM]; 

} uc_ecm_st;

typedef struct _uc_pvr_session_st
{
	unsigned char			statusSession;
	uc_dk_protection		typeSession;
	unsigned char			strSession[SESSION_DATA_SIZE];

} uc_pvr_session_st;

typedef struct _uc_pvr_meta_st
{
	uc_uint32				pvrMetaIndex;
	uc_uint32				pvrMetaTime;
	unsigned long long		pvrMetaAddress;
	uc_buffer_st			pvrMetaData;

} uc_pvr_meta_st;

typedef struct _uc_pvr_st
{
	uc_service_handle		pvrPlaybackServiceHandle;
	uc_stream_handle		pvrPlaybackStreamHandle;
	uc_stream_handle		pvrRecordStreamHandle;

	uc_uint16				pvrRecordPid;
	uc_uint16				pvrPlaybackPid;

	uc_uint32				pvrRecordSessionCount;
	uc_pvr_session_st		pvrRecordSession[MAX_SESSION];

	uc_uint32				pvrPlaybackSessionCount;
	uc_pvr_session_st		pvrPlaybackSession[MAX_SESSION];

	uc_uint32				pvrRecordMetaCount;
	uc_pvr_meta_st		pvrRecordMeta[MAX_METADATA];

	uc_uint32				pvrPlaybackMetaCount;
	uc_pvr_meta_st		pvrPlaybackMeta[MAX_METADATA];

    uc_dk_protection    KeyProtection;

} uc_pvr_st;

typedef struct _uc_system_st
{
	uc_bool				setPrivateData;
	unsigned char			strPrivateData[PRIVATE_DATA_SIZE];

	uc_bool				setCSSK;
	uc_dk_protection		typeCSSK;
	unsigned char			strCSSK[CSSK_DATA_SIZE];

	uc_bool				setSecure;
	unsigned char			strCSSN[CSSN_DATA_SIZE];
	uc_uint32				rsaMode;
	uc_uint32				cwMode;
	uc_uint32				jtag;

	uc_bool				setBBCB;
	uc_byte				bManufactureCode;
	uc_byte				bHW_Version;
	uc_byte				bLDR_Version_Major;
	uc_byte				bLDR_Version_Minor;
	uc_byte				abSerialNumber[4/*SERIAL_NUMBER_SIZE*/];

	uc_bool				setEEPROM;
	unsigned short		u16Variant;
	unsigned short		u16SysID;
	unsigned short		u16KeyVer;
	unsigned short		u16SigVer;

	uc_bool				setSPCB;
	unsigned char			bLoad_sequence_number;

    uc_bool				setDeviceId;
	unsigned char	    strDeviceId[CSSN_DATA_SIZE];

} uc_system_st;

typedef struct _spi_uc_system_st
{
	uc_bool				setPrivateData;
	unsigned char			strPrivateData[PRIVATE_DATA_SIZE];

	uc_bool				setCSSK;
	uc_dk_protection		typeCSSK;
	unsigned char			strCSSK[CSSK_DATA_SIZE];
	uc_uint8				lenCSSK;	
	uc_bool				setSecure;
	unsigned char			strCSSN[CSSN_DATA_SIZE];
	uc_uint32				rsaMode;
	uc_uint32				cwMode;
	uc_uint32				jtag;
	uc_uint32				crypto;
    uc_uint32               modeIFCP;

    uc_dk_protection    KeyProtection;

    uc_bool				setDeviceId;
	unsigned char	    strDeviceId[CSSN_DATA_SIZE];

} spi_uc_system_st;

//yenjen.0428 {.

#include "UniversalClient_SPI.h" //put here for compiler issue

typedef struct
{ 
	uc_sc_status_type       scStatus;
    uc_byte                 uProtocol;
	uc_byte                 smc_cid;
    uc_smartcard_handle     smartcardHandle;
    uc_sc_status_notify_fn  pfnNotify;
} UCSPI_SC_INFO;

extern UCSPI_SC_INFO Sc_Info;  


//yenjen.0428 .}



extern uc_emm_st    gEMMStreamInfo;
extern uc_ecm_st    gECMStreamInfo;
#if(CA_CCA_PVR_SUP == 1)
extern uc_pvr_st    gPVRStreamInfo;
#endif
extern uc_system_st    g_ucSystem;
extern spi_uc_system_st    s_spi_ucSystem;

extern uc_buffer_st		g_ucCatBuff;
extern uc_buffer_st		g_ucPmtBuff;



#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_H__INCLUDED__ */
