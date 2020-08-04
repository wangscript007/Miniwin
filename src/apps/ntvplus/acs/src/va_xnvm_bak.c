/*
  FILE : stub_xnvm.c
  PURPOSE: This file is a stub for linking tests.
*/

#include <stdio.h>
#if 1 //(!defined VAOPT_ENABLE_ACS30 && !defined VAOPT_ENABLE_ACS31)
#include "va_xnvm.h"
#include "aui_flash.h"
#include <va_setup.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_msgq.h>
NGL_MODULE(ACSXNVM)

typedef struct{
  UINT32 addr;
  UINT32 size;
  FILE*file;
  const char*fname;
  aui_hdl hdl;
}NVMSEGMENT;

typedef struct{
  int isread;
  void* hdl;
  DWORD vahdl;
  unsigned char*buffer;
  UINT offset;
  UINT size; 
}NVMREQUEST;
#define XNVM_START_ADDR    0// 0x730000  //ALI3528
//kVA_SETUP_ACS_DATA_SEGMENT_SIZE   64*1024
static NVMSEGMENT Segments[eSEGMENT_LAST]={
  //init segment addr and size eSEGMENT_SOFTWARE = 0, eSEGMENT_ACS_DATA_1, eSEGMENT_ACS_DATA_2, eSEGMENT_LOADER_DATA
  {XNVM_START_ADDR+kVA_SETUP_ACS_DATA_SEGMENT_SIZE*0,kVA_SETUP_ACS_DATA_SEGMENT_SIZE,NULL,"./software.dat",NULL},//this segment is optional
  {XNVM_START_ADDR+kVA_SETUP_ACS_DATA_SEGMENT_SIZE*1,kVA_SETUP_ACS_DATA_SEGMENT_SIZE,NULL,"./acsdata1.dat",NULL},
  {XNVM_START_ADDR+kVA_SETUP_ACS_DATA_SEGMENT_SIZE*2,kVA_SETUP_ACS_DATA_SEGMENT_SIZE,NULL,"./acsdata2.dat",NULL},
  {XNVM_START_ADDR+kVA_SETUP_ACS_DATA_SEGMENT_SIZE*3,kVA_SETUP_ACS_DATA_SEGMENT_SIZE,NULL,"./aceloader.dat",NULL}//this segment is optional
};
static DWORD msgq_nvm=0;
static int NVMWrite(void*hdl,UINT offset,BYTE*buf,UINT size);
static int NVMRead(void*hdl,UINT offset,BYTE*buf,UINT size);
static int va_find_mtd_dev_num(const char *mtd_name,char *mtd_dev, unsigned int max_len);
static void NVMProc(void*p){
   while(1){
       int ret=0;
       NVMREQUEST req;
       if(NGL_OK!=nglMsgQReceive(msgq_nvm,&req,sizeof(NVMREQUEST),1000))continue;
       if(req.isread)
            ret=NVMRead(req.hdl,req.offset,req.buffer,req.size);
       else
            ret=NVMWrite(req.hdl,req.offset,req.buffer,req.size);
       if(kVA_OK==ret)
            VA_XNVM_RequestDone(req.vahdl);
       else VA_XNVM_RequestFailed(req.vahdl);
       NGLOG_DEBUG("     =====%s::VA_XNVM_RequestDone(%p). ret=%d",__FUNCTION__,req.vahdl,ret);
   }
}

//#define USE_FILE 1 
DWORD   VA_XNVM_Open(DWORD dwVaXnvmHandle, tVA_XNVM_Segment eSegment)
{
    aui_flash_open_param open_param;
    aui_hdl flash_handle = 0;
    NVMSEGMENT*s=NULL;
    char mtd_dev[128];
    va_find_mtd_dev_num("va_xvnm",mtd_dev,32);
    if(eSegment<0||eSegment>= eSEGMENT_LAST)
        return  kVA_ILLEGAL_HANDLE;
    s=Segments+eSegment;
    if(s->file!=NULL||s->hdl!=NULL){
       NGLOG_DEBUG("VA_XNVM_Open file=%p ,hdl=%p",s->file,s->hdl);
       return  kVA_ILLEGAL_HANDLE;
    }
    if(0==msgq_nvm){
        DWORD tid;
        msgq_nvm=nglMsgQCreate(16,sizeof(NVMREQUEST));
        nglCreateThread(&tid,0,0,NVMProc,NULL);
    }
#ifndef USE_FILE
    static int inited=0;
    if(0==inited){
        aui_flash_init(NULL,NULL);
        inited++;
    }
    bzero(&open_param,sizeof(open_param));
    open_param.flash_id =atoi(mtd_dev + 3);//eSegment;//?
    open_param.flash_type =AUI_FLASH_TYPE_NAND;//AUI_FLASH_TYPE_NOR;//AUI_FLASH_TYPE_NAND;
    NGLOG_DEBUG("%s  dwVaXnvmHandle=0x%x flash_id=%d segment=%d",__FUNCTION__,dwVaXnvmHandle,open_param.flash_id,eSegment);
    AUI_RTN_CODE err = aui_flash_open(&open_param, &flash_handle);
    aui_flash_info flash_info;
    bzero(&flash_info,sizeof(flash_info));
    NGLOG_DEBUG("aui_flash_open segment %d result=%d  flash_handle=%p",eSegment,err,&flash_handle);
    if(err)
        return kVA_ILLEGAL_HANDLE;
    err=aui_flash_info_get(flash_handle, &flash_info);
    NGLOG_DEBUG("VA_XNVM_Open.aui_flash_info_get=%d segment %d startaddr=%x size=0x%x blockcng=%d,blksize=0x%x handle=%p",err,eSegment,flash_info.star_address,
           flash_info.flash_size, flash_info.block_cnt,flash_info.block_size,s);
    s->hdl=flash_handle;
    //s->size=flash_info.flash_size;
#else
    s->file=fopen(s->fname,"ab+");
    NGLOG_DEBUG("fname=%s file=%p  size=0x%x",s->fname,s->file,s->size);
    fseek(s->file,0,SEEK_END);
    long i,fsize=ftell(s->file);
    if(s->file&&(fsize<s->size)){
        int cnt=(s->size-fsize+127)/128;
        unsigned char buf[128];
        memset(buf,0xFF,128); 
        for(i=0;i<cnt;i++)
           fwrite(buf,128,1,s->file);
    }
#endif
    return (DWORD)s;
}

INT VA_XNVM_Close(DWORD dwStbXnvmHandle)
{
    NVMSEGMENT*s=(NVMSEGMENT*)dwStbXnvmHandle;
    NGLOG_DEBUG("%s\r\n",__FUNCTION__);
    if(s<Segments||s>=&Segments[eSEGMENT_LAST])
        return kVA_INVALID_PARAMETER;
    if(s->hdl==NULL&&s->file==NULL)
        return kVA_ILLEGAL_HANDLE;
#ifndef USE_FILE
    if(s->hdl)
        aui_flash_close(s->hdl);
#else
    if(s->file);
        fclose(s->file);
#endif
    s->hdl=NULL;
    s->file=NULL;
    return kVA_OK;
}

INT VA_XNVM_Read (DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pReadData)
{
   NVMSEGMENT*s=(NVMSEGMENT*)dwStbXnvmHandle;
   INT32 ret,readed;
   if(s<Segments||s>=&Segments[eSEGMENT_LAST])
       return kVA_INVALID_PARAMETER;
   NGLOG_DEBUG("handle=%p %s offset=0x%x size=0x%x segmentaddr=%x:%x",s,__FUNCTION__,uiOffset,uiSize,s->addr,s->size);
   if(uiOffset+uiSize>s->size||pReadData==NULL||uiSize ==0)
       return kVA_INVALID_PARAMETER;
   if(s->hdl==NULL&&s->file==NULL) return kVA_ILLEGAL_HANDLE;

   NVMREQUEST req;
   req.isread=1;
   req.vahdl=dwStbXnvmHandle;
   req.offset=uiOffset;
#ifndef USE_FILE
   req.hdl=s->hdl;
   req.offset+=s->addr;
#else
   req.hdl=s->file;
#endif
   req.size=uiSize;
   req.buffer=pReadData;
   nglMsgQSend(msgq_nvm,&req,sizeof(NVMREQUEST),100);
   return kVA_OK;
}
static int NVMRead(void*hdl,UINT offset,BYTE*buf,UINT size){
   UINT readed;
   int ret;
#ifndef USE_FILE
   ret=aui_flash_read(hdl,offset,size,&readed,buf);
#else
   ret=fseek((FILE*)hdl,offset,SEEK_SET);
   readed=fread(buf,1,size,(FILE*)hdl);
#endif
   NGLOG_DEBUG("readed=0x%x return %d",readed,ret);
   return ret==0?kVA_OK:kVA_ERROR;
}

INT  VA_XNVM_Write(DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pWriteData)
{
  NVMSEGMENT*s=(NVMSEGMENT*)dwStbXnvmHandle;
  INT ret, writed;
  if(s<Segments||s>=&Segments[eSEGMENT_LAST])
       return kVA_INVALID_PARAMETER;
  NGLOG_DEBUG("handle=%p %s offset=0x%x size=0x%x segmentaddr:%x:%x",s,__FUNCTION__,uiOffset,uiSize,s->addr,s->size);
  if(uiOffset+uiSize>s->size||pWriteData==NULL||uiSize ==0)
       return kVA_INVALID_PARAMETER;
  if(s->hdl==NULL&&s->file==NULL) return kVA_ILLEGAL_HANDLE;
  NVMREQUEST req;
  req.isread=0;
  req.vahdl=dwStbXnvmHandle; 
  req.offset=uiOffset;
#ifndef USE_FILE
   req.hdl=s->hdl;
   req.offset+=s->addr;
#else
   req.hdl=s->file;
#endif
  req.size=uiSize;
  req.buffer=pWriteData;
  nglMsgQSend(msgq_nvm,&req,sizeof(NVMREQUEST),100);
  return kVA_OK;
}
static int NVMWrite(void*hdl,UINT offset,BYTE*buf,UINT size){
   UINT writed;
   int ret;
#ifndef USE_FILE
   //ret=aui_flash_write(hdl,offset,size,&writed,buf);
   ret=aui_flash_auto_erase_write(hdl,offset,size,&writed,buf);
#else 
   ret=fseek((FILE*)hdl,offset,SEEK_SET);
   writed=fwrite(buf,1,size,(FILE*)hdl);
   fflush((FILE*)hdl);
#endif
  NGLOG_DEBUG("writed=0x%x return kVA_OK  fseek=%d",writed,ret);
  return ret==0?kVA_OK:kVA_ERROR;
}

static int va_find_mtd_dev_num(const char *mtd_name,
		char *mtd_dev, unsigned int max_len)
{
	char *buffer=NULL;
	unsigned int len = 0;
	int readed;
	FILE *fp = NULL;

	if((fp = fopen("/proc/mtd", "r"))==NULL){
		printf("[%s](%d) Can not open file!\n", __FUNCTION__, __LINE__);
		return 1;
	}
         
        while((readed=getline(&buffer,&len,fp))!=-1){
             char*start=strstr(buffer,mtd_name);
             if(start!=NULL){
                 char*p=strchr(buffer,':');printf("buffer=%p p=%p",buffer,p);
                 memcpy(mtd_dev,buffer,p-buffer);
                 mtd_dev[p-buffer]=0;
                 free(buffer);
                 fclose(fp);
                 return 0;
             }
        }
	return 1;
}
#endif

/* End of File */
