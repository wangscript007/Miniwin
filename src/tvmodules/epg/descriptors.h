#ifndef __DVBSI_DESCRIPTOR_H__
#define __DVBSI_DESCRIPTOR_H__
#include<destags.h>
#include<ngl_types.h>
///////////////////////////////////////////////////////////////////////////////////////
typedef struct ServiceLocator{
   USHORT netid;//original network id or tp(for DVBS)
   USHORT tsid; //transport stream id
   USHORT sid;  //service id
   USHORT tpid; //for satellite tp
   bool operator==(const ServiceLocator&b)const{return netid==b.netid && tsid==b.tsid && sid==b.sid;}
   ServiceLocator&operator=(const ServiceLocator &b){
      netid=b.netid;tsid=b.tsid;sid=b.sid;tpid=b.tpid;
   }
}SERVICELOCATOR;

typedef struct{
    BYTE lan639[4];
    BYTE subtitle_type;
    USHORT pid;
    USHORT composition_page_id;
    USHORT ancillary_page_id;
}SubtitleItem;

typedef struct{
    BYTE lan639[4];
    /*0x00 reserved for future use
      0x01 initial Teletext page
      0x02 Teletext subtitle page
      0x03 additional information page
      0x04 programme schedule page
      0x05 Teletext subtitle page for hearing impaired people*/
    BYTE type;
    BYTE magazine;
    BYTE page_number;
    USHORT pid;
}TeletextItem;

class Descriptors{
private:
   int ownedbuff;
protected:
   INT length;//length include tag and data...
   BYTE*descriptors;
public:
   Descriptors();
   Descriptors(const Descriptors&o);
   Descriptors(const BYTE*des,INT len,bool deepcopy=false);
   ~Descriptors();
   void setDescriptor(const BYTE*des,INT len,bool deepcopy=false);
   void setDescriptor(const Descriptors&o,bool deepcopy=true);
   BYTE*findDescriptor(INT tag)const;
   INT findDescriptors(INT,...)const;
   static BYTE*findDescriptor(BYTE*des,int len,int tag);
   int getLength()const{return length;}
   void dump(BOOL verbose=FALSE)const;
   operator const BYTE*(){return descriptors;}
};

class CADescriptor:Descriptors{
public:
    CADescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    USHORT getCAID(){return descriptors[2]<<8|descriptors[3];}
    USHORT getEcmPID(){return ((descriptors[4]&0x1F)<<8)|descriptors[5];}
};

class LinkageDescriptor:public Descriptors{
public:
    /*LinkageType 0x09:Private bytes is:System Software Update Service:ETSI TS 102 006[11]*/
    typedef struct{
        BYTE oui_data_length;
        BYTE *privates;
        struct{
           UINT OUI;
           BYTE selector_length;
           BYTE*selectors;/*PlatformType:1Byte */
        }sorfts;
    }SoftwareUpdateService;
    LinkageDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    BYTE getLinkageType()const{return descriptors[8];}
    const BYTE*getPrivates(int*len=0)const;
    operator const SERVICELOCATOR()const;
};

class NameDescriptor:public Descriptors{
public:
    NameDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    int getName(char*name);
};

class MultiNameDescriptor:public Descriptors{
public:
    MultiNameDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    int getName(char*name,const char*lan);
};
typedef NameDescriptor BouquetNameDescriptor;
typedef NameDescriptor NetworkNameDescriptor;
typedef MultiNameDescriptor MultiNetworkNameDescriptor;
typedef MultiNameDescriptor MultiBouquetNameDescriptor;

class ServiceDescriptor:Descriptors{
public:
    ServiceDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    BYTE getServiceType(){return descriptors[2];}
    int getName(char*name,char*provider);
};

class MultiServiceNameDescriptor:public Descriptors{
public:
    MultiServiceNameDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    int getName(char*name,char*provider,const char*lan);
};

class ShortEventNameDescriptor:Descriptors{
public:
    ShortEventNameDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    void getLanguage(char*lan);
    int getName(char*name,char*desc);
};

class ContentDescriptor:Descriptors{
public:
    ContentDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    void getNibble(BYTE*nibble1,BYTE*nibble2=nullptr,BYTE*ub=nullptr){//only get 1st nibble 
        if(nibble1)*nibble1=descriptors[2]>>4;
        if(nibble2)*nibble2=descriptors[2]&0x0F;
    }
};

class ExtendEventDescriptor:public Descriptors{
public:

    ExtendEventDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    int getDescriptorNumber(){return descriptors[2]>>4;}
    int getLastDescriptorNumber(){return descriptors[2]&0x0F;}
    int getItemLength(){return descriptors[6];}
    void getLanguage(char*lan);
    int getText(char*text);
};

class SubtitleDescriptor:public Descriptors{
public:
    SubtitleDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    int getCount(){return length>>3;}
    int getItems(SubtitleItem*items,char*lan);
};
class ParentRatingDescriptor:public Descriptors{
public:
    ParentRatingDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
    int getRatings(BYTE*clan);
    int getRating(const char*lan);
};

class ServiceListDescriptor:public Descriptors{
protected:
    USHORT netid,tsid;
public:
    ServiceListDescriptor(USHORT netid,USHORT tsid,const BYTE*pd,int len,bool deep=false);
    int getService(SERVICELOCATOR*svc,BYTE*service_type=nullptr);
};

class TeletextDescriptor:public Descriptors{
public:
public:
  TeletextDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
  int getCount(){return length/5;}
  int getItems(TeletextItem*items,char*lan=nullptr);
};

class DataBroadcastDescriptor:public Descriptors{
public:
  typedef struct{
      BYTE carousel_type;//bits:10 two layer carousel
      UINT transactionId;
      UINT timeout_DSI;
      UINT timeout_DII;
      UINT leak_rate;
  }CarouselInfo;
  
  DataBroadcastDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
  USHORT getBroascastId()const;
  BYTE getComponentTag()const;
  INT getSelectorBytes(BYTE*selectbytes)const;
  INT getCarouselInfo(CarouselInfo*di)const;
  INT getText(char*txt,char*lan)const;
};

class DataBroadcastIDDescriptor:public Descriptors{
public:
  typedef struct{//ETSI TS 101 006 V1.4.1 Page.13
      BYTE oui_length;
  }SoftwareUpdateInfo;
  DataBroadcastIDDescriptor(const BYTE*pd,int len,bool deep=false):Descriptors(pd,len,deep){}
  USHORT getBroascastId()const;/*0x000A for system software update info*/
  INT getSelectorBytes(BYTE*bytes)const;
  INT getUpdateInfo(SoftwareUpdateInfo*info)const;
};

class NordigLCNDescriptor:public Descriptors{//0x83 NordigLCN V1
protected:
    USHORT netid,tsid;
public:
    NordigLCNDescriptor(USHORT netid,USHORT tsid,const BYTE*pd,int len,bool deep=false);
    //lcn only used 14bit :1:visibleflag 1:reserved 14:lcn
    virtual int getLCN(SERVICELOCATOR*svc,USHORT*lcn);
};

class NordigLCNDescriptorV2:NordigLCNDescriptor{
public://used int BAT NIT in second descriptors' loop
    NordigLCNDescriptorV2(USHORT netid,USHORT tsid,const BYTE*pd,int len,bool deep=false);
    int getLCN(SERVICELOCATOR*svc,USHORT*lcn);
};

#endif //enf of _DVBSI_DESCRIPTORS
