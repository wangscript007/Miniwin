/********************************************************************
Author:Richard.Jian
Data:Aug. 8th,2008
Filename:qpsk.c
Description:imply Tuner's interface
Note:please call the functions in APPLICATION,DON'T directly call API Driver  fucntions
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_tuner.h>
#include <diseqc.h>

#define ISVALIDINDEX(index) ((int)(index) != -1)

#define FORMATDISEQCMD(body, frame, addr, cmd)	\
	body[TUNER_DISEQC_FRAME_BYTE_OFFSET] = (BYTE)(frame);\
	body[TUNER_DISEQC_SLAVE_ADDR_OFFSET] = (BYTE)(addr);\
       body[TUNER_DISEQC_CMD_BYTE_OFFSET] = (BYTE)(cmd);



#define TUNER_DISEQC_FRAME_BYTE_OFFSET            				0
#define TUNER_DISEQC_SLAVE_ADDR_OFFSET             				1
#define TUNER_DISEQC_CMD_BYTE_OFFSET       						2
#define TUNER_DISEQC_DATA_BYTE_0_OFFSET            				3
#define TUNER_DISEQC_DATA_BYTE_1_OFFSET            				4
#define TUNER_DISEQC_DATA_BYTE_2_OFFSET            				5
#define TUNER_DISEQC_CMD_LENGTH								5

#define DISEQC_USALS_G15_CONS	(double)3561680000.0
#define DISEQC_USALS_G17_CONS	(double)40576900.0
#define DISEQC_USALS_CONS_1	(double)6370.0
#define DISEQC_USALS_CONS_2	(double)1821416900.0
#define DISEQC_USALS_CONS_3	(double)537628000.0
#define DISEQC_USALS_CONS_4	(double)180.0
#define PI				    ((double)(22.0/7.0))

typedef enum {
    TUNER_DISEQC_FRAMING_FIRST = 0xE0,	/* the command is a simple diseqc command              */
    TUNER_DISEQC_FRAMING_REPEAT = 0XE1	/* there are cacscaded switch, the command id repeated */
} TUNER_DISEQC_FRAMING_ENUM;


typedef enum {
    TUNER_DISEQC_CMD_RESET = 0x00,	/* command for DISEQC 1.0, reset of all equipments    */
    TUNER_DISEQC_CMD_STANDBY = 0x02,	/* command for DISEQC 1.0, reset of all message sent  */
    TUNER_DISEQC_CMD_POWERON = 0x03,	/* command for DISEQC 1.0, init all equipments        */
    TUNER_DISEQC_CMD_SETPOSA = 0x22,	/* command for DISEQC 1.0, select the switch pos A    */
    TUNER_DISEQC_CMD_SETPOSB = 0x26,	/* command for DISEQC 1.0, select the switch pos B    */
    TUNER_DISEQC_CMD_SETSOA = 0x23,	/* command for DISEQC 1.0, select the switch option A, used for cascaded switch */
    TUNER_DISEQC_CMD_SETSOB = 0x27,	/* command for DISEQC 1.0, select the switch option B, used for cascaded switch */
    TUNER_DISEQC_CMD_SET_S1_IA = 0x28,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S2_IA = 0x29,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S3_IA = 0x2A,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S4_IA = 0x2B,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S1_IB = 0x2C,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S2_IB = 0x2D,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S3_IB = 0x2E,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_SET_S4_IB = 0x2F,	/* command for DISEQC 1.1, SMATV command              */
    TUNER_DISEQC_CMD_WRITE_N0 = 0x38,	/* command for DISEQC 1.0, write in DISEQC register   */
    TUNER_DISEQC_CMD_WRITE_N1 = 0x39,	/* command for DISEQC 1.0, write in DISEQC register   */
    TUNER_DISEQC_CMD_HALT = 0x60,	/* command for DISEQC 1.2, stop the motor        */
    TUNER_DISEQC_CMD_LIMITS_OFF = 0x63,	/* command for DISEQC 1.2, no limits for motor   */
    TUNER_DISEQC_CMD_LIMITS_EAST = 0x66,	/* command for DISEQC 1.2, set the current position as east limit */
    TUNER_DISEQC_CMD_LIMITS_WEST = 0x67,	/* command for DISEQC 1.2, set the current position as west limit */
    TUNER_DISEQC_CMD_DRIVE_EAST = 0x68,	/* command for DISEQC 1.2, motor drive to east */
    TUNER_DISEQC_CMD_DRIVE_WEST = 0x69,	/* command for DISEQC 1.2, motor drive to west */
    TUNER_DISEQC_CMD_STORE_POSITION = 0x6A,	/* command for DISEQC 1.2, store the current position as satellite position */
    TUNER_DISEQC_CMD_DRIVE_POSITION = 0x6B,	/* command for DISEQC 1.2, motor drive to a stored position */
    TUNER_DISEQC_CMD_RECALCULATE = 0x6F,	/* command for DISEQC 1.2, recalculate the satellite positions */
    TUNER_DISEQC_CMD_GOTO_ANGULAR = 0x6E	/* command for DISEQC 1.2, recalculate the satellite positions */
} TUNER_DISEQC_COMMAND_ENUM;

typedef struct {
    union {
	struct {
	    BYTE    bCmd0;
	    BYTE    bCmd1;
	    BYTE    bUnused0;
	    BYTE    bUnused1;
	} MotorCmd;
	struct {
	    BYTE    bTimeoutOrStep;
	    BYTE    bType;
	    BYTE    bUnused0;
	    BYTE    bUnused1;
	} MotorDriverType;
	BYTE    bStorePostion;
	BYTE    bGoToPostion;
	BYTE    bReferencePostion;

    } Body;
} TUNER_DISEQC_MOTOR_BODY_STRUCT;

typedef struct {
    TUNER_TP_POLARIZATION_ENUM ePolarization;
    TUNER_TONE_ENUM e22K;
    TUNER_MOTOR_ENUM eMotor;
    TUNER_DISEQC10_ENUM eDiSEqC10;
    TUNER_DISEQC11_ENUM eDiSEqC11;
    USHORT  usMotorPosition;	//Sat Index for 1.2, Angel for USAL(1.3)
} TUNER_DISEQC_BODY_STRUCT, *PTUNER_DISEQC_BODY_STRUCT;



//LOCAL BYTE   s_pbVersion[] = "Diseqc-Ver1.0.0";
#ifndef DOUBLE
#define DOUBLE double
#endif

BYTE diseqc_get_version(VOID)
{
    return 1;			//(s_pbVersion);
}

// 1,成功，0，失败
BOOL diseqc_send_diseqc_command(int eIndexOfTuner, BYTE * pcCommand, BYTE iLengthCommand)
{
    BOOL    tRet = FALSE;
    USHORT  i = 0;

    if (ISVALIDINDEX(eIndexOfTuner) && pcCommand) {
	tRet = NGL_OK == nglSendDiseqcCommand(eIndexOfTuner, pcCommand, iLengthCommand, 1);	// Diseqc version 1.2
    }
    return (tRet);
}


BOOL diseqc_format_diseqc10_command(TUNER_DISEQC_COMMAND_ENUM eCommand, BYTE * pbCommand,
				    PTUNER_DISEQC_BODY_STRUCT pstDiseqcCommand)
{
    BOOL    tRet = FALSE;

    if (pbCommand && pstDiseqcCommand) {
	FORMATDISEQCMD(pbCommand, TUNER_DISEQC_FRAMING_FIRST, 0x10, eCommand);
	switch (eCommand) {
	case TUNER_DISEQC_CMD_RESET:
	case TUNER_DISEQC_CMD_STANDBY:
	case TUNER_DISEQC_CMD_POWERON:
	case TUNER_DISEQC_CMD_SETPOSA:
	case TUNER_DISEQC_CMD_SETPOSB:
	case TUNER_DISEQC_CMD_SETSOA:
	case TUNER_DISEQC_CMD_SETSOB:
	    tRet = TRUE;
	    break;
	case TUNER_DISEQC_CMD_WRITE_N0:
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] = 0xF0;
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] = 0xf0 | ((((pstDiseqcCommand->eDiSEqC10 - 1) * 4) & 0x0f)
								 | ((pstDiseqcCommand->ePolarization == TUNER_POL_VERTICAL) ? 0
								    : 2)
								 | ((pstDiseqcCommand->e22K == TUNER_TONE_22K_ON) ? 1 : 0));
	    tRet = TRUE;
	    break;
	default:
	    break;
	}
    }
    return (tRet);
}

BOOL diseqc_format_diseqc11_uncommit_command(TUNER_DISEQC_COMMAND_ENUM eCommand, BYTE * pbCommand,
					     PTUNER_DISEQC_BODY_STRUCT pstDiseqcCommand)
{
    BOOL    tRet = FALSE;

    if (pbCommand && pstDiseqcCommand) {
	FORMATDISEQCMD(pbCommand, TUNER_DISEQC_FRAMING_FIRST, 0x10, eCommand);
	switch (eCommand) {
	case TUNER_DISEQC_CMD_RESET:
	case TUNER_DISEQC_CMD_STANDBY:
	case TUNER_DISEQC_CMD_POWERON:
	case TUNER_DISEQC_CMD_SETPOSA:
	case TUNER_DISEQC_CMD_SETPOSB:
	case TUNER_DISEQC_CMD_SETSOA:
	case TUNER_DISEQC_CMD_SETSOB:
	    tRet = TRUE;
	    break;
	case TUNER_DISEQC_CMD_WRITE_N0:
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] = 0xF0;
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] = 0xF0 | ((((pstDiseqcCommand->eDiSEqC11 - 1) * 4) & 0x0F)
								 | ((pstDiseqcCommand->ePolarization == TUNER_POL_VERTICAL) ? 0
								    : 2)
								 | ((pstDiseqcCommand->e22K == TUNER_TONE_22K_ON) ? 1 : 0));
	    tRet = TRUE;
	    break;
	default:
	    break;
	}
    }
    return (tRet);
}

BOOL diseqc_format_diseqc11_commit_command(TUNER_DISEQC_COMMAND_ENUM eCommand, BYTE * pbCommand,
					   PTUNER_DISEQC_BODY_STRUCT pstDiseqcCommand)
{
    BOOL    tRet = FALSE;

    if (pbCommand && pstDiseqcCommand) {
	FORMATDISEQCMD(pbCommand, TUNER_DISEQC_FRAMING_FIRST, 0x10, eCommand);
	switch (eCommand) {
	case TUNER_DISEQC_CMD_RESET:
	case TUNER_DISEQC_CMD_STANDBY:
	case TUNER_DISEQC_CMD_POWERON:
	case TUNER_DISEQC_CMD_SETPOSA:
	case TUNER_DISEQC_CMD_SETPOSB:
	case TUNER_DISEQC_CMD_SETSOA:
	case TUNER_DISEQC_CMD_SETSOB:
	    tRet = TRUE;
	    break;
	case TUNER_DISEQC_CMD_WRITE_N1:
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] = ((pstDiseqcCommand->eDiSEqC11 - 1) & 0x0f) | 0xf0;
	    tRet = TRUE;
	    break;
	default:
	    break;
	}
    }
    return (tRet);
}


BOOL diseqc_format_diseqc12_commit_command(TUNER_DISEQC_COMMAND_ENUM eCommand, BYTE * pbCommand,
					   TUNER_DISEQC_MOTOR_BODY_STRUCT Option)
{
    BOOL    tRet = TRUE;

    if (pbCommand) {
	FORMATDISEQCMD(pbCommand, TUNER_DISEQC_FRAMING_FIRST, 0x31, eCommand);
	switch (eCommand) {
	case TUNER_DISEQC_CMD_HALT:
	case TUNER_DISEQC_CMD_LIMITS_OFF:
	case TUNER_DISEQC_CMD_LIMITS_EAST:
	case TUNER_DISEQC_CMD_LIMITS_WEST:
	    break;
	case TUNER_DISEQC_CMD_DRIVE_EAST:
	case TUNER_DISEQC_CMD_DRIVE_WEST:
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] =
		0 == Option.Body.MotorCmd.bCmd1 ? Option.Body.MotorCmd.bCmd0 : (0xff - (Option.Body.MotorCmd.bCmd0 - 1));
	    break;
	case TUNER_DISEQC_CMD_GOTO_ANGULAR:
	    pbCommand[TUNER_DISEQC_DATA_BYTE_1_OFFSET] = Option.Body.MotorCmd.bCmd1;
	case TUNER_DISEQC_CMD_STORE_POSITION:
	case TUNER_DISEQC_CMD_DRIVE_POSITION:
	case TUNER_DISEQC_CMD_RECALCULATE:
	    pbCommand[TUNER_DISEQC_DATA_BYTE_0_OFFSET] = Option.Body.MotorCmd.bCmd0;
	    break;
	default:
	    tRet = FALSE;
	    break;
	}
    } else {
	tRet = FALSE;
    }
    return (tRet);
}

#if 1				//Alan add 2018/02/06
static BYTE DeseqcCommand[TUNER_DISEQC_CMD_LENGTH];
static BYTE DesCmdLen = 0;
static BYTE IndexOfTuner = 0;

void api_get_DeseqcSetInfo(BYTE * tunerid, BYTE ** cmd, BYTE * len)
{
    *tunerid = IndexOfTuner;
    *cmd = DeseqcCommand;
    *len = DesCmdLen;
}

void api_Set_DeseqcSetInfo(BYTE tunerid, BYTE * cmd, BYTE len)
{
    IndexOfTuner = tunerid;
    memcpy(DeseqcCommand, cmd, len);
    DesCmdLen = len;
}


#endif

BOOL diseqc_set_diseqc10(int eIndexOfTuner, TUNER_DISEQC10_ENUM eDiSEqC10, TUNER_TP_POLARIZATION_ENUM ePolar,
			 TUNER_TONE_ENUM e22K)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_BODY_STRUCT stDiseqcCommand;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	switch (eDiSEqC10) {
	case TUNER_DISEQC10_PORT_NONE:
	    tRet = TRUE;
	    break;
	case TUNER_DISEQC10_PORT_A:
	case TUNER_DISEQC10_PORT_B:
	case TUNER_DISEQC10_PORT_C:
	case TUNER_DISEQC10_PORT_D:
	    memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	    stDiseqcCommand.eDiSEqC10 = eDiSEqC10;
	    stDiseqcCommand.ePolarization = ePolar;
	    stDiseqcCommand.e22K = e22K;
	    tRet = diseqc_format_diseqc10_command(TUNER_DISEQC_CMD_WRITE_N0, pbCommand, &stDiseqcCommand);
	    if (tRet) {
		tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
		tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
		api_Set_DeseqcSetInfo(eIndexOfTuner, pbCommand, 4);
	    }
	    break;
	default:
	    break;
	}
    }
    return (tRet);
}

BOOL diseqc_set_diseqc11(int eIndexOfTuner, TUNER_DISEQC11_ENUM eDiSEqC11, TUNER_TP_POLARIZATION_ENUM ePolar,
			 TUNER_TONE_ENUM e22K)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_BODY_STRUCT stDiseqcCommand;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	if (TUNER_DISEQC11_PORT_NONE < eDiSEqC11 && TUNER_DISEQC11_PORT_MAX > eDiSEqC11) {
	    memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	    stDiseqcCommand.eDiSEqC11 = eDiSEqC11;
	    tRet = diseqc_format_diseqc11_commit_command(TUNER_DISEQC_CMD_WRITE_N1, pbCommand, &stDiseqcCommand);
	    if (tRet) {
		tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
	    }
	    nglSleep(100);	//OSTaskDelay(100);
	    memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	    stDiseqcCommand.ePolarization = ePolar;
	    stDiseqcCommand.e22K = e22K;
	    tRet = diseqc_format_diseqc11_uncommit_command(TUNER_DISEQC_CMD_WRITE_N0, pbCommand, &stDiseqcCommand);
	    if (tRet) {
		tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
	    }
	}
    }
    return (tRet);
}

BOOL diseqc_set_motor(int eIndexOfTuner, TUNER_MOTOR_ENUM eMotor, USHORT usMotorPosition)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];

    if (ISVALIDINDEX(eIndexOfTuner)) {
	switch (eMotor) {
	case TUNER_MOTOR_DISEQC12:
	case TUNER_MOTOR_USALS:
	    memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	    if (TUNER_MOTOR_DISEQC12 == eMotor) {
		tRet = diseqc_jump_to_motor_position(eIndexOfTuner, usMotorPosition);
		nglSleep(200);	//OSTaskDelay(200);
		tRet = diseqc_jump_to_motor_position(eIndexOfTuner, usMotorPosition);
	    } else if (TUNER_MOTOR_USALS == eMotor) {
		tRet = diseqc_rotate_motor_by_angular(eIndexOfTuner, usMotorPosition);
		nglSleep(200);	//OSTaskDelay(200);
		tRet = diseqc_rotate_motor_by_angular(eIndexOfTuner, usMotorPosition);
	    }
	    break;
	default:
	    break;
	}
    }
    return (tRet);
}



BOOL diseqc_stop_motor(int eIndexOfTuner)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	tRet = diseqc_format_diseqc12_commit_command(TUNER_DISEQC_CMD_HALT, pbCommand, stOption);
	if (tRet) {
	    tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 3);
	}
    }
    return (tRet);
}

BOOL diseqc_set_motor_limit(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection)
{
    BOOL    tRet = TRUE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;
    TUNER_DISEQC_COMMAND_ENUM eCommand;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	switch (eDirection) {
	case TUNER_DISEQC_EAST:
	    eCommand = TUNER_DISEQC_CMD_LIMITS_EAST;
	    break;
	case TUNER_DISEQC_WEST:
	    eCommand = TUNER_DISEQC_CMD_LIMITS_WEST;
	    break;
	case TUNER_DISEQC_DISABLE:
	    eCommand = TUNER_DISEQC_CMD_LIMITS_OFF;
	    break;
	default:
	    tRet = FALSE;
	    break;
	}
	if (tRet) {
	    memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	    tRet = diseqc_format_diseqc12_commit_command(eCommand, pbCommand, stOption);
	    if (tRet) {
		tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 3);
	    }
	}
    } else {
	tRet = FALSE;
    }
    return (tRet);
}

BOOL diseqc_rotate_motor(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection, TUNER_DISEQC_MOTOR_BODY_STRUCT stOption)
{
    BOOL    tRet = TRUE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_COMMAND_ENUM eCommand;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	switch (eDirection) {
	case TUNER_DISEQC_EAST:
	    eCommand = TUNER_DISEQC_CMD_DRIVE_EAST;
	    break;
	case TUNER_DISEQC_WEST:
	    eCommand = TUNER_DISEQC_CMD_DRIVE_WEST;
	    break;
	case TUNER_DISEQC_DISABLE:
	    tRet = FALSE;
	    break;
	}
	if (tRet) {
	    memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	    tRet = diseqc_format_diseqc12_commit_command(eCommand, pbCommand, stOption);
	    if (tRet) {
		tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
		//      printf("pbcommand = 0x%x  0x%x   0x%x   0x%x  0x%x ----\n",pbCommand[0],pbCommand[1],pbCommand[2],pbCommand[3],pbCommand[4]);
	    }
	}
    } else {
	tRet = FALSE;
    }
    return (tRet);
}

BOOL diseqc_rotate_motor_by_step(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection, BYTE bStep)
{
    BOOL    tRet = TRUE;
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;

    stOption.Body.MotorCmd.bCmd1 = 1;
    stOption.Body.MotorCmd.bCmd0 = bStep;
    tRet = diseqc_rotate_motor(eIndexOfTuner, eDirection, stOption);
    return (tRet);
}

BOOL diseqc_rotate_motor_by_timeout(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection, BYTE bTimeout)
{
    BOOL    tRet = TRUE;
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;

    stOption.Body.MotorCmd.bCmd1 = 0;
    stOption.Body.MotorCmd.bCmd0 = bTimeout;
    tRet = diseqc_rotate_motor(eIndexOfTuner, eDirection, stOption);
    return (tRet);
}

BOOL diseqc_store_motor_position(int eIndexOfTuner, BYTE bPositionStored)
{
    BOOL    tRet = TRUE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;
    //     printf("====eIndexOfTuner == %d=bPositionStored= %d=\n " , eIndexOfTuner , bPositionStored);
    if (ISVALIDINDEX(eIndexOfTuner)) {
	memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	stOption.Body.bStorePostion = bPositionStored;
	tRet = diseqc_format_diseqc12_commit_command(TUNER_DISEQC_CMD_STORE_POSITION, pbCommand, stOption);
	if (tRet) {
	    tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
	}
    } else {
	tRet = FALSE;
    }
    return (tRet);
}

BOOL diseqc_goto_motor_position0(int eIndexOfTuner)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	stOption.Body.bStorePostion = 0;
	tRet = diseqc_format_diseqc12_commit_command(TUNER_DISEQC_CMD_DRIVE_POSITION, pbCommand, stOption);
	if (tRet) {
	    tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
	}
    } else {
	tRet = FALSE;
    }
    return (tRet);
}

BOOL diseqc_jump_to_motor_position(int eIndexOfTuner, BYTE bIdxPosition)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	memset(pbCommand, 0, TUNER_DISEQC_CMD_LENGTH);
	stOption.Body.bGoToPostion = bIdxPosition;
	tRet = diseqc_format_diseqc12_commit_command(TUNER_DISEQC_CMD_DRIVE_POSITION, pbCommand, stOption);
	if (tRet) {
	    tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 4);
	}
    } else {
	tRet = FALSE;
    }
    return (tRet);
}


BOOL diseqc_rotate_motor_by_angular(int eIndexOfTuner, USHORT usAngle)
{
    BOOL    tRet = FALSE;
    BYTE    pbCommand[TUNER_DISEQC_CMD_LENGTH];
    TUNER_DISEQC_MOTOR_BODY_STRUCT stOption;

    if (ISVALIDINDEX(eIndexOfTuner)) {
	stOption.Body.MotorCmd.bCmd0 = ((usAngle >> 8) & 0xFF);
	stOption.Body.MotorCmd.bCmd1 = ((usAngle) & 0xFF);
	tRet = diseqc_format_diseqc12_commit_command(TUNER_DISEQC_CMD_GOTO_ANGULAR, pbCommand, stOption);
	if (tRet) {
	    tRet = diseqc_send_diseqc_command(eIndexOfTuner, pbCommand, 5);
	}
    }
    return ((tRet));
}

#define ABS(X)   ( (X) <0 ? (-(X)) : (X) )
USHORT diseqc_caculate_motor_angular(USHORT usSatLongtitude, USHORT usLocalLongtitude, USHORT usLocalLatitude)
{
    USHORT  uiRet = 0;
    BYTE    usals_rotate_angle, bCmd0, bCmd1;
    BYTE    usals_fractional_deg;
    DOUBLE  move_satangle, move_longitude, move_latitude;
    DOUBLE  G8_sat_angle, G9_longitude, G10_latitude;
    DOUBLE  G15, G16, G17, G18, G19, G20, G21, G22, G23, G24, G25, G26, G27;
    DOUBLE  temp_var;

    if (TRUE) {
	if (usLocalLongtitude >= 1800) {
	    move_longitude = 3600.0 - usLocalLongtitude;
	    move_longitude = -move_longitude;
	    move_longitude = move_longitude / 10.0;
	} else {
	    move_longitude = usLocalLongtitude;
	    move_longitude = move_longitude / 10.0;
	}
	if (usLocalLatitude >= 900) {
	    move_latitude = 1800.0 - usLocalLatitude;
	    move_latitude = -move_latitude;
	    move_latitude = move_latitude / 10.0;
	} else {
	    move_latitude = usLocalLatitude;
	    move_latitude = move_latitude / 10.0;
	}
	if (usSatLongtitude >= 1800) {
	    move_satangle = 3600.0 - usSatLongtitude;
	    move_satangle = -move_satangle;
	    move_satangle = move_satangle / 10.0;
	} else {
	    move_satangle = usSatLongtitude;
	    move_satangle = move_satangle / 10.0;
	}
	G8_sat_angle = move_satangle;
	G9_longitude = move_longitude;
	G10_latitude = move_latitude;
	temp_var = G8_sat_angle - G9_longitude;
	G15 =
	    (DOUBLE) sqrtf(DISEQC_USALS_G15_CONS - (DISEQC_USALS_G15_CONS * cosf((PI * ABS(temp_var) / DISEQC_USALS_CONS_4))));
	G16 = DISEQC_USALS_CONS_1 * sinf((PI * G10_latitude / DISEQC_USALS_CONS_4));
	G17 = (DOUBLE) sqrtf(DISEQC_USALS_G17_CONS - (G16 * G16));
	G18 = (DOUBLE) G17 *sinf((PI * ABS(temp_var) / DISEQC_USALS_CONS_4));

	G19 = (DOUBLE) sqrtf(((G16 * G16) + (G18 * G18)));
	G20 = G19 / DISEQC_USALS_CONS_1;
	G21 = asinf(G20);
	G22 = (G21 * DISEQC_USALS_CONS_4) / PI;
	G23 = (DOUBLE) sqrtf(DISEQC_USALS_CONS_2 - (DISEQC_USALS_CONS_3 * cosf((PI * G22 / DISEQC_USALS_CONS_4))));
	G26 = (DOUBLE) sqrtf(DISEQC_USALS_CONS_2 - (DISEQC_USALS_CONS_3 * cosf((PI * G10_latitude / DISEQC_USALS_CONS_4))));
	G24 = ((G23 * G23) + (G26 * G26) - (G15 * G15)) / (2.0 * G23 * G26);
	G25 = acosf(G24);
	G27 = (G25 * DISEQC_USALS_CONS_4) / PI;

	if (((G8_sat_angle < G9_longitude) * (G10_latitude > 0)) + ((G8_sat_angle > G9_longitude) * (G10_latitude < 0)))
	    G27 = -G27;
	usals_rotate_angle = (BYTE) ABS(G27);
	usals_fractional_deg = (BYTE) ((ABS(G27) - (DOUBLE) usals_rotate_angle) * 10.0);
	if (G27 < 0) {
	    bCmd0 = (BYTE) (0xD0 | ((usals_rotate_angle >> 4) & 0xF));
	    bCmd1 = (BYTE) ((usals_rotate_angle & 0xF) << 4);
	} else {
	    bCmd0 = (BYTE) (0xE0 | ((usals_rotate_angle >> 4) & 0xF));
	    bCmd1 = (BYTE) ((usals_rotate_angle & 0xF) << 4);
	}
	switch (usals_fractional_deg) {
	case 0:
	    bCmd1 |= 0x0;
	    break;
	case 1:
	    bCmd1 |= 0x2;
	    break;
	case 2:
	    bCmd1 |= 0x3;
	    break;
	case 3:
	    bCmd1 |= 0x5;
	    break;
	case 4:
	    bCmd1 |= 0x6;
	    break;
	case 5:
	    bCmd1 |= 0x8;
	    break;
	case 6:
	    bCmd1 |= 0xA;
	    break;
	case 7:
	    bCmd1 |= 0xB;
	    break;
	case 8:
	    bCmd1 |= 0xD;
	    break;
	case 9:
	    bCmd1 |= 0xE;
	    break;
	default:
	    bCmd1 |= 0x0;
	    break;
	}
	uiRet = ((bCmd0 << 8) | bCmd1);
    }
    return ((uiRet));
}
