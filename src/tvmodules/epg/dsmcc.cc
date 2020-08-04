#include <dsmcc.h>
#include <ngl_log.h>
#include <unordered_map>
#include <utils.h>

NGL_MODULE(DSMCC);

int DSMCC::getHeader(Header*hd)const{
    BYTE*p=data+8;
    hd->protocolDiscriminator=p[0];//(p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
    hd->dsmccType=p[1];//03-->  
    hd->messageId=(p[2]<<8)|p[3];
    hd->transactionId=(p[4]<<24)|(p[5]<<16)|(p[6]<<8)|p[7];
    hd->reserved=p[8];//reserved 1 bye
    hd->adaptationLength=p[9];
    hd->messageLength=p[10]<<8|p[11];
    hd->adaptors=(data+12+8);
    hd->descriptors=(data+12+8+hd->adaptationLength);
    return (int)20+hd->adaptationLength;    
}

UINT DSMCC::getTransactionId()const{
    Header hd;
    getHeader(&hd);
    return hd.transactionId;
}

USHORT DSMCC::getMessageId()const{
    BYTE*p=data+8;
    return (p[2]<<8)|p[3];
}

int DSMCC::getCompatibilityDescriptor(BYTE*pd)const{
    BYTE*p=pd;
    int dlen=(p[0]<<8)|p[1];
    return dlen;
}

void DSI::getServerId(BYTE*sid)const{
    memcpy(sid,data+20,20);
}

int DSI::getPrivateLength()const{
    
}

BYTE*DSI::getPrivate()const{//GroupInfoIndication
}

INT DSI::getGroupCount()const{
   DSMCC::Header hd;
   BYTE*p=data+getHeader(&hd);
   p+=20;//skip Server-ID
   NGLOG_VERBOSE("compatibledes.len=%d",(p[0]<<8)|p[1]);
   p+=getCompatibilityDescriptor(p)+2;//skip dsmcc_CompatibilityDescriptor (b)
   INT plen=(p[0]<<8)|p[1];p+=2;//skip privateDataLength
   NGLOG_VERBOSE("plen=%d %02x %02x %02x %02x\r\n",plen,p[0],p[1],p[2],p[3]);
   return (p[0]<<8)|p[1];
}

INT DSI::getGroups(GroupInfoIndication*infos)const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    p+=20;//skip Server-ID
    p+=getCompatibilityDescriptor(p)+2;//skip dsmcc_CompatibilityDescriptor (b)
    p+=2;//skip privateDataLength
    INT count=(p[0]<<8)|p[1];p+=2;//groupcount
    for(int i=0;i<count;i++){
        GroupInfoIndication*g=infos+i;
        g->GroupId  =(p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
        g->GroupSize=(p[4]<<24)|(p[5]<<16)|(p[6]<<8)|p[7];
        p+=8;
        
	p+=getCompatibilityDescriptor(p)+2;//skip dsmcc_CompatibilityDescriptor (b);
        g->setDescriptor(p+2,(p[0]<<8)|p[1]);
        p+=2+g->getLength();
    }
    return count;
}


std::string DSI::GroupInfoIndication::getName()const{
   BYTE*des=findDescriptor(0x02);
   char buf[256];
   if(des){
      int len=ToUtf8((const char*)(des+2),des[1],buf);
      return std::string(buf,len);
   }return std::string();
}

std::string DII::ModuleInfo::getName()const{
   BYTE*des=findDescriptor(0x02);
   char buf[256];
   if(des){
      int len=ToUtf8((const char*)(des+2),des[1],buf);
      return std::string(buf,len);
   }return std::string();
}

INT DII::getDownloadId()const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
}

INT DII::getBlockSize()const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    return (p[4]<<8)|p[5];
}

INT DII::getModuleCount()const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    p+=16;
    p+=getCompatibilityDescriptor(p)+2;//skip CompatibalDescriptors
    return (p[0]<<8)|p[1];
}

INT DII::getModules(ModuleInfo*mds)const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    p+=16;//skip downloadId blockSize ...
    p+=getCompatibilityDescriptor(p)+2;//skip CompatibalDescriptors
    INT count=(p[0]<<8)|p[1];
    p+=2;
    std::unordered_map<int,ModuleInfo*>maps;
    for(int i=0;i<count;i++){
        ModuleInfo*m=mds+i;
        m->moduleId=(p[0]<<8)|p[1];
        m->moduleSize=(p[2]<<24)|(p[3]<<16)|(p[4]<<8)|p[5];
        m->moduleVersion=p[6];
        m->position=0;
        m->setDescriptor(p+8,p[7]);
        p+=8+m->getLength();
        maps[m->moduleId]=m;
        NGLOG_VERBOSE("Module[%d].Size=%d V:%d DLEN:%d",m->moduleId,m->moduleSize,m->moduleVersion,m->getLength());
    }
    ModuleInfo*first=nullptr;
    for(int i=0;i<count;i++){
        BYTE*des=mds[i].findDescriptor(0x04);
        if(des==nullptr)continue;
        if(des[2]==0){
            first=mds+i;
            first->position=0;
            break;
        }
    }
    for(;first;){
        BYTE*des=first->findDescriptor(0x04);
        if(des==nullptr)break;
        USHORT link=(des[3]<<8)|des[4];
        if((des[2]==2)||(link==0))break;
        maps[link]->position=first->position+1;
        first=maps[link];
    }
    return count;
}

USHORT DDB::getModuleId()const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    return (p[0]<<8)|p[1];
}

BYTE DDB::getModuleVersion()const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    return p[2];
}

USHORT DDB::getBlockNumber()const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    return (p[4]<<8)|p[5];
}

INT DDB::getBlockData(BYTE*pdata)const{
    DSMCC::Header hd;
    BYTE*p=data+getHeader(&hd);
    p+=6;
    int len=sectionLength()+3-4;
    len-=(p-data);
    if(pdata)memcpy(pdata,p,len);
    return len;
}

