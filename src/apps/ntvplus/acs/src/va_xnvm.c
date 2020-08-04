#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>

#include <aui_common.h>
#include <aui_flash.h>

#include "va_setup.h"
#include "va_xnvm.h"
#include <ngl_types.h>
#include <ngl_log.h>

NGL_MODULE(VAXNVM)

#define VA_NVM_DUMP  NGLOG_DUMP 

#define VA_NVM_FLASH_ID     10 //mtd8 for VA XNVM module
#define SEGMENT_SOFTWARE_SIZE       kVA_SETUP_ACS_DATA_SEGMENT_SIZE
#define SEGMENT_ACS_DATA_SIZE       kVA_SETUP_ACS_DATA_SEGMENT_SIZE
#define SEGMENT_LOADER_DATA_SIZE    kVA_SETUP_ACS_DATA_SEGMENT_SIZE

#define XNVM_BLOCK_NAME         "va_xvnm"

#define XNVM_STATUS_IDEL        0
#define XNVM_STATUS_OPEN        (1 << 0)
#define XNVM_STATUS_READING     (1 << 1)
#define XNVM_STATUS_READ        (1 << 2)
#define XNVM_STATUS_WRITING     (1 << 3)
#define XNVM_STATUS_WRITTEN      (1 << 4)
#define XNVM_STATUS_CLOSE       (1 << 5)


typedef struct nvm_segment_info{
    DWORD dwVaXnvmHandle;
    DWORD segment_offset;
    DWORD segment_size;
    aui_hdl flash_hdl;
    DWORD dwStbXnvmHandle;

    UINT32 read_uiOffset;
    UINT32 read_uiSize;
    BYTE* pReadData;

    UINT32 write_uiOffset;
    UINT32 write_uiSize;
    BYTE* pWriteData;
    
    int status;
}nvm_segment_info;


static unsigned long m_segmnet_size[] =
{
    SEGMENT_SOFTWARE_SIZE,
    SEGMENT_ACS_DATA_SIZE,
    SEGMENT_ACS_DATA_SIZE,
    SEGMENT_LOADER_DATA_SIZE,
};

nvm_segment_info m_nvm_segment[eSEGMENT_LAST];
static int m_nvm_init = 0;
static int va_find_mtd_dev_num(const char *mtd_name,char *mtd_dev, unsigned int max_len);

static aui_hdl va_nvm_flash_handle_get()
{
    int i;
    for (i = 0; i < eSEGMENT_LAST; i ++){
        if (m_nvm_segment[i].flash_hdl)
            return m_nvm_segment[i].flash_hdl;
    }

    return NULL;
}

//check if the VNVM handle is a legal handle
static int va_nvm_legal_handle(DWORD dwStbXnvmHandle)
{
    int i;
    for (i = 0; i < eSEGMENT_LAST; i ++){
        if (dwStbXnvmHandle == m_nvm_segment[i].dwStbXnvmHandle)
            return 1;
    }
    return 0;
}

static int va_nvm_if_device_busy(DWORD dwStbXnvmHandle)
{

    nvm_segment_info *segment_info = (nvm_segment_info*)dwStbXnvmHandle;

    if ((XNVM_STATUS_READING & segment_info->status) ||
        (XNVM_STATUS_WRITING & segment_info->status))
        return 1;
    return 0;
}


static void *va_nvm_read_thread(void *arg)
{
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    unsigned long real_size = 0;
    unsigned long start_addr = 0;
    nvm_segment_info *segment_info = (nvm_segment_info*)arg;

    pthread_detach(pthread_self());

    if (!arg){
        NGLOG_ERROR("arg is NULL!");
        ret = AUI_RTN_FAIL;
        goto read_exit;
    }

    start_addr = segment_info->segment_offset + segment_info->read_uiOffset;
	ret = aui_flash_read(segment_info->flash_hdl, start_addr, 
        segment_info->read_uiSize, &real_size, segment_info->pReadData);
    if (ret != AUI_RTN_SUCCESS){
        NGLOG_ERROR("aui_flash_read error! ret=%d", (int)ret);
        goto read_exit;
    }

read_exit:    
    segment_info->status = XNVM_STATUS_READ;

    if (ret){
        VA_XNVM_RequestFailed(segment_info->dwVaXnvmHandle);
    }else{
        VA_XNVM_RequestDone(segment_info->dwVaXnvmHandle);
    }

    pthread_exit(NULL);
}


static void *va_nvm_write_thread(void *arg)
{
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    unsigned long real_size = 0;
    unsigned long start_addr = 0;
    nvm_segment_info *segment_info = (nvm_segment_info*)arg;

    pthread_detach(pthread_self());

    if (!arg){
        NGLOG_ERROR("arg is NULL!");
        ret = AUI_RTN_FAIL;
        goto write_exit;
    }

    start_addr = segment_info->segment_offset + segment_info->write_uiOffset;
	ret = aui_flash_auto_erase_write(segment_info->flash_hdl, start_addr, 
        segment_info->write_uiSize, &real_size, segment_info->pWriteData);
    if (ret != AUI_RTN_SUCCESS){
        NGLOG_ERROR("aui_flash_write error! ret=%d", (int)ret);
        goto write_exit;
    }

write_exit:    
    segment_info->status = XNVM_STATUS_WRITTEN;

    if (ret){
        VA_XNVM_RequestFailed(segment_info->dwVaXnvmHandle);
    }else{
        VA_XNVM_RequestDone(segment_info->dwVaXnvmHandle);
    }
    pthread_exit(NULL);
}

static char* va_find_last_mtd_dev(const char *head,
		const char *tail, const char *key)
{
	char *pos;
	unsigned int len;
	if (tail == NULL || key == NULL)
	{
		return NULL;
	}

	len = strlen(key);
	pos = (char *)(tail - len);
	while (strncmp(pos, key, len) != 0 &&
			(pos - head >= 0))
	{
		pos--;
	}

	if (pos - head < 0)
	{
		return NULL;
	}

	return pos;
}


DWORD   VA_XNVM_Open(DWORD dwVaXnvmHandle, tVA_XNVM_Segment eSegment)
{
    AUI_RTN_CODE ret = AUI_RTN_SUCCESS;
    aui_hdl flash_hdl = NULL;
    aui_flash_open_param open_param;
    int i;

    
    NGLOG_DEBUG("parameter: dwVaXnvmHandle=0x%x, eSegment=%d!", (unsigned int)dwVaXnvmHandle, (int)eSegment);
    
    if (0 == m_nvm_init){
        ret = aui_flash_init(NULL, NULL);
        if (ret){
            NGLOG_ERROR("aui_flash_init error: %d", (int)ret);
            return kVA_ILLEGAL_HANDLE;
        }
        unsigned long offset = 0;
        memset(m_nvm_segment, 0, sizeof(m_nvm_segment));
        for (i = 0; i < eSEGMENT_LAST; i ++){
            m_nvm_segment[i].dwVaXnvmHandle = kVA_ILLEGAL_HANDLE;
            m_nvm_segment[i].segment_size = m_segmnet_size[i];
            m_nvm_segment[i].segment_offset = (eSegment-1)*m_segmnet_size[i];//offset;
            m_nvm_segment[i].status = XNVM_STATUS_IDEL;
            offset += m_segmnet_size[i];
        }
        m_nvm_init = 1;
    }

    if (m_nvm_segment[eSegment].flash_hdl){
        NGLOG_ERROR("XNVM segment %d is already open!", (int)eSegment);
        return kVA_ILLEGAL_HANDLE;
    }
    for (i = 0; i < eSEGMENT_LAST; i ++){
        if (dwVaXnvmHandle == m_nvm_segment[i].dwVaXnvmHandle){
            NGLOG_ERROR("dwVaXnvmHandle(%d) has already been used!", i);
            return kVA_ILLEGAL_HANDLE;
        }
    }

    flash_hdl = va_nvm_flash_handle_get();
    memset(&open_param, 0, sizeof(aui_flash_open_param));
    if (!flash_hdl){
        char mtd_dev[128] = {0};
        //all segments use one same mtd flash device
        if (va_find_mtd_dev_num(XNVM_BLOCK_NAME, mtd_dev, 32)){
            NGLOG_ERROR("find_mtd_dev_num() error!");
            return kVA_ILLEGAL_HANDLE;
        }else{
            open_param.flash_id = atoi(mtd_dev + 3); // mtd_dev: "mtd1"
        }
    	open_param.flash_type = AUI_FLASH_TYPE_NAND;

    	ret = aui_flash_open(&open_param, &flash_hdl);
        if (ret){
            NGLOG_ERROR("aui_flash_open error: %d", (int)ret);
            return kVA_ILLEGAL_HANDLE;
        }
    }

    NGLOG_DEBUG("flash handle: 0x%x", (unsigned int)flash_hdl);    
    m_nvm_segment[eSegment].flash_hdl = flash_hdl;
    m_nvm_segment[eSegment].dwVaXnvmHandle = dwVaXnvmHandle;
    m_nvm_segment[eSegment].dwStbXnvmHandle= (DWORD)(&(m_nvm_segment[eSegment]));
    m_nvm_segment[eSegment].status = XNVM_STATUS_OPEN;

    return (m_nvm_segment[eSegment].dwStbXnvmHandle);
    
}

    
INT     VA_XNVM_Close(DWORD dwStbXnvmHandle)
{
    int i;
    int segment_cnt = 0;
    AUI_RTN_CODE ret = AUI_RTN_SUCCESS;
    nvm_segment_info *segment_info = (nvm_segment_info*)dwStbXnvmHandle;

    NGLOG_DEBUG("parameter: dwStbXnvmHandle=0x%x!",(unsigned int)dwStbXnvmHandle);

    if (0 == dwStbXnvmHandle){
        NGLOG_ERROR("dwStbXnvmHandle is 0!");
        return kVA_INVALID_PARAMETER;
    }

    if (!va_nvm_legal_handle(dwStbXnvmHandle)){
        NGLOG_ERROR("dwStbXnvmHandle is illegal handle!");
        return kVA_INVALID_PARAMETER;
    }
    
    segment_cnt = 0;
    for (i = 0; i < eSEGMENT_LAST; i ++){
        if (m_nvm_segment[i].flash_hdl)
            segment_cnt ++;
    }

    if (0 == segment_cnt){
        //Close mtd flash device when all segments are closed.
        NGLOG_DEBUG("aui_flash_close handle: 0x%x", (unsigned int)(segment_info->flash_hdl));
        ret = aui_flash_close(segment_info->flash_hdl);
        if (ret){
            NGLOG_ERROR("aui_flash_close error: %d", (int)ret);
            return kVA_ERROR;
        }
    }
    segment_info->dwVaXnvmHandle = kVA_ILLEGAL_HANDLE;
    segment_info->dwStbXnvmHandle = 0;
    segment_info->flash_hdl = NULL;
    segment_info->status = XNVM_STATUS_CLOSE;

    segment_info->read_uiOffset = 0;
    segment_info->read_uiSize = 0;
    segment_info->pReadData = NULL;
    segment_info->write_uiOffset = 0;
    segment_info->write_uiSize = 0;
    segment_info->pWriteData = NULL;

    return kVA_OK;
}


INT     VA_XNVM_Read (DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pReadData)
{
    nvm_segment_info *segment_info = (nvm_segment_info*)dwStbXnvmHandle;
	pthread_t va_read_t;

    
    NGLOG_DEBUG("parameter: dwStbXnvmHandle=0x%x, uiOffset=%d, uiSize=%d, pReadData=0x%x!",
        (unsigned int)dwStbXnvmHandle, (int)uiOffset, (int)uiSize, (unsigned int)pReadData);

    if (0 == dwStbXnvmHandle){
        NGLOG_ERROR("dwStbXnvmHandle is 0!");
        return kVA_INVALID_PARAMETER;
    }

    if (!va_nvm_legal_handle(dwStbXnvmHandle)){
        NGLOG_ERROR("dwStbXnvmHandle is illegal handle!");
        return kVA_INVALID_PARAMETER;
    }

    if ((0 == uiSize) || ((uiSize+uiOffset) > segment_info->segment_size)){
        NGLOG_ERROR("size is illegal!");
        return kVA_INVALID_PARAMETER;
    }
        
    if (NULL == pReadData){
        NGLOG_ERROR("pReadData is NULL!");
        return kVA_INVALID_PARAMETER;
    }

    if (va_nvm_if_device_busy(dwStbXnvmHandle)){
        NGLOG_ERROR("device is busy!");
        return kVA_RESOURCE_BUSY;
    }
    segment_info->status = XNVM_STATUS_READING;

    segment_info->read_uiOffset = uiOffset;
    segment_info->read_uiSize = uiSize;
    segment_info->pReadData = pReadData;
    if (pthread_create(&va_read_t, NULL, va_nvm_read_thread, (void*)(segment_info)) != 0){
 	NGLOG_ERROR("create thread failure!");
        segment_info->status = XNVM_STATUS_READ;
	return kVA_ERROR;
    }
    return kVA_OK;
}

INT     VA_XNVM_Write(DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pWriteData)
{
    nvm_segment_info *segment_info = (nvm_segment_info*)dwStbXnvmHandle;
	pthread_t va_write_t;

    
    NGLOG_DEBUG("parameter: dwStbXnvmHandle=0x%x, uiOffset=%d, uiSize=%d, pWriteData=0x%x!",
        (unsigned int)dwStbXnvmHandle, (int)uiOffset, (int)uiSize, (unsigned int)pWriteData);

    if (0 == dwStbXnvmHandle){
        NGLOG_ERROR("dwStbXnvmHandle is 0!");
        return kVA_INVALID_PARAMETER;
    }

    if (!va_nvm_legal_handle(dwStbXnvmHandle)){
        NGLOG_ERROR("dwStbXnvmHandle is illegal handle!");
        return kVA_INVALID_PARAMETER;
    }

    if ((0 == uiSize) || ((uiSize+uiOffset) > segment_info->segment_size)){
        NGLOG_ERROR("size is illegal!");
        return kVA_INVALID_PARAMETER;
    }
        
    if (NULL == pWriteData){
        NGLOG_ERROR("pReadData is NULL!");
        return kVA_INVALID_PARAMETER;
    }

    if (va_nvm_if_device_busy(dwStbXnvmHandle)){
        NGLOG_ERROR("device is busy!");
        return kVA_RESOURCE_BUSY;
    }
    segment_info->status = XNVM_STATUS_WRITING;

    segment_info->write_uiOffset = uiOffset;
    segment_info->write_uiSize = uiSize;
    segment_info->pWriteData = pWriteData;
    if (pthread_create(&va_write_t, NULL, va_nvm_write_thread, (void*)(segment_info)) != 0){
          NGLOG_ERROR("create thread failure!");
          segment_info->status = XNVM_STATUS_WRITTEN;
	  return kVA_ERROR;
    }  
    return kVA_OK;
}

static int va_find_mtd_dev_num(const char *mtd_name,char *mtd_dev, unsigned int max_len)
{
        char *buffer=NULL;
        unsigned int len = 0;
        int readed;
        FILE *fp = NULL;

        if((fp = fopen("/proc/mtd", "r"))==NULL){
                NGLOG_DEBUG("Can not open file /proc/mtd");
                return 1;
        }

        while((readed=getline(&buffer,&len,fp))!=-1){
             char*start=strstr(buffer,mtd_name);
             if(start!=NULL){
                 char*p=strchr(buffer,':');
                 NGLOG_DEBUG("buffer=%p p=%p",buffer,p);
                 memcpy(mtd_dev,buffer,p-buffer);
                 mtd_dev[p-buffer]=0;
                 free(buffer);
                 fclose(fp);
                 return 0;
             }
        }
        fclose(fp);
        return 1;
}

