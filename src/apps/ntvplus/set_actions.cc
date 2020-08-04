#include<appmenus.h>
#include<ngl_disp.h>
#include<ngl_snd.h>
#include<netsetting.h>
#include<ngl_log.h>

NGL_MODULE(SetAction);

namespace ntvplus{

#define ID_FIRST_EDITABLE_ID 100
static void time_load(Window*w){
    char buf[32];
    Selector*s=(Selector*)w->findViewById(ID_FIRST_EDITABLE_ID);
    for(int i=-12;i<=12;i++){
        sprintf(buf,"%02d:00",i);
        s->addItem(new Selector::ListItem(buf,i));
    }
    NGLOG_DEBUG("zone offset=%ld  TZNAME=%s/%s\r\n",timezone/3600,tzname[0],tzname[1]);
    s->setIndex(12+timezone/3600);

    s=(Selector*)w->findViewById(ID_FIRST_EDITABLE_ID+1);

    time_t tnow=time(NULL);
    struct tm *tmnow=localtime(&tnow);
    EditText*edt=(EditText*)w->findViewById(ID_FIRST_EDITABLE_ID+2);
    edt->setEditMode(EditText::REPLACE);
    sprintf(buf,"%02d:%02d",tmnow->tm_hour,tmnow->tm_min);
    edt->setText(buf);
}
static void time_changed(View&v,int value){
    switch(v.getId()){
    case ID_FIRST_EDITABLE_ID:{
             Selector&s=(Selector&)v;
             char zone[8];
             sprintf(zone,"GMT%s%d",(value>=0?"+":""),value);
             setenv("TZ",zone,1);tzset();
             NGLOG_DEBUG("SetTimeSoze->%s",zone);             
        }break;
    default:break;
    }
}

static void picture_load(Window*w){}
static void picture_changed(View&v,int value){
   Selector&s=(Selector&)v;
   switch(v.getId()){
   case ID_FIRST_EDITABLE_ID:    nglDispSetResolution(value);break;
   case ID_FIRST_EDITABLE_ID+1:  nglDispSetAspectRatio(value);break;
   case ID_FIRST_EDITABLE_ID+2:  nglDispSetMatchMode(value);break;
   case ID_FIRST_EDITABLE_ID+3:  nglDispSetBrightNess((5+value)*10);break;
   case ID_FIRST_EDITABLE_ID+4:  nglDispSetContrast((5+value)*10);break; 
   case ID_FIRST_EDITABLE_ID+5:  nglDispSetSaturation((5+value)*10);break;
   }
}

static void sound_changed(View&v,int value){
   switch(v.getId()){
   case ID_FIRST_EDITABLE_ID  :nglSndSetOutput(SDT_SPDIF,value);break;
   case ID_FIRST_EDITABLE_ID+1:nglSndSetOutput(SDT_HDMI,value);break;
   case ID_FIRST_EDITABLE_ID+2:nglSndSetOutput(SDT_CVBS,value);break;
   }
}

static void network_load(Window*w){
    std::vector<std::string>nets;
    int cnt=getNetworkInterface(nets);
    Selector*s=(Selector*)w->findViewById(ID_FIRST_EDITABLE_ID);
    for(auto name:nets){
        s->addItem(new Selector::ListItem(name));
    }
    s->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&item,int index){
          char ip[32],mask[32],mac[32],gateway[32];
          std::string ifname=item.getText();
          getNetworkInfo(ifname.c_str(),ip,mask,gateway,mac);
          Window*w=(Window*)lv.getParent();
          EditText*e=(EditText*)w->findViewById(ID_FIRST_EDITABLE_ID+1);
          e->setText(ip);
          e=(EditText*)w->findViewById(ID_FIRST_EDITABLE_ID+2);
          e->setText(mask);
          e=(EditText*)w->findViewById(ID_FIRST_EDITABLE_ID+3);
          e->setText(gateway);    
    });
    s->setIndex(0);
}

static void network_changed(View&v,int value){
    NGLOG_DEBUG("=====network_changed %d",v.getId());
    Selector*sif=(Selector*)v.getParent()->findViewById(ID_FIRST_EDITABLE_ID);
    std::string ifname=sif->getItem(sif->getIndex())->getText();
    std::string ipaddr;
    if(v.getId()>ID_FIRST_EDITABLE_ID){
        ipaddr=((EditText&)v).getText();
        NGLOG_DEBUG("===ipaddr=%s",ipaddr.c_str());
    }
    switch(v.getId()-ID_FIRST_EDITABLE_ID){
    case 1:setNetworkIP(ifname.c_str(),ipaddr.c_str());break;
    case 2:setNetworkMask(ifname.c_str(),ipaddr.c_str());break;
    default:break;
    }
}

typedef struct{
   int id;
   SettingDataLoadingListener onLoadData;
   SettingChangeListener onSettingChange; 
}UI_ACTIONS;

static UI_ACTIONS setting_actions[]={
   {1,time_load,time_changed},
   {2,picture_load,picture_changed},
   {3,nullptr,sound_changed},
   {11,network_load,network_changed}
};

bool NTVSettingLoadData(Window*w,int id){
   for(int i=0;i<sizeof(setting_actions)/sizeof(UI_ACTIONS);i++){
       if(setting_actions[i].id==id){
           if(setting_actions[i].onLoadData!=nullptr)
               setting_actions[i].onLoadData(w);
           return true;
       }
   }
   return false;
}

void NTVSettingChanged(int winid,View&v,int value){
   NGLOG_DEBUG("Editable UI[%d].ctrls[%d] changed to %d\r\n",winid,v.getId(),value);
   for(int i=0;i<sizeof(setting_actions)/sizeof(UI_ACTIONS);i++){
       if( (setting_actions[i].id==winid) && (setting_actions[i].onSettingChange!=nullptr) ){
            setting_actions[i].onSettingChange(v,value);
            return ;
       }
   }
}

Window*NTVCreateCustomSettingWindow(int id){
    switch(id){
    case -1: return CreatePVRWindow();
    case -2: return CreateMultiMedia();
    case -50:return CreateChannelSearch();
    case -51:return CreateChannelSearch(1);
    }
}

}//namespace 
