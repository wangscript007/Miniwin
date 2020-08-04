/**
 * @file UniversalClient_SPI.c
 */

//////////////////////////////////////////////////////////////////////////////////////////
//  include header file
//////////////////////////////////////////////////////////////////////////////////////////
#include "UniversalClient.h"
#include "UniversalClient_API.h"
#include "UniversalClient_SPI.h"
#include "UniversalClient_Common_SPI.h"
#include "common/gxmalloc.h"
#include "UniversalClient_CRC.h"
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_dmx.h>
#include <ngl_os.h>

NGL_MODULE(SPI);

#define LOADER_V3_2 0
#define LOADER_V5	1
#define DVB_CLIENT  1
#if 0 //0825 temp to mark {.
#if (PRE_ENABLEMENT == TRUE)
#include "Cloake_PS_CA_OTP.h"
#endif
#endif //0825 temp to mark .}

//#include "i_hlsup.h"
#if(LOADER_V3_2 == 1)
#include "_M_def.h"
#endif

//#include "MApp_Ird_loader_status.h"

#if(LOADER_V5 == 1)
//#include "Bbcb.h"
#include "LoaderCoreAPI_ApplicationHelper.h"
#endif

//new  MX
#include "gxos/gxcore_os_filesys.h"
#include <downloader/LoaderCore_Types.h>
#include "gxsecure/CA/gxifcp_api.h"
#include "gxsecure/gxsecure.h"
#include "dvbhal/gxdescrambler_hal.h"
//#include "gxsecure/gxfuse_api.h"
#include "gxsecure/gxsecure_api.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Definition
//////////////////////////////////////////////////////////////////////////////////////////
#define MP_PATITION_ADDR                    (0x00240000) // manufacture params (private data, personlized data)
#define UC_THREAD_STAKC_SIZE                (128 * 1024)
#define E_TASK_PRI_HIGH						(13)//gx default i 15,ui 14, ecm emm 13  MX		

#define MAX_TASK_NUM                        (128)
#define MAX_MUTEX_NUM                       (64)
#define SPI_MUTEX_TIMEOUT                   (10)

#define MUTEX_DBG                           (0)
#define DISABLE_MUTEX                       (0)
#define CA_CCA_PS_CACHE_SUP					(0)

//Stream Definition
//EMM/ECM monitor event
#define E_DMX_CB                    0xFFFFFFFF
#define E_DMX_EMM_CB                0x10000000
#define E_DMX_ECM_CB                0x20000000
#define EMM_FLT_MASK(_bit_)         (1<<(_bit_))        //emm callback triggered event: 0x100FFFFF _bit_ = 0~19
#define ECM_FLT_MASK(_bit_)         ((1<<(_bit_))<<20)   //ecm callback triggered event: 0x2FF00000 _bit_ = 0~4


#define FILTER_MASK_DEPTH_DSIZE      (16)      

//PS cache
#if (CA_CCA_PS_CACHE_SUP == 1)
#define PS_BASE_ADDR				(_M_FLASH_CCA_DATA1_ADDR_)
#define PS_FLUSH_CHK_TASK_STK_SIZE	(16 * 1024)
#define PS_BLK_SIZE					(64 * 1024) //64k
#define PS_BLK_NUM_CCA_INFO			(1) //64k
#define PS_BLK_NUM_SC_INIT			(6) //320k
#define PS_BLK_NUM_SC_DNLD			(6) //320k
#define PS_BLK_NUM_CFG			    (1) //64k
#define PS_BLK_NUM_ALL			    (PS_BLK_NUM_CCA_INFO + PS_BLK_NUM_SC_INIT + PS_BLK_NUM_SC_DNLD + PS_BLK_NUM_CFG)
#define PS_FLUSH_TIMEOUT			(3000)

#define PS_BLK_START_CCA_INFO       (0)
#define PS_BLK_START_SC_INIT        (PS_BLK_NUM_CCA_INFO)
#define PS_BLK_START_SC_DNLD        (PS_BLK_NUM_CCA_INFO + PS_BLK_NUM_SC_INIT)
#define PS_BLK_START_CFG            (PS_BLK_NUM_CCA_INFO + PS_BLK_NUM_SC_INIT + PS_BLK_NUM_SC_DNLD)
#endif

//DSMCB set key
#define DSCMB_KL_TRY_COUNT          (100)
#define DESC_KEY_SIZE               (8)

//.0428 {.

//K5L no need to use fw  #define SC_USE_IO_51
#define T0_PROTOCOL                 (0)
#define T14_PROTOCOL                (14)

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
uc_stream_handle Current_StreamHandle=0;



//////////////////////////////////////////////////////////////////////////////////////////
// Local Variable
//////////////////////////////////////////////////////////////////////////////////////////
static uc_sint32               s_ucSemaphorePS = -1;
//static uc_sint32 			   _s32FlashMutexId = -1; 
static HANDLE mtx_dmx=0;


//PS cache
#if (CA_CCA_PS_CACHE_SUP == 1)
const SERFLASH_Info* gstSpiInfo;
static uc_sint32			s_ucPSTaskID = -1;
static uc_uint8			s_ucPSStack[PS_FLUSH_CHK_TASK_STK_SIZE];
static uc_bool			s_ucPSDirty[PS_BLK_NUM_ALL] = { FALSE };
static uc_uint8			*s_ucAllPSBuffer = NULL;
#endif

#ifdef SC_USE_IO_51
uc_uint8 u8SCFirmware[] = {
#include "../../../../bsp/lib/fw/fwSC.dat"
};

static SC_Param astScParam[2] =
{
    {0, E_SC_CLK_6M,   SC_UART_CHAR_8|SC_UART_STOP_1|SC_UART_PARITY_NO, 620, E_SC_VCC_CTRL_8024_ON, 0,NULL, 0}, //t14
    {0, E_SC_CLK_4P5M, SC_UART_CHAR_8|SC_UART_STOP_2|SC_UART_PARITY_EVEN, 372, E_SC_VCC_CTRL_8024_ON, 0,NULL, 0}, //t0
};
#endif


typedef struct
{
    uc_sint32              s32timerId;
    uc_timer_callback   stimercallbk;
    void*  pCon;

} TimerCallback;

typedef struct
{
    uc_sint32                          iId;
    uint32_t                    ePriority;
    void                            *pStack;
    uc_uint32                          u32StackSize;
    char szName[16];
} Task_Info, *PTask_Info;

static  TimerCallback    s_gtimercallbk[MAX_TASK_NUM];
static  uc_uint16        s_timernum=0;


static Task_Info   s_gucTask[MAX_TASK_NUM];
static uc_uint16      s_tasknum=0;
static int      s_Mutex=0;


typedef struct
{
    uc_sint32      s32timerId;
    uc_sint32      s32ThreadHandle;

} SusThread;

static  SusThread       s_gSusthread[MAX_TASK_NUM];


//Stream Local Variable
//EMM/ECM monitor task
static uc_sint32 _s32CBMonitorIsStarted = 0;
static uc_sint32 _s32CBMonitorTaskId = -1;
static uc_sint32 _s32CBMonitorEventId = -1;
static uc_uint8 s_u8CBMonitorStack[4096];

static uc_sint32 _s32CBMonitorEMMTaskId = -1;
static uc_sint32 _s32CBMonitorEMMEventId = -1;
static uc_uint8 s_u8CBMonitorEMMStack[4096];
//////////////////////////////////////////////////////////////////////////////////////////
//  global variable
//////////////////////////////////////////////////////////////////////////////////////////
//uc_stream_handle Current_StreamHandle = 0;
//handle_t cas_v=-1;
//handle_t cas_a=-1; 

//extern uc_sint32 gs32NonCachedPoolID;
//#define gs32MstarCachedPoolID gs32NonCachedPoolID
//extern uc_sint32 gs32MstarNonCachedPoolID;
//////////////////////////////////////////////////////////////////////////////////////////
//  external function
//////////////////////////////////////////////////////////////////////////////////////////
extern lc_result LoaderCoreAPI_GetUKInfo(lc_uint16* pSystemId, lc_uint16* pKeyVersion, lc_uint16* pSignatureVersion);
extern lc_result LoaderCoreAPI_GetVariant(lc_uint16 *pVariant, lc_uint16 *pSubVariant);
extern lc_result LoaderCoreAPI_ReadBBCB(lc_bbcb_parameters *pBBCBParameters, lc_bool *pUpdateCRC);
extern lc_result LoaderCoreAPI_GetDownloadSequenceNumber(lc_uint16 *pDownloadSequenceNumber);

extern uc_result UniversalClient_PVR_setSessionKey(uc_stream_handle streamHandle, const uc_buffer_st *pPVRSessionKey);
extern handle_t DESCR_Allocate(uc_uint16 Pid,GxDescAlg alg,uc_uint8 *pDescHandleIdx);

//extern SC_Result MApi_SC_T14_Communication(uc_uint8 u8SCID, uc_uint8 *pu8SendData, uc_uint16 *pu16SendDataLen, uc_uint8 *pu8ReadData, uc_uint16 *pu16ReadDataLen);

#if( LOADER_V3_2 == 1)
//extern void BLOADER_GetBBCB(  FLASH_BB_VERSION_STRUCTURE *stFlashLoaderData );
#else
//extern lc_result BBCB_GetBBCB(lc_bbcb_st *pBBCB, lc_bool *pIsCrcCorrect);
#endif
extern uc_uint16 MApi_GetLOAD_SEQ_NR(void);
#if 0
extern uc_sint32 		_s32FlashMutexId; //HB_KEEP_TST  add flash W/R protect
extern uc_uint32 gs32CAMemPoolID;
#endif

/* Exported Macros --------------------------------------------------------- */
#define DESCR_CHN_MAX_CNT  (8)
#define DESCR_INVALID_HANDLE     (0xFFFFFFFF)
#define DESCR_INVALID_PID        (0x1FFF)

typedef enum
{
    DESCR_STATUS_FREE,
    DESCR_STATUS_ALLOC,
    DESCR_STATUS_USED,
}DESCR_Status_e;
typedef struct 
{
	DESCR_Status_e Status;
	handle_t DescrHandle;
	GxDescAlg dscmbType;
    GxDescKLMAlg KLAlgo;
    uint16_t Pid;
	uint8_t EvenKey[16];
    uint8_t OddKey[16];
    uint8_t EvenIV[16];
    uint8_t OddIV[16];
}DESCR_Chn_s;
typedef struct 
{
    uc_mutex_handle MutexHandle;
    DESCR_Chn_s DescrChn[DESCR_CHN_MAX_CNT];
}DESCR_Info_s;
/* Private Variables ------------------------------------------------------ */
static DESCR_Info_s DESCR_Info =
{
    .MutexHandle = 0
};

/* Compile Options --------------------------------------------------------- */


#define DESCR_LOCK() if(0 != DESCR_Info.MutexHandle) UniversalClientSPI_Mutex_Lock(DESCR_Info.MutexHandle)
#define DESCR_UNLOCK() if(0 != DESCR_Info.MutexHandle) UniversalClientSPI_Mutex_Unlock(DESCR_Info.MutexHandle)
#ifndef USE_OSMUTEX
#define LOCK_DMX    GxCore_MutexLock(mtx_dmx)
#define UNLOCK_DMX  GxCore_MutexUnlock(mtx_dmx)
#else
#define LOCK_DMX    nglLockMutex(mtx_dmx) 
#define UNLOCK_DMX  nglUnlockMutex(mtx_dmx)
#endif

extern void *AP_NC_Alloc(uc_uint32 u32Size );
extern void AP_NC_Free(uc_uint8* pAddr );
extern void ca_get_section_callbak(HANDLE filter,const BYTE *buf, UINT len,void * pridata);

//////////////////////////////////////////////////////////////////////////////////////////
//  Local Function
//////////////////////////////////////////////////////////////////////////////////////////

unsigned int UniversalClient_calculateCRC16(unsigned char *pvStartAddress, long ulSize_in_bytes)
{
    unsigned int        wCRC = 0;
    unsigned char       *pbData = NULL;

    /* init the start value */
    wCRC = 0x0;

    /* calculate CRC */
    pbData = pvStartAddress;
    while(ulSize_in_bytes--)
    {
        wCRC = (((wCRC >> 8) & 0xFF) ^ CRC16_Table[((wCRC ^ *pbData++) & 0xFF)]);
    }

    return (wCRC);
}

/** @defgroup universalclient_spi Cloaked CA Agent SPIs

 *  Cloaked CA Agent SPIs
 *
 *  Service Provider Interfaces(SPIs) are driver-level APIs called by Cloaked CA Agent.
 *  Client device platform must implement these SPIs in order to run Cloaked CA Agent.
 *
 *  \note In the SPI implementation, APIs shall not be called to avoid dead lock between the
 *  application and Cloaked CA Agent thread.
 *  @{
 */

/** @page allspis Cloaked CA Agent SPIs
 * Cloaked CA Agent SPIs
 *
 * Service Provider Interfaces(SPIs) are the components that must be implemented on a per-platform basis.
 * These SPIs are grouped into sections according to
 * common functionality:
 *
 * - \ref memoryspi "Cloaked CA Agent Memory SPIs"
 * - \ref semaphorespi "Cloaked CA Agent Semaphore SPIs"
 * - \ref mutexspi "Cloaked CA Agent Mutex SPIs"
 * - \ref threadspi "Cloaked CA Agent Thread SPIs"
 * - \ref psspi "Cloaked CA Agent Persistent Storage SPIs"
 * - \ref devicespi "Cloaked CA Agent Device SPIs"
 * - \ref pvrspi "Cloaked CA Agent PVR SPIs"
 * - \ref streamspi "Cloaked CA Agent Stream SPIs"
 * - \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs"
 * - \ref smartcardspi "Cloaked CA Agent Smartcard SPIs"
 * - \ref datetimespi "Cloaked CA Agent Date Time SPIs"
 *
 * In order to indicate which SPIs are implemented, the integrator MUST
 * implement the top-level SPIs
 * ::UniversalClientSPI_GetVersion and ::UniversalClientSPI_GetImplementation.
 */


/** @defgroup toplevelspi Cloaked CA Agent Top Level SPIs
 *  Cloaked CA Agent Top Level SPIs
 *
 *  These SPIs implement a version compatibility mechanism that allows
 *  new version releases of the Cloaked CA Agent API to be linked with
 *  an existing SPI implementation without having to update the SPI implementation
 *  in order to build.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */

/**
 * Get the current version of the Cloaked CA Agent SPI.
 *
 * This function is called when the client is started is called in order to check
 * that the version of the SPI implementation is compatible with the version of the client.
 *
 * This function MUST return #UNIVERSALCLIENTSPI_VERSION. Recompiling an existing SPI implementation
 * will update this version automatically.
 */
uc_uint32 UniversalClientSPI_GetVersion(void)
{
    return  UNIVERSALCLIENTSPI_VERSION;
}
/** @} */ /* End of toplevelspi*/

/** @defgroup memoryspi Cloaked CA Agent Memory SPIs
 *  Cloaked CA Agent Memory SPIs
 *
 *  These SPIs implement basic memory management functionality required for most operations.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Allocates a block of uninitialized memory.
 *
 * This function is called to allocate a block of memory and return a pointer to the beginning
 * of the block. If the system is out of memory, or another failure occurs, it must return NULL.
 * When the memory is no longer needed, ::UniversalClientSPI_Memory_Free will be called to free it.
 *
 * @param[in] bytes Number of bytes to allocate. If this is 0, the implementation MUST return an empty block
 *     of allocated memory.
 */
void* UniversalClientSPI_Memory_Malloc(uc_uint32 bytes)
{
    void *pvMem = NULL;

    if(bytes == 0)
    {
        return NULL;
    }
    else
    {
        pvMem =  (void*)GxCore_Mallocz(bytes);

	    if(pvMem == NULL)
	    {
	        NGLOG_ERROR("[END] Allocate Memory fail . NULL point! \n");
	    }
	    return pvMem;
    }
}

/**
 * Frees a block of memory.
 *
 * This function is called to free a block of memory previously allocated using ::UniversalClientSPI_Memory_Malloc.
 *
 * @param[in] lpVoid Pointer to a block of previously allocated memory. If this is NULL, the implementation
 *     MUST handle it gracefully and ignore it. If this points to an area of memory that was not previously
 *     allocated by ::UniversalClientSPI_Memory_Malloc, OR it has already been freed, the result is undefined.
 */

void UniversalClientSPI_Memory_Free(void * lpVoid)
{
    if(lpVoid == NULL)
    {
        return;
    }
    else
    {
        GxCore_FreeRelease(lpVoid);
        lpVoid = NULL;
        return;
    }
}

/**
 * Report a fatal error.
 *
 * This function is called to report a fatal error.
 *
 * @param[in] type Error code, see ::uc_fatal_error_type.
 * @param[in] lpVoid Pointer to the null-terminated string to describe this error.
 */
void  UniversalClientSPI_FatalError(uc_fatal_error_type type,void* lpVoid)
{
    if(type == UC_FATAL_ERROR_PS_DAMAGED )
    {
        NGLOG_ERROR("UC_FATAL_ERROR_PS_DAMAGED::%s\n", (char *)lpVoid);
    }
    else if(type == UC_FATAL_ERROR_MEMORY)
    {
        NGLOG_ERROR("UC_FATAL_ERROR_MEMORY::%s\n", (char *)lpVoid);
    }
    else
    {
        NGLOG_ERROR("UNKONWN Fatal Error Type:%d ::%s\n",type ,(char *)lpVoid);
    }

    return;

}

/** @} */ /* End of memory */

/** @defgroup semaphorespi Cloaked CA Agent Semaphore SPIs
 *  Cloaked CA Agent Semaphore SPIs
 *
 * These SPIs implement basic thread synchronization, semaphores.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a semaphore
 *
 * This function is called to create a semaphore.
 *
 * A 'semaphore' is a basic synchronization object with the following properties:
 * - It maintains a value, which is used internally to determine when to unblock waiters.
 * - Calling ::UniversalClient_Semaphore_Post atomically increments the value.
 * - Calling ::UniversalClient_Semaphore_Wait waits until the value > 0, then atomically decrements the value.
 *
 * Semaphores are used by the Cloaked CA Agent to signal when events occur and to protect
 * data structures from simultaneous access.
 *
 * @param[in] initialValue Initial value for the semaphore.
 * @param[out] pSemaphoreHandle Receives a handle to a newly created semaphore. When the semaphore is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Semaphore_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */

uc_result UniversalClientSPI_Semaphore_Open(uc_uint32 initialValue, uc_semaphore_handle * pSemaphoreHandle)
{
    if(pSemaphoreHandle == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    uc_int8 sRest = FALSE;
	//*pSemaphoreHandle = UniversalClientSPI_Memory_Malloc(sizeof(uc_semaphore_handle));
	sRest = GxCore_SemCreate(pSemaphoreHandle, initialValue);

    //Note the sRest Handle start from 0  or 1!
#if 0    
    if(sRest < 0)
    {
        return UC_ERROR_UNABLE_TO_CREATE;
    }
#endif
    return UC_ERROR_SUCCESS;
}

/**
 * Increment the semaphore value
 *
 * This function is called to atomically increment the value of a semaphore.
 * If the value goes above 0, this should cause threads waiting in a call to
 * ::UniversalClientSPI_Semaphore_Wait to be unblocked accordingly.
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Semaphore_Post(uc_semaphore_handle semaphoreHandle)
{
    uc_bool bRest = FALSE;
	bRest = GxCore_SemPost(semaphoreHandle);
#if 0	
    if(bRest == FALSE)
    {
        return UC_ERROR_OS_FAILURE;
    }
#endif
    return UC_ERROR_SUCCESS;
}

/**
 * Decrement the semaphore value
 *
 * This function is called to atomically decrement the value of a semaphore.
 * It waits for the semaphore value to be greater than 0, then atomically decrements the value.
 * For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0.
 * If another thread calls ::UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and
 * ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two
 * threads remain blocked.
 *
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Semaphore_Wait(uc_semaphore_handle semaphoreHandle)
{
    uc_bool bRest = FALSE;
	bRest = GxCore_SemWait(semaphoreHandle);
#if 0
    if(bRest == FALSE)
    {
        return UC_ERROR_OS_FAILURE;
    }
#endif
    return UC_ERROR_SUCCESS;
}

uc_result UniversalClientSPI_Semaphore_WaitTimeout(uc_semaphore_handle semaphoreHandle , uc_uint32  milliseconds )
{
    if(milliseconds == 0)
    {
        return UC_ERROR_SUCCESS;
    }

    uc_bool bRest = FALSE;
    if(milliseconds!=0xffffffff)
    {
    	bRest = GxCore_SemTimedWait(semaphoreHandle, milliseconds);
		#if 0
        if(bRest == FALSE)
        {
            return UC_ERROR_WAIT_ABORTED ;
        }
		#endif
    }
    else
    {
        bRest = GxCore_SemTimedWait(semaphoreHandle, milliseconds);
		#if 0
        if(bRest == FALSE)
        {
            return UC_ERROR_OS_FAILURE;
        }
		#endif
    }
    return UC_ERROR_SUCCESS;

}


/**
 * Destroy a semaphore object.
 *
 * This function is called to destroy a semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @param[in] pSemaphoreHandle Pointer to handle of semaphore to destroy. The implementation
 *     should free any resources associated with the semaphore, and MUST set the value of *pSemaphoreHandle to NULL.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Semaphore_Close(uc_semaphore_handle * pSemaphoreHandle)
{
    if(pSemaphoreHandle == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    uc_bool bRest = FALSE;
	bRest = GxCore_SemDelete(*pSemaphoreHandle);
	#if 0
    if(bRest == FALSE)
    {
        return UC_ERROR_OS_FAILURE;
    }
	#endif
    pSemaphoreHandle=NULL;
    return UC_ERROR_SUCCESS;
}

/** @} */ /* End of semaphore */
/** @defgroup mutexspi Cloaked CA Agent Mutex SPIs
 *  Cloaked CA Agent Mutex SPIs
 *
 * These SPIs implement basic thread mutual exclusion via Mutex objects. This
 * is used to prevent multiple threads from simultaneously accessing
 * mutex-protected objects at the same time.
 *
 * Recursive mutex support is \b not required. That is, one thread will never
 * attempt to lock a mutex that is already locked by the thread. The mutex
 * implementation may safely ignore this case.
 *
 * \note The 'mutex' abstraction is provided to allow platforms to implement
 *    mutexes in a higher-performance manner than a typical semaphore implementation.
 *    Platforms which do not support a separate 'mutex' concept may safely use
 *    a semaphore internal to the implementation of the mutex.
 *
 *
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a mutex
 *
 * This function is called to create a mutex object.
 *
 * A 'mutex' is a basic synchronization object with the following properties:
 * - Calling ::UniversalClientSPI_Mutex_Lock locks a mutex. If the mutex was already locked, this
 *   waits until the mutex is unlocked, then locks it.
 * - Calling ::UniversalClientSPI_Mutex_Unlock unlocks the mutex.
 *
 * Mutexes are used by the Cloaked CA Agent API to protect internal data structures
 * from being corrupted by simultaneous access.
 *
 * @param[out] pMutexHandle Receives a handle to a newly created mutex. When the mutex is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Mutex_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Mutex_Open(uc_mutex_handle * pMutexHandle)
{
    if(pMutexHandle == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    // junchi.lin 20140508: avoid dead lock
	GxCore_MutexCreate(pMutexHandle);
    s_Mutex++;
    //s32MutexId = MsOS_CreateMutex(E_MSOS_FIFO, "UC_SPI_MUTEX", MSOS_PROCESS_PRIVATE);

    if(*pMutexHandle < 0)
    {
        return UC_ERROR_DRIVER_FAILURE;
    }

    /*
    These Recursive mutex support is not required. That is, one thread will never attempt to lock a mutex that is already locked by the thread.
    The mutex implementation may safely ignore this case.
    */

#if 0
    // 2012.11.20:keep the first mutex id and task id as recursion index
    if(initMutex == 0 && initTask == 0)
    {
        initMutex = s32MutexId;
        initTask = MsOS_InfoTaskID();

#if 0
        printf("\ninitMutex: %d\n", initMutex);
        printf("\ninitTask: %d\n", initTask);
#endif
    }
#endif
    return UC_ERROR_SUCCESS;
}
/**
 * Locks a mutex
 *
 * This function is called to lock a mutex to prevent other threads from entering
 * a potentially unsafe section of code until the mutex is unlocked.
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Mutex_Lock(uc_mutex_handle mutexHandle)
{
    uc_bool Rest = UC_ERROR_SUCCESS;
	Rest = GxCore_MutexLock(mutexHandle);
    if(Rest != UC_ERROR_SUCCESS)
    {
        return UC_ERROR_OS_FAILURE;
    }
    return UC_ERROR_SUCCESS;
}
/**
 * Unlocks a mutex
 *
 * This function is called to unlock a mutex, and release a single thread waiting in
 * call to ::UniversalClientSPI_Mutex_Lock, if any.  If no threads are waiting, then
 * the first thread to attempt to lock the mutex will not have to wait.
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Mutex_Unlock(uc_mutex_handle mutexHandle)
{
    uc_bool Rest = UC_ERROR_SUCCESS;
    Rest= GxCore_MutexUnlock(mutexHandle);
    if(Rest != UC_ERROR_SUCCESS)
    {
        return UC_ERROR_OS_FAILURE;
    }
    return UC_ERROR_SUCCESS;
}
/**
 * Destroy a mutex object.
 *
 * This function is called to destroy a mutex object previously created
 * using ::UniversalClientSPI_Mutex_Open.
 *
 * @param[in] pMutexHandle Pointer to handle of semaphore to destroy.
 *     The implementation
 *     should free any resources associated with the mutex, and MUST set
 *     the value of *pMutexHandle to NULL.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Mutex_Close(uc_mutex_handle * pMutexHandle)
{
    if(pMutexHandle == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    uc_bool Rest = UC_ERROR_SUCCESS;
    Rest = GxCore_MutexDelete(*pMutexHandle);
    if(Rest != UC_ERROR_SUCCESS)
    {
        return UC_ERROR_OS_FAILURE;
    }

    *pMutexHandle=NULL;
    return UC_ERROR_SUCCESS;
}
/** @} */ /* End of mutexspi */
/** @defgroup threadspi Cloaked CA Agent Thread SPIs
 *  Cloaked CA Agent Thread SPIs
 *
 *  These SPIs implement basic thread management functions. The Cloaked CA Agent requires
 *  one background thread that is uses for processing commands, ECMs, and EMMs.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */
/**
 * Create a new thread and begin executing it.
 *
 * \note Cloaked CA Agent currently only owns one thread, so it is better to let it have a higher priority. Choose a suitable priority when implementing UniversalClientSPI_Thread_Open.
 * UniversalClientSPI_Thread_Open requirements about 25K bytes as stack.
 * If client device doesn't provide it with enough stack size, it may crash.Be careful when implementing UniversalClientSPI_Thread_Open!
 *
 * This function is called to create a new instance of a thread, a thread procedure and associated parameter.
 *
 * @param[in] threadProc Starting point of new thread
 * @param[in] lpVoid Parameter to pass to newly created thread
 * @param[out] pThreadHandle Receives an implementation-specific handle to the newly created thread.
 *     The caller will close the handle in a subsequent call to ::UniversalClientSPI_Thread_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */

uc_result UniversalClientSPI_Thread_Open(
    uc_threadproc threadProc,
    void* lpVoid,
    uc_thread_handle * pThreadHandle)
{
    uc_int16 index=0;

    if(s_tasknum==0)
    {
        NGLOG_DEBUG("[Set] Tasknum=0 reset all task-data \n");
        for(index=0 ; index<MAX_TASK_NUM ; index++ )
        {
            s_gucTask[index].iId = -1;
            s_gucTask[index].pStack=NULL;
            s_gucTask[index].ePriority=E_TASK_PRI_HIGH;//E_TASK_PRI_MEDIUM-2;
            s_gucTask[index].u32StackSize=UC_THREAD_STAKC_SIZE;
            memset( &s_gucTask[index].szName[0] , 0x00 , sizeof( &s_gucTask[index].szName[0] ) );

            s_gSusthread[index].s32ThreadHandle=-1;
            s_gSusthread[index].s32timerId=-1;
        }
    }

    index=0;
    for(index=0 ; index<s_tasknum ; index++ )
    {
        if( s_gucTask[index].iId==-1)
            break;
    }

    if(index>=MAX_TASK_NUM)
    {
        NGLOG_ERROR("[START] Index out of range!:%d \n", index);
        return UC_ERROR_TOO_MANY_HANDLES;
    }

    NGLOG_DEBUG("[START] Find First Index:%d \n", index);

    //Task_Info             ucTask;
    //uc_sint32                taskID = -1;

    s_gucTask[index].iId = -1;
    s_gucTask[index].pStack=NULL;
    s_gucTask[index].ePriority=E_TASK_PRI_HIGH;

    memset( &s_gucTask[index].szName[0] , 0x00 , sizeof( &s_gucTask[index].szName[0] ) );
    sprintf( &s_gucTask[index].szName[0] , "UniversalClient_Task");
    s_gucTask[index].u32StackSize = UC_THREAD_STAKC_SIZE;
    s_gucTask[index].pStack = GxCore_Mallocz(s_gucTask[index].u32StackSize);

    if(s_gucTask[index].pStack == NULL)
    {
        NGLOG_ERROR(">>>> UC_ERROR_OUT_OF_MEMORY\n");
        return UC_ERROR_OUT_OF_MEMORY;
    }

	GxCore_ThreadCreate(s_gucTask[index].szName, (handle_t *)&s_gucTask[index].iId,
										  threadProc, lpVoid,
										  UC_THREAD_STAKC_SIZE,
										  E_TASK_PRI_HIGH);

    if( s_gucTask[index].iId < 0)
    {
        NGLOG_DEBUG(">>>> UC_ERROR_UNABLE_TO_CREATE\n");
        GxCore_Free(s_gucTask[index].pStack);
        s_gucTask[index].pStack=NULL;
        return UC_ERROR_UNABLE_TO_CREATE;
    }

    s_tasknum++;

    NGLOG_DEBUG(">>>> taskID = %ld , TaskNum=:%u \n",  s_gucTask[index].iId , s_tasknum );

    *pThreadHandle = (uc_thread_handle)s_gucTask[index].iId;

    return UC_ERROR_SUCCESS;
}
/**
 * Suspend the thread for the period specified.
 *
 * This function is called to suspend the thread for at least the period specified. The minimum granularity of the timer should be 10ms.
 *
 * \note If the requested delay time does not correspond with a boundary of the granularity, the delay time needs to be extended to the next available boundary of the granularity. \n
 *     For example: \n
 *     for platforms that have  1 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread  1 ms; \n
 *     for platforms that have  5 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 5 ms; \n
 *     for platforms that have 10 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 10 ms. \n
 *
 * @param[in] hThreadHandle Pointer to handle of thread.
 * @param[in] wDelay is the time in milliseconds to suspend the thread.
 *
 * \note If the value of hThreadHandle is NULL, this is special pointer to handle of thread itself which is invoking the SPI.
 * If the value of hThreadHandle is not NULL, this is right pointer to this handle of thread.
 *
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Thread_Sleep(uc_thread_handle hThreadHandle, uc_uint16 wDelay)
{
    GxCore_ThreadDelay(wDelay);
    return UC_ERROR_SUCCESS;
}
/**
 * Closes a thread
 *
 * This function is called to close a thread object. This function should block until
 * the thread has completed execution. After this, the function should clean up any data associated with
 * the thread and free the handle.
 *
 * \note Threads are typically not closed until ::UniversalClient_Stop is called.
 *     For platforms that do not have the ability to properly wait for a thread to complete execution (i.e. thread 'join'),
 *     this can be simulated as necessary using some platform-specific approximation-- it will only affect the client during
 *     shutdown. For platforms that do not have the means to shut down, this can be ignored.
 *
 * @param[in] pThreadHandle Pointer to handle of thread to close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Thread_Close(uc_thread_handle * pThreadHandle)
{
    NGLOG_DEBUG("[START] UniversalClientSPI_Thread_Close  ThreadHandle:%lu \n" ,(uc_sint32)*pThreadHandle );

    if(pThreadHandle == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    uc_bool Rest = FALSE;
    Rest = GxCore_ThreadDelete((uc_sint32)*pThreadHandle);
#if 0
    if(Rest== FALSE)
    {
        return UC_ERROR_OS_FAILURE;
    }
#endif 
    uc_int16 index=0;

    for(index=0 ; index < MAX_TASK_NUM ; index++ )
    {
        if( s_gucTask[index].iId==(uc_sint32)*pThreadHandle)
        {
            GxCore_FreeRelease(s_gucTask[index].pStack);

            s_gucTask[index].pStack = NULL;
            s_gucTask[index].iId = -1;

            *(pThreadHandle)=(uc_thread_handle)(s_gucTask[index].iId);//is it meaningful?

            s_gSusthread[index].s32ThreadHandle=-1;
            s_gSusthread[index].s32timerId=-1;

            NGLOG_DEBUG("[Reset Free] UniversalClientSPI_Thread_Close Reset-task data index:%d \n" , index );
            break;
        }
    }

    s_tasknum--;
    NGLOG_DEBUG("[END] UniversalClientSPI_Thread_Close Tasknum:%u \n" ,s_tasknum );
    *pThreadHandle=NULL;

    return UC_ERROR_SUCCESS;
}
/** @} */ /* End of thread */
#define SECTOR_LEN		0x10000
#if (CA_CCA_SECOND_SEC_CORE_SUP == 1)
#define FILE_MAX_LEN	0x1B0000+0xA0000
#else
#define FILE_MAX_LEN	0x1B0000
#endif
#define _M_FLASH_CCA_DATA1_ADDR_ 		  0x0//		BASEADDRESS_CCA_INFO_IN_FLASH
#define _M_FLASH_CCA_DATA1_SIZE_           0x20000//128K

#define _M_FLASH_CCA_SC_INIT_ADDR_         _M_FLASH_CCA_DATA1_ADDR_+_M_FLASH_CCA_DATA1_SIZE_ //Secure Core packages (initial) (index 1)
#define _M_FLASH_CCA_SC_INIT_SIZE_         0xA0000  //640K

#define _M_FLASH_CCA_SC1_INIT_ADDR_         _M_FLASH_CCA_SC_INIT_ADDR_+_M_FLASH_CCA_SC_INIT_SIZE_ //Secure Core packages2 (initial) (index 2) for multiple secure core
#if (CA_CCA_SECOND_SEC_CORE_SUP == 1)
#define _M_FLASH_CCA_SC1_INIT_SIZE_         0xA0000
#else
#define _M_FLASH_CCA_SC1_INIT_SIZE_         0x0
#endif

 //Secure Core packages Partition II   (downloading) (index 31)
#define _M_FLASH_CCA_SC_DNLD_ADDR_         _M_FLASH_CCA_SC1_INIT_ADDR_ + _M_FLASH_CCA_SC1_INIT_SIZE_ //Secure Core packages (downloading) (index 31)
#define _M_FLASH_CCA_SC_DNLD_SIZE_         0xA0000

#define _M_FLASH_CCA_SC1_DNLD_ADDR_         _M_FLASH_CCA_SC_DNLD_ADDR_ + _M_FLASH_CCA_SC_DNLD_SIZE_ //Secure Core packages2 (downloading) (index 32) for multiple secure core
#if (CA_CCA_SECOND_SEC_CORE_SUP == 1)
#define _M_FLASH_CCA_SC1_DNLD_SIZE_         0xA0000
#else
#define _M_FLASH_CCA_SC1_DNLD_SIZE_         0x0
#endif

//CFG file(index 9)
#define _M_FLASH_CCA_CFG_ADDR_             _M_FLASH_CCA_SC1_DNLD_ADDR_+_M_FLASH_CCA_SC1_DNLD_SIZE_ //CFG file(index 9)
#define _M_FLASH_CCA_CFG_SIZE_             0x1000 // 4KB per operator , assuem two sercure core
//SoftCell3 CA data(index 20)
#define _M_FLASH_CCA_SOFTCELL_ADDR_        _M_FLASH_CCA_CFG_ADDR_+_M_FLASH_CCA_CFG_SIZE_ //SoftCell3 CA data(index 20)
#define _M_FLASH_CCA_SOFTCELL_SIZE_        0//0x2000 //2KB per smart card slot  this project withou SC slot.MX


//IFCP index51 and index56
#define _M_FLASH_CCA_IFCP1_ADDR_        _M_FLASH_CCA_SOFTCELL_ADDR_+_M_FLASH_CCA_SOFTCELL_SIZE_ //IFCP1(index 51)
#define _M_FLASH_CCA_IFCP1_SIZE_        0x20000 // 128KB

#define _M_FLASH_CCA_IFCP2_ADDR_        _M_FLASH_CCA_IFCP1_ADDR_+_M_FLASH_CCA_IFCP1_SIZE_ //IFCP1(index 61)
#define _M_FLASH_CCA_IFCP2_SIZE_        0x20000 // 128KB

#define CONFIG_FOR_NGL_CCA_PATH  "/home/gx/data.bin"

#if  (CA_CCA_PS_CACHE_SUP == 1)
static uc_bool _WriteFlash(uc_uint32 u32Addr, uc_uint8* pu8Src, uc_uint32 u32Size)
{
	uc_bool		ret = FALSE;

	if(pu8Src == NULL || u32Size == 0)
	{
		return FALSE;
	}

	ret = MsOS_ObtainMutex(_s32FlashMutexId, MSOS_WAIT_FOREVER);
	ret = MDrv_SERFLASH_WriteProtect(FALSE);

	while(FALSE == MDrv_SERFLASH_AddressErase(u32Addr, u32Size, TRUE));

	ret = MDrv_SERFLASH_Write(u32Addr, u32Size, pu8Src);
	ret = MDrv_SERFLASH_WriteProtect(TRUE);
	ret = MsOS_ReleaseMutex(_s32FlashMutexId);

	return ret;
}

static uc_bool _ReadFlash(uc_uint32 u32Addr, uc_uint8* pu8Dst, uc_uint32 u32Size)
{
	if(pu8Dst == NULL || u32Size == 0)
	{
		return FALSE;
	}

	return MDrv_SERFLASH_Read(u32Addr, u32Size, pu8Dst);
}

static void UniversalClient_PSTask(void)
{
	uc_byte		index;
	uc_byte		*pBuf = NULL;

	while(1)
	{
		/* Check if the dirty bit is set */
		for(index = 0; index < PS_BLK_NUM_ALL; index++)
		{
			/* If not dirty, do nothing */
			if(UC_FALSE == s_ucPSDirty[index])
			{
				continue;
			}
			else
			{
				NGLOG_DEBUG("\n===s_ucPSDirty :[%d]===\n", index);

				/* Calculate the corresponding offset in PS cache */
				pBuf = s_ucAllPSBuffer + (index * PS_BLK_SIZE);

				//simon 20121116: stop WDT
				#if (WDT_ENABLE == TRUE)
				MApi_WDT_Stop();
                #endif

				// simon: 2012.11.10 ... Add check write done
				while(FALSE == MDrv_FLASH_CheckWriteDone());

				/* flush dirty block back to PS */
				_WriteFlash(PS_BASE_ADDR + (index * PS_BLK_SIZE), pBuf, PS_BLK_SIZE);

				// simon: 2012.11.07 ... Modify timeout to per block
				/* Wait timeout of miliseconds */
				MsOS_DelayTask(PS_FLUSH_TIMEOUT);

				//simon 20121116: start WDT
				#if (WDT_ENABLE == TRUE)
				MApi_WDT_Open();
                #endif

				/* Reset dirty bit */
				s_ucPSDirty[index] = UC_FALSE;
			}
		}
	}

	return;
}

/** @defgroup psspi Cloaked CA Agent Persistent Storage SPIs
 *  Cloaked CA Agent Persistent Storage SPIs
 *
 *  These SPIs implement the Persistent Storage layer that is required in order to save contents of EMMs
 *  that are delivered to the device. This persistent storage layer must be transaction safe, such that if
 *  the device loses power in the middle of a 'write' operation, the data is not corrupted and the device
 *  can continue to function when it is powered back on.
 *
 *  \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  Please refer to \ref Note9 "Persistent storage requirements" to get more information.
 *
 *  @{
 */
/**
 * Deletes an existing resource.
 *
 * This function is called to delete an existing resource in the Persistent Store, identified by 'index'.
 * If the resource
 * cannot be found, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * The index may have previously been created by a call to ::UniversalClientSPI_PS_Write.
 * A very simple implementation of this may just delete the file corresponding with the index of the resource.
 *
 * @param[in] index Index of the resource to delete. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n
 *     Index 0 is used for CA data.\n
 *     Index 1 is used for FlexiFlash.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas.
 *     Other devices without Pre-enablement support can ignore these 3 indexes.
 *     Pre-enablement is not available to most of the devices by default.
 *
 *     New indexes may be used for other features in future.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Delete(uc_uint32 index)
{
	uc_bool	bRet = FALSE;
	uc_uint32		ulSize = 0;
	uc_uint8		ucBlock = 0, ucBlockStart = 0, ucBlockEnd = 0;

	uc_byte		*pBuf = NULL;

	/* Check if PS is inited */
	if(NULL == s_ucAllPSBuffer)
	{
		NGLOG_DEBUG("[%s %d] UC_ERROR_SUCCESS\n", __FUNCTION__, __LINE__);
		return UC_ERROR_SUCCESS;
	}

    /* Calculate the actual offset in PS cache */
	if(index == 0)
	{
        ucBlockStart = PS_BLK_START_CCA_INFO;
        ucBlockEnd = PS_BLK_START_SC_INIT;
        ulSize = (PS_BLK_NUM_CCA_INFO* PS_BLK_SIZE);
	}
	else if(index == 1)
	{
        ucBlockStart = PS_BLK_START_SC_INIT;
        ucBlockEnd = PS_BLK_START_SC_DNLD;
        ulSize = (PS_BLK_NUM_SC_INIT* PS_BLK_SIZE);
	}
	else if(index == 31)
	{
        ucBlockStart = PS_BLK_START_SC_DNLD;
        ucBlockEnd = PS_BLK_START_CFG;
        ulSize = (PS_BLK_NUM_SC_DNLD* PS_BLK_SIZE);
	}
    else if(index == 9)
	{
        ucBlockStart = PS_BLK_START_CFG;
        ucBlockEnd = PS_BLK_NUM_ALL;
        ulSize = (PS_BLK_NUM_CFG* PS_BLK_SIZE);
	}
	else
	{
        NGLOG_ERROR("[%s %d] UC_ERROR_NOT_IMPLEMENTED\n", __FUNCTION__, __LINE__);
		return UC_ERROR_NOT_IMPLEMENTED;
	}

	/* wait */
	bRet = MsOS_ObtainSemaphore(s_ucSemaphorePS, MSOS_WAIT_FOREVER);
	if(bRet == FALSE)
	{
		NGLOG_ERROR("[%s %d] UC_ERROR_OS_FAILURE\n", __FUNCTION__, __LINE__);
		return UC_ERROR_OS_FAILURE;
	}

    pBuf = s_ucAllPSBuffer + (ucBlockStart * PS_BLK_SIZE);

	// simon 20121107: Remove memory compare
	/* Copy Zero to PS cache in RAM first */
	memset(pBuf, 0x00, ulSize);

	/* Assign dirty according to index */
    for(ucBlock = ucBlockStart; ucBlock < ucBlockEnd; ucBlock++)
	{
		s_ucPSDirty[ucBlock] = UC_TRUE;
	}

	/* signal */
	bRet = MsOS_ReleaseSemaphore(s_ucSemaphorePS);
	if(bRet == FALSE)
	{
		NGLOG_ERROR("[%s %d] UC_ERROR_OS_FAILURE\n", __FUNCTION__, __LINE__);
		return UC_ERROR_OS_FAILURE;
	}

	return UC_ERROR_SUCCESS;
}

/**
 * Writes data to an existing resource.
 *
 * This function is called to write a block of data to a resource identified by an index.  The data length and offset
 * within the resource to begin writing is also specified.
 *
 * If the resource did not exist before the call, it should be created.
 *
 * A very simple implementation of this may just write the data to a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to write into. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n
 *     Index 0 is used for CA data.\n
 *     Index 1 is used for FlexiFlash.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas.
 *     Other devices without Pre-enablement support can ignore these 3 indexes.
 *     Pre-enablement is not available to most of the devices by default.
 *
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin writing. Data before this offset must not be affected.
 * @param[in] pData Buffer containing data to write to the resource. The uc_buffer_st::bytes member
 *     points to the start of the buffer, and the uc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Write(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData)
{
	uc_byte			*pBuf = NULL;
	uc_uint8			ucBlockNum = 0, ucBlockStart = 0, ucBlockEnd = 0, ucBlockLen = 0;
	uc_sint32	        blk_offset, slLength = 0;
	uc_bool		bRet = FALSE;

	/* Check if PS is inited */
	if(NULL == s_ucAllPSBuffer)
	{
		return UC_ERROR_SUCCESS;
	}

	/* Check pData */
	if(pData == NULL)
	{
		return UC_ERROR_NULL_PARAM;
	}

	/* Check index and offset and length */
	if(index == 0)
	{
        ucBlockStart = PS_BLK_START_CCA_INFO;
        ucBlockEnd = PS_BLK_START_SC_INIT;
        ucBlockLen = PS_BLK_NUM_CCA_INFO;
	}
	else if(index == 1)
	{
        ucBlockStart = PS_BLK_START_SC_INIT;
        ucBlockEnd = PS_BLK_START_SC_DNLD;
        ucBlockLen = PS_BLK_NUM_SC_INIT;
	}
	else if(index == 31)
	{
        ucBlockStart = PS_BLK_START_SC_DNLD;
        ucBlockEnd = PS_BLK_START_CFG;
        ucBlockLen = PS_BLK_NUM_SC_DNLD;
	}
    else if(index == 9)
	{
        ucBlockStart = PS_BLK_START_CFG;
        ucBlockEnd = PS_BLK_NUM_ALL;
        ucBlockLen = PS_BLK_NUM_CFG;
	}
	else
	{
		return UC_ERROR_NOT_IMPLEMENTED;
	}

    if((ucBlockLen * PS_BLK_SIZE) < offset || (ucBlockLen* PS_BLK_SIZE) < pData->length)
	{
		return UC_ERROR_INVALID_LENGTH;
	}

	/* wait */
	bRet = MsOS_ObtainSemaphore(s_ucSemaphorePS, MSOS_WAIT_FOREVER);
	if(bRet == FALSE)
	{
		return UC_ERROR_OS_FAILURE;
	}

	//simon 20121116: stop WDT
	#if (WDT_ENABLE == TRUE)
	MApi_WDT_Stop();
    #endif

	//simon 20121106: modify blk_num & offset calculation
	/* Calculate the actual offset in PS cache */
	blk_offset = offset % PS_BLK_SIZE;
    
    ucBlockNum = (offset / PS_BLK_SIZE) + ucBlockStart;
	pBuf = s_ucAllPSBuffer + (ucBlockStart * PS_BLK_SIZE) + offset;

	/* Compare the data from write buffer and data from PS cache */
	/* If not equal, then do something */
	/* If equal, skip all this */
	if(0 != memcmp(pBuf, pData->bytes, pData->length))
	{
		/* Copy to PS cache in RAM first */
		memcpy(pBuf, pData->bytes, pData->length);

		s_ucPSDirty[ucBlockNum] = UC_TRUE;

		/* Assign dirty according to blk_num & write length */
		slLength = pData->length;
		for(; ucBlockNum < ucBlockEnd; ucBlockNum++)
		{
			if(PS_BLK_SIZE < slLength + blk_offset)
			{
				s_ucPSDirty[ucBlockNum + 1] = UC_TRUE;
				slLength -= PS_BLK_SIZE;
			}
			else
			{
				break;
			}
		}//for(; ucBlockNum < ((index == 0)?PS_BLK_NUM_INDEX0:PS_BLK_NUM); ucBlockNum++)
	}//if(0 != memcmp(pBuf, pData->bytes, pData->length))

	//simon 20121116: start WDT
	#if (WDT_ENABLE == TRUE)
	MApi_WDT_Open();
    #endif

	/* signal */
	bRet = MsOS_ReleaseSemaphore(s_ucSemaphorePS);
	if(bRet == FALSE)
	{
		return UC_ERROR_OS_FAILURE;
	}

	return UC_ERROR_SUCCESS;
}

/**
 * Reads data from an existing resource.
 *
 * This function is called to read a block of data from a resource identified by an index. The data length and offset
 * within the resource to begin reading is also specified.
 *
 * If the resource did not exist before the call, the function should return UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * A very simple implementation of this may just read the data from a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to read from. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n
 *     Index 0 is used for CA data.\n
 *     Index 1 is used for FlexiFlash.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas.
 *     Other devices without Pre-enablement support can ignore these 3 indexes.
 *     Pre-enablement is not available to most of the devices by default.
 *
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin reading. Data before this offset must not be affected.
 * @param[in,out] pData Buffer to read resource data into.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member
 *     to the length of the buffer, in bytes.  The function will read uc_buffer_st::length bytes from the resource
 *     and store them in uc_buffer_st::bytes.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Read(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData)
{
	uc_byte		*pBuf = NULL;
	uc_bool     bRet = FALSE;
    uc_uint8		ucBlockStart = 0, ucBlockEnd = 0, ucBlockLen = 0;

	/* Check if PS is inited */
	if(NULL == s_ucAllPSBuffer)
	{
		NGLOG_DEBUG(" UC_ERROR_OUT_OF_MEMORY\n",);
 		return UC_ERROR_OUT_OF_MEMORY;
	}

	/* Check pData */
	if(pData == NULL)
	{
		NGLOG_DEBUG("UC_ERROR_NULL_PARAM\n");
 		return UC_ERROR_NULL_PARAM;
	}

    /* Check index and offset and length */
	if(index == 0)
	{
        ucBlockStart = PS_BLK_START_CCA_INFO;
        ucBlockEnd = PS_BLK_START_SC_INIT;
        ucBlockLen = PS_BLK_NUM_CCA_INFO;
	}
	else if(index == 1)
	{
        ucBlockStart = PS_BLK_START_SC_INIT;
        ucBlockEnd = PS_BLK_START_SC_DNLD;
        ucBlockLen = PS_BLK_NUM_SC_INIT;
	}
	else if(index == 31)
	{
        ucBlockStart = PS_BLK_START_SC_DNLD;
        ucBlockEnd = PS_BLK_START_CFG;
        ucBlockLen = PS_BLK_NUM_SC_DNLD;
	}
    else if(index == 9)
	{
        ucBlockStart = PS_BLK_START_CFG;
        ucBlockEnd = PS_BLK_NUM_ALL;
        ucBlockLen = PS_BLK_NUM_CFG;
	}
	else
	{
		return UC_ERROR_NOT_IMPLEMENTED;
	}

    if((ucBlockLen * PS_BLK_SIZE) < offset || (ucBlockLen* PS_BLK_SIZE) < pData->length)
	{
		return UC_ERROR_INVALID_LENGTH;
	}

#if (CA_CCA_PS_CACHE_SYNC == 1)
    /* wait PS task writing the data back to flash before read*/
    uc_uint8 ucBlockNum, cnt;
    ucBlockNum = (offset / PS_BLK_SIZE) + ucBlockStart;
    cnt= 0;

	while((ucBlockNum < ucBlockEnd) && cnt < ucBlockLen*PS_FLUSH_TIMEOUT/100)
	{
		if(s_ucPSDirty[ucBlockNum] == UC_TRUE)
        {
            ucBlockNum = (offset / PS_BLK_SIZE) + ucBlockStart;
            MsOS_DelayTask(100);
            cnt ++;
        }
        else
        {
            ucBlockNum++;
        }
	}
#endif    

	/* wait */
	bRet = MsOS_ObtainSemaphore(s_ucSemaphorePS, MSOS_WAIT_FOREVER);
	if(bRet == FALSE)
	{
		NGLOG_DEBUG("[%s %d] UC_ERROR_OS_FAILURE::index=%d, offset=%d\n", __FUNCTION__, __LINE__, index, offset);
 		return UC_ERROR_OS_FAILURE;
	}

	/* Calculate the actual offset in PS cache */
    pBuf = s_ucAllPSBuffer + (ucBlockStart * PS_BLK_SIZE) + offset;

	/* Copy from PS cache in RAM */
	memcpy(pData->bytes, pBuf, pData->length);


	/* signal */
	bRet |= MsOS_ReleaseSemaphore(s_ucSemaphorePS);
	if(bRet == FALSE)
	{
		
 		return UC_ERROR_OS_FAILURE;
	}

	return UC_ERROR_SUCCESS;
}

/**
 * Performs any initialization of the persistent storage layer
 *
 * This function is called during to allow any initialization of the persistent storage layer to occur
 * before any calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete are made.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Initialize(void)
{
    uc_bool		retFlash = FALSE;
	uc_uint8		index = 0;
    
   	//DUMP THE SPI ARE
	NGLOG_DEBUG("\n\nCCA SPI AREA USAGE\n");
    NGLOG_DEBUG("_M_FLASH_CCA_DATA1_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_DATA1_ADDR_);
    NGLOG_DEBUG("_M_FLASH_CCA_SC_INIT_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC_INIT_ADDR_);
    NGLOG_DEBUG("_M_FLASH_CCA_SC1_INIT_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC1_INIT_ADDR_);
    NGLOG_DEBUG("_M_FLASH_CCA_SC_DNLD_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC_DNLD_ADDR_);
    NGLOG_DEBUG("_M_FLASH_CCA_SC1_DNLD_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC1_DNLD_ADDR_);
    NGLOG_DEBUG("_M_FLASH_CCA_CFG_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_CFG_ADDR_);
    NGLOG_DEBUG("_M_FLASH_CCA_SOFTCELL_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SOFTCELL_ADDR_);
	NGLOG_DEBUG("CCA SPI AREA USAGE\n\n");

    s_ucSemaphorePS = MsOS_CreateSemaphore(1, E_MSOS_FIFO, NULL);
    if(s_ucSemaphorePS < 0)
    {
        return UC_ERROR_UNABLE_TO_CREATE;
    }
    MDrv_SERFLASH_Init();
    gstSpiInfo = MDrv_SERFLASH_GetInfo();

    
    /* Reset Dirty bit */
	for(index = 0; index < PS_BLK_NUM_ALL; ++index)
	{
		s_ucPSDirty[index] = FALSE;
	}

	/* Allocate Cache for PS */
	s_ucAllPSBuffer = AP_NC_Alloc(PS_BLK_SIZE * PS_BLK_NUM_ALL);
	if(NULL == s_ucAllPSBuffer)
	{
		return UC_ERROR_OUT_OF_MEMORY;
	}

    for(index = 0; index < PS_BLK_NUM_ALL; index++)
    {
        retFlash = _ReadFlash(PS_BASE_ADDR + (index * PS_BLK_SIZE), s_ucAllPSBuffer + (index * PS_BLK_SIZE), PS_BLK_SIZE);
        
    	if(retFlash == FALSE)
    	{
    		return UC_ERROR_RESOURCE_NOT_FOUND;
    	}
    }

#if 0
	//simon 20121106: dump flash
	uc_byte i;
	for(i = 0; i < 32; i = i + 8)
	{
		printf("\n [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] \n",
			s_ucAllPSBuffer[i], s_ucAllPSBuffer[i+1], s_ucAllPSBuffer[i+2], s_ucAllPSBuffer[i+3],
			s_ucAllPSBuffer[i+4], s_ucAllPSBuffer[i+5], s_ucAllPSBuffer[i+6], s_ucAllPSBuffer[i+7]);
	}
#endif

	/* Create PS flush task */
	s_ucPSTaskID = MsOS_CreateTask(
							(TaskEntry)UniversalClient_PSTask,
							(uc_uint32)0,
                       		E_TASK_PRI_LOW,
							TRUE,
							s_ucPSStack,
							PS_FLUSH_CHK_TASK_STK_SIZE,
							"PSFlashCheck"
							);

	if(s_ucPSTaskID < 0)
	{
		MsOS_DeleteSemaphore(s_ucSemaphorePS);
		s_ucSemaphorePS = -1;

		return UC_ERROR_OS_FAILURE;
	}

    return UC_ERROR_SUCCESS;
}



/**
 * Performs any termination of the persistent storage layer
 *
 * This function is called during to allow any clean up of the persistent storage layer done by ::UniversalClientSPI_PS_Initialize to occur.
 * No calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete will be made after this function
 * returns.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Terminate(void)
{
    uc_result		ret;
    /* Free PS cache */
	AP_NC_Free(s_ucAllPSBuffer);
	s_ucAllPSBuffer = NULL;

	/* Delete Semaphore for PS */
	ret = UniversalClientSPI_Semaphore_Close((uc_semaphore_handle *)s_ucSemaphorePS);
	if(ret != UC_ERROR_SUCCESS)
	{
		return ret;
	}
	s_ucSemaphorePS = -1;

	/* Delete PS flush task */
	ret = MsOS_DeleteTask(s_ucPSTaskID);
	if (FALSE == ret)
	{
		return UC_ERROR_OS_FAILURE;
	}
	s_ucPSTaskID = -1;
    
    return UC_ERROR_SUCCESS;
}
/** @} */ /* End of ps */
#else
/** @defgroup psspi Cloaked CA Agent Persistent Storage SPIs
 *  Cloaked CA Agent Persistent Storage SPIs
 *
 *  These SPIs implement the Persistent Storage layer that is required in order to save contents of EMMs
 *  that are delivered to the device. This persistent storage layer must be transaction safe, such that if
 *  the device loses power in the middle of a 'write' operation, the data is not corrupted and the device
 *  can continue to function when it is powered back on.
 *
 *  \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  Please refer to \ref Note9 "Persistent storage requirements" to get more information.
 *
 *  @{
 */
/**
 * Deletes an existing resource.
 *
 * This function is called to delete an existing resource in the Persistent Store, identified by 'index'.
 * If the resource
 * cannot be found, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * The index may have previously been created by a call to ::UniversalClientSPI_PS_Write.
 * A very simple implementation of this may just delete the file corresponding with the index of the resource.
 *
 * @param[in] index Index of the resource to delete. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n
 *     Index 0 is used for CA data.\n
 *     Index 1 is used for FlexiFlash.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas.
 *     Other devices without Pre-enablement support can ignore these 3 indexes.
 *     Pre-enablement is not available to most of the devices by default.
 *
 *     New indexes may be used for other features in future.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Delete(uc_uint32 index)
{
    
    uc_uint32 u32BaseAddr = 0;
    uc_uint32 u32Size = 0;
    uc_bool bRet = FALSE;

    switch(index)
    {
        case 0:
        {
            u32BaseAddr = _M_FLASH_CCA_DATA1_ADDR_;
            u32Size = _M_FLASH_CCA_DATA1_SIZE_;
        }
        break;
        case 1:
        {
            u32BaseAddr = _M_FLASH_CCA_SC_INIT_ADDR_;
            u32Size = _M_FLASH_CCA_SC_INIT_SIZE_;
        }
        break;
		case 2:
        {
			#if (CA_CCA_SECOND_SEC_CORE_SUP == 0)
			NGLOG_ERROR("NOT SUPPORT UniversalClientSPI_PS_Delete index 0x%x\n",index  );
			return UC_ERROR_NOT_IMPLEMENTED;
			#endif		
            u32BaseAddr = _M_FLASH_CCA_SC1_INIT_ADDR_;
            u32Size = _M_FLASH_CCA_SC1_INIT_SIZE_;
        }
        break;
        case 9:
        {
            u32BaseAddr = _M_FLASH_CCA_CFG_ADDR_;
            u32Size = _M_FLASH_CCA_CFG_SIZE_;
        }
        break;
        case 20:
        {
            u32BaseAddr = _M_FLASH_CCA_SOFTCELL_ADDR_;
            u32Size = _M_FLASH_CCA_SOFTCELL_SIZE_;
        }
        break;
        case 31:
        {
            u32BaseAddr = _M_FLASH_CCA_SC_DNLD_ADDR_;
            u32Size = _M_FLASH_CCA_SC_DNLD_SIZE_;
        }
        break;
		case 32:
        {
			#if (CA_CCA_SECOND_SEC_CORE_SUP == 0)
			NGLOG_ERROR("NOT SUPPORT UniversalClientSPI_PS_Delete index 0x%x\n",index  );
			return UC_ERROR_NOT_IMPLEMENTED;
			#endif	
            u32BaseAddr = _M_FLASH_CCA_SC1_DNLD_ADDR_;
            u32Size = _M_FLASH_CCA_SC1_DNLD_SIZE_;
        }
        break;
        case 51:
        {

            u32BaseAddr = _M_FLASH_CCA_IFCP1_ADDR_;
            u32Size = _M_FLASH_CCA_IFCP1_SIZE_;
        }
        break;
        case 61:
        {

            u32BaseAddr = _M_FLASH_CCA_IFCP2_ADDR_;
            u32Size = _M_FLASH_CCA_IFCP2_SIZE_;
        }
        break;
        default:
        {
            NGLOG_ERROR("Unsupported index %d\n",index);
            return UC_ERROR_RESOURCE_NOT_FOUND;
        }
        break;
    }
	FILE *file;

	char *p=GxCore_Mallocz(u32Size);
	if(p==0)
	{
		NGLOG_ERROR("mallocz failure\n");
		return 1;
	}
	file=fopen(CONFIG_FOR_NGL_CCA_PATH,"rb+");
	fseek(file,u32BaseAddr,SEEK_SET);
	bRet=fwrite(p,u32Size,1,file);
	fclose(file);
	GxCore_Free(p);

    if(bRet == FALSE)
    {
        return UC_ERROR_OS_FAILURE;
    }
    return UC_ERROR_SUCCESS;
}

/**
 * Writes data to an existing resource.
 *
 * This function is called to write a block of data to a resource identified by an index.  The data length and offset
 * within the resource to begin writing is also specified.
 *
 * If the resource did not exist before the call, it should be created.
 *
 * A very simple implementation of this may just write the data to a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to write into. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n
 *     Index 0 is used for CA data.\n
 *     Index 1 is used for FlexiFlash.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas.
 *     Other devices without Pre-enablement support can ignore these 3 indexes.
 *     Pre-enablement is not available to most of the devices by default.
 *
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin writing. Data before this offset must not be affected.
 * @param[in] pData Buffer containing data to write to the resource. The uc_buffer_st::bytes member
 *     points to the start of the buffer, and the uc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Write(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData)
{    
    uc_uint32 u32BaseAddr = 0;
    uc_bool bRet = FALSE;
    uc_uint8 *pu8TmpBuf = NULL;
    uc_uint32 u32StartBlock = 0;
    uc_uint32 u32EndBlock = 0;
    uc_uint32 u32BlockStartAddr = 0;
    uc_uint32 u32BlockStartShift = 0;
    uc_uint32 u32BufSize = 0;

    switch(index)
    {
        case 0:
        {
            u32BaseAddr = _M_FLASH_CCA_DATA1_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_DATA1_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 1:
        {
            u32BaseAddr = _M_FLASH_CCA_SC_INIT_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC_INIT_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
		case 2:
        {
			#if (CA_CCA_SECOND_SEC_CORE_SUP == 0)
			NGLOG_ERROR("NOT SUPPORT UniversalClientSPI_PS_Delete index 0x%x\n",index  );
			return UC_ERROR_NOT_IMPLEMENTED;
			#endif	
            u32BaseAddr = _M_FLASH_CCA_SC1_INIT_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC1_INIT_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 9:
        {
            u32BaseAddr = _M_FLASH_CCA_CFG_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_CFG_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 20:
        {
            u32BaseAddr = _M_FLASH_CCA_SOFTCELL_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SOFTCELL_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 31:
        {
            u32BaseAddr = _M_FLASH_CCA_SC_DNLD_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC_DNLD_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
		case 32:
        {
			#if (CA_CCA_SECOND_SEC_CORE_SUP == 0)
			NGLOG_ERROR("NOT SUPPORT UniversalClientSPI_PS_Delete index 0x%x\n",index  );
			return UC_ERROR_NOT_IMPLEMENTED;
			#endif		
			
            u32BaseAddr = _M_FLASH_CCA_SC1_DNLD_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC1_DNLD_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
       case 51:
        {
            u32BaseAddr = _M_FLASH_CCA_IFCP1_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_IFCP1_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 61:
        {
            u32BaseAddr = _M_FLASH_CCA_IFCP2_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_IFCP2_SIZE_)
            {
                NGLOG_ERROR("Write PS exceed partion size\n");
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        default:
        {
            NGLOG_ERROR("Unsupported index %d\n",index);
            return UC_ERROR_RESOURCE_NOT_FOUND;
        }
        break;
    }

	FILE *file;
	file=fopen(CONFIG_FOR_NGL_CCA_PATH,"rb+");
	fseek(file,u32BaseAddr+offset,SEEK_SET);
	bRet=fwrite(pData->bytes,pData->length,1,file);
	fclose(file);

    if(bRet <= 0)
    {
        return UC_ERROR_OS_FAILURE;
    }
    
    return UC_ERROR_SUCCESS;
}

/**
 * Reads data from an existing resource.
 *
 * This function is called to read a block of data from a resource identified by an index. The data length and offset
 * within the resource to begin reading is also specified.
 *
 * If the resource did not exist before the call, the function should return UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * A very simple implementation of this may just read the data from a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to read from. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n
 *     Index 0 is used for CA data.\n
 *     Index 1 is used for FlexiFlash.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas.
 *     Other devices without Pre-enablement support can ignore these 3 indexes.
 *     Pre-enablement is not available to most of the devices by default.
 *
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin reading. Data before this offset must not be affected.
 * @param[in,out] pData Buffer to read resource data into.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member
 *     to the length of the buffer, in bytes.  The function will read uc_buffer_st::length bytes from the resource
 *     and store them in uc_buffer_st::bytes.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Read(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData)
{
    
    uc_uint32 u32BaseAddr = 0;
    uc_bool bRet = FALSE;
    uc_uint8 *pu8TmpBuf = NULL;

    switch(index)
    {
        case 0:
        {
            u32BaseAddr = _M_FLASH_CCA_DATA1_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_DATA1_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 1:
        {
            u32BaseAddr = _M_FLASH_CCA_SC_INIT_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC_INIT_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 2:
        {

			#if (CA_CCA_SECOND_SEC_CORE_SUP == 0)
			NGLOG_ERROR("NOT SUPPORT UniversalClientSPI_PS_Delete index 0x%x\n",index  );
			return UC_ERROR_NOT_IMPLEMENTED;
			#endif	
			
            u32BaseAddr = _M_FLASH_CCA_SC1_INIT_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC1_INIT_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;		
        case 9:
        {
            u32BaseAddr = _M_FLASH_CCA_CFG_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_CFG_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 20:
        {
            u32BaseAddr = _M_FLASH_CCA_SOFTCELL_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SOFTCELL_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 31:
        {
            u32BaseAddr = _M_FLASH_CCA_SC_DNLD_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC_DNLD_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 32:
        {

			#if (CA_CCA_SECOND_SEC_CORE_SUP == 0)
			NGLOG_DEBUG("NOT SUPPORT UniversalClientSPI_PS_Delete index 0x%x\n",index  );
			return UC_ERROR_NOT_IMPLEMENTED;
			#endif
			
            u32BaseAddr = _M_FLASH_CCA_SC1_DNLD_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_SC1_DNLD_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;		
        case 51:
        {
            u32BaseAddr = _M_FLASH_CCA_IFCP1_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_IFCP1_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        case 61:
        {
            u32BaseAddr = _M_FLASH_CCA_IFCP2_ADDR_;
            if((offset+(pData->length)) > _M_FLASH_CCA_IFCP2_SIZE_)
            {
                
                return UC_ERROR_OUT_OF_MEMORY;
            }
        }
        break;
        default:
        {
            NGLOG_DEBUG("Unsupported index %d\n",index);
            return UC_ERROR_RESOURCE_NOT_FOUND;
        }
        break;
    }
    pu8TmpBuf = GxCore_Mallocz(pData->length);
#if 0	
    bRet = MsOS_ObtainMutex(_s32FlashMutexId, MSOS_WAIT_FOREVER);
    if(bRet == FALSE)
    {
        
        return UC_ERROR_DRIVER_FAILURE;
    }
#endif	
	FILE *file;
	file=fopen(CONFIG_FOR_NGL_CCA_PATH,"rb");
	fseek(file,u32BaseAddr+offset,SEEK_SET);
	bRet=fread(pu8TmpBuf,pData->length,1,file);
	fclose(file);
	
    //printf("\n %s:   0x%x\n",__FUNCTION__,bRet);
#if 0
	if(9==index)//cfg file
	{
		int i = 0;
		while(i<pData->length)
		{
			printf("%d  0x%x  \n",i,pu8TmpBuf[i]);
			i++;
		}
	}
#endif
    //bRet = MsOS_ReleaseMutex(_s32FlashMutexId);
    memcpy(pData->bytes,pu8TmpBuf,pData->length);
    GxCore_Free(pu8TmpBuf);
	pu8TmpBuf = NULL;
	#if 0
    if(bRet < 0)
    {
        
        return UC_ERROR_OS_FAILURE;
    }
	#endif
    
    return UC_ERROR_SUCCESS;
}

uc_result UniversalClientSPI_PS_GetProperty(uc_uint32 index, uc_ps_property *pProperty)
{
		
	switch(index)
		{
			case 0:
			{
				pProperty->reservedSize = _M_FLASH_CCA_DATA1_SIZE_/0x400;//kbytes
			}
			break;
			case 1:
			{
				pProperty->reservedSize = _M_FLASH_CCA_SC_INIT_SIZE_/0x400;
			}
			break;
			case 2:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_SC1_INIT_SIZE_/0x400;
			}
			break;		
			case 9:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_CFG_SIZE_/0x400;
			}
			break;
			case 20:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_SOFTCELL_SIZE_/0x400;
			}
			break;
			case 31:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_SC_DNLD_SIZE_/0x400;
			}
			break;
			case 32:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_SC1_DNLD_SIZE_/0x400;
			}
			break;		
			case 51:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_IFCP1_SIZE_/0x400;
			}
			break;
			case 61:
			{
				pProperty->reservedSize =  _M_FLASH_CCA_IFCP2_SIZE_/0x400;
			}
			break;
			default:
			{
				NGLOG_DEBUG("Unsupported index %d\n",index);
				return UC_ERROR_RESOURCE_NOT_FOUND;
			}
			break;
		}
    
	return UC_ERROR_SUCCESS;
}

/**
 * Performs any initialization of the persistent storage layer
 *
 * This function is called during to allow any initialization of the persistent storage layer to occur
 * before any calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete are made.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */

uc_result UniversalClientSPI_PS_Initialize(void)
{
    
	//DUMP THE SPI ARE
	NGLOG_VERBOSE("\n\nCCA SPI AREA USAGE\n");
    NGLOG_VERBOSE("_M_FLASH_CCA_DATA1_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_DATA1_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_SC_INIT_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC_INIT_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_SC1_INIT_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC1_INIT_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_SC_DNLD_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC_DNLD_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_SC1_DNLD_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SC1_DNLD_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_CFG_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_CFG_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_SOFTCELL_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_SOFTCELL_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_IFCP1_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_IFCP1_ADDR_);
    NGLOG_VERBOSE("_M_FLASH_CCA_IFCP2_ADDR_ = %08lX \n", (uc_uint32)_M_FLASH_CCA_IFCP2_ADDR_);
	NGLOG_VERBOSE("CCA SPI AREA USAGE\n\n");
#if 0
	GxCore_MutexCreate(&_s32FlashMutexId);
    if(_s32FlashMutexId < 0)
    {
        return UC_ERROR_UNABLE_TO_CREATE;
    }
#endif	
    //MDrv_SERFLASH_Init();
   // gstSpiInfo = MDrv_SERFLASH_GetInfo();
   
	FILE *file;
	
	file=fopen(CONFIG_FOR_NGL_CCA_PATH,"ab+");
	#if 0
	if(file==0)
	{	file=fopen(CONFIG_FOR_NGL_CCA_PATH,"wb+");
		char *p;
		p=GxCore_Mallocz(SECTOR_LEN);
		int i;
		int sector_num= FILE_MAX_LEN/SECTOR_LEN;
		for(i=0;i<sector_num;i++)
		{
			fwrite(p,SECTOR_LEN,1,file);
		}
		
		GxCore_Free(p);
	}
	#endif
	fclose(file);
    return UC_ERROR_SUCCESS;
}
/**
 * Performs any termination of the persistent storage layer
 *
 * This function is called during to allow any clean up of the persistent storage layer done by ::UniversalClientSPI_PS_Initialize to occur.
 * No calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete will be made after this function
 * returns.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PS_Terminate(void)
{
    return UC_ERROR_SUCCESS;
}
/** @} */ /* End of ps */
#endif

/** @defgroup devicespi Cloaked CA Agent Device SPIs
 *  Cloaked CA Agent Device SPIs
 *
 *  These SPIs are responsible for retrieving values that are provisioned onto a device at manufacture time,
 *  and for setting any hardware specific parameters in the device.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */

uc_result UniversalClientSPI_Device_GetDeviceID(uc_buffer_st *pData)
{
    
    if(pData == NULL || pData->bytes == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

	if(pData->length<4)// cssn 有效字节�?
	{
		pData->length = 4;
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}
    if(s_spi_ucSystem.setDeviceId== UC_FALSE)
    {
        uc_bool     RestCa = FALSE;
        uc_uint8       strDID[CSSN_DATA_SIZE];
        memset(&strDID[0], 0x00, sizeof(&strDID[0]));

		RestCa = GxFuse_GetCSSN(&strDID[0],CSSN_DATA_SIZE);
        if(RestCa == -1)
        {
            return UC_ERROR_OS_FAILURE;
        }
		if(0==RestCa)
		{
			memset(&s_spi_ucSystem.strDeviceId[0], 0x00, CSSN_DATA_SIZE);	
			memcpy(&s_spi_ucSystem.strDeviceId[0], &strDID[0], CSSN_DATA_SIZE);
		}
		//printf("[%d] [%x][%x][%x][%x][%x][%x][%x][%x] cssn[%llx]",sizeof(cssn),buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],cssn);
        s_spi_ucSystem.setDeviceId = UC_TRUE;

    }

    memset(&pData->bytes[0], 0x00, CSSN_DATA_SIZE);
    memcpy(&pData->bytes[0], &s_spi_ucSystem.strDeviceId[0], pData->length);

    NGLOG_DEBUG(">>>>DeviceID::0x%02X 0x%02X 0x%02X 0x%02X\n",
                pData->bytes[0], pData->bytes[1], pData->bytes[2], pData->bytes[3]);

    return UC_ERROR_SUCCESS;
}



/**
 * Gets the Secure Chipset Serial Number
 *
 * This function is called to retrieve the secure chipset serial number associated with the device.
 * This serial number is used to deliver EMMs addressed to this particular instance of a client.
 *
 * @param[in] pData Buffer to read serial number data into.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the secure chipset serial number, and set the uc_buffer_st::length member
 *     to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetCSSN(uc_buffer_st *pData)
{
  	
    if(pData == NULL || pData->bytes == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

	if(pData->length<4)// cssn 有效字节�?
	{
		pData->length = 4;
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}
	
    if(s_spi_ucSystem.setSecure == UC_FALSE)
    {
        uc_bool     RestCa = TRUE;
        uc_uint8       strCSSN[CSSN_DATA_SIZE];

        memset(strCSSN, 0x00, CSSN_DATA_SIZE);

		RestCa = GxFuse_GetCSSN(strCSSN,CSSN_DATA_SIZE);
        if(RestCa == -1)
        {
            return UC_ERROR_OS_FAILURE;
        }
		if(0==RestCa)
		{
       	 	memset(s_spi_ucSystem.strCSSN, 0x00, CSSN_DATA_SIZE);
		#if 1
			memcpy(s_spi_ucSystem.strCSSN, strCSSN+4, CSSN_DATA_SIZE);
		#else
			s_spi_ucSystem.strCSSN[0] = strCSSN[7];
        	s_spi_ucSystem.strCSSN[1] = strCSSN[6];
        	s_spi_ucSystem.strCSSN[2] = strCSSN[5];
        	s_spi_ucSystem.strCSSN[3] = strCSSN[4];
		#endif
        	s_spi_ucSystem.setSecure = UC_TRUE;
		}
    }
    memset(pData->bytes, 0x00, pData->length);
    memcpy(pData->bytes, s_spi_ucSystem.strCSSN, pData->length);

#ifdef C_Debug
	int i;
	NGLOG_DEBUG("\n >>>>GetCSSN:");
	for (i = 0; i<pData->length; i++)
	{
		printf("\n 0x%02x  0x%02x ",pData->bytes[i],s_spi_ucSystem.strCSSN[i]);
	}
	NGLOG_DEBUG(" >>>>GetCSSN:  end \n");
#endif
    return UC_ERROR_SUCCESS;
}

/**
 * Gets the device private data
 *
 * This returns the personalized device specific private data.  This is an optional SPI, the device may not implement it
 * if no such private data is available.
 *
 * \note Currently, device private data is 16 bytes long. These data are provided by manufacturer and need to be personalized into device persistently.
 * Devices that are for demo purpose or for test purpose may NOT have private data at the first beginning, in such case, this SPI implementation
 * can just set the pData->length to ZERO and return UC_ERROR_SUCCESS directly.

 *
 * @param[in] pData Buffer to read private data into.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the private data, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetPrivateData(uc_buffer_st * pData)
{
    
	//CB9359AB6FE0B07452054771153FE948
	char buffer[16]={0xCB,0x93,0x59,0xAB,0x6F,0xE0,0xB0,0x74,0x52,0x05,0x47,0x71,0x15,0x3F,0xE9,0x48};
	//pData->bytes = buffer;
	memset(pData->bytes,0,16);
	memcpy(pData->bytes,buffer,16);
	pData->length = 16;
	return UC_ERROR_SUCCESS;
}
/**
 * Sets the Secure Chipset Session Key
 *
 * This function is called to provision a secure descrambler chipset with a secure session key.  This session key is
 * used to protect descrambler keys that are delivered to the system by further encrypting them with another
 * key that only this device knows about.
 *
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 *    \n
 *
 * \note Whether the chipset needs to use secure control word mode or clear control word mode is not decided by CSSK.
 *    Once a client with secure chipset has been initialized, Cloaked CA Agent will always set the CSSK to the chipset at startup or on CSSK update.
 *    The driver must judge current working mode using the control word type from ::UniversalClientSPI_Stream_SetDescramblingKey.
 *    In order to support 911 mode, client device with secure chipset must support to switch the working mode dynamically.
 *
 *
 * @param[in] pKeyMaterial CSSK key material to provision into the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains
 *     the number of bytes in the buffer.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_SetCSSK(const uc_buffer_st * pKeyMaterial)
{
    return UC_ERROR_SUCCESS;
}

/**
 * Sets the Secure Chipset Session Key With key Infomation to support multi key ladder.
 *
 * This is the extension of ::UniversalClientSPI_Device_SetCSSK function with additional key protection infomation provided,
 * in order to support differents key ladder.This session key is used to protect descrambler keys that are delivered to the
 * system by further encrypting them with another key that only this device knows about.
 *
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 * @param[in] pCSSKInfo Information about the CSSK being used to protected the descrambling key. The CSSK is also protected by CSUK.
 *     This contain CSSK protection and CSSK material. See ::uc_cssk_info.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_SetExtendedCSSK(const uc_cssk_info * pCSSKInfo)
{
    
    if(pCSSKInfo == NULL || pCSSKInfo->pCSSK == NULL || pCSSKInfo->pCSSK->bytes == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    if(pCSSKInfo->pCSSK->length == 0 || pCSSKInfo->pCSSK->length > CSSK_DATA_SIZE )
    {
        return UC_ERROR_INVALID_LENGTH;
    }

#if(CA_CCA_PVR_SUP == 1)
    //TODO CA PVR: For CPAK and CPSK, used in secure PVR solution as {CPSK}cpuk, {PVRMSK}cpsk, 
    //you may use with different setting UC_DK_LADDER_PVR and UC_DK_LADDER_PVR_L2
#endif

    s_spi_ucSystem.setCSSK = UC_TRUE;
    s_spi_ucSystem.typeCSSK = pCSSKInfo->KeyProtection;
	s_spi_ucSystem.lenCSSK = pCSSKInfo->pCSSK->length;
    memset(&s_spi_ucSystem.strCSSK[0], 0x00, sizeof(&s_spi_ucSystem.strCSSK[0]));
    memcpy(&s_spi_ucSystem.strCSSK[0], pCSSKInfo->pCSSK->bytes, pCSSKInfo->pCSSK->length);
    NGLOG_DEBUG(" pKeyMaterial->bytes= %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X\n",
                s_spi_ucSystem.strCSSK[0], s_spi_ucSystem.strCSSK[1], s_spi_ucSystem.strCSSK[2], s_spi_ucSystem.strCSSK[3],\
                s_spi_ucSystem.strCSSK[4], s_spi_ucSystem.strCSSK[5], s_spi_ucSystem.strCSSK[6], s_spi_ucSystem.strCSSK[7],\
				s_spi_ucSystem.strCSSK[8], s_spi_ucSystem.strCSSK[9], s_spi_ucSystem.strCSSK[10], s_spi_ucSystem.strCSSK[11],\
				s_spi_ucSystem.strCSSK[12], s_spi_ucSystem.strCSSK[13], s_spi_ucSystem.strCSSK[14], s_spi_ucSystem.strCSSK[15]);
    
    return UC_ERROR_SUCCESS;
}
/**
 * Gets device's security state
 *
 * This function is called to retrieve security state information from a device.
 * Cloaked CA Agent will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record.
 *
 * @param[in,out] pDeviceSecurityState Secure chipset working information to be filled in by the platform.
 *
 * See ::uc_device_security_state for the detail information needed.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetSecurityState(uc_device_security_state *pDeviceSecurityState)
{
#if 1
    if(pDeviceSecurityState == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }
	
#if 0
    uc_bool         RestCa = FALSE;

    RestCa = MDrv_CA_Init();

    if(RestCa == FALSE)
    {
        return UC_ERROR_OS_FAILURE;
    }


    {
        CA_BOOT_MODE        retBootMode = E_CA_BOOT_OTP;
        retBootMode = MDrv_CA_BootMode();//boot from OTP, flash, SPI ...etc.
    }


    pDeviceSecurityState->rsaMode = MDrv_CA_OTP_IsSecureBootEnabled();
    pDeviceSecurityState->cwMode = MDrv_CA_OTP_IsSecureCWMode();

    RestCa = MDrv_CA_OTP_GetDebugPortMode(E_CA_DEBUG_PORT_JTAG, &pDeviceSecurityState->jtag);

    if(RestCa == FALSE)
    {
        pDeviceSecurityState->jtag = 1;
        NGLOG_DEBUG("jtag=%d\n", pDeviceSecurityState->jtag);
    }
#endif

    //#if (CA_CCA_LOADER_TEST_PARAM == 1)
	pDeviceSecurityState->rsaMode = 0;
	pDeviceSecurityState->cwMode = 0;
	pDeviceSecurityState->jtag = 0;
	pDeviceSecurityState->crypto = 0x33;// 0x11 for TDES in 2 layer keyladder, 0x22 for AES in 2 layer keyladder
    //#endif

    //add for IFCP feature enable
    pDeviceSecurityState->modeIFCP = 1; //if chip support it~

    NGLOG_DEBUG("rsaMode=0x%x, cwMode=0x%x , jTag:0x%x , crypto:0x%x modeIFCP:0x%x\n", pDeviceSecurityState->rsaMode, pDeviceSecurityState->cwMode , pDeviceSecurityState->jtag ,pDeviceSecurityState->crypto,pDeviceSecurityState->modeIFCP);
    {
        s_spi_ucSystem.rsaMode = pDeviceSecurityState->rsaMode;
        s_spi_ucSystem.cwMode = pDeviceSecurityState->cwMode;
        s_spi_ucSystem.jtag  =  pDeviceSecurityState->jtag;
		s_spi_ucSystem.crypto  =  pDeviceSecurityState->crypto = 0x33;//0x66;  //org is 0x11      
        s_spi_ucSystem.modeIFCP  =  pDeviceSecurityState->modeIFCP;
    }
#endif
    return UC_ERROR_SUCCESS;
}

#if(CA_CCA_PVR_SUP == 1)
/**
 * Retrieves the PVR key ladder security state in secure chipset
 *
 * This function is called to retrieve security state information of a PVR key ladder.
 * \note This SPI is needed only if the device supports PVR key ladder and PVR is enabled. If the device doesn't support PVR key ladder or
 * PVR is not enabled, no need to support this SPI.
 * Cloaked CA Agent will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record. 
 *
 * @param[in,out] pPVRSecurityState Device application must fill this structure with the PVR key ladder working information in secure chipset.
 *
 * See ::uc_pvr_security_state for the detail information needed.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Device_GetPVRSecurityState(uc_pvr_security_state * pPVRSecurityState)
{
    NGLOG_DEBUG("\33[35m[START] GetPVRSecurityState \33[m\n"  );

    if(pPVRSecurityState == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }
    pPVRSecurityState->valid = TRUE;
    pPVRSecurityState->level = 3;
    pPVRSecurityState->algorithm = UC_DK_PROTECTION_AES;

    return UC_ERROR_SUCCESS;
}
#endif

/**
 * Gets device's platform identifiers
 *
 * This function is called to retrieve relevant identifiers from a device.
 *
 * @param[in,out] pDevicePlatformIdentifiers Device's identifiers to be filled in by the platform.
 *
 * See ::uc_device_platform_identifiers for the detail information needed.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
//extern uc_uint16 GetLoaderStatus(void);
uc_result UniversalClientSPI_Device_GetPlatformIdentifiers(uc_device_platform_identifiers *pDevicePlatformIdentifiers)
{
   
    if(pDevicePlatformIdentifiers == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }
	else
	{
		memset(pDevicePlatformIdentifiers,0,sizeof(uc_device_platform_identifiers));
	}
    
	uint16_t ret = 0xFF;
	uint16_t key_ver = 0, key_sys_id = 0, sign_ver = 0;
	uint16_t variant = 0, sub_variant = 0;
	lc_bbcb_parameters bbcb_parameters;
	lc_uint16 dl_seq_nr =0;
	uint16_t crc16_flag = 0;
	
	ret = LoaderCoreAPI_GetUKInfo(&key_sys_id, &key_ver, &sign_ver);
	ret |= LoaderCoreAPI_GetVariant(&variant, &sub_variant);
	ret |= LoaderCoreAPI_ReadBBCB(&bbcb_parameters, &crc16_flag);
	ret |= LoaderCoreAPI_GetDownloadSequenceNumber(&dl_seq_nr);
	if(ret!=0)
	{
		NGLOG_ERROR("\n %s %d ret[%d]\n",__FUNCTION__,__LINE__,ret);
		return UC_ERROR_OS_FAILURE;
	}
	pDevicePlatformIdentifiers->systemId = key_sys_id;
	pDevicePlatformIdentifiers->variant = variant;
	pDevicePlatformIdentifiers->keyVersion = key_ver;
	pDevicePlatformIdentifiers->signatureVersion = sign_ver;
	pDevicePlatformIdentifiers->loadVersion = dl_seq_nr;
	pDevicePlatformIdentifiers->manufacturerId = bbcb_parameters.manufacturer_id;
	pDevicePlatformIdentifiers->loaderVersion = ((bbcb_parameters.loader_major_version<< 8) & 0xFF00) | (bbcb_parameters.loader_minor_version& 0x00FF);
	pDevicePlatformIdentifiers->hardwareVersion = bbcb_parameters.hardware_version;

    NGLOG_DEBUG("pDownloadSequenceNumber %x \n",dl_seq_nr);
    NGLOG_DEBUG(" BBCBinfo bLDR_Version_Major=%d\n", bbcb_parameters.loader_major_version);
    NGLOG_DEBUG(" BBCBinfo bLDR_Version_Minor=%d\n", bbcb_parameters.loader_minor_version);
	NGLOG_DEBUG(" BBCBinfo bManufactureCode=%d\n", bbcb_parameters.manufacturer_id);
	NGLOG_DEBUG(" BBCBinfo  bHW_Version=%d\n", bbcb_parameters.hardware_version);
    NGLOG_DEBUG(" manufacturerId=%d\n", pDevicePlatformIdentifiers->manufacturerId);
    NGLOG_DEBUG(" loaderVersion=%d\n", pDevicePlatformIdentifiers->loaderVersion);
    NGLOG_DEBUG(" hardwareVersion=%d\n", pDevicePlatformIdentifiers->hardwareVersion);
    NGLOG_DEBUG(" temp loadVersion=%d\n", pDevicePlatformIdentifiers->loadVersion);
    NGLOG_DEBUG(" systemId=%d\n", pDevicePlatformIdentifiers->systemId);
    NGLOG_DEBUG(" variant=%d\n", pDevicePlatformIdentifiers->variant);
    NGLOG_DEBUG(" keyVersion=%d\n", pDevicePlatformIdentifiers->keyVersion);
    NGLOG_DEBUG(" signatureVersion=%d\n", pDevicePlatformIdentifiers->signatureVersion);

    
    return UC_ERROR_SUCCESS;
}
/**
 * Gets device's personalized data
 *
 * This function is called to retrieve personalized data in a device.
 * The personalized data was written to device on the product line.
 * Now this SPI is only used in Pre-Enablement, which is not a generally available feature. It can be ignored if
 * Pre-Enablement is not integrated and no personalized data is used.
 *
 * @param[in,out] pData The device's personalized data.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the personalized data, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Device_GetPersonalizedData(uc_buffer_st* pData )
{
    
#if 0

    if( pData == NULL || pData->bytes == NULL )
    {
        
        return UC_ERROR_NULL_PARAM;
    }


    /*if(pData->length<PERSONALIZED_DATA_SIZE)
    {
        NGLOG_DEBUG("GetIPAddress::UC_ERROR_INVALID_LENGTH\n");
        return UC_ERROR_INVALID_LENGTH;
    }*/

    //static uc_bool _ReadFlash(uc_uint32 u32Addr, uc_uint8* pu8Dst, uc_uint32 u32Size)
#if 0
    uc_uint32 u32SpcbAddr=0x110000;
    uc_uint32 u32SpcbSize=44;

    _ReadFlash(u32SpcbAddr,pData->bytes,u32SpcbSize);
#endif
    pData->length=44;


    // Read personalized data from manufacture params partition(0x230000, 0x10000)
    //pData->length=PERSONALIZED_DATA_SIZE/2;

/*    manufacture_params_info stRead;
    _FLASH_Read(1, MP_PATITION_ADDR/2, (U16*)&stRead, sizeof(stRead)/2);
    memset(&pData->bytes[0], 0x00, PERSONALIZED_DATA_SIZE/2);
    memcpy(&pData->bytes[0], &stRead.ca_PersonalizedDatap[0], PERSONALIZED_DATA_SIZE/2);

    if(u16PriPerDataParam == 0)
    {
        printf(" GetPersonalizedData->bytes(0~7)= %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    pData->bytes[0], pData->bytes[1], pData->bytes[2], pData->bytes[3],
                    pData->bytes[4], pData->bytes[5], pData->bytes[6], pData->bytes[7]);

        printf(" GetPersonalizedData->bytes(36~43)= %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    pData->bytes[36], pData->bytes[37], pData->bytes[38], pData->bytes[39],
                    pData->bytes[40], pData->bytes[41], pData->bytes[42], pData->bytes[43]);
    }
*/
#endif
    return UC_ERROR_SUCCESS;

    //return UC_ERROR_NOT_IMPLEMENTED;
}

uc_result UniversalClientSPI_Device_GetIPAddress( uc_buffer_st *  pData  )
{
    return UC_ERROR_SUCCESS;
}

/** @} */ /* End of devicespi */

/** @defgroup pvrspi Cloaked CA Agent PVR SPIs
 *  Cloaked CA Agent PVR SPIs
 *
 *  Cloaked CA Agent will output PVR relevant info via these SPIs.
 *  To access PVR functionality, the application should also invoke \ref pvrapis "Cloaked CA Agent PVR APIs"
 *  to cooperate with these SPIs.
 *
 *
 *  @{
 */

/**
 * Set PVR session key
 *
 * This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving it to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSessionKey Buffer containing a PVR Session Key.
 *     The uc_buffer_st::bytes member points to a buffer containing the PVR Session Key, and
 *     the uc_buffer_st::length member is set to the length of the session key.Session key length currently is 16 bytes long.
 *
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PVR_SetSessionKey(
    uc_stream_handle streamHandle,
    const uc_buffer_st *pPVRSessionKey)
{    
#if 0
    if( pPVRSessionKey == NULL ||  pPVRSessionKey->bytes==NULL || streamHandle == UC_INVALID_HANDLE_VALUE )
    {
        return UC_ERROR_NULL_PARAM;
    }
    if( pPVRSessionKey->length == 0 )
    {
        return UC_ERROR_INVALID_LENGTH;
    }

    NGLOG_DEBUG(" streamHandle=0x%08X  PVESKeyLength:%d \n" , streamHandle , pPVRSessionKey->length );

    unsigned char       ucLength = SESSION_DATA_SIZE;

    if(pPVRSessionKey->length < SESSION_DATA_SIZE)
    {
        ucLength = (unsigned char)pPVRSessionKey->length;
    }

    NGLOG_DEBUG(" pPVRSessionKey->bytes= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
                *(pPVRSessionKey->bytes+0), *(pPVRSessionKey->bytes+1), *(pPVRSessionKey->bytes+2), *(pPVRSessionKey->bytes+3),
                *(pPVRSessionKey->bytes+4), *(pPVRSessionKey->bytes+5), *(pPVRSessionKey->bytes+6), *(pPVRSessionKey->bytes+7),
                *(pPVRSessionKey->bytes+8), *(pPVRSessionKey->bytes+9), *(pPVRSessionKey->bytes+10), *(pPVRSessionKey->bytes+11),
                *(pPVRSessionKey->bytes+12), *(pPVRSessionKey->bytes+13), *(pPVRSessionKey->bytes+14), *(pPVRSessionKey->bytes+15));

//.0428    UniversalClient_PVR_setSessionKey(streamHandle, pPVRSessionKey);
#endif
    return UC_ERROR_SUCCESS;
}
/**
 * Set PVR session key with key infomation to support multi key ladder
 *
 * This is the extension of ::UniversalClientSPI_PVR_SetSessionKey function with additional key protection infomation provided,
 * in order to support differents key ladder.This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving it to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSKInfo Information about the PVRSK, contains CSSK protection and CSSK material.See ::uc_pvrsk_info.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_PVR_SetExtendedSessionKey(
    uc_stream_handle streamHandle,
    const uc_pvrsk_info * pPVRSKInfo)
{
    
#if 0

    if(pPVRSKInfo == NULL || pPVRSKInfo->pPVRSK == NULL|| pPVRSKInfo->pPVRSK->bytes==NULL ||streamHandle == UC_INVALID_HANDLE_VALUE )
    {
        return UC_ERROR_NULL_PARAM;
    }

    if(pPVRSKInfo->pPVRSK->length == 0)
    {
        return UC_ERROR_INVALID_LENGTH;
    }

    NGLOG_DEBUG(" streamHandle=0x%08X  PVESKeyLength:%d KeyProt:%d \n" , streamHandle , pPVRSKInfo->pPVRSK->length , pPVRSKInfo->KeyProtection );

    unsigned char       Length = SESSION_DATA_SIZE;

    if(pPVRSKInfo->pPVRSK->length  < SESSION_DATA_SIZE)
    {
        Length = (unsigned char)pPVRSKInfo->pPVRSK->length;
    }

    //g_ucPVR.setSession[0] = UC_TRUE;
    //g_ucPVR.typeSession[0]  = pPVRSKInfo->KeyProtection;
    //memset(&g_ucPVR.strSession[0][0], 0x00, sizeof(&g_ucPVR.strSession[0]));
    //memcpy(&g_ucPVR.strSession[0][0], pPVRSKInfo->pPVRSK->bytes, Length);

//.0428    UniversalClient_PVR_setSessionKey(streamHandle, pPVRSKInfo->pPVRSK);
    s_spi_ucSystem.KeyProtection =  pPVRSKInfo->KeyProtection;

    
#endif
    return UC_ERROR_SUCCESS;
}
/** @} */ /* End of pvrspi */
/** @defgroup streamspi Cloaked CA Agent Stream SPIs
 * Cloaked CA Agent Stream SPIs
 *
 * These SPIs implement the functionality of a 'stream'. A stream is an object that the client
 * can connect to in order to set up filters and receive CA messages, like ECMs and EMMs, and receive
 * keys that can be used to descramble content.
 *
 * Streams are created by the client when a method that requires a stream instance is
 * called.
 *
 * \note Cloaked CA Agent doesn't have the concept of descrambler, please refer to \ref Note12 "When and how to open descrambler"
 * for more information on descrambler handling.
 *
 * A stream implementation is responsible for the following:
 *
 * - Managing lifetime of an instance of a stream, via ::UniversalClientSPI_Stream_Open and ::UniversalClientSPI_Stream_Close methods.
 * - Connecting to the CA processing aspect of the client via ::UniversalClientSPI_Stream_Connect and ::UniversalClientSPI_Stream_Disconnect.
 * - Handling filters via the ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_CloseFilter methods.
 * - Providing sections data via the ::uc_notify_callback method.
 * - Forwarding control words received via ::UniversalClientSPI_Stream_SetDescramblingKey to the corresponding descrambler(s).
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 * For more information on how streams are used, see \ref stream_interface_call_sequence "Stream Interface Call Sequence".
 *  @{
 */

/**
 * Starts all filters for this stream.
 *
 * This function is called to start all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter.
 *
 * Once this method is called, a stream implementation is free to call the ::uc_notify_callback
 * function originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
*/
uc_result UniversalClientSPI_Stream_Start(uc_stream_handle streamHandle)
{
    NGLOG_DEBUG("streamHandle: %x \n", streamHandle);
	uc_result retVal = UC_ERROR_SUCCESS;
	if(streamHandle == UC_INVALID_HANDLE_VALUE) retVal = UC_ERROR_INVALID_HANDLE;
	else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK))	{
		uc_uint32 idx = 0;
		for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++){
			if(gEMMStreamInfo.streamHandle[idx] == streamHandle) break;
		}

		if(idx >= MAX_EMM_STREAM_NUM) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
		else{
			int i = 0;
			for(i = 0; i < MAX_EMM_FLT_NUM; i++){
				if((gEMMStreamInfo.filterHandle[i] == UC_INVALID_HANDLE_VALUE) ||(gEMMStreamInfo.filterToStream[i] != idx))continue;
				else retVal = nglStartSectionFilter((HANDLE)gEMMStreamInfo.filterHandle[i]);
			}
		}
	}
	else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK)){
		//ECM stream
		int i = 0;
		for(i = 0; i < MAX_ECM_STREAM_NUM; i++){
			if(streamHandle == gECMStreamInfo.streamHandle[i]) break;
		}
		if(i == MAX_ECM_STREAM_NUM || gECMStreamInfo.filterHandle[i] == UC_INVALID_HANDLE_VALUE) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
		else retVal = nglStartSectionFilter((HANDLE)gECMStreamInfo.filterHandle[i]);
	}
	else return UC_ERROR_OS_FAILURE;
	NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, streamHandle);

	return retVal;
}
/**
 * Stops all filters for this stream.
 *
 * This function is called to stop all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter.
 *
 * Once this method is called, then no more data should be passed to ::uc_notify_callback function
 * originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Stop(uc_stream_handle streamHandle)
{
	uc_result retVal = UC_ERROR_SUCCESS;
	uc_filter_handle FilterHandle = UC_INVALID_HANDLE_VALUE;

	NGLOG_DEBUG("ret: %d streamHandle: 0x%x\n", retVal, streamHandle);

	if(streamHandle == UC_INVALID_HANDLE_VALUE) retVal = UC_ERROR_INVALID_HANDLE;
	else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK)){
		//EMM stream
		uc_uint32 idx = 0;
		for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++){
			if(gEMMStreamInfo.streamHandle[idx] == streamHandle) break;
		}
	
		if(idx >= MAX_EMM_STREAM_NUM||gEMMStreamInfo.filterHandle[idx] == UC_INVALID_HANDLE_VALUE)
			retVal = UC_ERROR_RESOURCE_NOT_FOUND;
		else{
			int i = 0;
			for(i = 0; i < MAX_EMM_FLT_NUM; i++){
				if((gEMMStreamInfo.filterHandle[i] == UC_INVALID_HANDLE_VALUE) ||
					(gEMMStreamInfo.filterToStream[i] != idx)){
					continue;
				}
				else {
					retVal = nglFreeSectionFilter((HANDLE)gEMMStreamInfo.filterHandle[i]);
					gEMMStreamInfo.filterHandle[i] = UC_INVALID_HANDLE_VALUE;
					gEMMStreamInfo.filterToStream[i] = 0xFFFFFFFF;
				}
			}	
			NGLOG_DEBUG("ret: %d filterHandle: %d stop\n", retVal, gEMMStreamInfo.filterHandle[idx]);
		}
	}
	else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK)){
		//ECM stream
		int i = 0;
		for(i = 0; i < MAX_ECM_STREAM_NUM; i++){
			if(streamHandle == gECMStreamInfo.streamHandle[i]) break;
		}
		if(i == MAX_ECM_STREAM_NUM || gECMStreamInfo.filterHandle[i] == UC_INVALID_HANDLE_VALUE){
			retVal = UC_ERROR_RESOURCE_NOT_FOUND;
		}
		else{
			retVal = nglFreeSectionFilter((HANDLE)gECMStreamInfo.filterHandle[i]);
			gECMStreamInfo.filterHandle[i] = UC_INVALID_HANDLE_VALUE;
			NGLOG_DEBUG("ret: %d filterHandle: %d stop\n", retVal, gECMStreamInfo.filterHandle[i]);
		}
	}
	else retVal = UC_ERROR_NOT_IMPLEMENTED;
	NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, streamHandle);

    return retVal;
}

//Not yet implemented.
uc_result UniversalClientSPI_Stream_Send(
     uc_stream_handle streamHandle,
     const uc_stream_send_payload *pBytes)
{
    return UC_ERROR_SUCCESS;
}

/**
 * Stream method for opening a filter.
 *
 * A stream is controlled by the adding
 * and removal of 'filters'. Each filter describes
 * what kinds of sections are expected to be received from the stream. The stream implementation
 * is only required to provide sections that match one of the currently active filters.
 *
 * This method is called to add a filter to an existing stream. Before this callback is
 * called, no data should be provided via the notification function.
 *
 * After an EMM stream is opened and connected, this method is called several times to set up different filters.
 * Each EMM stream will need 7 filters.
 *
 * For an ECM stream, only a single filter is used.
 *
 * After this method is called, ::UniversalClientSPI_Stream_SetFilter is called to provide or change the filter
 * rules on the filter handle. After all filters are initially set up, ::UniversalClientSPI_Stream_Start is called.
 *
 * #UC_INVALID_HANDLE_VALUE is not a valid filter handle. If the function returns success, it must not
 * set *pFilterHandle to #UC_INVALID_HANDLE_VALUE.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[out] pFilterHandle Receives an implementation defined filter handle that can be used to
 *      identify the filter in a subsequent call to ::UniversalClientSPI_Stream_CloseFilter.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_OpenFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle)
{
	uc_result retVal = UC_ERROR_SUCCESS;
	GxDemuxProperty_Slot dmx_slot_prop;
	GxDemuxProperty_Filter dmx_filter_prop;
	int api_ret;
	
	if(streamHandle == UC_INVALID_HANDLE_VALUE){
		retVal = UC_ERROR_INVALID_HANDLE;
	}
	else{
		if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK)){
			//EMM stream
			uc_uint32 idx = 0;
			for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++){
				if(gEMMStreamInfo.streamHandle[idx] == streamHandle) break;
			}

			if(idx >= MAX_EMM_STREAM_NUM) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
			else{
				int i = 0;
				for(i = 0; i < MAX_EMM_FLT_NUM; i++){
					if(gEMMStreamInfo.filterHandle[i] == UC_INVALID_HANDLE_VALUE) break;
				}

				if(i == MAX_EMM_FLT_NUM) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
				else{
					LOCK_DMX;
					gEMMStreamInfo.filterToStream[i] = idx;
					gEMMStreamInfo.filterHandle[i]=*pFilterHandle=nglAllocateSectionFilter(0,gEMMStreamInfo.pid[idx],ca_get_section_callbak,streamHandle,DMX_SECTION);
					UNLOCK_DMX;
				}
			}
		}
		else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK)){
			//ECM stream
			int i = 0;
			for(i; i < MAX_ECM_STREAM_NUM; i++){
				if(streamHandle == gECMStreamInfo.streamHandle[i]) break;			
			}
			//note: MAX_ECM_STREAM_NUM == MAX_ECM_FLT_NUM, 1 to 1 mapping
			if(i == MAX_ECM_STREAM_NUM ) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
			else{
				LOCK_DMX;
				gECMStreamInfo.filterHandle[i]=*pFilterHandle=nglAllocateSectionFilter(0,gECMStreamInfo.pid[i],ca_get_section_callbak,streamHandle,DMX_SECTION);
				UNLOCK_DMX;
				}
		}
		else retVal = UC_ERROR_NOT_IMPLEMENTED;
	}
    NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, streamHandle);
    return retVal;
}

/**
 * Stream method for setting filter information.
 *
 * This method is called after ::UniversalClientSPI_Stream_OpenFilter for each filter.  This method is also
 * called when a message is received that requires changing of filter rules.
 *
 * \note Mask/match/filterDepth used by UniversalClientSPI_Stream_SetFilter includes 2 bytes after table_id, i.e. section_syntax_indicator and section_length.
 * If filterDepth is equal to 2 or above and client device driver doesn't support to set filter conditions to these 2 bytes, Client device shall disregard these 2 bytes.
 *
 *  - For EMM streams, filter rules change when a new group unique address is assigned to the client.
 *    In a typical Irdeto CA system, a group unique address can be reassigned as necessary to
 *    minimize bandwidth.
 *  - For ECM streams, filter rules change when a new ECM arrives with a different table ID than the
 *    previous ECM. In a typical Irdeto CA system, old ECMs are played out until the descrambling key
 *    cycles. After that, the new ECM is delivered with an alternate table IDs, and repeated with that
 *    table ID until the next key cycle. This filter is used to prevent the resubmission of duplicate
 *    ECMs to the client.
 *
 * @param[in] filterHandle Filter handle previously allocated by ::UniversalClientSPI_Stream_OpenFilter.
 * @param[in] pFilterRules The rule set of the filter to create.  If this is NULL, the implementation
 *      should return all data that arrives, regardless of pattern. I.e., NULL means data is unfiltered.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_SetFilter(
    uc_filter_handle filterHandle,
    const uc_filter *pFilterRules)
{
	NGLOG_DEBUG("filterHandle: %x \n", filterHandle);
	int i;
	int retVal=NGL_OK;

    BYTE mask[16],match[16];
	int masklen=(pFilterRules->filterDepth >= FILTER_MASK_DEPTH_DSIZE)? FILTER_MASK_DEPTH_DSIZE:pFilterRules->filterDepth;
	if(masklen<1){
		NGLOG_DEBUG("filterHandle: %x retVal:[%d] masklen:[%d]/[0x%x]\n", filterHandle,NGL_INVALID_PARA,masklen,masklen);
		return NGL_INVALID_PARA;
	}
    bzero(mask,sizeof(mask));
    bzero(match,sizeof(match));
	for(i=0;(i+2)<masklen;i++)
	{
		match[i]= pFilterRules->match[i==0?i:i+2];
		mask[i]= pFilterRules->mask[i==0?i:i+2];
	}
	masklen = masklen>2?masklen-2:1;
	//masklen=masklen|0xE0;//add CCA FILTER FLAG.MX 2020/5/14
    retVal=nglSetSectionFilterParameters((HANDLE)filterHandle,mask,match,masklen);	
	retVal|=nglFilterSetCRC((HANDLE)filterHandle,FALSE);
	retVal|=nglStartSectionFilter((HANDLE)filterHandle);
	NGLOG_DEBUG("filterHandle: %x retVal:[%d]\n", filterHandle,retVal);

	return retVal;
}
/**
 * Stream method for closing a filter
 *
 * This function is called to close an existing filter from a stream that was previously
 * added by a call to ::UniversalClientSPI_Stream_OpenFilter.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in,out] pFilterHandle On input, points to a filter handle previously allocated by
 *      ::UniversalClientSPI_Stream_OpenFilter.  Any private data
 *      associated with filterHandle should be cleaned up by this method. When the method returns,
 *      it must set *pFilterHandle to #UC_INVALID_HANDLE_VALUE to indicate it has freed the handle.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NOT_IMPLEMENTED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_CloseFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle)
{
    uc_result retVal = UC_ERROR_SUCCESS;
    NGLOG_DEBUG("ret: %d filterHandle: %d streamHandle: 0x%x \n", retVal, *pFilterHandle,streamHandle);

    if(streamHandle == UC_INVALID_HANDLE_VALUE) retVal = UC_ERROR_INVALID_HANDLE;
    else{
        if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK)){
            //EMM stream
            uc_uint32 idx = 0;
            for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++){
                if(gEMMStreamInfo.streamHandle[idx] == streamHandle) break;
            }

            if(idx >= MAX_EMM_STREAM_NUM) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
            else{
                int i = 0;
                for(i = 0; i < MAX_EMM_FLT_NUM; i++){
                    if(gEMMStreamInfo.filterHandle[i] == *pFilterHandle) break;
                } 

                if(i == MAX_EMM_FLT_NUM)
                    retVal = UC_ERROR_RESOURCE_NOT_FOUND;
                else{
					if(UC_ERROR_SUCCESS==nglFreeSectionFilter(*pFilterHandle)){
						gEMMStreamInfo.filterHandle[i] = UC_INVALID_HANDLE_VALUE;
						gEMMStreamInfo.filterToStream[i] = 0xFFFFFFFF;
					}
                }
            }
        }
        else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK)){
            //ECM stream
            int i = 0;
            for(i = 0; i < MAX_ECM_STREAM_NUM; i++){
                if(gECMStreamInfo.streamHandle[i] == streamHandle && gECMStreamInfo.filterHandle[i] == *pFilterHandle)
                    break;
            }

            if(i == MAX_ECM_STREAM_NUM) retVal = UC_ERROR_RESOURCE_NOT_FOUND;
            else{                    	
				if(UC_ERROR_SUCCESS==nglFreeSectionFilter(*pFilterHandle))				
					gECMStreamInfo.filterHandle[i] = UC_INVALID_HANDLE_VALUE;
            }
        }
        else
            retVal = UC_ERROR_NOT_IMPLEMENTED;

    }
    NGLOG_DEBUG("ret: %d filterHandle: %d\n", retVal, *pFilterHandle);
    return retVal;

}

	//ca callback function MX
void ca_get_section_callbak(HANDLE filter,const BYTE *buf, UINT len,void * pridata)
{
	uc_stream_handle streamHandle = (uc_stream_handle)pridata;
	 //NGLOG_DEBUG("streamHandle: %d\n", streamHandle);
	if(streamHandle == UC_INVALID_HANDLE_VALUE) return ;
	else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK)){
	   //EMM stream
	   uc_uint32 idx = 0;
	   for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++){
		   if(gEMMStreamInfo.streamHandle[idx] == streamHandle) break;
	   }

	   if(idx >= MAX_EMM_STREAM_NUM) return ;
	   else{
	       gEMMStreamInfo.byteBufferStruct[idx].bytes = buf;
	       gEMMStreamInfo.byteBufferStruct[idx].length = len;
		   gEMMStreamInfo.notifyCallback[idx](gEMMStreamInfo.connectionHandle[idx],&gEMMStreamInfo.byteBufferStruct[idx]);
		   NGLOG_VERBOSE("streamHandle: 0x%x len: %d connectionHandle: %d\n", pridata, len,gEMMStreamInfo.connectionHandle[idx]);
		#if 0
			int j= 0;
		   for(j=0;j<len;j++)
		   {
				if(0==(j%16))
					printf("\n");
				printf("0x%x 0x%x  ",gEMMStreamInfo.byteBufferStruct[idx].bytes[j],buf[j]);
		   }
		   printf("emm data dump len 0x%x  j 0x%x",len,j);
		  #endif 
	   }
	}
	else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK)){
	   //ECM stream
	   int i = 0;
	   for(i = 0; i < MAX_ECM_STREAM_NUM; i++){
		   if(streamHandle == gECMStreamInfo.streamHandle[i]) break;
	   }

	   if(i >= MAX_ECM_STREAM_NUM) return ;
	   else{
	       gECMStreamInfo.byteBufferStruct[i].bytes = buf;
	       gECMStreamInfo.byteBufferStruct[i].length = len;
		   gECMStreamInfo.notifyCallback[i](gECMStreamInfo.connectionHandle[i],&gECMStreamInfo.byteBufferStruct[i]);
		   NGLOG_VERBOSE("streamHandle: 0x%x len: %d connectionHandle: %d pid: %d\n", pridata, len,gECMStreamInfo.connectionHandle[i],gECMStreamInfo.pid[i]);
#if 0
		   int k= 0;
		   for(k=0;k<len;k++)
		   {
				if(0==(k%16))
					printf("\n");
				printf("0x%x 0x%x  ",gECMStreamInfo.byteBufferStruct[i].bytes[k],buf[k]);
		   }
		   printf("ecm data dump len 0x%x  k 0x%x",len,k);
#endif
	   }
	}
	else return ;

	return;	
}
/**
 * Method to connect to a stream.
 *
 * After a stream is opened with a call to ::UniversalClientSPI_Stream_Open, internally the client will call
 * this stream interface method in order to complete the connection setup.
 *
 * Part of the connection process involves informing the stream of a notification callback and connection handle
 * that are used to deliver new ECM and EMM sections to the client.
 *
 * After connecting to a stream, the client may set up filters by calling ::UniversalClientSPI_Stream_OpenFilter.
 * After this is done, the stream may begin sending sections to the notification callback, specifying the
 * connection handle passed into this call to \a Connect.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle to associate with this stream. This must also be
 *     passed to the \a notifyCallback function when sections arrive.
 * @param[in] notifyCallback Function to be called when sections arrive.Implementations that receive one section at a time can safely pass the data directly
 *     (possibly in parallel, as this function is reentrant), whereas implementations that receive
 *      blocks of sections can also safely pass the data directly without having to parse the sections
 *      and break it into individual section.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Connect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle,
    uc_notify_callback notifyCallback)
{
    uc_result retVal = UC_ERROR_SUCCESS;
	NGLOG_DEBUG("ret: %d streamHandle: %x connectionHandle: %d\n", retVal, streamHandle,connectionHandle);

    if(streamHandle == UC_INVALID_HANDLE_VALUE)
    {
        retVal = UC_ERROR_INVALID_HANDLE;
    }
    else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //EMM stream
        uc_uint32 idx = 0;
        for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++)
        {
            if(gEMMStreamInfo.streamHandle[idx] == streamHandle)
            {
                //found resource
                break;
            }
        }

        if(idx >= MAX_EMM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            gEMMStreamInfo.connectionHandle[idx] = connectionHandle;
            gEMMStreamInfo.notifyCallback[idx] = notifyCallback;
        }
    }
    else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //ECM stream
        int i = 0;
        for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
        {
            if(streamHandle == gECMStreamInfo.streamHandle[i])
            {
                //found stream handle
                break;
            }
        }

        if(i == MAX_ECM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            gECMStreamInfo.connectionHandle[i] = connectionHandle;
            gECMStreamInfo.notifyCallback[i] = notifyCallback;
        }
    }
    else
    {
        retVal = UC_ERROR_INVALID_HANDLE;
    }
    NGLOG_DEBUG("ret: %d streamHandle: %x connectionHandle %d\n", retVal, streamHandle, connectionHandle);

    return retVal;
}
/**
 * Stream method to disconnect a stream.
 *
 * This method is provided to give the stream an opportunity to clean up resources related to a connection previously
 * made by a call to ::UniversalClientSPI_Stream_Connect.
 *
 * A stream that has been disconnected should not continue to call ::uc_notify_callback
 * after this call returns.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle originally passed to ::UniversalClientSPI_Stream_Connect.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Disconnect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle)
{
    uc_result retVal = UC_ERROR_SUCCESS;
	NGLOG_DEBUG("ret: %d streamHandle: 0x%x connectionHandle: %d\n", retVal, streamHandle,connectionHandle);

    if(streamHandle == UC_INVALID_HANDLE_VALUE)
    {
        retVal = UC_ERROR_INVALID_HANDLE;
    }
    else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //EMM stream
        uc_uint32 idx = 0;
        for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++)
        {
            if(gEMMStreamInfo.streamHandle[idx] == streamHandle)
            {
                //found resource
                break;
            }
        }

        if(idx >= MAX_EMM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            gEMMStreamInfo.connectionHandle[idx] = (uc_connection_handle)NULL;
            gEMMStreamInfo.notifyCallback[idx] = (uc_notify_callback)NULL;
        }
    }
    else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //ECM stream
        int i = 0;
        for(i; i < MAX_ECM_STREAM_NUM; i++)
        {
            if(streamHandle == gECMStreamInfo.streamHandle[i])
            {
                //found stream handle
                break;
            }
        }

        if(i >= MAX_ECM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            gECMStreamInfo.connectionHandle[i] = (uc_connection_handle)NULL;
            gECMStreamInfo.notifyCallback[i] = (uc_notify_callback)NULL;
        }
    }
    else
    {
        retVal = UC_ERROR_NOT_IMPLEMENTED;
    }
    NGLOG_DEBUG("ret: %d streamHandle: 0x%x connectionHandle %d\n", retVal, streamHandle, connectionHandle);

    return retVal;
}
/**
 * Stream method to pass a key to the descrambler
 *
 * For ECM streams only: this method is provided to send descrambling keys extracted from ECMs on
 * this stream to the descrambler for each active component previously added by ::UniversalClientSPI_Stream_AddComponent.
 *
 * For other streams, this method will not be called.
 *
 * \note Currently, Cloaked CA Agent will output clear CW when the service is in 911 mode or the client is anchored with security ID.
 *    For platform with secure chipset anchor, it is highly recommended that the platform supports dual mode, i.e. the platform shall be able to disable or enable secure chipset function dynamically,
 *    so that the secure chipset can descramble the content with clear CW. In case the platform can NOT support dual mode, the platform will NOT be able to descramble the service in 911 mode.
 *
 *    \n
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pKeyInfo Information about the key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material.
 * @param[in] keyVersion Key version that this descrambling key is associated with.
 *     Different key versions refer to potentially different keys. The descrambler
 *     should maintain separate descrambling keys for each key version. The key version
 *     typically cycles between 1 and N, where N is the number of different key
 *     versions supported by the scrambler. This value is used to
 *     allow for smooth transition from one key to the next when the descrambling key
 *     changes. At a minimum, implementations should be able to support at least 2
 *     key identifiers, 0 and 1.
 *     \note For DVB, 0 indicates Even Key and 1 indicates Odd Key.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_SetDescramblingKey(
    uc_stream_handle streamHandle,
    const uc_key_info *pKeyInfo,
    uc_uint32 keyVersion)
{
    uc_result retVal = UC_ERROR_SUCCESS;

	GxDescAlg DscmbType = GXDESC_ALG_CAS_CSA2;
	uc_dk_algorithm eAlgorithm = UC_DK_ALGORITHM_DVB_CSA;
	GxDescCWParam param;	

	NGLOG_DEBUG("YJC %s %d\n",__func__,__LINE__);

    if(pKeyInfo == NULL)
    {
        retVal = UC_ERROR_NULL_PARAM;
    }
    else
    {
        if(pKeyInfo->descramblingKeyAlgorithm == UC_DK_ALGORITHM_UNKNOWN)
        {
            DscmbType = GXDESC_ALG_CAS_CSA2;
        }
        else if(pKeyInfo->descramblingKeyAlgorithm == UC_DK_ALGORITHM_DVB_CSA)
        {
            DscmbType = GXDESC_ALG_CAS_CSA2;
        }
        else if(pKeyInfo->descramblingKeyAlgorithm == UC_DK_ALGORITHM_AES_128_RCBC)
        {
            DscmbType = GXDESC_ALG_CAS_CSA3;
        }
        else if(pKeyInfo->descramblingKeyAlgorithm == UC_DK_ALGORITHM_AES_128_CBC)
        {
            DscmbType = GXDESC_ALG_CAS_AES_CBC;
        }
		else if(pKeyInfo->descramblingKeyAlgorithm == UC_DK_ALGORITHM_MULTI2)
		{
			DscmbType = GXDESC_ALG_CAS_MULTI2;
		}

    
        if(streamHandle == UC_INVALID_HANDLE_VALUE)
        {
            retVal = UC_ERROR_INVALID_HANDLE;
        }
        else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
        {
            //ECM stream
            int i = 0, j = 0;
            for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
            {
                if(streamHandle == gECMStreamInfo.streamHandle[i])
                {
                    //found stream handle
                    break;
                }
            }
            if(i >= MAX_ECM_STREAM_NUM)
            {
                retVal = UC_ERROR_RESOURCE_NOT_FOUND;
            }
            else
            {
		#if	0//def	DEBUG
   				printf("encrypt key start\n");
				uc_uint8 c = 0;
				for(c=0;c<pKeyInfo->pDescramblingKey->length;c++)
				{
					printf("0x%x ",pKeyInfo->pDescramblingKey->bytes[c]);
				}
				printf("\nencrypt key end\n");
		#endif		
                for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
                {
                    if(gECMStreamInfo.componentPid[i][j] != CCA_INVALID_PID)
                    {

                        NGLOG_DEBUG("streamHandle: [%x] keyVersion [%d] descramblingKeyProtection [%d] componentPid [0x%x] dscmbHandle [0x%x] DscmbType [0x%x]\n", streamHandle, keyVersion,pKeyInfo->descramblingKeyProtection,gECMStreamInfo.componentPid[i][j], gECMStreamInfo.dscmbHandle[i][j],DscmbType);
						uc_uint8 DescHandleIdx = 0xFF;
						uc_uint8 ECW_len = 0;
						gECMStreamInfo.dscmbHandle[i][j] = DESCR_Allocate(gECMStreamInfo.componentPid[i][j],DscmbType,&DescHandleIdx);
						if(0XFF==DescHandleIdx||DescHandleIdx>=DESCR_CHN_MAX_CNT||gECMStreamInfo.dscmbHandle[i][j]<0||DESCR_INVALID_HANDLE==gECMStreamInfo.dscmbHandle[i][j])
						{
							NGLOG_ERROR("\n %s %d DESCR_Allocate failed! 0x%x\n",__FUNCTION__,__LINE__,gECMStreamInfo.componentPid[i][j]);
							return UC_ERROR_DRIVER_FAILURE;
						}
						if(pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_TDES\
                                || pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_AES)
                               // || pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_AES_TRANSFORM)//TAES和T3DES中的T都是Transform的意思，需要二额外调用接口配置Transform系数
                        {					
							if ((pKeyInfo->pDescramblingKey->length > 16))
							{
								NGLOG_ERROR("invalid param\n");
								//DESCR_UNLOCK();
								return UC_ERROR_INVALID_LENGTH;
							}
							DESCR_LOCK();
							if(pKeyInfo->pDescramblingKey->bytes != NULL)
							{
								ECW_len = pKeyInfo->pDescramblingKey->length;
								if(0==keyVersion)//evenkey
									memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenKey, pKeyInfo->pDescramblingKey->bytes, ECW_len);
								if(1==keyVersion)//oddkey
									memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddKey, pKeyInfo->pDescramblingKey->bytes, ECW_len);
							}
							
							if(pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_TDES)
								DESCR_Info.DescrChn[DescHandleIdx].KLAlgo = GXDESC_KLM_ALG_3DES;
							else if(pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_AES)
								DESCR_Info.DescrChn[DescHandleIdx].KLAlgo = GXDESC_KLM_ALG_AES;
							//else
								//DESCR_Info.DescrChn[DescHandleIdx].KLAlgo = GXDESC_KLM_ALG_TAES;
							GxDescConfig(DESCR_Info.DescrChn[DescHandleIdx].DescrHandle, DESCR_Info.DescrChn[DescHandleIdx].dscmbType, GXDESC_KLM_ACPU_MSR2,(s_spi_ucSystem.lenCSSK/16)+1);
							uc_int8 k;
							for (k = 0; k < (s_spi_ucSystem.lenCSSK/16); k++)
							{
								retVal = GxDescSetKn(DESCR_Info.DescrChn[DescHandleIdx].DescrHandle,k, DESCR_Info.DescrChn[DescHandleIdx].KLAlgo, (uint8_t *) &(s_spi_ucSystem.strCSSK[k * 16]), 16);
								if (retVal < 0)
								{
									NGLOG_ERROR("GxDescSetKn error\n");
									DESCR_UNLOCK();
									return UC_ERROR_DRIVER_FAILURE;
								}
							}
							memset(&param, 0, sizeof(param));
							memcpy(param.even_ECW, DESCR_Info.DescrChn[DescHandleIdx].EvenKey, ECW_len);
							memcpy(param.odd_ECW, DESCR_Info.DescrChn[DescHandleIdx].OddKey, ECW_len);
							param.ECW_len = ECW_len;
						#if	0//def	DEBUG	
							printf("\n even cw:: 0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x\n",param.even_ECW[0],param.even_ECW[1],param.even_ECW[2],param.even_ECW[3],param.even_ECW[4],param.even_ECW[5],param.even_ECW[6],param.even_ECW[7],\
								param.even_ECW[8],param.even_ECW[9],param.even_ECW[10],param.even_ECW[11],param.even_ECW[12],param.even_ECW[13],param.even_ECW[14],param.even_ECW[15]);				
							printf("\n odd cw:: 0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x	0x%x 0x%x 0x%x 0x%x\n",param.odd_ECW[0],param.odd_ECW[1],param.odd_ECW[2],param.odd_ECW[3],param.odd_ECW[4],param.odd_ECW[5],param.odd_ECW[6],param.odd_ECW[7],\
									param.odd_ECW[8],param.odd_ECW[9],param.odd_ECW[10],param.odd_ECW[11],param.odd_ECW[12],param.odd_ECW[13],param.odd_ECW[14],param.odd_ECW[15]);
						#endif	
							NGLOG_DEBUG("DescrHandle 0x%x pid 0x%x\n",DESCR_Info.DescrChn[DescHandleIdx].DescrHandle,DESCR_Info.DescrChn[DescHandleIdx].Pid);
							#if 1
							if(DESCR_Info.DescrChn[DescHandleIdx].dscmbType == GXDESC_ALG_CAS_AES_CBC)
							{								
								uc_uint8 AES_CBC_IV[] = {0x49, 0x72, 0x64, 0x65, 0x74, 0x6F, 0xA9, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74}; //is fixed by irdeto cca  911mode
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenIV,AES_CBC_IV, 16);
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddIV,AES_CBC_IV,16);
								memcpy(param.even_IV, DESCR_Info.DescrChn[DescHandleIdx].EvenIV, 16);
								memcpy(param.odd_IV, DESCR_Info.DescrChn[DescHandleIdx].OddIV, 16);
							}
							if(DESCR_Info.DescrChn[DescHandleIdx].dscmbType == GXDESC_ALG_CAS_CSA3)
							{	
								uc_uint8 AES_RCBC_IV[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};//is fixed by irdeto cca  911mode
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenIV,AES_RCBC_IV, 16);
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddIV,AES_RCBC_IV,16);
								memcpy(param.even_IV, DESCR_Info.DescrChn[DescHandleIdx].EvenIV, 16);
								memcpy(param.odd_IV, DESCR_Info.DescrChn[DescHandleIdx].OddIV, 16);
							}
							#endif
							retVal = GxDescSetCw(DESCR_Info.DescrChn[DescHandleIdx].DescrHandle, DESCR_Info.DescrChn[DescHandleIdx].Pid, DESCR_Info.DescrChn[DescHandleIdx].KLAlgo, &param);
							if (retVal < 0)
							{
								NGLOG_ERROR("GxDescSetCw error\n");
								DESCR_UNLOCK();
								return UC_ERROR_DRIVER_FAILURE;
							}
							DESCR_UNLOCK();
                        }
                        else if(pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_CLEAR
                                || pKeyInfo->descramblingKeyProtection == UC_DK_PROTECTION_UNKNOWN)
                        {
							//int32_t ret;
							//GxDescCWParam param;
							
							DESCR_LOCK();
							if ((DescHandleIdx >= DESCR_CHN_MAX_CNT) || (DESCR_STATUS_USED != DESCR_Info.DescrChn[DescHandleIdx].Status)
								|| (pKeyInfo->pDescramblingKey->bytes == NULL))
							{
								NGLOG_ERROR("DESCR_Info.DescrChn[%d].Status = %d,Pid = 0x%x\n",DescHandleIdx,DESCR_Info.DescrChn[DescHandleIdx].Status,DESCR_Info.DescrChn[DescHandleIdx].Pid); 	   
								DESCR_UNLOCK();
								return UC_ERROR_OS_FAILURE;
							}
							
							memset(&param, 0, sizeof(param));
							if(DESCR_Info.DescrChn[DescHandleIdx].dscmbType == GXDESC_ALG_CAS_AES_CBC)
							{								
								uc_uint8 AES_CBC_IV[] = {0x49, 0x72, 0x64, 0x65, 0x74, 0x6F, 0xA9, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74}; //is fixed by irdeto cca  911mode
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenIV,AES_CBC_IV, 16);
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddIV,AES_CBC_IV,16);
								memcpy(param.even_IV, DESCR_Info.DescrChn[DescHandleIdx].EvenIV, 16);
								memcpy(param.odd_IV, DESCR_Info.DescrChn[DescHandleIdx].OddIV, 16);
							}
							if(DESCR_Info.DescrChn[DescHandleIdx].dscmbType == GXDESC_ALG_CAS_CSA3)
							{	
								uc_uint8 AES_RCBC_IV[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};//is fixed by irdeto cca  911mode
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenIV,AES_RCBC_IV, 16);
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddIV,AES_RCBC_IV,16);
								memcpy(param.even_IV, DESCR_Info.DescrChn[DescHandleIdx].EvenIV, 16);
								memcpy(param.odd_IV, DESCR_Info.DescrChn[DescHandleIdx].OddIV, 16);
							}
							#if 0
							if(DESCR_Info.DescrChn[DescHandleIdx].dscmbType == GXDESC_ALG_CAS_CSA3 || DESCR_Info.DescrChn[DescHandleIdx].dscmbType == GXDESC_ALG_CAS_AES_CBC)
							{
								param.ECW_len = 16;
							}
							else
							{
								param.ECW_len = 8;
							}
							#endif
							param.ECW_len = pKeyInfo->pDescramblingKey->length;
							NGLOG_DEBUG("pDescramblingKey->length %x\n",pKeyInfo->pDescramblingKey->length);
							if(0==keyVersion)//evenkey
							{
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenKey, pKeyInfo->pDescramblingKey->bytes, param.ECW_len);
							}
							else if(1==keyVersion)//oddkey
							{
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddKey, pKeyInfo->pDescramblingKey->bytes, param.ECW_len);
							}
							else
							{
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].EvenKey, pKeyInfo->pDescramblingKey->bytes, param.ECW_len);
								memcpy(DESCR_Info.DescrChn[DescHandleIdx].OddKey, pKeyInfo->pDescramblingKey->bytes, param.ECW_len);
							}
							
							memcpy(param.even_ECW, DESCR_Info.DescrChn[DescHandleIdx].EvenKey, param.ECW_len);
							memcpy(param.odd_ECW, DESCR_Info.DescrChn[DescHandleIdx].OddKey, param.ECW_len);
							
							GxDescConfig(DESCR_Info.DescrChn[DescHandleIdx].DescrHandle, DESCR_Info.DescrChn[DescHandleIdx].dscmbType, GXDESC_KLM_NONE, 0);
							retVal = GxDescSetCw(DESCR_Info.DescrChn[DescHandleIdx].DescrHandle, DESCR_Info.DescrChn[DescHandleIdx].Pid, DESCR_Info.DescrChn[DescHandleIdx].KLAlgo, &param);
							if(retVal < 0)
							{
								NGLOG_ERROR("GxDescSetCw error %x\n",retVal);
								DESCR_UNLOCK();
								return UC_ERROR_INVALID_LENGTH;
							}
							
							DESCR_UNLOCK();
						#if 0
							printf("encrypt key start\n");
							uc_uint8 c = 0;
							for(c=0;c<pKeyInfo->pDescramblingKey->length;c++)
							{
								printf("0x%x ",pKeyInfo->pDescramblingKey->bytes[c]);
							}
							printf("\nencrypt key end\n");
						#endif
                       }	
                    }
                }
            }
        }
        else
        {
            retVal = UC_ERROR_NOT_IMPLEMENTED;
        }
    }
    NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, streamHandle);
    return retVal;

}

/**
 * Stream method to clean the descrambler keys stored in client device.
 *
 * Cloaked CA Agent will invoke this SPI immediately when the client is not entitled to view the content.
 * Client device shall make descramblers stop working by setting random keys(both even and odd) to descramblers or by stopping descramblers.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_CleanDescramblingKey(
    uc_stream_handle streamHandle)
{
    uc_result retVal = UC_ERROR_SUCCESS;
    uc_byte bytes[16] = {0};
    uc_buffer_st stBuffer;
    stBuffer.length = 16;
    stBuffer.bytes = bytes;

    uc_key_info stKeyInfo;
    stKeyInfo.descramblingKeyAlgorithm = UC_DK_ALGORITHM_DVB_CSA;
    stKeyInfo.descramblingKeyProtection = UC_DK_PROTECTION_TDES;
    stKeyInfo.pDescramblingKey = &stBuffer;

	NGLOG_DEBUG("YJC %s %d\n",__func__,__LINE__);
#if 0
    if(streamHandle == UC_INVALID_HANDLE_VALUE)
    {
        retVal = UC_ERROR_INVALID_HANDLE;
    }
    else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //ECM stream
        int i = 0, j = 0;
        for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
        {
            if(streamHandle == gECMStreamInfo.streamHandle[i])
            {
                //found stream handle
                break;
            }
        }
        if(i >= MAX_ECM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            //memset(&u8clearKey[0], 0x00, DESC_KEY_SIZE);
            for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
            {
                if(gECMStreamInfo.componentPid[i][j] != CCA_INVALID_PID)
                {
                    //_MS_Start_KeyLadder(gECMStreamInfo.componentPid[i][j], E_DSCMB_KEY_EVEN, &u8clearKey[0]);
                    //_MS_Start_KeyLadder(gECMStreamInfo.componentPid[i][j], E_DSCMB_KEY_ODD, &u8clearKey[0]);

                    _MS_Start_KeyLadder(gECMStreamInfo.dscmbHandle[i][j], E_DSCMB_KEY_EVEN, &stKeyInfo);
                    _MS_Start_KeyLadder(gECMStreamInfo.dscmbHandle[i][j], E_DSCMB_KEY_ODD, &stKeyInfo);
                }
            }
        }
    }
    else
    {
        retVal = UC_ERROR_NOT_IMPLEMENTED;
    }
#endif
    NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, streamHandle);

    return retVal;
}
/**
 * Stream method to stop and cleanup a stream instance
 *
 * This method is called when the stream is being closed by UniversalClient_Stream_Close and is
 * intended to give the stream implementation a chance to clean up any leftover resources set
 * up during a call to open.
 *
 * @param[in,out] pStreamHandle On input, points to a stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *      On output, the function must set this to #UC_INVALID_HANDLE_VALUE.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Close(uc_stream_handle *pStreamHandle)
{
    uc_result retVal = UC_ERROR_SUCCESS;

    NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, *pStreamHandle);
	//return UC_ERROR_SUCCESS;

    if(*pStreamHandle == UC_INVALID_HANDLE_VALUE)
    {
        retVal = UC_ERROR_INVALID_HANDLE;
    }
    else if((*pStreamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)EMM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //EMM stream
        uc_uint32 idx = 0;
        for(idx = 0; idx < MAX_EMM_STREAM_NUM; idx++)
        {
            if(gEMMStreamInfo.streamHandle[idx] == *pStreamHandle)
            {
                //found resource
                break;
            }
        }

        if(idx >= MAX_EMM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            gEMMStreamInfo.pid[idx] = (uc_uint16)CCA_INVALID_PID;
            *pStreamHandle = gEMMStreamInfo.streamHandle[idx] = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
        }
    }
    else if((*pStreamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //ECM stream
        int i = 0;
        for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
        {
            if(*pStreamHandle == gECMStreamInfo.streamHandle[i])
            {
                //found stream handle
                break;
            }
        }

        if(i >= MAX_ECM_STREAM_NUM)
        {
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }
        else
        {
            gECMStreamInfo.pid[i] = (uc_uint16)CCA_INVALID_PID;
            *pStreamHandle = gECMStreamInfo.streamHandle[i] = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
        }
    }
    else if(*pStreamHandle == (uc_stream_handle)PVR_RECORD_STREAM_HANDLE)
    {
        *pStreamHandle = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
    }
    else if(*pStreamHandle == (uc_stream_handle)PVR_PLAYBACK_STREAM_HANDLE)
    {
        *pStreamHandle = (uc_stream_handle)UC_INVALID_HANDLE_VALUE;
    }

    else
    {
        retVal = UC_ERROR_NOT_IMPLEMENTED;
    }

    NGLOG_DEBUG("ret: %d streamHandle: %x\n", retVal, *pStreamHandle);

    return retVal;
}
/** @} */ /* End of streamspi */

/** @defgroup copycontrolspi Cloaked CA Agent Copy Control SPIs
 *  Cloaked CA Agent Copy Control SPIs
 *
 *  Cloaked CA Agent will output copy control information via these SPIs.
 *  To access copy control functionality, the application should also invoke \ref copycontrolapis "Cloaked CA Agent Copy Control APIs"
 *  to cooperate with these SPIs.
 *
 *
 *  @{
 */


//////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct
{
    unsigned char           mvVersion;
    unsigned int            mvLength;
    unsigned char           mvMode;
    unsigned char           mvData[8];

    unsigned char           cciProtocolVersion;
    unsigned char           cciApsInfo;
    unsigned char           cciEmiInfo;
    unsigned char           cciIctInfo;
    unsigned char           cciRtcInfo;
    unsigned char           cciRlInfo;

} uc_macrovision_st;

extern uc_macrovision_st        g_ucCCI;
extern uc_result UniversalClient_CCI_Macrovision(unsigned char ucAction);
extern uc_result  UniversalClientSPI_CopyControl_Analog_SetCCI( uc_copycontrol_cci CopyControlInfo);
extern uc_result  UniversalClientSPI_CopyControl_Digital_SetCCI( uc_copycontrol_cci CopyControlInfo);
//
//////////////////////////////////////////////////////////////////////////////////////////


/**
 * Sets the macrovision configuration data
 *
 * This method is called during the initialization of Cloaked CA Agent, or when the Cloaked CA Agent received Macrovision CAM emm.
 *
 * @param[in] mac_config_data macrovsion configuration data structure. this contains MacrovisionVersion and
    Macrovision configuration data.
 *
 * See ::uc_macrovision_config for the detail information needed.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetConfig(uc_macrovision_config mac_config_data)
{
    uc_result ret;
#if 0
#ifdef UC_SPI_DEBUG
    {
        unsigned int        i = 0;

        printf("\n");
        printf("[START] UniversalClientSPI_CopyControl_Macrovision_SetConfig\n");
        printf("[%s %d] macrovisionVersion=%d\n", __FUNCTION__, __LINE__, mac_config_data.macrovisionVersion);
        printf("[%s %d] macrovisionConfig.length=%d\n", __FUNCTION__, __LINE__, mac_config_data.macrovisionConfig.length);
        printf("[%s %d] macrovisionConfig.bytes=%s\n", __FUNCTION__, __LINE__,mac_config_data.macrovisionConfig.bytes);

        for(i=0; i<mac_config_data.macrovisionConfig.length; i++)
        {
            if((i%80) == 0)
            {
                printf("\n");
            }
            printf("%x", *(mac_config_data.macrovisionConfig.bytes + i));
        }
        printf("\n");
    }
#endif

    g_ucCCI.mvVersion = mac_config_data.macrovisionVersion;
    g_ucCCI.mvLength = mac_config_data.macrovisionConfig.length;

    memset(&g_ucCCI.mvData[0], 0x00, sizeof(&g_ucCCI.mvData[0]));
    memcpy(&g_ucCCI.mvData[0], mac_config_data.macrovisionConfig.bytes, g_ucCCI.mvLength);

    ret = UniversalClient_CCI_Macrovision(1);

    
#endif
    return UC_ERROR_SUCCESS;
}
/**
 * CopyControl method to set the Macrovision mode
 *
 * This method is called when the received stream containes Macrovision mode info or submitted Session Metadata containes Macrovision mode.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] mac_mode The mode of macrovision to be set
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetMode(uc_stream_handle streamHandle, uc_macrovision_mode mac_mode)
{
    uc_result       ret;

    NGLOG_DEBUG("\n");
    NGLOG_DEBUG("[%s %d] streamHandle=0x%08X\n", __FUNCTION__, __LINE__, streamHandle);
    NGLOG_DEBUG("[%s %d] mac_mode=%d\n", __FUNCTION__, __LINE__, mac_mode.mode);
#if 0
    g_ucCCI.mvMode = mac_mode.mode;
    ret = UniversalClient_CCI_Macrovision(2);   
#endif
    return UC_ERROR_SUCCESS;
}
/**
 * CopyControl and CI Plus method to set the CCI bits info [uc_copy_control_info] to client
 *
 * This method is called when the received ECM containes the CopyControl/CI Plus info or the submitted Session Metadata containes the CopyControl/CI Plus info.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pCopyControlInfo points to the structure of detail copy control information.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClientSPI_CopyControl_SetCCI(uc_stream_handle streamHandle, uc_copy_control_info* pCopyControlInfo)
{
    uc_result   ret = UC_ERROR_NULL_PARAM;
    NGLOG_DEBUG("\n");
    return UC_ERROR_SUCCESS;
#if 0
    if(pCopyControlInfo == NULL)
    {
        
        return UC_ERROR_NULL_PARAM;
    }

    NGLOG_DEBUG("[%s %d] streamHandle=0x%08X\n", __FUNCTION__, __LINE__, streamHandle);
    NGLOG_DEBUG("[%s %d] protocol_version=0x%02X\n", __FUNCTION__, __LINE__, pCopyControlInfo->protocol_version);
    NGLOG_DEBUG("[%s %d] aps_copy_control_info=0x%02X\n", __FUNCTION__, __LINE__, pCopyControlInfo->aps_copy_control_info);
    NGLOG_DEBUG("[%s %d] emi_copy_control_info=0x%02X\n", __FUNCTION__, __LINE__, pCopyControlInfo->emi_copy_control_info);
    NGLOG_DEBUG("[%s %d] ict_copy_control_info=0x%02X\n", __FUNCTION__, __LINE__, pCopyControlInfo->ict_copy_control_info);
    NGLOG_DEBUG("[%s %d] rct_copy_control_info=0x%02X\n", __FUNCTION__, __LINE__, pCopyControlInfo->rct_copy_control_info);
    NGLOG_DEBUG("[%s %d] rl_copy_control_info=0x%02X\n", __FUNCTION__, __LINE__, pCopyControlInfo->rl_copy_control_info);

    g_ucCCI.cciProtocolVersion = pCopyControlInfo->protocol_version;
    g_ucCCI.cciApsInfo = pCopyControlInfo->aps_copy_control_info;
    g_ucCCI.cciEmiInfo = pCopyControlInfo->emi_copy_control_info;
    g_ucCCI.cciIctInfo = pCopyControlInfo->ict_copy_control_info;
    g_ucCCI.cciRtcInfo = pCopyControlInfo->rct_copy_control_info;
    g_ucCCI.cciRlInfo = pCopyControlInfo->rl_copy_control_info;

    ret = UniversalClientSPI_CopyControl_Analog_SetCCI((uc_copycontrol_cci)pCopyControlInfo->aps_copy_control_info);
    ret = UniversalClientSPI_CopyControl_Digital_SetCCI((uc_copycontrol_cci)pCopyControlInfo->emi_copy_control_info);

    

    return ret;
#endif	
}
/** @} */ /* End of copycontrolspi */
/** @defgroup smartcardspi Cloaked CA Agent Smart Card SPIs
 *  Cloaked CA Agent Smart Card SPIs
 *
 *  Cloaked CA Agent will communicate with smartcard via these SPIs.
 *  \note Currently, smartcard SPIs are used by \ref glossary_testcore "TestCore" only. They are NOT used by Cloaked CA Agent.
 *  \ref glossary_testcore "TestCore" will use these SPIs to communicate with smartcard, so that the smartcard slot can be verified.
 *
 *  @{
 */


#if 1  //.0825 smc not implement first

uc_result UniversalClientSPI_Smartcard_Open(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData)
{
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}

uc_result UniversalClientSPI_Smartcard_Close(uc_uint32 smartcardID)
{
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}

uc_result UniversalClientSPI_Smartcard_Reset(uc_uint32 smartcardID)   
{
	return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}

uc_result UniversalClientSPI_Smartcard_Communicate(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer )
{
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}
#else
/**
 * Smartcard method to initialize the smartcard device.
 *
 * This method is called during the initialization of Cloaked CA Agent.
 *
 * @param[out] pSmartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @param[in] pSCOpenData pointer of data transmitted to smartcard driver, the pointer
 *            points to structure ::uc_sc_open_parameter.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Smartcard_Open(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData)
{
#if 0
    printf("[START] UniversalClientSPI_Smartcard_Open\n");
    printf("[END] UniversalClientSPI_Smartcard_Open\n");
#endif


    if((pSCOpenData == NULL) || (pSmartcardID == NULL))
    {
        return UC_ERROR_NULL_PARAM;
    }

    memset(&Sc_Info,0,sizeof(UCSPI_SC_INFO));

    //init smc driver
    if( E_SC_OK != MDrv_SC_Init(0)) //use SMC CID 0
    {
        if( E_SC_OK != MDrv_SC_Init(1))
        {
            printf("[%s][%d] NO SMC slot can use\n",__func__,__LINE__);
            return UC_ERROR_DRIVER_FAILURE;
        }
        *pSmartcardID = 1;
    }
    else
    {
        *pSmartcardID = 0;
    }


    if (MDrv_SC_Open(*pSmartcardID, T0_PROTOCOL, &astScParam[0], _SMC_Notify) == E_SC_OK)
    {

        Sc_Info.smc_cid = *pSmartcardID;
        Sc_Info.smartcardHandle = pSCOpenData->smartcardHandle;
        Sc_Info.uProtocol = T0_PROTOCOL;
        Sc_Info.pfnNotify = pSCOpenData->pfnStatusNotify;
        return UC_ERROR_SUCCESS;
    }
    else
    {
        return UC_ERROR_DRIVER_FAILURE;
    }
}

/**
 * Smartcard method to close the smartcard device.
 *
 * This method is called when closing the Cloaked CA Agent.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Smartcard_Close(uc_uint32 smartcardID)
{
    #if 0
    printf("[START] UniversalClientSPI_Smartcard_Close\n");
    printf("[END] UniversalClientSPI_Smartcard_Close\n");
    #endif


    MDrv_SC_Close(smartcardID);
    memset(&Sc_Info, 0, sizeof(UCSPI_SC_INFO));
    return UC_ERROR_SUCCESS;
}

/**
 * Smartcard method to reset the smartcard device.
 *
 * This method is called when Cloaked CA Agent wants to reset the smartcard device.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 */
uc_result UniversalClientSPI_Smartcard_Reset(uc_uint32 smartcardID)
{
#if 0
    printf("[START] UniversalClientSPI_Smartcard_Reset\n");
    printf("[END] UniversalClientSPI_Smartcard_Reset\n");
#endif
    //printf("[START] UniversalClientSPI_Smartcard_Reset\n");

    uc_uint8               ATR[SC_ATR_LEN_MAX], history[SC_HIST_LEN_MAX],*atr;
    uc_uint16              atr_len, history_len;
    SC_Param            sParam;
    uc_buffer_st*       pvInfo;
    uc_buffer_st        Info;

    uc_smartcard_error_info  card_in_error;

    memset(&Info,0,sizeof(Info));
    memset(ATR,0,sizeof(SC_ATR_LEN_MAX));
    memset(history,0,sizeof(SC_HIST_LEN_MAX));
    atr = &ATR[0];
    pvInfo = &Info;

    Sc_Info.uProtocol=T14_PROTOCOL;
    if (MDrv_SC_Reset(smartcardID,&astScParam[0]) != E_SC_OK) //check t14
    {
        return UC_ERROR_DRIVER_FAILURE;
    }


    MDrv_SC_ClearState(smartcardID);


    // Timeout value can be tuned
    if (MDrv_SC_GetATR(smartcardID, 100*5, atr, &atr_len, history, &history_len) != E_SC_OK) // the timeout value should be tumed by user
    {
        card_in_error.cardErrorType = UC_SC_HARDWARE_ERROR;
        Sc_Info.scStatus = UC_SC_CARD_IN;
        pvInfo->length = sizeof(card_in_error);
        pvInfo->bytes  = (uc_byte *)&card_in_error;
        Sc_Info.pfnNotify(smartcardID, Sc_Info.smartcardHandle, UC_SC_CARD_IN_ERROR, pvInfo);

        return UC_ERROR_DRIVER_FAILURE;
    }



    //_SC_DumpData(atr, atr_len);
    if((atr[1]&0x80) == 0x0) //mean no TD, then default as T0 protocel
    {
        sParam.u8Protocal = T0_PROTOCOL;
    }
    else
    {
        sParam.u8Protocal=atr[3]&0x0F;
        //printf("u8Protocol ==%x\n",sParam.u8Protocol);
    }

    if(sParam.u8Protocal!=T14_PROTOCOL)
    {
        Sc_Info.uProtocol=T0_PROTOCOL;
        if (MDrv_SC_Reset(smartcardID,&astScParam[1]) != E_SC_OK)
        {
            return UC_ERROR_DRIVER_FAILURE;
        }

        MDrv_SC_ClearState(smartcardID);
        memset(ATR,0,sizeof(SC_ATR_LEN_MAX));

        // Timeout value can be tuned
        if (MDrv_SC_GetATR(smartcardID, 100*5, atr, &atr_len, history, &history_len) != E_SC_OK) // the timeout value should be tumed by user
        {
            card_in_error.cardErrorType = UC_SC_HARDWARE_ERROR;
            Sc_Info.scStatus = UC_SC_CARD_IN;
            pvInfo->length = sizeof(card_in_error);
            pvInfo->bytes  = (uc_byte *)&card_in_error;
            Sc_Info.pfnNotify(smartcardID, Sc_Info.smartcardHandle, UC_SC_CARD_IN_ERROR, pvInfo);

            return UC_ERROR_DRIVER_FAILURE;

        }
        //_SC_DumpData(atr, atr_len);
        sParam.u8Protocal=T0_PROTOCOL; //&*&*&*j1_0619
        //printf("u8Protocol ==%x\n",sParam.u8Protocol);
        if((atr[0]!=0x3f)&&(atr[0]!=0x3b))//&*&*&*j1_0619
        {
            return UC_ERROR_DRIVER_FAILURE;
        }
        if (MDrv_SC_PPS(smartcardID) != E_SC_OK)
        {
            return UC_ERROR_DRIVER_FAILURE;
        }

    }



    printf("SC Reset Ok \n");
#if 0
    printf("dump atr\n");
    uc_uint8 count = 0;
    for(count = 0;count <atr_len ; count++)
    {
        printf("%x ",atr[count]);
    }
    printf("\ndump atr\n");

#endif


    uc_smartcard_ATR_info Atr_Info;
    Atr_Info.ATRLength = atr_len;
    Atr_Info.ATRData = atr;
    pvInfo->bytes = (uc_byte *)&Atr_Info;
    pvInfo->length = sizeof(Atr_Info);

    Sc_Info.scStatus=UC_SC_CARD_IN;
    Sc_Info.pfnNotify(smartcardID,Sc_Info.smartcardHandle, UC_SC_CARD_IN, pvInfo);


#if 0 //reserve for atr check
        if(atr_len != ((atr[1]&0x0F)+5))
        {
            eErrCode = VD_SC_ATR_INVALID_FORMAT;
            result = E_SC_FAIL;
            printf("atr_len error %x \n",atr_len);
        }
        else if((_s3vdScInfo[ref_id].uProtocol == T0_PROTOCOL)&&(memcmp(&atr[5], "IRDETO", 6)!=0))
        {
            eErrCode = VD_SC_ATR_INVALID_HISTORY;
            result = E_SC_FAIL;
            printf("IRDETO error %c %c %c %c %C \n",atr[5],atr[6],atr[7],atr[8],atr[9]);
        }
        else if((_s3vdScInfo[ref_id].uProtocol == T14_PROTOCOL)&&(memcmp(&atr[4], "IRDETO", 6)!=0))
        {
            eErrCode = VD_SC_ATR_INVALID_HISTORY;
            result = E_SC_FAIL;
            printf("IRDETO error %c %c %c %c %C \n",atr[5],atr[6],atr[7],atr[8],atr[9]);
        }

        if (result == E_SC_OK)
        {
            printf("SC Reset Ok \n");
            vd_sc_atr_st *info = (vd_sc_atr_st*)ATR;
            info->wAtrLength = atr_len;
            //_SC_DumpData((uc_uint8 *)info, atr_len+2);
            _s3vdScInfo[ref_id].scStatus=VD_SC_CARD_IN;
            _s3vdScInfo[ref_id].pfnNotify(ref_id, _s3vdScInfo[ref_id].wSoftCellVal, VD_SC_CARD_IN,  (void*)info);
        }
        else
        {
            printf("VD_SC_CARD_IN_ERROR\n");
            card_in_error.eCardError = eErrCode;
            _s3vdScInfo[ref_id].scStatus=VD_SC_CARD_IN_ERROR;
            _s3vdScInfo[ref_id].pfnNotify(ref_id, _s3vdScInfo[ref_id].wSoftCellVal, VD_SC_CARD_IN_ERROR, &card_in_error);
        }

#endif
    //printf("[END] UniversalClientSPI_Smartcard_Reset\n");
    return UC_ERROR_SUCCESS;
}



/**
 * Smartcard method to communicate with the smartcard device
 *
 * This method is called when Cloaked CA Agent wants to read/write data from/to the smartcard device
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses this ID
 *            to map to a smartcard reader slot.
 * @param[in] headerLen header length of the APDU.
 * @param[in] payloadLen payload length of the APDU.
 * @param[in] pSendBuffer buffer for the  APDU.
 * @param[out] pRecvDataLen received buffer length.
 * @param[out] pRecvBuffer received buffer.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Smartcard_Communicate(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer )
{
    // junchi.lin 20140508: avoid crash
    //return    UC_ERROR_NULL_PARAM;
    // junchi.lin 20140508: avoid crash
    #if 0
        printf("[START] UniversalClientSPI_Smartcard_Communicate\n");
        printf("[END] UniversalClientSPI_Smartcard_Communicate\n");
    #endif

        SC_Result           result = E_SC_OK;

        uc_uint8             abReadData[255];
        uc_uint16           wReadDataLen;
        uc_uint16           wSendDataLen;




        wSendDataLen=headerLen+payloadLen;
        wReadDataLen = sizeof(abReadData);
		memset(abReadData,0,255);

        switch (Sc_Info.uProtocol)
        {

            case T0_PROTOCOL:
            {
                result = MApi_SC_T0_Communication(smartcardID, (uc_uint8*)pSendBuffer, &wSendDataLen , abReadData, &wReadDataLen);
                if (result != E_SC_OK)
                {
                    //S3VDSCDBG__(5, NGLOG_DEBUG("retry MApi_SC_T0_Communication\n\n"));
                    result = MApi_SC_T0_Communication(smartcardID, (uc_uint8*)pSendBuffer, &wSendDataLen , abReadData, &wReadDataLen);
                }


                if (result == E_SC_OK)
                {
                    *pRecvDataLen = wReadDataLen;
                    memcpy(pRecvBuffer, abReadData,wReadDataLen);
                }
                else
                {
//					printf("result %d\n",result);
                    //S3VDSCDBG__(0, NGLOG_DEBUG("    MApi_SC_T0_Communication FAIL %d\n",result));
                }

                break;
            }

            case T14_PROTOCOL:
            {
                result = MApi_SC_T14_Communication(smartcardID, (uc_uint8*)pSendBuffer, &wSendDataLen , abReadData, &wReadDataLen);
                if (result != E_SC_OK)
                {
                    //S3VDSCDBG__(5, NGLOG_DEBUG("retry MApi_SC_T14_Communication\n\n"))
                    result = MApi_SC_T14_Communication(smartcardID, (uc_uint8*)pSendBuffer, &wSendDataLen , abReadData, &wReadDataLen);
                }
                if (result == E_SC_OK)
                {
                    *pRecvDataLen = wReadDataLen;
                    memcpy(pRecvBuffer, abReadData,wReadDataLen);

                }
                break;
            }
            default:
                break;
        }

        if (result != E_SC_OK)
        {
            //*pwDataReadSize = sizeof(vd_sc_read_error_st);
            switch (result)
            {
                case E_SC_CARDOUT :
                    //((vd_sc_read_error_st*)(pvDataRead))->eError = VD_SC_CARD_OUT_ERROR;
                    break;
                case E_SC_TIMEOUT :
                    NGLOG_DEBUG("E_SC_TIMEOUT\n");
                    //((vd_sc_read_error_st*)(pvDataRead))->eError = VD_SC_TIME_OUT;
                    break;
                default:
                    //((vd_sc_read_error_st*)(pvDataRead))->eError = VD_SC_COMM_ERROR;
                    break;
            }
            return  UC_ERROR_NULL_PARAM;
        }

        return UC_ERROR_SUCCESS;
}
#endif
/** @} */ /* End of smartcardspi */

/** @defgroup datetimespi Cloaked CA Agent Date Time SPIs
 *  Cloaked CA Agent Date Time SPIs
 *
 *  Cloaked CA Agent will retrieve date time info via these SPIs.
 *
 *  @{
 */


/**
 * Method to get current date time.
 *
 * This SPI is optional currently.
 *
 * This method is called to retrieve second and microsecond of current time info.
 * In Linux-like system, gettimeofday can be used to get the result(but the result needs to be converted to millisecond).
 * In MS Windows-like system, GetTickCount, GetLocalTime and GetSystemTime can be used to get the result.
 *
 * \note Cloaked CA Agent only uses the time info to measure the performance. So the time can be time elapsed since reboot, or
 * it can also be the real day time.
 *
 * @param[out] pCurrentTime To save current time info.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_DateTime_GetTimeOfDay(uc_time *pCurrentTime)
{
    
#if 0
    if(pCurrentTime == NULL)
    {
        return UC_ERROR_NULL_PARAM;
    }

    // Get current system time in ms.
    pCurrentTime->millisecond = (uc_uint32)MsOS_GetSystemTime() % 1000;
    pCurrentTime->second      = (uc_uint32)MsOS_GetSystemTime() / 1000;

    //
#endif
    return UC_ERROR_SUCCESS;
}
/** @} */ /* End of datetimespi */

/** @defgroup manufacturercryptospi Cloaked CA Agent Crypto SPIs
 *  Cloaked CA Agent Crypto SPIs
 *
 *  Cloaked CA Agent will do the verification step for digital signature via these SPIs.
 *
 *  @{
 */


/**
 * Method to do the verification for digital signature.
 *
 * This SPI is optional currently.
 *
 * This functional modules are implemented by both Cloaked CA Agent and STB manufacturer.
 * Generally speaking, the SPI form is faster than the Cloaked CA Agent's implementation.
 * Cloaked CA Agent will decide which implementation should be used in digital signature process.
 *
 * @param[in] pCryptoInfo send relative info indicating the deciphering algorithm type and the parameter related to it.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Crypto_Verify(uc_crypto_info *pCryptoInfo)
{
    return UC_ERROR_SUCCESS;
}
/**
 * Get the list of implemented methods
 *
 * This method is called when the client is started in order to
 * get the list of SPI methods currently available on the platform.
 *
 * @param [in,out] pImpl On input, this structure is zeroed out.
 *    The implementation MUST populate this structure in order to successfully
 *    integrate with the platform.
 *
 */

static void _TimerCallback(uc_uint32 u32StTimer, uc_uint32 u32TimerID)
{
#if 0
    uc_int16 index=0;

    for(index=0 ; index<MAX_TASK_NUM ; index++ )
    {
        if( s_gtimercallbk[index].s32timerId == u32TimerID )
        {
            NGLOG_DEBUG("[Match] find index:%d \n" , index );
            break;
        }
    }

    if(index==MAX_TASK_NUM)
    {
        NGLOG_DEBUG("[Match] Can not find match index . Return! \n" );
        return;
    }

    if( s_gtimercallbk[index].stimercallbk !=NULL )
    {
        s_gtimercallbk[index].stimercallbk( s_gtimercallbk[index].s32timerId ,s_gtimercallbk[index].pCon );
    }
#endif
}


uc_result UniversalClientSPI_Timer_Open  ( uc_uint32 *  pTimerId,uc_timer_info *  pTimerInfo )
{
	NGLOG_DEBUG("[start] UniversalClientSPI_Timer_Open\n");
#if 0	
    if(pTimerId == NULL||pTimerInfo==NULL||pTimerInfo->onTimeCallback==NULL)
    {
        
        return UC_ERROR_NULL_PARAM;
    }

    uc_int16  index=0;
    if(s_timernum==0)
    {
        NGLOG_DEBUG("[Set] s_timernum=0 reset all timer-data \n");
        for(index=0 ; index<MAX_TASK_NUM ; index++ )
        {
            s_gtimercallbk[index].s32timerId=-1;
            s_gtimercallbk[index].stimercallbk=NULL;
            s_gtimercallbk[index].pCon=NULL;
        }
    }

    index=0;
    for(index=0 ; index<s_timernum ; index++ )
    {
        if( s_gtimercallbk[index].s32timerId == -1 )
            break;
    }

    if(index>=MAX_TASK_NUM)
    {
        
        return UC_ERROR_TOO_MANY_HANDLES;
    }

    

    uc_sint32        s32TimerId              = -1;
	event_list* create_timer(timer_event event, int time, void *data,  TIMER_FLAG flag);

    s32TimerId = MsOS_CreateTimer (  _TimerCallback,
                                     pTimerInfo->timeout,
                                     pTimerInfo->timeout,
                                     TRUE,
                                     "UC_SPI_Timer_Open");
    if(s32TimerId  < 0)
    {
        NGLOG_DEBUG("[End] Timer_Open fail! \n");
        return UC_ERROR_DRIVER_FAILURE;
    }

    *pTimerId=s32TimerId;
    s_timernum++;

    s_gtimercallbk[index].s32timerId   = s32TimerId;
    s_gtimercallbk[index].stimercallbk = pTimerInfo->onTimeCallback;
    s_gtimercallbk[index].pCon = pTimerInfo->timerContext;

    NGLOG_DEBUG("[End]  Timer_Open TimerID:%lu ! timenum:%d \n" ,
                s_gtimercallbk[index].s32timerId , s_timernum );
#endif
    return UC_ERROR_SUCCESS;

}

uc_result UniversalClientSPI_Timer_Close  ( uc_uint32  timerId  )
{
    
#if 0
    uc_bool Rest=FALSE;

    Rest = MsOS_DeleteTimer(timerId);

    if(Rest==FALSE)
    {
        NGLOG_DEBUG("[ERROR] MsOS_DeleteTimer FALSE! \n"  );
        return UC_ERROR_DRIVER_FAILURE;
    }

    uc_int16  index=0;

    for(index=0 ; index < MAX_TASK_NUM ; index++ )
    {
        if(s_gtimercallbk[index].s32timerId == timerId )
        {
            s_gtimercallbk[index].s32timerId   = -1;
            s_gtimercallbk[index].stimercallbk = NULL;
            s_gtimercallbk[index].pCon = NULL;
            break;
        }
    }

    s_timernum--;

    NGLOG_DEBUG("[End] TimerID:%u  Index:%d  timerNum:%d\n" , timerId ,index , s_timernum);
#endif
    return UC_ERROR_SUCCESS;

}

uc_result UniversalClientSPI_Timer_Stop  ( uc_uint32  timerId  )
{
    
#if 0
    uc_bool Rest=FALSE;
    Rest = MsOS_StopTimer(timerId);
    if(Rest==FALSE)
    {
        NGLOG_DEBUG("[ERROR] MsOS_StopTimer FALSE!\n" );
        return UC_ERROR_DRIVER_FAILURE;
    }
#endif
    return UC_ERROR_SUCCESS;
}

uc_result UniversalClientSPI_Timer_Start  ( uc_uint32  timerId  )
{
    
#if 0
    uc_bool Rest=FALSE;
    Rest=MsOS_StartTimer(timerId);
    if(Rest==FALSE)
    {
        NGLOG_DEBUG("[ERROR] MsOS_StartTimer FALSE! \n"  );
        return UC_ERROR_DRIVER_FAILURE;
    }
#endif
    return UC_ERROR_SUCCESS;
}

//add the spi function from later of 2.10 in above

/**
 * Generate the chip configuration checking.
 *
 * This function is called to ensure the configuration data integrity and authenticity. 
 * Secure chips must be able to generate a MAC using the standard CBC-MAC scheme with AES-128 block cipher.
 * \note If this SPI is not implemented, UC_ERROR_CCC_NOT_SUPPORT must be returned. This function is only used by Cloaked CA Agent for Secure Chipset based STBs.
 *
 * @param [in] chipConfigurationRequest Chip Configuration Check Request
 * @param [out] pChipConfigurationResponse Chip Configuration Check Response.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_CCC_NOT_SUPPORT
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */

uc_result UniversalClientSPI_Device_GetChipConfigurationCheck(
                    uc_chip_configuration_request chipConfigurationRequest, 
                    uc_chip_configuration_response *pChipConfigurationResponse)
{
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}


 /**
  * Sets the Secure Chipset Serial Number
  *
  * This function is called to set the secure chipset serial number associated with the device. This function is only used by 
  * Cloaked CA Agent for IP only STBs. This serial number is used to deliver EMMs addressed to this particular instance of a client.
  * The Cloaked CA Agent obtains the Secure Chipset Serial Number from the Client registration response message.
  * 
  * \note Only after the CSSN is set via ::UniversalClientSPI_Device_SetCSSN, can the Cloaked CA Agent get the valid CSSN 
  * via ::UniversalClientSPI_Device_GetCSSN.
  *
  * @param[in] pData Buffer of serial number data. 
  *     On input, the uc_buffer_st::bytes member points to a buffer stored the serial number, and the uc_buffer_st::length member 
  *     to the maximum length of the buffer, in bytes.  
  *
  * @retval
  *     ::UC_ERROR_SUCCESS
  * @retval
  *     ::UC_ERROR_NULL_PARAM
  * @retval
  *     ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *     ::UC_ERROR_INSUFFICIENT_BUFFER
  * @retval
  *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
  */
uc_result UniversalClientSPI_Device_SetCSSN(const uc_buffer_st * pData)
{
    //TBD:  This function is only used by Cloaked CA Agent for IP only STBs
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}

/**
 * Gets the device PIN code
 *
 * This function is called to obtain the PIN code associated with the device. This function is only used by Cloaked CA Agent for IP only STBs.  
 *
 * \note Currently, the length of the PIN code is 8 bytes, and bytes in pData points to a null-terminated char string.
 * e.g. bytes in pData points to "12345678", and length in pData equals to strlen("12345678").
 *
 * @param[in] pData Buffer to read PIN code data info. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes. The function modifies the contents of the buffer to include
 *     the PIN code, and sets the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     must be set to the minimum required buffer size, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Device_GetPINCode(uc_buffer_st * pData)
{    
    //TBD:  This function is only used by Cloaked CA Agent for IP only STBs
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}


/**
 * Sets the Multi-2 parameters for Multi-2 descrambling
 *
 * This function is called when service is scrambled by Multi-2 algorithm. The parameters will be set before CWs are output.
 *
 * Refer to \ref baf_arib "Basic Application Flow - ARIB" to get an overview.
 *
 * @param[in] streamHandle indicates the associated stream.
 * @param[in] pMulti2Parameter multi-2 parameters.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Device_SetMulti2Parameter(uc_stream_handle streamHandle, uc_device_multi2_parameter *pMulti2Parameter)
{
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}


void GL_DESCR_Init(void)
{
	int Ret = UC_ERROR_SUCCESS;
	int i = 0;
	GxDescInitParam param;
	//GxSecureLoader loader;

	memset(&DESCR_Info, 0, sizeof(DESCR_Info));

	for (i = 0; i < DESCR_CHN_MAX_CNT; i++)
	{
		DESCR_Info.DescrChn[i].Status = DESCR_STATUS_FREE;
		DESCR_Info.DescrChn[i].DescrHandle = DESCR_INVALID_HANDLE;
		DESCR_Info.DescrChn[i].Pid = DESCR_INVALID_PID;
	}
	if(0 == DESCR_Info.MutexHandle)
	{
		if(UC_ERROR_SUCCESS!=UniversalClientSPI_Mutex_Open(&(DESCR_Info.MutexHandle)))
		{
			NGLOG_ERROR("  UniversalClientSPI_Mutex_Open failed 0x%x \n", DESCR_Info.MutexHandle);
			return UC_ERROR_OS_FAILURE;
		}
	}

	memset(&param, 0, sizeof(param));
	param.klm = GXDESC_KLM_NONE|GXDESC_KLM_ACPU_SOFT_DES|GXDESC_KLM_SCPU_DYNAMIC|GXDESC_KLM_ACPU_MSR2|GXDESC_KLM_SCPU_MSR3;
	Ret = GxDescInit(&param);
	NGLOG_DEBUG(" GxDescInit Ret =%d",Ret);
	if(Ret < 0)
	{
		NGLOG_ERROR("GxDescInit error\n");
		return ;
	}
	#if 0
	Ret =0;
	GxSecureCCCParam cccparam;
	memset(&cccparam,0,sizeof(cccparam));
	cccparam.alg = GX_CCC_ALG_AES;
	memset(&cccparam.input,0xFF,16);
	Ret = GxSecure_GetChipCfgCheck(&cccparam);
	printf("\n GxSecure_GetChipCfgCheck ret[%d]  alg[%d] \n input:",Ret,cccparam.alg);
	for (i =0;i<16;i++)
		printf("%02x",cccparam.input[i]);
	printf("\n MAC:");
	for (i =0;i<16;i++)
		printf("%02x",cccparam.MAC[i]);
	printf("\n plain:[");
	for (i =0;i<16;i++)
		printf("%02x",cccparam.plain[i]);
	printf("] \n:");
	
	#endif
#if 0
	Ret = GxFuse_GetCwDualModeDisableStatus();
	NGLOG_DEBUG(" GxFuse_GetCwDualModeDisableStatus Ret =%d",Ret);
	Ret = GxFuse_GetMSR2KlmCWStage();
	NGLOG_DEBUG(" GxFuse_GetMSR2KlmCWStage Ret =%d",Ret);
	Ret = GxFuse_GetAlgDisableStatus();
	NGLOG_DEBUG(" GxFuse_GetAlgDisableStatus Ret =%d",Ret);
#endif
#if 0
	memset(&loader, 0, sizeof(loader));
	loader.dst_addr = 0x90000;
	if(GxSecure_LoadFirmware(&loader) < 0)
	{
		printf("GxSecure_LoadFirmware error\n");
	}
#endif
	NGLOG_DEBUG("  end \n");
#if 0	
	GxDescAlg dst_alg = GXDESC_ALG_CAS_CSA2;//GXDESC_ALG_CAS_AES_CBC;
    if (cas_v < 0) { 
        cas_v = GxDescOpen(GXDESC_MOD_TS, 0, dst_alg);
        GxDescConfig(cas_v, dst_alg, GXDESC_KLM_ACPU_MSR2, 2);
    }

    if (cas_a < 0) { 
        cas_a = GxDescOpen(GXDESC_MOD_TS, 0, dst_alg);
        GxDescConfig(cas_a, dst_alg, GXDESC_KLM_ACPU_MSR2, 2);
    }
#endif
	return ;
}
handle_t DESCR_Allocate(uc_uint16 Pid,GxDescAlg alg,uc_uint8 *pDescHandleIdx)
{
    uc_uint32 i = 0;
    handle_t descramble_handle;
    //GxDescAlg alg = GXDESC_ALG_CAS_CSA2;
   
    DESCR_LOCK();
	 for (i = 0; i < DESCR_CHN_MAX_CNT; i++)
    {
        if (Pid == DESCR_Info.DescrChn[i].Pid&&DESCR_STATUS_USED==DESCR_Info.DescrChn[i].Status)
        {
        	NGLOG_DEBUG(" pid existed 0x%x alg0x%x dscmbType 0x%x deshandle 0x%x\n",DESCR_Info.DescrChn[i].Pid,alg,DESCR_Info.DescrChn[i].dscmbType, DESCR_Info.DescrChn[i].DescrHandle);
			DESCR_UNLOCK();
			if(NULL!=pDescHandleIdx)
				*pDescHandleIdx = i;
			return DESCR_Info.DescrChn[i].DescrHandle;
			break;
        }
    }
    for (i = 0; i < DESCR_CHN_MAX_CNT; i++)
    {
        if (DESCR_STATUS_FREE == DESCR_Info.DescrChn[i].Status)
        {
            break;
        }
    }
    if (i >= DESCR_CHN_MAX_CNT)
    {       
        NGLOG_DEBUG(" \n  max  desc ");
        DESCR_UNLOCK();
        return DESCR_INVALID_HANDLE;
    }

    descramble_handle = GxDescOpen(GXDESC_MOD_TS, 0, alg);

    if (descramble_handle < 0)
    {
        NGLOG_DEBUG("GxDescOpen error  alg=%d\n",alg);
        DESCR_UNLOCK();
        return DESCR_INVALID_HANDLE;
    }

    DESCR_Info.DescrChn[i].DescrHandle = descramble_handle;

    NGLOG_DEBUG(" +++ GxDescOpen 0x%x \n", DESCR_Info.DescrChn[i].DescrHandle);
   
    DESCR_Info.DescrChn[i].Pid = Pid;
    DESCR_Info.DescrChn[i].Status = DESCR_STATUS_USED;
    DESCR_Info.DescrChn[i].dscmbType = alg;

    if(NULL!=pDescHandleIdx)
		*pDescHandleIdx = i;

    DESCR_UNLOCK();
    return descramble_handle;
}

uc_result DESCR_Free(handle_t DescrHandle)
{   
	uc_uint8 i;
 	NGLOG_DEBUG(" enter ");
    DESCR_LOCK();
    //if ((DescrHandle >= DESCR_CHN_MAX_CNT) || (DESCR_STATUS_FREE == DESCR_Info.DescrChn[DescrHandle].Status))
	if( DESCR_INVALID_HANDLE==DescrHandle )
	{
        NGLOG_DEBUG(" invalid handle \n");
        DESCR_UNLOCK();
        return UC_ERROR_NULL_PARAM;
    }

    GxDescClose(DescrHandle);
	for (i=0; i<DESCR_CHN_MAX_CNT; i++)
	{
		if(DescrHandle==DESCR_Info.DescrChn[i].DescrHandle)
		{
			memset(&(DESCR_Info.DescrChn[i]), 0, sizeof(DESCR_Chn_s));
			//DESCR_Info.DescrChn[DescrHandle].DemuxId = 0;
			DESCR_Info.DescrChn[i].Pid = DESCR_INVALID_PID;
			DESCR_Info.DescrChn[i].DescrHandle = DESCR_INVALID_HANDLE;
			DESCR_Info.DescrChn[i].Status = DESCR_STATUS_FREE; 
		}
	}
    DESCR_UNLOCK();
    return UC_ERROR_SUCCESS;
}

uc_result DESCR_Enable(uc_stream_handle streamHandle,GxDescAlg dscmbType,GxDescCWParam* _param)
{
	uc_result retVal = UC_ERROR_SUCCESS;
	if(streamHandle == UC_INVALID_HANDLE_VALUE)
    {
    	
        retVal = UC_ERROR_INVALID_HANDLE;
    }
    else if((streamHandle&STREAM_TYPE_MASK) == ((uc_stream_handle)ECM_STREAM_HANDLE&STREAM_TYPE_MASK))
    {
        //ECM stream
        
        int i = 0, j = 0;
        for(i = 0; i < MAX_ECM_STREAM_NUM; i++)
        {
            if(streamHandle == gECMStreamInfo.streamHandle[i])
            {
				for(j = 0; j < MAX_ECM_COMPONENT_NUM; j++)
                {
                    if(gECMStreamInfo.componentPid[i][j] != DESCR_INVALID_PID)//&&gECMStreamInfo.dscmbHandle[i][j]!=DESCR_INVALID_HANDLE)//&&) NEED PAIR PID? MX
                    {
                        //found
                        
						gECMStreamInfo.dscmbHandle[i][j] = DESCR_Allocate(gECMStreamInfo.componentPid[i][j],dscmbType,NULL);
						if(gECMStreamInfo.dscmbHandle[i][j]<0||DESCR_INVALID_HANDLE==gECMStreamInfo.dscmbHandle[i][j])
						{
							printf("\n %s %d DESCR_Allocate failed! 0x%x\n",__FUNCTION__,__LINE__,gECMStreamInfo.componentPid[i][j]);
							continue;
						}
						GxDescConfig(gECMStreamInfo.dscmbHandle[i][j], dscmbType, GXDESC_KLM_SCPU_MSR3,3);
						retVal = GxDescSetCw(gECMStreamInfo.dscmbHandle[i][j], gECMStreamInfo.componentPid[i][j], GXDESC_KLM_ALG_3DES, _param);//the third param is not used. MX 
                    }
                }

            }
        }
        if(i >= MAX_ECM_STREAM_NUM)
        {
        	
            retVal = UC_ERROR_RESOURCE_NOT_FOUND;
        }            
    }
	return  retVal;
}
/**
 * Send IFCP Command to the secure chipset.
 *
 * Cloaked CA Agent calls this to send an IFCP Command to the secure chipset and get response if any.
 * When a device is required to work in IFCP mode by the headend, the Cloaked CA Agent will call this SPI
 * to set the descrambling key, to set the secure PVR session key, etc.
 * This is the uniform IFCP command communication interface.
 *
 * \note This SPI provides IFCP command information to STB Application, and STB Application constructs the IFCP Command according to chip's specification. 
 *
 * \n
 * 
 * \note The IFCP command does not support clear descrambling key or clear PVR session key delivery.
 *       If the descrambling key is clear, it will be set via ::UniversalClientSPI_Stream_SetDescramblingKey.
 *       If the PVR session key is clear, it will be set via ::UniversalClientSPI_PVR_SetExtendedSessionKey.
 *
 * \n
 *
 * \note When this SPI is used to set the descrambling key or set the Secure PVR session key, it is a stream related SPI, refer to \ref streamspi "Cloaked CA Agent Stream SPIs".
 *    The stream handle is indicated in ::uc_IFCP_input::additionalInfo.
 *
 * @param[in] pInput includes IFCP command data and some additional information.
 * @param[out] pOutput output data from IFCP module, refer to ::_uc_IFCP_output for more details.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval 
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_IFCP_Communicate(uc_IFCP_input *pInput, uc_IFCP_output *pOutput)
{

	GxIfcpInput gx_in;
	GxIfcpOutput gx_out;
	int ret = 0xff;
	
    if(pInput == NULL)
    {
        NGLOG_DEBUG("[%s][%d] invaild input buffer\n",__func__,__LINE__);
        return UC_ERROR_INSUFFICIENT_BUFFER;
    }
	
	GxDescCWParam _param;
	GxDescAlg dscmbType = GXDESC_ALG_CAS_CSA2;
	uc_dk_algorithm eAlgorithm = UC_DK_ALGORITHM_DVB_CSA;
	GxDescKLMAlg alg = GXDESC_KLM_ALG_3DES;//need fix  MX
	
    uc_uint8 AES_CBC_IV[] = {0x49, 0x72, 0x64, 0x65, 0x74, 0x6F, 0xA9, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74}; //is fixed by irdeto cca

	memset(&_param, 0, sizeof(_param));
    _param.ifcp_header = pInput->pIFCPInfo->pKLCInfo->header;
	//
    _param.ifcp_header_len = 3;                      // 必定�?个字�?
    _param.ifcp_payload = (pInput->pIFCPInfo->pKLCInfo->payload).bytes;                             // CA库给�?
    _param.ifcp_payload_len = (pInput->pIFCPInfo->pKLCInfo->payload).length;                         // CA库给�?
	if(NULL!=(void *)(pInput->pIFCPInfo->pApplicationControlInfo))//->payload)
	{		
		_param.ifcp_app_header = pInput->pIFCPInfo->pApplicationControlInfo->header;						  // CA库给�?
		_param.ifcp_app_header_len = IFCP_APP_CONTROL_INFO_HEADER_LEN;						// CA库给的，没有的时候需要填0
		_param.ifcp_app_payload = (pInput->pIFCPInfo->pApplicationControlInfo->payload).bytes;						  // CA库给�?
		_param.ifcp_app_payload_len = (pInput->pIFCPInfo->pApplicationControlInfo->payload).length;					  // CA库给的，没有的时候需要填0
	}
	else
	{
		_param.ifcp_app_header = NULL;
		_param.ifcp_app_header_len = 0;
		_param.ifcp_app_payload = NULL;						  // CA库给�?
		_param.ifcp_app_payload_len = 0; 				  // CA库给的，没有的时候需要填0
	}
	_param.ifcp_resp =  pOutput->response.bytes;
	_param.ifcp_resp_len =  pOutput->response.length;
	_param.ifcp_app_resp =  pOutput->appResponse.bytes;
	_param.ifcp_app_resp_len =  pOutput->appResponse.length;
	if(UC_IFCP_COMMAND_SET_DESCRAMBLING_KEY==pInput->commandType)
    {
    	eAlgorithm = (pInput->additionalInfo).forDescramblingKey.descramblingKeyAlgorithm;
		
		if(eAlgorithm == UC_DK_ALGORITHM_DVB_CSA)
	    {
	        dscmbType = GXDESC_ALG_CAS_CSA2;
	    }
	    else if(eAlgorithm == UC_DK_ALGORITHM_AES_128_RCBC)
	    {
	        dscmbType = GXDESC_ALG_CAS_CSA3;
	    }
	    else if(eAlgorithm == UC_DK_ALGORITHM_AES_128_CBC)
	    {
	        dscmbType = GXDESC_ALG_CAS_AES_CBC;
	    }
		else if(eAlgorithm == UC_DK_ALGORITHM_MULTI2)
	    {
	        dscmbType = GXDESC_ALG_CAS_MULTI2;
	    }
	    else 
	    {
	        dscmbType = GXDESC_ALG_CAS_CSA2;
	    }
		
	    if(dscmbType == GXDESC_ALG_CAS_AES_CBC)
	    {
	        memcpy(_param.even_IV, AES_CBC_IV, 16);
	        memcpy(_param.odd_IV, AES_CBC_IV, 16);
	    }
		_param.ifcp_set_even = 1-(pInput->additionalInfo).forDescramblingKey.keyVersion;//keyVersion  For DVB, 0 indicates Even Key and 1 indicates Odd Key. 
		
		DESCR_Enable( (pInput->additionalInfo).forDescramblingKey.streamHandle, dscmbType,&_param);
		return UC_ERROR_SUCCESS;
	}
	if(UC_IFCP_COMMAND_SET_PVR_SESSION_KEY==pInput->commandType)
	{
		;//pInput->additionalInfo.forPVRSessionKey
	}

    return UC_ERROR_SUCCESS;
}

/**
 * Load IFCP image into the secure chipset.
 *
 * Cloaked CA Agent calls this to load an IFCP image into the secure chipset.
 * When a secure chipset is required to work in IFCP mode by the headend, which is triggered by an IFCP Pairing EMM, 
 * Cloaked CA Agent will call this SPI to load the IFCP image (if it exists).
 * If it has switched to IFCP mode, Cloaked CA Agent will also call this SPI when the STB boots up.
 *
 * @param[in] pImage image data, including activation message (AM) and RAM image.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_IFCP_LoadImage(uc_IFCP_image *pImage)
{
      
    GxIfcpApp app;    
 	int ret = 0xff;
    if(pImage == NULL)
    {
        NGLOG_DEBUG("Null  pImage\n");
        return UC_ERROR_NULL_PARAM;
    }
		
    GxSecureLoader loader = 
    {
    .loader   = NULL,
    .size     = 0,
    .dst_addr = 0x90000,
	};

	GxSecure_LoadFirmware(&loader);
	//while(GxSecure_GetStatus() != FW_APP_START);

	app.app = pImage->ramImage.bytes;
	app.app_len = pImage->ramImage.length;
	app.active_msg = pImage->activationMessage.bytes;
	app.active_msg_len = pImage->activationMessage.length;

	ret = GxIFCP_LoadAPP(&app);

    return UC_ERROR_SUCCESS;
}


/** @} */ /* End of ifcpspi */


/** @defgroup trickmodecontrolspi Cloaked CA Agent Trick Mode Control SPIs
 *  Cloaked CA Agent Trick Mode Control SPIs
 *
 *  Cloaked CA Agent will do trick mode control via these SPIs.
 *  
 *  @{
 */

/**
 * Set extra trick mode control information.
 *
 * Cloaked CA Agent calls this to set extra trick mode control information on a client.
 *
 * This SPI is currently optional.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pExtraTrickModeControl points to the structure of extra trick mode control.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */

uc_result UniversalClientSPI_SetExtraTrickModeControl(uc_stream_handle streamHandle,uc_extra_trick_mode_control *pExtraTrickModeControl)
{
    return UC_ERROR_SUCCESS;//UC_ERROR_CCC_NOT_SUPPORT;
}

/** @} */ /* End of trickmodecontrolspi */


/**
 * Method to connect to a stream with extended callback for data(ECM sections or web service response or proximity detection message) delivery with additional information. 
 *
 * After a stream is opened with a call to ::UniversalClientSPI_Stream_Open, internally the client will call 
 * this stream interface method in order to complete the connection setup. 
 *
 * Part of the connection process involves informing the stream of a notification callback and connection handle
 * that are used to deliver data(ECM sections or web service response or proximity detection message) to the client.
 *
 * After connecting to a stream, the client may set up filters by calling ::UniversalClientSPI_Stream_OpenFilter. 
 * After this is done, the stream may begin sending data(ECM sections or web service response or proximity detection message) to the notification callback, specifying the 
 * connection handle passed into this call to \a Connect .
 *
 * See also \ref vod_playback_flow "VOD Playback Flow - Out-Of-Band ECM"
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle to associate with this stream. This must also be 
 *     passed to the \a notifyCallback function when data(ECM sections or web service response or proximity detection message) arrives. 
 * @param[in] notifyCallback Function to be called when data(ECM sections or web service response or proximity detection message) arrives. 
 *      For ECM sections delivery, implementations that receive one section at a time can safely pass the data directly
 *      (possibly in parallel, as this function is reentrant), whereas implementations that receive
 *      blocks of sections can also safely pass the data directly without having to parse the sections
 *      and break it into individual section.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Extended_Connect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle,
    uc_notify_callback_extended notifyCallback)
    
{    
    NGLOG_DEBUG("streamHandle: %x connectionHandle %d\n", streamHandle, connectionHandle);
    return UC_ERROR_SUCCESS;
}

uc_result UniversalClientSPI_SCOT_LoadTransformationData(const uc_buffer_st *pEncryptedTask, uc_uint32 tdcCount, uc_tdc_data_for_spi *pTdcData)
{
	 return UC_ERROR_SUCCESS;
}

uc_result UniversalClientSPI_Message_CallBack(uc_message_callback messageCallback)
{
	return UC_ERROR_SUCCESS;
}

void UniversalClientSPI_GetImplementation(uc_spi_implementation_st *pImpl)
{
    
    if(pImpl == NULL)
    {
        return;
    }

    pImpl->Memory_Malloc    = UniversalClientSPI_Memory_Malloc;
    pImpl->Memory_Free      = UniversalClientSPI_Memory_Free;
    pImpl->FatalError       = UniversalClientSPI_FatalError;

    pImpl->Semaphore_Open   = UniversalClientSPI_Semaphore_Open;
    pImpl->Semaphore_Post   = UniversalClientSPI_Semaphore_Post;
    pImpl->Semaphore_Wait   = UniversalClientSPI_Semaphore_Wait;
    pImpl->Semaphore_WaitTimeout   =  UniversalClientSPI_Semaphore_WaitTimeout;
    pImpl->Semaphore_Close  = UniversalClientSPI_Semaphore_Close;

    pImpl->Mutex_Open       = UniversalClientSPI_Mutex_Open;
    pImpl->Mutex_Lock       = UniversalClientSPI_Mutex_Lock;
    pImpl->Mutex_Unlock     = UniversalClientSPI_Mutex_Unlock;
    pImpl->Mutex_Close      = UniversalClientSPI_Mutex_Close;

    pImpl->Thread_Open      = UniversalClientSPI_Thread_Open;
    pImpl->Thread_Sleep     = UniversalClientSPI_Thread_Sleep;
    pImpl->Thread_Close     = UniversalClientSPI_Thread_Close;

    pImpl->PS_Delete        = UniversalClientSPI_PS_Delete;
    pImpl->PS_Write         = UniversalClientSPI_PS_Write;
    pImpl->PS_Read          = UniversalClientSPI_PS_Read;
    pImpl->PS_Initialize    = UniversalClientSPI_PS_Initialize;
    pImpl->PS_Terminate     = UniversalClientSPI_PS_Terminate;

    pImpl->Device_GetDeviceID       = UniversalClientSPI_Device_GetDeviceID ;
    pImpl->Device_GetCSSN           = UniversalClientSPI_Device_GetCSSN;
    pImpl->Device_SetCSSK           = UniversalClientSPI_Device_SetCSSK;
    pImpl->Device_SetExtendedCSSK   = UniversalClientSPI_Device_SetExtendedCSSK;
    pImpl->Device_GetPrivateData            = UniversalClientSPI_Device_GetPrivateData;
    pImpl->Device_GetSecurityState      = UniversalClientSPI_Device_GetSecurityState;
#if(CA_CCA_PVR_SUP == 1)	
    pImpl->Device_GetPVRSecurityState   = UniversalClientSPI_Device_GetPVRSecurityState;
#endif	
    pImpl->Device_GetPlatformIdentifiers    = UniversalClientSPI_Device_GetPlatformIdentifiers;

    pImpl->Device_GetPersonalizedData   = UniversalClientSPI_Device_GetPersonalizedData;
    pImpl->Device_GetIPAddress      = UniversalClientSPI_Device_GetIPAddress;

    pImpl->Stream_Open                  = UniversalClientSPI_Stream_Open;
    pImpl->Stream_AddComponent          = UniversalClientSPI_Stream_AddComponent;
    pImpl->Stream_RemoveComponent       = UniversalClientSPI_Stream_RemoveComponent;
    pImpl->Stream_Start                 = UniversalClientSPI_Stream_Start;
    pImpl->Stream_Stop                  = UniversalClientSPI_Stream_Stop;
    pImpl->Stream_Send                  = UniversalClientSPI_Stream_Send;
    pImpl->Stream_OpenFilter            = UniversalClientSPI_Stream_OpenFilter;
    pImpl->Stream_SetFilter             = UniversalClientSPI_Stream_SetFilter;
    pImpl->Stream_CloseFilter           = UniversalClientSPI_Stream_CloseFilter;
    pImpl->Stream_Connect               = UniversalClientSPI_Stream_Connect;
    pImpl->Stream_Disconnect            = UniversalClientSPI_Stream_Disconnect;
    pImpl->Stream_SetDescramblingKey    = UniversalClientSPI_Stream_SetDescramblingKey;
    pImpl->Stream_CleanDescramblingKey  = UniversalClientSPI_Stream_CleanDescramblingKey;
    pImpl->Stream_Close                 = UniversalClientSPI_Stream_Close;

    pImpl->PVR_SetSessionKey            = UniversalClientSPI_PVR_SetSessionKey;
    pImpl->PVR_SetExtendedSessionKey    = UniversalClientSPI_PVR_SetExtendedSessionKey;

    pImpl->CopyControl_Macrovision_SetConfig= UniversalClientSPI_CopyControl_Macrovision_SetConfig;
    pImpl->CopyControl_Macrovision_SetMode  = UniversalClientSPI_CopyControl_Macrovision_SetMode;
    pImpl->CopyControl_SetCCI               = UniversalClientSPI_CopyControl_SetCCI;

  #if 1//.0825 tmp to make
    pImpl->Smartcard_Open           = UniversalClientSPI_Smartcard_Open;
    pImpl->Smartcard_Close          = UniversalClientSPI_Smartcard_Close;
    pImpl->Smartcard_Reset          = UniversalClientSPI_Smartcard_Reset;
    pImpl->Smartcard_Communicate    = UniversalClientSPI_Smartcard_Communicate;
  #endif
    pImpl->DateTime_GetTimeOfDay    = UniversalClientSPI_DateTime_GetTimeOfDay;
    pImpl->Crypto_Verify            = UniversalClientSPI_Crypto_Verify;

    pImpl->Timer_Open       = UniversalClientSPI_Timer_Open;
    pImpl->Timer_Close      = UniversalClientSPI_Timer_Close;
    pImpl->Timer_Start      = UniversalClientSPI_Timer_Stop;
    pImpl->Timer_Stop       = UniversalClientSPI_Timer_Start;

    //add the following fucnion from later of 2.10
    pImpl->Device_GetChipConfigurationCheck =   UniversalClientSPI_Device_GetChipConfigurationCheck;
    pImpl->Device_SetCSSN = 					UniversalClientSPI_Device_SetCSSN;
    pImpl->Device_GetPINCode = 					UniversalClientSPI_Device_GetPINCode;
    pImpl->Device_SetMulti2Parameter =  		UniversalClientSPI_Device_SetMulti2Parameter;  


    pImpl->IFCP_Communicate = 			UniversalClientSPI_IFCP_Communicate;
    pImpl->IFCP_LoadImage =  			UniversalClientSPI_IFCP_LoadImage;
    pImpl->SetExtraTrickModeControl =	UniversalClientSPI_SetExtraTrickModeControl;
    pImpl->Stream_Extended_Connect = 	UniversalClientSPI_Stream_Extended_Connect;
    
	pImpl->PS_GetProperty	= UniversalClientSPI_PS_GetProperty;
	//pImpl->Device_GetPVRSecurityState = UniversalClientSPI_Device_GetPVRSecurityState;
	//pImpl->HGPC_SendHNAMessage	= UniversalClientSPI_HGPC_SendHNAMessage;
	pImpl->SCOT_LoadTransformationData	= UniversalClientSPI_SCOT_LoadTransformationData;
	pImpl->Message_CallBack	= UniversalClientSPI_Message_CallBack;
    return;
}

/** @} */ /* End of manufacturercryptospi */

/** @} */ /* End of Cloaked CA Agent SPIs*/

