/*
  FILE : stub_fs.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#if 1 //defVAOPT_ENABLE_FILESYSTEM
#include "va_errors.h"
#include "va_fs.h"
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#if 1//(!defined VAOPT_ENABLE_ACS30 && !defined VAOPT_ENABLE_ACS31 && !defined VAOPT_ENABLE_ACS40)


DWORD VA_FS_Open(const char * szFilename, tVA_FS_Mode eOpeningMode)
{
    printf("%s\r\n",__FUNCTION__);
    int md=0;
    switch(eOpeningMode){
    case eFILE_READ:md=O_RDONLY;break;
    case eFILE_WRITE:md=O_RDWR|O_CREAT;break;
    case eFILE_PURGE:md=O_RDWR|O_CREAT|O_TRUNC;break;
    }
    return (DWORD)open(szFilename,md, S_IRWXU | S_IXGRP | S_IROTH | S_IXOTH);
}

INT VA_FS_Close(DWORD dwFileHandle)
{
    printf("%s\r\n",__FUNCTION__);
    struct stat st;
    if(fstat((INT)dwFileHandle,&st)==0)
        return close(dwFileHandle);
    return kVA_INVALID_PARAMETER;
}

INT VA_FS_Read (DWORD dwFileHandle, UINT32 *pNbReadBytes, BYTE *pBuffer)
{
    printf("%s\r\n",__FUNCTION__);
    struct stat st;
    if(fstat((INT)dwFileHandle,&st)!=0||NULL==pNbReadBytes||NULL==pBuffer)
        return kVA_INVALID_PARAMETER;
    switch(*pNbReadBytes=read(dwFileHandle,pBuffer,*pNbReadBytes)){
    case -1:return kVA_ERROR;
    case 0: return kVA_EOF;
    default:return kVA_OK;
    }
}

INT VA_FS_Write (DWORD dwFileHandle, UINT32 *pNbWriteBytes, BYTE * pBuffer)
{
    printf("%s\r\n",__FUNCTION__);
    struct stat st;
    if(fstat((INT)dwFileHandle,&st)!=0||NULL==pNbWriteBytes||NULL==pBuffer)
        return kVA_INVALID_PARAMETER;
     *pNbWriteBytes=write(dwFileHandle,pBuffer,*pNbWriteBytes);
    return 0;
}

INT VA_FS_Seek (DWORD dwFileHandle, 
                tVA_FS_SeekOrigin eOrigin, INT32 iOffset, 
                UINT32 *pNewPositionFromBegin)
{
    printf("%s\r\n",__FUNCTION__);
    int ss=SEEK_SET;
    struct stat st;
    if(fstat((INT)dwFileHandle,&st)!=0||NULL==pNewPositionFromBegin)
         return kVA_INVALID_PARAMETER;
    switch(eOrigin){
    case eSEEK_FROM_BEGIN:  ss=SEEK_SET;break;
    case eSEEK_FROM_CURRENT:ss=SEEK_CUR;break;
    case eSEEK_FROM_END:    ss=SEEK_END;break;
    }
    return lseek(dwFileHandle,iOffset,ss)?kVA_ERROR:kVA_OK;
}

INT VA_FS_GetSize (const char * szFilename, UINT32 *pFileSize)
{
    printf("%s\r\n",__FUNCTION__);
    struct stat st;
    if(NULL==szFilename||NULL==pFileSize||strlen(szFilename)>=kVA_FS_MAX_FILE_NAME_SIZE)
         return kVA_INVALID_PARAMETER; 
    int rc=lstat(szFilename,&st);
    *pFileSize=(UINT32)st.st_size;
    return 0==rc?kVA_OK:kVA_FILE_NOT_FOUND;
}

INT VA_FS_Remove (const char * szFilename)
{
    printf("%s\r\n",__FUNCTION__);
    if(NULL==szFilename||strlen(szFilename)>=kVA_FS_MAX_FILE_NAME_SIZE)
        return kVA_INVALID_PARAMETER;
    return 0==remove(szFilename)?kVA_OK:kVA_FILE_NOT_FOUND;
}

#define VIACCESS_DIR "/usr/lib/gio"
//tobe confirmed :where is "reserved Viaccess-Orca directory"
UINT32 VA_FS_GetFilesCount(void)
{
    UINT32 count=0;
    printf("%s\r\n",__FUNCTION__);
    
    DIR*dir=opendir(VIACCESS_DIR);
    struct dirent*entry;
    if(dir==NULL)
      return 0;
    while(entry=readdir(dir)){
        struct stat st;
        char fname[PATH_MAX];
        sprintf(fname,"%s/%s",VIACCESS_DIR,entry->d_name);
        int rc=stat(fname,&st);
        if( (0==rc) && S_ISREG(st.st_mode))
            count++;
    }
    closedir(dir);
    return count;
}

INT VA_FS_GetFiles(UINT32 *pFilesCount, tVA_FS_FileInfo aFiles[])
{
    UINT32 idx=0;
    int i,cnt;
    printf("%s\r\n",__FUNCTION__);
    if(NULL==pFilesCount)
       return kVA_INVALID_PARAMETER;
    cnt=*pFilesCount;
    for(i=0;i<cnt;i++){
       DIR*dir=opendir(aFiles[i].szName);
       struct dirent*entry;
       if(dir==NULL)
         return 0;
       while(entry=readdir(dir)){
           if(idx>=*pFilesCount)break;
           strcpy(aFiles[idx++].szName,(const char*)entry->d_name);
       }
       closedir(dir);
    }
   *pFilesCount=idx;
    return 0;
}

#endif /* (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40) */

#endif /* VAOPT_ENABLE_FILESYSTEM */

/* End of File */

