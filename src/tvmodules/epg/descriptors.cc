#include<stdarg.h>
#include<stdio.h>
#include<string.h>
#include<descriptors.h>
#include<ngl_os.h>
#include<utils.h>
#include<ngl_log.h>

NGL_MODULE(DESCRIPTORS);

Descriptors::Descriptors(){
    ownedbuff=0;
    descriptors=NULL;
    length=0;
}

Descriptors::Descriptors(const Descriptors&o)
  :Descriptors(){
    setDescriptor(o.descriptors,o.length,true);
}

Descriptors::Descriptors(const BYTE*des,INT len,bool deepcopy)
  :Descriptors(){
     setDescriptor(des,len,deepcopy);
}

Descriptors::~Descriptors(){
    if(ownedbuff && descriptors)
        nglFree(descriptors);
    descriptors=NULL;
}

void Descriptors::dump(BOOL verbose)const{
    int len=length;
    BYTE*pd=descriptors;
    while(len>0){
        int dlen=pd[1];
        if(verbose)
            NGLOG_DUMP("DES:",pd,pd[1]+2);
        else NGLOG_DEBUG("DES:0x%02X[%02x]",*pd,pd[1]);
        len-=(dlen+2);
        pd+=2+dlen;
    }
}

void Descriptors::setDescriptor(const BYTE*des,INT len,bool deepcopy){
    if(ownedbuff && descriptors)
         nglFree(descriptors);
    if(deepcopy){
       descriptors=(BYTE*)nglMalloc(len);
       memcpy(descriptors,des,len);
    }else{
       descriptors=(BYTE*)des;
    }
    length=len;
    ownedbuff=deepcopy;
}

void Descriptors::setDescriptor(const Descriptors&o,bool deepcopy){
    NGLOG_DEBUG("ownedbuff=%d",ownedbuff);
    setDescriptor(o.descriptors,o.length,deepcopy);
}

BYTE*Descriptors::findDescriptor(BYTE*des,int len,int tag){
    for(int pos=0;pos<len;){
        if(des[pos]==tag)return des+pos;
        pos+=2+des[pos+1];
    }
    return NULL;
}

BYTE*Descriptors::findDescriptor(INT tag)const{
    for(int pos=0;pos<length;){
        if(descriptors[pos]==tag)return descriptors+pos;
        pos+=2+descriptors[pos+1];
    }
    return NULL;
}

INT Descriptors::findDescriptors(INT tag,...)const
{
    int found=0;
    va_list ap;
    va_start(ap,tag);
    found=(findDescriptor(tag)!=NULL); 
    do{
       tag=va_arg(ap,int);
       found+=(NULL!=findDescriptor(tag));
    }while(tag);
    va_end(ap);
    return found;
}

const BYTE*LinkageDescriptor::getPrivates(int*len)const{
    BYTE*pd=descriptors+9;
    switch(getLinkageType()){//descriptors[8]
    case 0x08:pd+=3;break;//Mobile hand-over linkage:fixedlength 3byes
    case 0x0d:pd+=3;break;//event linkage info://fixedlength 3byes
    default://[0x0e...0x1f]extended_event_linkage_info
         if(getLinkageType()>=0x0e&&getLinkageType()<=0x1F)
             pd+=(1+pd[1]);
         break;
    }
    if(len)*len=length-(pd-descriptors);
    return pd;
}

LinkageDescriptor::operator const SERVICELOCATOR()const{
    SERVICELOCATOR l;
    l.tsid = (descriptors[2]<<8)|descriptors[3];
    l.netid= (descriptors[4]<<8)|descriptors[5];
    l.sid  = (descriptors[6]<<8)|descriptors[7];
    l.tpid = 0xFFFF;
    return l;
}

int NameDescriptor::getName(char*name){
    ToUtf8((const char*)(descriptors+2),length,name);
    return descriptors[1];
}

int ServiceDescriptor::getName(char*name,char*provider){
     BYTE*pd=descriptors;
     int plen=pd[3];
     int slen=pd[4+plen];
     if(provider)ToUtf8((const char*)(pd+4),plen,provider);
     if(name)    ToUtf8((const char*)(pd+5+plen),slen,name);
     return (slen<<16)|plen;
}

int MultiServiceNameDescriptor::getName(char*name,char*provider,const char*lan){
    BYTE*p=descriptors+2;
    for(int i=0;i<length;i++){
        char nlan[3];
        memcpy(nlan,p,3);
        int plen=p[3];
        int slen=p[4+plen];
        if((lan==nullptr)||(memcmp(p,lan,3)==0)){
            if(provider)ToUtf8((const char*)(p+4),plen,provider);
            if(name)ToUtf8((const char*)(p+5+plen),slen,name);
            return (slen<<16)|plen;
        }
        p+=(5+plen+slen);
    }   
    return 0;
}

int ParentRatingDescriptor::getRatings(BYTE*clan){
    if(clan)memcpy(clan,descriptors,length);
    return length/4;
}

int ParentRatingDescriptor::getRating(const char*lan){
    for(int i=0;i<length;i+=4){
        if((lan==nullptr)||(memcmp(descriptors+i+2,lan,3)==0))
           return descriptors[+i+5];
    }
    return 0;
}

void ShortEventNameDescriptor::getLanguage(char*lan){
    memcpy(lan,descriptors+2,3);
}

int ShortEventNameDescriptor::getName(char*name,char*desc){
    BYTE*p=descriptors;
    if(name)name[0]=0;
    if(desc)desc[0]=0;
    if(p){
        int nmlen=p[5];
        ToUtf8((const char*)(p+6),nmlen,name); 
        int dlen=p[6+nmlen];
        ToUtf8((const char*)(p+7+nmlen),dlen,desc);
        return nmlen<<16|dlen;
    }
    return 0;
}

void ExtendEventDescriptor::getLanguage(char*lan){
    memcpy(lan,descriptors+3,3);
}

int  ExtendEventDescriptor::getText(char*text){
    BYTE*p=descriptors+(getItemLength()+7);
    return ToUtf8((const char*)(p+1),p[0],text);
}

int MultiNameDescriptor::getName(char*name,const char*lan){
    BYTE*p=descriptors+2;
    for(int i=0;i<length;){
        char nlan[4];
        memcpy(nlan,p,3);
        int nlen=p[3];
        if(lan==NULL||memcmp(nlan,lan,3)==0){
            ToUtf8((const char*)(p+4),nlen,name);
            return nlen;
        }
        p+=(3+nlen);
    }
    return 0;
}

ServiceListDescriptor::ServiceListDescriptor(USHORT nid,USHORT tid,const BYTE*pd,int len,bool deep)
   :Descriptors(pd,len,deep){
    netid=nid;
    tsid=tid;
}

int ServiceListDescriptor::getService(SERVICELOCATOR*svc,BYTE*types){
    BYTE*pd=descriptors+2;
    int count=length/3;
    for(int i=0;i<count;i++){
        svc[i].netid=netid;
        svc[i].tsid=tsid;
        svc[i].sid=(pd[0]<<8)|pd[1];
        if(types)types[i]=pd[2];
        pd+=3;
    }
    return count;
}

NordigLCNDescriptor::NordigLCNDescriptor(USHORT nid,USHORT tid,const BYTE*pd,int len,bool deep)
   :Descriptors(pd,len,deep){
    netid=nid;
    tsid=tid;
}

int NordigLCNDescriptor::getLCN(SERVICELOCATOR*svc,USHORT*lcn){
    BYTE*pd=descriptors+2;
    int count=length/4;
    for(int i=0;i<count;i++){
        svc[i].netid=netid;
        svc[i].tsid=tsid;
        svc[i].sid=(pd[0]<<8)|pd[1];
        if(lcn)lcn[i]=(pd[2]<<8)|pd[3];
        pd+=4;
    }
    return count;
}

NordigLCNDescriptorV2::NordigLCNDescriptorV2(USHORT nid,USHORT tid,const BYTE*pd,int len,bool deep)
   :NordigLCNDescriptor(nid,tid,pd,len,deep){
}

int NordigLCNDescriptorV2::getLCN(SERVICELOCATOR*svc,USHORT*lcn){
    BYTE*pd=descriptors+2;
    int idx=0;
    for(;pd<descriptors+getLength();){
        BYTE chid=pd[0];
        BYTE chnamelen=pd[1];
        BYTE country[3];
        memcpy(country,pd+2,3);
        int len=pd[5+chnamelen];
        pd+=6+chnamelen;
        for(int i=0;i<len;i+=4,pd+=4,idx++){
            svc[idx].netid=netid;
            svc[idx].tsid=tsid;
            svc[idx].sid=(pd[0]<<8)|pd[1]; 
            lcn[idx]=(pd[2]<<8)|pd[3];
        }
    }
    return idx;
}

int SubtitleDescriptor::getItems(SubtitleItem*items,char*lan){
    SubtitleItem*pitm=items;
    BYTE*pb=descriptors+2;
    for(int i=0;i<descriptors[1]/8;i++){
        SubtitleItem itm;
        memcpy(itm.lan639,pb,3);
        itm.lan639[3]=0;
        itm.subtitle_type=pb[3];
        itm.composition_page_id=(pb[4]<<8)|pb[5];
        itm.ancillary_page_id=(pb[6]<<8)|pb[7];
        pb+=8;
        if((lan==NULL)||(memcmp(itm.lan639,lan,3)==0))
            *pitm++=itm;
    }
    return pitm-items;
}

int TeletextDescriptor::getItems(TeletextItem*items,char*lan){
    TeletextItem*pitm=items;
    BYTE*pb=descriptors+2;
    for(int i=0;i<getCount();i++){
        TeletextItem itm;
        memcpy(itm.lan639,pb,3);
        itm.lan639[3]=0;
        itm.type=pb[3]>>3;
        itm.magazine=pb[3]&0x07;
        itm.page_number=pb[4];
        pb+=5;
        if((lan==NULL)||(memcmp(itm.lan639,lan,3)==0))
           *pitm++=itm;
    }
    return pitm-items;
}
/*0x0006 data_carousel_*/
USHORT DataBroadcastDescriptor::getBroascastId()const{
    return (descriptors[2]<<8)|descriptors[3];
}

BYTE  DataBroadcastDescriptor::getComponentTag()const{
    return descriptors[4];
}

INT  DataBroadcastDescriptor::getSelectorBytes(BYTE*selectbytes)const{
    int len=descriptors[5];
    memcpy(selectbytes,descriptors+6,len);
    return len;
}

INT DataBroadcastDescriptor::getCarouselInfo(CarouselInfo*ci)const{
    BYTE*p=descriptors+6;
    ci->carousel_type=p[0]>>6;//bits:10 two layer carousel
    ci->transactionId=(p[1]<<24)|(p[2]<<16)|(p[3]<<8)|p[4];
    ci->timeout_DSI=(p[5]<<24)|(p[6]<<16)|(p[7]<<8)|p[8];
    ci->timeout_DII=(p[9]<<24)|(p[10]<<16)|(p[11]<<8)|p[12];
    p+=13;
    ci->leak_rate=(p[0]<<16)|(p[1]<<8)|p[2];
    ci->leak_rate&=0x3FFFFF;
    return 16;
}

INT  DataBroadcastDescriptor::getText(char*txt,char*lan)const{
    BYTE*p=descriptors+6+descriptors[5];
    if(lan)memcpy(lan,p,3);
    p+=3;
    memcpy(txt,p+1,p[0]);
    return p[0];
}

USHORT DataBroadcastIDDescriptor::getBroascastId()const{
    return (descriptors[2]<<8)|descriptors[3];
}

INT DataBroadcastIDDescriptor::getSelectorBytes(BYTE*bytes)const{
    memcpy(bytes,descriptors+4,descriptors[1]-2);
    return descriptors[1]-2;
}

