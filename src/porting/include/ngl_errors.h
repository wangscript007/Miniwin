#ifndef __NGL_ERROR_H__
#define __NGL_ERROR_H__
#ifdef __cplusplus
extern "C"{
#endif
////////////////////////////ERROR DEFINITIONS///////////////////
#define NGL_OK    0
#define NGL_INVALID_HANDLE 0xFFFFFFFF
#define NGL_INVALID_PID 0x1FFF
#define NGL_ERROR -1
#define NGL_INVALID_PARA -2
#define NGL_NO_WAIT 0
#define NGL_WAIT_INFINITE 0xFFFFFFFF
#define NGL_RESOURCE_BUSY -5
#define NGL_NOT_SUPPORT   -6
#define NGL_INSERT_CARD_FIRST            -100       /**< Insert card first         */
#define NGL_RESET_CARD_FIRST             -101       /**< Reset card first          */


#ifdef __cplusplus
}
#endif
#endif
