/********************************************************************
** 2010.9.19 create by li
********************************************************************/
#ifndef __DISEQC_H__
#define __DISEQC_H__


typedef enum {
    TUNER_DISEQC10_PORT_NONE = 0,
    TUNER_DISEQC10_PORT_A,
    TUNER_DISEQC10_PORT_B,
    TUNER_DISEQC10_PORT_C,
    TUNER_DISEQC10_PORT_D,
    TUNER_DISEQC10_PORT_MAX
} TUNER_DISEQC10_ENUM;

typedef enum {
    TUNER_DISEQC11_PORT_NONE = 0,
    TUNER_DISEQC11_PORT_1,
    TUNER_DISEQC11_PORT_2,
    TUNER_DISEQC11_PORT_3,
    TUNER_DISEQC11_PORT_4,
    TUNER_DISEQC11_PORT_5,
    TUNER_DISEQC11_PORT_6,
    TUNER_DISEQC11_PORT_7,
    TUNER_DISEQC11_PORT_8,
    TUNER_DISEQC11_PORT_9,
    TUNER_DISEQC11_PORT_10,
    TUNER_DISEQC11_PORT_11,
    TUNER_DISEQC11_PORT_12,
    TUNER_DISEQC11_PORT_13,
    TUNER_DISEQC11_PORT_14,
    TUNER_DISEQC11_PORT_15,
    TUNER_DISEQC11_PORT_16,
    TUNER_DISEQC11_PORT_MAX
} TUNER_DISEQC11_ENUM;

typedef enum {
    TUNER_MOTOR_NONE = 0,
    TUNER_MOTOR_DISEQC12,
    TUNER_MOTOR_USALS,		//DISEQC13
    TUNER_MOTOR_MAX
} TUNER_MOTOR_ENUM;

typedef enum {
    TUNER_DISEQC_EAST = 0,
    TUNER_DISEQC_WEST,
    TUNER_DISEQC_DISABLE
} TUNER_DISEQC_DIRECTION;


typedef enum {
    TUNER_POL_HORIZONTAL = 0,
    TUNER_POL_VERTICAL,
    TUNER_POL_LEFT,
    TUNER_POL_RIGHT
} TUNER_TP_POLARIZATION_ENUM;

typedef enum {
    TUNER_TONE_22K_NONE = 0,
    TUNER_TONE_22K_OFF,
    TUNER_TONE_22K_ON,
    TUNER_TONE_22K_MAX
} TUNER_TONE_ENUM;

#ifdef __cplusplus
extern  "C" {
#endif

    BYTE    diseqc_get_version(void);

/*功能说明：设置diseqc1.0
参数说明：
输入参数:   tuner index ; eDiSEqC10,要设置的diseqc1.0；
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_set_diseqc10(int eIndexOfTuner, TUNER_DISEQC10_ENUM eDiSEqC10, TUNER_TP_POLARIZATION_ENUM ePolar,
				TUNER_TONE_ENUM e22K);

/*功能说明：设置diseqc1.1
参数说明：
输入参数:   tuner index ; eDiSEqC11,要设置的diseqc1.1；
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_set_diseqc11(int eIndexOfTuner, TUNER_DISEQC11_ENUM eDiSEqC11, TUNER_TP_POLARIZATION_ENUM ePolar,
				TUNER_TONE_ENUM e22K);

/*功能说明：设置motor 位置
参数说明：
输入参数:   tuner index ; usMotorPosition ,要设置的motor位置；
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_set_motor(int eIndexOfTuner, TUNER_MOTOR_ENUM eMotor, USHORT usMotorPosition);

/*功能说明：停止motor
参数说明：
输入参数:   tuner index
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_stop_motor(int eIndexOfTuner);

/*功能说明： 设置motor当前的移动方向
参数说明：
输入参数:   tuner index ; eDirection ,要设置的motor移动方向；
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_set_motor_limit(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection);


/*功能说明： 单步移动motor
参数说明：
输入参数:   tuner index ; eDirection ,motor移动方向；bStep ，移动幅度
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_rotate_motor_by_step(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection, BYTE bStep);

/*功能说明： 超时移动motor
参数说明：
输入参数:   tuner index ; eDirection ,motor移动方向；bTimeout ，超时时间
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_rotate_motor_by_timeout(int eIndexOfTuner, TUNER_DISEQC_DIRECTION eDirection, BYTE bTimeout);

/*功能说明： 存储当前motor位置
参数说明：
输入参数:   tuner index ;bPositionStored ， 是否存储
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_store_motor_position(int eIndexOfTuner, BYTE bPositionStored);

/*功能说明： 移动至上次存储的位置
参数说明：
输入参数:   tuner index ; bIdxPosition ，移动 位置
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_goto_motor_position0(int eIndexOfTuner);

/*功能说明： 移动至上次某个的位置
参数说明：
输入参数:   tuner index ; bIdxPosition ，移动 位置
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_jump_to_motor_position(int eIndexOfTuner, BYTE bIdxPosition);

/*功能说明： 通过角度移动motor
参数说明：
输入参数:   tuner index ; usAngle ， 移动角度
输出参数：    
返   回  值：1表示成功，0表示出错
*/
    BOOL    diseqc_rotate_motor_by_angular(int eIndexOfTuner, USHORT usAngle);

/*功能说明：计算motor角度
参数说明：
输入参数:   tuner index ; usLocalLongtitude ，经度 ，usLocalLatitude ， 纬度
输出参数：    
返   回  值：motor角度
*/
    USHORT  diseqc_caculate_motor_angular(USHORT usSatLongtitude, USHORT usLocalLongtitude, USHORT usLocalLatitude);

#ifdef __cplusplus
}
#endif
#endif
