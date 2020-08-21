#include <windows.h>
#include <stdlib.h>
#include <ngl_log.h>
#include <json/json.h>
#include <appmenus.h>
#include <iostream> 
#include <strstream>
#include <preferences.h>

NGL_MODULE(SETTING)
#define IDC_FIRST_EDITABLE_ID 100
#define IDC_LEFT 101
#define IDC_RIGHT 102

namespace ntvplus{

bool NTVSettingLoadData(Window*w,int id);
void NTVSettingChanged(int winid,View&v,int value);
Window*NTVCreateCustomSettingWindow(int id);

class SettingItem:public AbsListView::ListItem{
public:
   Json::Value value;
public:
   SettingItem(Json::Value v):AbsListView::ListItem(std::string()){
       if(v.isMember("name")){
           std::string id=v["name"].asCString();
           setText(App::getInstance().getString(id));
       }else{//no name ,value is name
           char buf[32];
           sprintf(buf,"%d",v["value"].asInt());
           setText(buf);
       }
       if(v.isMember("value")&&v["value"].isInt())
            setId(v["value"].asInt());
       if(v.isMember("value")&&v["value"].isBool())
            setId(v["value"].asBool());
       value=v;
   }
   bool isLeaf(){
      return value.isMember("id");
   }
};

class EditableWindow:public NTVWindow{
protected:
public :
   int id;
   Preferences*pref;
   EditableWindow(Preferences*p,int x,int y,int w,int h):NTVWindow(x,y,w,h){
        id=-1;
        pref=p;
        initContent(NWS_TITLE|NWS_TOOLTIPS);
   }
   static void ItemChangeListener(AbsListView&lv,const AbsListView::ListItem&itm,int index){
       EditableWindow*win=(EditableWindow*)lv.getParent();
       NGLOG_DEBUG("index=%d value=%d",index,itm.getId());
       win->pref->setValue(win->getText(),lv.getText(),itm.getId());
       NTVSettingChanged(win->id,lv,itm.getId());
   }
   static void OnTextChanged(EditText&e){
       EditableWindow*win=(EditableWindow*)e.getParent();
       NTVSettingChanged(win->id,e,-1);
   }
};

class SettingWindow:public NTVWindow{
protected:
    ListView*left;
    ListView*right;
    std::vector<Json::Value >stacks;
    INT control_id;
    Preferences pref;
public:
    SettingWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
       initContent(NWS_TITLE|NWS_TOOLTIPS);
       pref.load("settings.pref");
       left=new ListView(680,580);
       left->setPos(40,70);
       left->setItemSize(-1,40);
       left->setItemPainter(SettingPainter);
       left->setBgColor(0).setFgColor(0xFFFFFFFF);
       addChildView(left);
       right=new ListView(400,580);
       right->setPos(820,70);
       right->setItemSize(-1,40);
       right->setItemPainter(SettingPainter);
       right->setBgColor(0).setFgColor(0xFFFFFFFF);
       addChildView(right)->setId(IDC_RIGHT);
       right->clearFlag(View::FOCUSABLE);
       left->setItemSelectListener(onItemSelect);
   }
   ~SettingWindow(){
       pref.save("settings.pref");
   }
   bool onKeyUp(KeyEvent&k)override;
   int loadSettings(Json::Value root,bool statcked=true);
   void loadUIItems(EditableWindow*w,Json::Value root,int y);
   Window*createEditable(Json::Value root);
   int getArray(const std::string&key,Json::Value&,Json::Value*root_=nullptr);
   static void onItemSelect(AbsListView&lv,const ListView::ListItem&lvitem,int index){
       SettingItem&itm=(SettingItem&)lvitem;
       ListView*right=(ListView*)lv.getParent()->findViewById(IDC_RIGHT);
       Json::Value array=itm.value["items"];
       right->clear();
       for(int i=0;i<array.size();i++){
           Json::Value b,a=array[i];
           NGLOG_VERBOSE("%d:type:%d %s",i,array.type(),a.toStyledString().c_str());
           /*if(a.isMember("items")&&a["items"].isString()){
                 std::string key=a["items"].asString();
                 if(stacks.size()){
                     getArray(key,b);a=b;
                     for(int i=0;i<a.size();i++)right->addItem(new SettingItem(a[i]));
                 } 
             }else*/
          right->addItem(new SettingItem(a));
       }
  }
};

int SettingWindow::getArray(const std::string&key,Json::Value&array,Json::Value*root_){
    Json::Value rot=root_?*root_:stacks[0];
    NGLOG_VERBOSE("key=%s type=%d",key.c_str(),rot.type());
    switch(rot.type()){
    case Json::objectValue:{
             Json::Value::Members mems = rot.getMemberNames();
             for(auto m:mems){
                 if(!rot[m].isArray())continue;
                 if(key==m){
                      array=rot[m];
                      return 0;
                 }else for(int i=0;i<rot[m].size();i++){
                      int rc=getArray(key,array,&rot[m][i]);
                      if(rc==0)return 0;
                 }
             }
         }break;
    case Json::arrayValue:
         break;//array=
    }
    return -1;
}

bool SettingWindow::onKeyUp(KeyEvent&k){
    switch(k.getKeyCode()){
    case KEY_LEFT:
         NGLOG_VERBOSE("presskey %d",k.getKeyCode());
         stacks.pop_back();
         loadSettings(stacks.back(),false);
         break;
    case KEY_RIGHT:{
             NGLOG_VERBOSE("presskey %d item=%d",k.getKeyCode(),left->getIndex());
             SettingItem*itm=(SettingItem*)left->getItem(left->getIndex());
             if(itm){
                 stacks.back()["remembered_index"]=left->getIndex();
                 NGLOG_VERBOSE("child is leaf=%d index=%d",itm->isLeaf(),left->getIndex()); 
                 if(itm->isLeaf())createEditable(itm->value);
                 else loadSettings(itm->value,true);
             }
         }break;
       default:return NTVWindow::onKeyUp(k);
    }
}

int SettingWindow::loadSettings(Json::Value root_,bool stacked){
   NGLOG_VERBOSE("%s",root_.toStyledString().c_str());
   Json::Value array=root_["items"];
   int index=0;
   NGLOG_VERBOSE("items.size=%d",array.size());
   if(left)left->clear();
   if(root_.isMember("remembered_index")&&root_["remembered_index"].isInt())
       index=root_["remembered_index"].asInt();
   for(int i=0;i<array.size();i++){
       Json::Value v=array[i];
       NGLOG_VERBOSE("%d:%s",i,v["name"].asCString());
       left->addItem(new SettingItem(v));
   }
   NGLOG_VERBOSE("set focus to last focused item %d",index);
   left->setFlag(View::FOCUSED);
   left->setIndex(index);
   if(stacked)stacks.push_back(root_);
   return array.size();
}

void SettingWindow::loadUIItems(EditableWindow*w,Json::Value root,int y){
    if(root.isMember("type")&&root["type"].isString()){
        std::string ctrl=root["type"].asString();
        Json::Value v=root;
        if(ctrl.compare("selector")==0){
            Selector*s=new NTVSelector(v["name"].asCString(),1000,40);
            if(v.isMember("focused")&&v["focused"].asBool()){
                 s->setFlag(View::FOCUSED);
            }
            if(v.isMember("items")){
                 Json::Value values=v["items"];
                if(values.isString())getArray(values.asString(),values);
                int vsaved=pref.getInt(w->getText(),s->getText(),0);
                NGLOG_DEBUG("%s.%s=%d",w->getText().c_str(),s->getText().c_str(),vsaved);
                for(int j=0;j<values.size();j++){
                    SettingItem*itm=new SettingItem(values[j]);
                    s->addItem(itm);
                    if(itm->getId()==vsaved)
                        s->setIndex(j);
                } 
            }
            s->setLabelWidth(600);
            s->setPos(140,y);y+=40;
            s->setPopupRect(900,80,360,560);
            s->setPopupListener([](int w,int h)->Window*{
                Window*win=new Window(0,0,w,h);
                ListView*lv=new ListView(w,h);
                lv->setBgColor(0xFF222222);
                lv->setFgColor(0xFFFFFFFF);
                lv->setItemPainter(SettingPainter);
                win->addChildView(lv);
                return win;
            });
            s->setItemSelectListener(EditableWindow::ItemChangeListener);
            s->setBgColor(0xFF000000);s->setFgColor(0xFFFFFFFF);
            s->setLabelColor(0xFF000000);s->setBgColor(0xFF222222);
            w->addChildView(s)->setId(control_id++);
        }else if(ctrl.compare("edit")==0){
            EditText *e=new NTVEditText(1000,40);
            e->setPos(140,y);y+=40;
            e->setLabelWidth(600);
            e->setBgColor(0xFF222222).setFgColor(0xFFFFFFFF);
            e->setLabelColor(0xFF000000);
            e->setLabel(v["name"].asCString());
            w->addChildView(e)->setId(control_id++);
            e->setTextWatcher(EditableWindow::OnTextChanged);
        }else{
            NGLOG_DEBUG("unknown ui component:%s",ctrl.c_str());
        } 
    }else{
         Json::Value array=root["items"];
         for(int i=0;i<array.size();i++,y+=40){
            loadUIItems(w,array[i],y);
         }
    }
}

Window*SettingWindow::createEditable(Json::Value root){
    int y=80;
    int id=root["id"].asInt();
    if(id<0){
        return NTVCreateCustomSettingWindow(id);
    }
    EditableWindow*w=new EditableWindow(&pref,0,0,1280,720);
    w->setText(root["name"].asString());
    NGLOG_VERBOSE("%s",root.toStyledString().c_str());
    NGLOG_VERBOSE("settings.uiid=%d",id);
    w->id=id;
    w->addTipInfo("help_icon_4arrow.png","Navigation",160);
    w->addTipInfo("help_icon_ok.png","Select",160);
    w->addTipInfo("help_icon_back.png","Back",160);
    w->addTipInfo("help_icon_exit.png","Exit",260);
    w->addTipInfo("help_icon_blue.png","",160);
    w->addTipInfo("help_icon_red.png","",160);
    w->addTipInfo("help_icon_yellow.png","",160);
    control_id=IDC_FIRST_EDITABLE_ID;
    loadUIItems(w,root,y);
    
    NTVSettingLoadData(w,w->id);
    w->show();
    return w;
}

static Window*createSettingMenuFromFile(const std::string&fname){
    SettingWindow*w=new SettingWindow(0,0,1280,720);
    std::shared_ptr<std::istream> sin=App::getInstance().getInputStream(fname);
    
    Json::CharReaderBuilder builder;
    Json::Value root;
    Json::String errs;
    builder["collectComments"] = false;
    
    w->setText("Settings");
    
    w->addTipInfo("help_icon_4arrow.png","Navigation",160);
    w->addTipInfo("help_icon_ok.png","Select",160);
    w->addTipInfo("help_icon_back.png","Back",160);
    w->addTipInfo("help_icon_exit.png","Exit",260);
    w->addTipInfo("help_icon_blue.png","",160);
    w->addTipInfo("help_icon_red.png","",160);
    w->addTipInfo("help_icon_yellow.png","",160);
    
    bool rc=Json::parseFromStream(builder,*sin, &root, &errs);//reader.parse(data,data+size,root);
    NGLOG_DEBUG("%s json.parse=%d %s",fname.c_str(), rc,errs.c_str());
    w->loadSettings(root);
    w->show();
    return w;
}

Window*CreateSettingWindow(){
    return createSettingMenuFromFile("settings.json");
}
Window*CreateMediaWindow(){
    return createSettingMenuFromFile("media.json");
}
}//namespace 
