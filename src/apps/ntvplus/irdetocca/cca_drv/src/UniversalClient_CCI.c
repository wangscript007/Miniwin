/**
 * @file UniversalClient_SPI.c
 */

//////////////////////////////////////////////////////////////////////////////////////////
//	include header file
//////////////////////////////////////////////////////////////////////////////////////////
#if 0
#include "MsCommon.h"
#include "HbCommon.h"

#include "UniversalClient_API.h"
#include "UniversalClient_Common_API.h"
#include "UniversalClient_SPI.h"
//yenjen.0424 #include "UniversalClient_Core_SPI.h"
//yenjen.0424 #include "UniversalClient_MS.h"
//yenjen.0424 #include "UniversalClient.h"

//yenjen.0424 #include "i_hlsup.h"
#include "drvSERFLASH.h"
#include "drvCA.h"
#include "drvTVEncoder.h"
#include "drvDSCMB.h"
#include "apiDMX.h"
#include "apiHDMITx.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Definition
//////////////////////////////////////////////////////////////////////////////////////////
#define MSTAR_MV_SETTING					(8)
#define MV_CFG_LEN							(17)


//////////////////////////////////////////////////////////////////////////////////////////
// Structure
//////////////////////////////////////////////////////////////////////////////////////////
/* macro vision */
typedef enum
{
	MS_VE_MV_NTSC_START = 0,
	MS_VE_MV_NTSC_TYPE1 = MS_VE_MV_NTSC_START,
	MS_VE_MV_NTSC_TYPE2,
	MS_VE_MV_NTSC_TYPE3,
	MS_VE_MV_NTSC_TEST_N01,
	MS_VE_MV_NTSC_TEST_N02,
	MS_VE_MV_PAL_START = 0x10,
	MS_VE_MV_PAL_TYPE1 = MS_VE_MV_PAL_START,
	MS_VE_MV_PAL_TYPE2,
	MS_VE_MV_PAL_TYPE3,
	MS_VE_MV_PAL_TEST_P01,
	MS_VE_MV_PAL_TEST_P02,

} MS_VE_MV_TYPE;

typedef enum
{
    MS_WSS_4x3_FULL                    = 0x08,
    MS_WSS_14x9_LETTERBOX_CENTER       = 0x01,
    MS_WSS_14x9_LETTERBOX_TOP          = 0x02,
    MS_WSS_16x9_LETTERBOX_CENTER       = 0x0B,
    MS_WSS_16x9_LETTERBOX_TOP          = 0x04,
    MS_WSS_ABOVE16x9_LETTERBOX_CENTER  = 0x0D,
    MS_WSS_14x9_FULL_CENTER            = 0x0E,
    MS_WSS_16x9_ANAMORPHIC             = 0x07,

    MS_WSS_625_COPYRIGHT               = 0x1000,
    MS_WSS_625_COPY_PROTECTION         = 0x2000

} MS_VE_WSS_TYPE;

typedef struct
{
	unsigned char			mvVersion;
	unsigned int			mvLength;
	unsigned char			mvMode;
	unsigned char			mvData[MV_CFG_LEN];

	unsigned char			cciProtocolVersion;
	unsigned char			cciApsInfo;
	unsigned char			cciEmiInfo;
	unsigned char			cciIctInfo;
	unsigned char			cciRtcInfo;
	unsigned char			cciRlInfo;

} uc_macrovision_st;


//////////////////////////////////////////////////////////////////////////////////////////
// Local Variable
//////////////////////////////////////////////////////////////////////////////////////////
uc_macrovision_st			g_ucCCI;

static const uc_byte MV_Type[MSTAR_MV_SETTING] =
{
	MS_VE_MV_NTSC_TYPE1,MS_VE_MV_NTSC_TYPE2,
	MS_VE_MV_NTSC_TYPE3,MS_VE_MV_NTSC_TEST_N01,
	MS_VE_MV_NTSC_TEST_N02,MS_VE_MV_PAL_TYPE1,
	MS_VE_MV_PAL_TEST_P01,MS_VE_MV_PAL_TEST_P02
};

static const uc_byte MACROVISION_MODE[MSTAR_MV_SETTING] =  {0x63, 0xE3, 0xE3, 0xE3, 0xE3, 0x63, 0x3E, 0x3E};

//According to Macrovision required, we have 8 setting value. CPS[0:33]
static const uc_byte MACROVISION_CFG[MSTAR_MV_SETTING][MV_CFG_LEN] =
{
	{0x0F,0xFC,0x20,0xD0,0x6F,0x0F,0x00,0x00,0x0C,0xF3,0x09, 0xBD,0x6C,0x31,0x92,0x32,0xDD},	//MACROVISION_TYPE_MACROVISION_NTSC_TYPE1
	{0x0F,0xFC,0x20,0xD0,0x6F,0x0F,0x00,0x00,0x0C,0xF3,0x09, 0xBD,0x6C,0x31,0x92,0x32,0xDD},	//MACROVISION_TYPE_MACROVISION_NTSC_TYPE2
	{0x0F,0xFC,0x20,0xD0,0x6F,0x0F,0x00,0x00,0x0C,0xF3,0x09, 0xBD,0x66,0xB5,0x90,0xB2,0x7D},	//MACROVISION_TYPE_MACROVISION_NTSC_TYPE3
	{0x00,0xFA,0x9F,0xA1,0x9E,0x0F,0x07,0x0E,0x7C,0xF7,0xC8, 0xC5,0x67,0xB5,0xD0,0xB2,0x7D},	//MACROVISION_TYPE_MACROVISION_NTSC_TEST_N01
	{0x00,0xF5,0xD1,0x10,0x73,0x13,0x2F,0xCD,0x6C,0x5B,0x57, 0x52,0x93,0x56,0x7D,0x45,0xF2},	//MACROVISION_TYPE_MACROVISION_NTSC_TEST_N02
	{0x05,0x57,0x20,0x40,0x6E,0x7E,0xF4,0x51,0x0F,0xF1,0x05, 0xD3,0x78,0xA2,0x25,0x54,0xA5},	//MACROVISION_TYPE_MACROVISION_PAL_TYPE1_2_3
	{0x05,0x57,0x20,0x40,0x6E,0x7E,0xF4,0x51,0x0F,0xF1,0x05, 0xD3,0x78,0xA2,0x65,0x54,0xA5},	//MACROVISION_TYPE_MACROVISION_PAL_TEST_P01
	{0x01,0xEA,0x1E,0xB0,0xF3,0xA3,0x53,0x4F,0x13,0x6C,0xFA, 0x2D,0x8B,0xA2,0x65,0x54,0xA5}		//MACROVISION_TYPE_MACROVISION_PAL_TEST_P02
};


//////////////////////////////////////////////////////////////////////////////////////////
//	global variable
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
//	external function
//////////////////////////////////////////////////////////////////////////////////////////
extern VE_Result MDrv_VE_SetMV(MS_BOOL bEnble, MS_VE_MV_TYPE eMvType);


//////////////////////////////////////////////////////////////////////////////////////////
//	Local Function
//////////////////////////////////////////////////////////////////////////////////////////
uc_result _COPYTCONTROL_Chk_MV_Mode(void)
{
	int		k;
	uc_byte	mode;

	mode = g_ucCCI.mvMode;
	for(k=0;k<MSTAR_MV_SETTING;k++)
	{
		if(MACROVISION_MODE[k] == mode)
			return UC_ERROR_SUCCESS;
	}

	return UC_ERROR_NULL_PARAM;
}

uc_result _COPYTCONTROL_Set_MV(void)
{
	int			k,i;
	uc_result		ret = UC_ERROR_NULL_PARAM;

	for(k=0; k<MSTAR_MV_SETTING; k++)
	{
		for(i=0; i<MV_CFG_LEN; i++)
		{
			#if 0
		       printf("_COPYTCONTROL_Set_MV    %x ==%x\n",g_ucCCI.mvData[i],MACROVISION_CFG[k][i]);
			#endif

			if((k==0)&&(i==5))
				if((g_ucCCI.mvData[i]==0x6f)||(g_ucCCI.mvData[i]==0x4f))
					continue;
			if((k==0)&&(i==13))
				if((g_ucCCI.mvData[i]==0x6C)||(g_ucCCI.mvData[i]==0x66))
					continue;
			if((k==0)&&(i==14))
				if((g_ucCCI.mvData[i]==0x31)||(g_ucCCI.mvData[i]==0x35)||(g_ucCCI.mvData[i]==0xB1)||(g_ucCCI.mvData[i]==0xB5))
					continue;
			if((k==0)&&(i==15))
				if((g_ucCCI.mvData[i]==0x92)||(g_ucCCI.mvData[i]==0x90))
					continue;
			if((k==0)&&(i==16))
				if((g_ucCCI.mvData[i]==0x32)||(g_ucCCI.mvData[i]==0xB2))
					continue;
			if((k==0)&&(i==17))
				if((g_ucCCI.mvData[i]==0xDD)||(g_ucCCI.mvData[i]==0x7D))
					continue;
			if((k==2)&&(i==4))
				if((g_ucCCI.mvData[i]==0x6f)||(g_ucCCI.mvData[i]==0x5f))
					continue;
			if((k==5)&&(i==4))
				if((g_ucCCI.mvData[i]==0x4E)||(g_ucCCI.mvData[i]==0x5E)||(g_ucCCI.mvData[i]==0x6E))
					continue;
			if(g_ucCCI.mvData[i]!=MACROVISION_CFG[k][i])
				break;
		}

		#if 0
              printf("i=%d k=%d g_ucCCI.mvMode=%x MACROVISION_MODE[k]=%x\n",i,k,g_ucCCI.mvMode,MACROVISION_MODE[k]);
		#endif

		if(i == MV_CFG_LEN)
		{
			if((g_ucCCI.mvMode) == 1)
			{
				#if 0
			       printf("MV_Type==%d\n",MV_Type[k]);
				#endif

				MDrv_VE_SetMV(TRUE,MV_Type[k]);

				return UC_ERROR_SUCCESS;
			}

			ret = UC_ERROR_SUCCESS;
		}
	}

	return ret;
}

uc_result  UniversalClientSPI_CopyControl_Digital_SetCCI( uc_copycontrol_cci CopyControlInfo)
{
	#if 0
	printf("[START] UniversalClientSPI_CopyControl_Digital_SetCCI\n");
	#endif

	if(CopyControlInfo == UC_COPYCONTROL_COPYFREELY)
	{
		#if 0
		printf ("==MDrv_HDCP_Enable Disable==\n");
		#endif

		MApi_HDMITx_SetHDMITxMode(HDMITX_HDMI_HDCP);
		MsOS_DelayTask(10);
		MApi_HDMITx_SetHDCPOnOff(FALSE);
		MsOS_DelayTask(10);
		MApi_HDMITx_Exhibit();
		MsOS_DelayTask(10);
	}
	else if((CopyControlInfo == UC_COPYCONTROL_NOFURTHERCOPY)
		||(CopyControlInfo==UC_COPYCONTROL_COPYONCE)
		||(CopyControlInfo==UC_COPYCONTROL_COPYNEVER))
	{
		#if 0
		printf ("==MDrv_HDCP_Enable Enable==\n");
		#endif

		MApi_HDMITx_SetHDMITxMode(HDMITX_HDMI_HDCP);
		MsOS_DelayTask(10);
		MApi_HDMITx_SetHDCPOnOff(TRUE);
		MsOS_DelayTask(10);
		MApi_HDMITx_Exhibit();
		MsOS_DelayTask(10);
	}
	else
	{
		return UC_ERROR_NULL_PARAM;
	}

	#if 0
	printf("[END] UniversalClientSPI_CopyControl_Digital_SetCCI\n");
	#endif

	return UC_ERROR_SUCCESS;
}

uc_result  UniversalClientSPI_CopyControl_Analog_SetCCI(uc_copycontrol_cci CopyControlInfo)
{
	MS_U16 u16Wss = 0x0;

	u16Wss = MDrv_VE_GetWSSData();

	#if 0
	printf("[START] UniversalClientSPI_CopyControl_Analog_SetCCI\n");
	#endif

	if(CopyControlInfo==UC_COPYCONTROL_COPYFREELY)
	{
		#if 0
		printf ("==MDrv_VE_SetWSS525Data Disable==\n");
		#endif

		u16Wss = u16Wss & ~(MS_WSS_625_COPYRIGHT) & ~(MS_WSS_625_COPY_PROTECTION);
	}
	else if(CopyControlInfo==UC_COPYCONTROL_NOFURTHERCOPY)
	{
		#if 0
		printf ("==MDrv_VE_SetWSS525Data Enable==\n");
		#endif

		u16Wss = (u16Wss & ~(MS_WSS_625_COPYRIGHT)) | MS_WSS_625_COPY_PROTECTION;
	}
	else if(CopyControlInfo==UC_COPYCONTROL_COPYONCE)
	{
		#if 0
		printf ("==MDrv_VE_SetWSS525Data Enable==\n");
		#endif

		u16Wss = (u16Wss | MS_WSS_625_COPYRIGHT) & ~(MS_WSS_625_COPY_PROTECTION);
	}
	else if(CopyControlInfo==UC_COPYCONTROL_COPYNEVER)
	{
		#if 0
		printf ("==MDrv_VE_SetWSS525Data Enable==\n");
		#endif

		u16Wss = u16Wss | MS_WSS_625_COPYRIGHT | MS_WSS_625_COPY_PROTECTION;
	}
	else
	{
		return UC_ERROR_NULL_PARAM;
	}

       MDrv_VE_SetWSSData(TRUE, u16Wss);

	#if 0
	printf("[END] UniversalClientSPI_CopyControl_Analog_SetCCI %x\n",u16Wss);
	#endif

	return UC_ERROR_SUCCESS;
}


uc_result UniversalClient_CCI_Macrovision(unsigned char ucAction)
{
	uc_result		ret = UC_ERROR_NULL_PARAM;

	if(ucAction == 1)
	{
	    ret =_COPYTCONTROL_Set_MV();
	}
	else if(ucAction == 2)
	{
		ret = _COPYTCONTROL_Chk_MV_Mode();
		if(ret == UC_ERROR_SUCCESS)
		{
			ret = _COPYTCONTROL_Set_MV();
		}
	}

	return ret;
}
#endif
