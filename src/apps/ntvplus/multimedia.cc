#include <windows.h>
#include <appmenus.h>
#include <dvbepg.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_mediaplayer.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <assets.h>
#include <fstream>
#include <lyricsview.h>

NGL_MODULE(MULTIMEDIA)
#define IDC_CHANNELS 100
#define W_ID   100
#define W_PATH 540
#define W_SIZE 200
#define W_DATE 250

namespace ntvplus{

class MediaItem:public ListView::ListItem{
public:
public:
    int id;
    std::string path;
    size_t size;
    bool isdir;
    std::string datetime;
public:
    MediaItem(int id_,const std::string&fname):ListView::ListItem(std::string()){
        struct stat st;
        id=id_;
        size_t pos=fname.rfind("/");
        std::string name=fname.substr(pos+1);
        setText(name);
        path=fname.substr(0,pos);
        stat(fname.c_str(),&st);
        size=st.st_size;
        isdir=S_ISDIR(st.st_mode);
        struct tm *ftm=localtime(&st.st_mtime);
        char buf[64];
        sprintf(buf,"%02d/%02d/%d %02d:%02d",ftm->tm_mon+1,ftm->tm_mday,ftm->tm_year+1900,ftm->tm_hour,ftm->tm_min);
        datetime=buf;
    }
};

static void MediaPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    MediaItem& md=(MediaItem&)itm;
    char buf[32];
    if(state){
       canvas.set_source_rgba(.6,0,0,.8);
       canvas.rectangle(itm.rect);
       canvas.fill();
    }
 
    RECT r=itm.rect;
    canvas.set_color(lv.getFgColor());
    sprintf(buf,"%3d",md.id);
    canvas.draw_text(r,buf,DT_LEFT|DT_VCENTER);//id
    r.offset(W_ID,0);//id width;
    canvas.draw_text(r,md.getText(),DT_LEFT|DT_VCENTER);//media filename;
    
    r.offset(W_PATH,0);//program width
    if(md.size>1024*1024){
         sprintf(buf,"%.2fM",((float)md.size)/(1024*1024));
    }else if(md.size>1024){
         sprintf(buf,"%.2fK",((float)md.size)/1024);
    }else{
         sprintf(buf,"%dB",md.size);
    }
    canvas.draw_text(r,buf,DT_LEFT|DT_VCENTER);//media size
    r.offset(W_SIZE,0);
    canvas.draw_text(r,md.datetime,DT_LEFT|DT_VCENTER);//datetime
}

class MediaWindow:public NTVWindow{
protected:
   ToolBar*mdtype;
   ToolBar*header;
   ListView*mdlist;
   LyricsView*lyrics;
   RefPtr<ImageSurface>image;
   std::string media_path;
   bool sort_revert;
   HANDLE player;
   int filter_type;
   std::vector<std::string> split(const std::string& s,const std::string& delim);
public:
   enum{
     VIDEO=0,
     MUSIC=1,
     PICTURE=2,
     FOLDERS=3
   }Filter;
const static std::string FILTERS[];
public:
   MediaWindow(int x,int y,int w,int h);
   ~MediaWindow(){
      if(player)nglMPClose(player);
      player=nullptr;
   }
   int loadMedia(const std::string&path,int filter);
   int processMedia(MediaItem&itm);
   virtual bool onKeyUp(KeyEvent&k)override;
   bool match(const std::string fname,int filttp){
      std::string ext=fname.substr(fname.rfind(".")+1);
      std::vector<std::string>exts=split(FILTERS[filttp],":");
      for(auto e:exts){
          if(strcasecmp(e.c_str(),ext.c_str())==0){return true;}
      }
      return false;
   }
   std::string filename2URL(const std::string&name){
      std::string url;
      url.append("file://");
      for(int i=0;i<name.length();i++){
         BYTE cc=name.at(i);
         if((cc&0x80)||(cc==' ')){
            char tmp[8];
            sprintf(tmp,"%%%02x",cc);
            url.append(tmp);
         }else url.append(1,(char)cc);
      }
      return url;
   }
   void onDraw(GraphContext&canvas)override{
       RECT rect=getClientRect();
       if(image==nullptr)
            NTVWindow::onDraw(canvas);
       else canvas.draw_image(image,&rect,nullptr,ST_CENTER_CROP);//ST_CENTER_INSIDE); 
   }
};
const std::string MediaWindow::FILTERS[]={
      "mp4;mkv;avi;mpg;mpeg;ts",
      "mp3;wma;wav",
      "jpg;jpeg;png;bmp;ps;pdf",
      ""//empty string for all files
 };


MediaWindow::MediaWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
    player=nullptr;
    filter_type=VIDEO;
    sort_revert=false;
    clearFlag(ATTR_ANIMATE_FOCUS);
    initContent(NWS_TITLE|NWS_TOOLTIPS);
    mdtype=CreateNTVToolBar(1280,30);

    mdtype->addButton(" Movie",120);
    mdtype->addButton("Music",120);
    mdtype->addButton("Photo",120);
    mdtype->addButton("Folders",120);
    mdtype->setIndex(0);
    addChildView(mdtype)->setPos(0,68).setBgColor(0xAA000000);

    header=new ToolBar(1280,30);
    header->addButton(" NO.",160);
    header->addButton("Program",W_PATH);
    header->addButton("Size",200);
    header->addButton("Date/Time",380);
    addChildView(header)->setPos(0,98).setBgColor(0x88000000);
       
    mdlist=new ListView(1200,CHANNEL_LIST_ITEM_HEIGHT*13);
    mdlist->setPos(40,130);
    mdlist->setItemSize(-1,CHANNEL_LIST_ITEM_HEIGHT);
    mdlist->setFlag(View::SCROLLBARS_VERTICAL);
    mdlist->setBgColor(getBgColor());
    mdlist->setFgColor(getFgColor());
    mdlist->setItemPainter(MediaPainter);
    addChildView(mdlist);

    lyrics=new LyricsView("",440,mdlist->getHeight()+30);
    lyrics->setVisible(false);
    addChildView(lyrics)->setPos(840,100).setBgColor(0x80000000).setFgColor(0xAAFFFFFF);

    mdtype->setItemSelectListener([&](AbsListView&lv,const ListView::ListItem&itm,int index){
        filter_type=index;
        loadMedia(media_path,filter_type);
    });
    mdlist->setItemClickListener([&](AbsListView&lv,const ListView::ListItem&itm,int index){
        MediaItem&mi=(MediaItem&)itm;;
        if(!mi.isdir){
           processMedia(mi); 
        }else
           loadMedia(mi.path+"/"+mi.getText(),filter_type);
    });
}
int MediaWindow::processMedia(MediaItem&item){
   std::string path=item.path+"/"+item.getText();
   std::string url=path;//filename2URL(path);
   NGLOG_DEBUG("%s[%s]",path.c_str(),url.c_str()); 
   switch(filter_type){
   case VIDEO: 
   case MUSIC: 
        if(player){
             nglMPStop(player);
             nglMPClose(player);
        }
        player=nullptr;
        if(filter_type==MUSIC){
           lyrics->setText(url);
        }
        player=nglMPOpen(url.c_str());
        nglMPPlay(player);
        break;
   case PICTURE:{
           std::ifstream fs(path.c_str());
           image=Assets::loadImage(fs);
           //mglist->invalidate(nullptr
           invalidate(nullptr);
        }break;
   case FOLDERS:
        break;
   }
}
bool MediaWindow::onKeyUp(KeyEvent&k){
    std::string media_file;
    MediaItem*itm=(MediaItem*)mdlist->getItem(mdlist->getIndex());
    if(itm)media_file=itm->path+"/"+itm->getText();

    switch(k.getKeyCode()){
    case KEY_LEFT:
    case KEY_RIGHT:{
            bool rc=mdtype->onKeyUp(k);
            int idx=mdtype->getIndex();
            filter_type=idx;
            loadMedia(media_path,filter_type);
            mdlist->setSize((filter_type==MUSIC?800:1280),mdlist->getHeight());
            header->setSize((filter_type==MUSIC?840:1280),header->getHeight());
            lyrics->setVisible(filter_type==MUSIC);
            return rc;
         }break;
    case KEY_RED:
         mdlist->sort([](const ListView::ListItem&a,const ListView::ListItem&b)->int{
                            return strcmp(a.getText().c_str(),b.getText().c_str())>0;
                       },sort_revert); 
         sort_revert=!sort_revert;
         for(int i=0;i<mdlist->getItemCount();i++){
              MediaItem*itm=(MediaItem*)mdlist->getItem(i);
              itm->id=i;
         }mdlist->invalidate(nullptr);
         break;
    case KEY_YELLOW:remove(media_file.c_str());break;
    case KEY_GREEN: 
           rename(media_file.c_str(),media_file.c_str());
         break;
    case KEY_VOLUMEDOWN:
    case KEY_VOLUMEUP:
         NGLOG_DEBUG("KEY_VOLUME");
         if(filter_type==VIDEO||filter_type==MUSIC)
             ShowVolumeWindow(2000);
         break;
    default: return NTVWindow::onKeyUp(k);
    }
}

std::vector<std::string> MediaWindow::split(const std::string& s,const std::string& delim){
    std::vector<std::string> elems;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();
    if (delim_len == 0) return elems;
    while (pos < len){
        int find_pos = s.find(delim, pos);
        if (find_pos < 0){
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}

int MediaWindow::loadMedia(const std::string&path,int filter_type){
    int count=0;
    DIR *dir=opendir(path.c_str());
    struct dirent *ent;
    std::string filter=FILTERS[filter_type];
    std::vector<std::string>exts=split(filter,";");
    mdlist->clearAllItems();
    NGLOG_DEBUG("%s :%s",path.c_str(),filter.c_str());
    if(dir==nullptr)return 0;
    while(ent=readdir(dir)){
        bool match=exts.size()==0;
        std::string fname=path+"/"+ent->d_name;
        std::string ext=fname.substr(fname.rfind(".")+1);
        for(auto e:exts){
             if(strcasecmp(e.c_str(),ext.c_str())==0){match=true;break;}
        }
        if((match&&(ent->d_type==DT_REG) )||(ent->d_type==DT_DIR))
             mdlist->addItem(new MediaItem(count++,fname));
    }
    media_path=path;
    closedir(dir);
    return count;
}

Window*CreateMultiMedia(){
    MediaWindow*w=new MediaWindow(0,0,1280,720);
    w->setText("Media");
    w->addTipInfo("help_icon_4arrow.png","Navigation",160);
    w->addTipInfo("help_icon_ok.png","Select",160);
    w->addTipInfo("help_icon_back.png","Back",160);
    w->addTipInfo("help_icon_exit.png","Exit",260);
    w->addTipInfo("help_icon_red.png","A->Z",160);
    w->addTipInfo("help_icon_green.png","Rename",160);
    w->addTipInfo("help_icon_yellow.png","Delete",160);
    w->loadMedia("/",MediaWindow::VIDEO);
    w->show();
    return w;
}
}//namespace
