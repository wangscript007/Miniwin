#ifndef __DSMCC_H__
#define __DSMCC_H__
#include <si_table.h>
#include <string>

class DSMCC:public PSITable{
protected:
    INT getCompatibilityDescriptor(BYTE*pd)const;
public:
    typedef enum{
        DownloadInfoIndication=0x1002,
      	DownloadDataBlock     =0x1003,
        DownloadCancel	      =0x1005,
        DownloadServerInitiate=0x1006
    }MessageID;/**/
    typedef struct{
       BYTE protocolDiscriminator;
       BYTE dsmccType;
       USHORT messageId;
       UINT transactionId;
       BYTE reserved;
       BYTE adaptationLength; 
       USHORT messageLength;
       BYTE*adaptors;//adaptor infos
       BYTE*descriptors;//CompatibalDescriptors
    }Header;
    DSMCC(const BYTE*buf,bool deepcopy=true):PSITable(buf,deepcopy){}
    DSMCC(const PSITable&b,bool deepcopy=true):PSITable(b,deepcopy){}
    UINT getTransactionId()const;
    int getHeader(Header*hdr)const;
    USHORT getMessageId()const;
};

class DSI:public DSMCC{
public:
   class GroupInfoIndication:public Descriptors{
   public:
       UINT GroupId;
       UINT GroupSize;
       GroupInfoIndication():Descriptors(){}
       std::string getName()const;
   };
   DSI(const BYTE*buf,bool deepcopy=true):DSMCC(buf,deepcopy){}
   DSI(const PSITable&b,bool deepcopy=true):DSMCC(b,deepcopy){}
   void getServerId(BYTE*)const;
   int getPrivateLength()const;
   BYTE*getPrivate()const;//GroupInfoIndication
   INT getGroupCount()const;
   INT getGroups(GroupInfoIndication*)const; 
};

class DII:public DSMCC{
public:
   class ModuleInfo:public Descriptors{
   public:
       USHORT moduleId;
       UINT moduleSize;
       BYTE moduleVersion;
       INT position;/*module position(for multi modules carousel)*/
       ModuleInfo():Descriptors(){}
       std::string getName()const;
   };
   DII(const BYTE*buf,bool deepcopy=true):DSMCC(buf,deepcopy){}
   DII(const PSITable&b,bool deepcopy=true):DSMCC(b,deepcopy){}
   INT getDownloadId()const;
   INT getBlockSize()const;
   INT getModuleCount()const;
   INT getModules(ModuleInfo*mds)const;
};

class DDB:public DSMCC{
public:
  DDB(const BYTE*buf,bool deepcopy=true):DSMCC(buf,deepcopy){}
  DDB(const PSITable&b,bool deepcopy=true):DSMCC(b,deepcopy){}
  USHORT getModuleId()const;
  UINT getDownloadId()const{return getTransactionId();} 
  BYTE getModuleVersion()const;
  USHORT getBlockNumber()const;
  INT getBlockData(BYTE*p)const;
};

#endif
