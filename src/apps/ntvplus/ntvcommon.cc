#include<ntvcommon.h>
#include<app.h>
#include <ngl_video.h>
#include<ngl_timer.h>
#include<ngl_log.h>

NGL_MODULE(NTVCOMMON)

namespace ntvplus{

NTVEditText::NTVEditText(int w,int h):EditText(w,h){
    setBgColor(0xFF222222);
    setFgColor(0xFFFFFFFF);
}
void NTVEditText::onDraw(GraphContext&canvas){
    setBgColor(hasFlag(FOCUSED)?0xFF00FF00:0xFF222222);
    EditText::onDraw(canvas);
}

NTVTitleBar::NTVTitleBar(int w,int h):View(w,h){
   setFgColor(0xFFFFFFFF);
   setBgColor(0xAA000000);
   logo=App::getInstance().loadImage("mainmenu_small_logo.png");
}

void NTVTitleBar::setTime(time_t tn){
   time_now=tn;
   invalidate(nullptr);
}

void NTVTitleBar::setTitle(const std::string&txt){
   title=txt;//App::getInstance().getString(txt);
   invalidate(nullptr);
}

void NTVTitleBar::onDraw(GraphContext&canvas){
    int xx=50;
    char buf[32];
    const char*weekday[]={"sunday","monday","tuesday","wednesday","thursday","friday","saturday"};
    RECT rect=getClientRect();
    View::onDraw(canvas);
    
    RECT rcimg=rect;
    rcimg.x=50;
    rcimg.width=200;
    canvas.set_font_size(40);
    canvas.set_source_rgb(.6,0,0);
    canvas.draw_text(rcimg,"NEWGLEE",DT_LEFT|DT_VCENTER);
    xx+=rcimg.width;
 
    RefPtr<Gradient>pat=LinearGradient::create(xx,0,xx,getHeight());
    pat->add_color_stop_rgba(.0,1.,.0,.0,.2);
    pat->add_color_stop_rgba(.5,.0,1.,.0,1.);
    pat->add_color_stop_rgba(1.,.0,.0,1.,.2);
    canvas.set_source_rgb(.6,0,0);
    canvas.set_font_size(50);
    canvas.move_to(xx,0);
    canvas.line_to(xx,getHeight());
    canvas.rectangle(rect.x,rect.y+rect.height,rect.width,1);
    canvas.stroke();
    rect.x=xx+8;
    canvas.set_source(pat);
    canvas.draw_text(rect,title,DT_LEFT|DT_VCENTER); 
    time_t tnow=time(NULL); 
    struct tm tmnow;
    localtime_r(&tnow,&tmnow);
    TextExtents te1,te2;
    
    sprintf(buf,"%02d:%02d:%02d",tmnow.tm_hour,tmnow.tm_min,tmnow.tm_sec);
    std::string stime=buf;
    sprintf(buf,"%02d/%02d/%d",tmnow.tm_mon+1,tmnow.tm_mday,1900+tmnow.tm_year);
    std::string sdate=buf;
    std::string sweek=weekday[tmnow.tm_wday];

    canvas.get_text_extents(stime,te2);
    rect.x=getWidth()-220;
    rect.width=te2.width;
    canvas.draw_text(rect,stime);
    
    canvas.set_font_size(25); 
    canvas.get_text_extents(sdate,te1);
    rect.x=getWidth()-380;
    rect.width=te1.width+te1.height;
    sweek=App::getInstance().getString(sweek);
    canvas.draw_text(rect,sweek+"\n"+sdate,DT_CENTER|DT_VCENTER|DT_MULTILINE);
}


NTVSelector::NTVSelector(const std::string&txt,int w,int h):Selector(txt,w,h){
    setBgColor(0xFF000000);
    setFgColor(0xFFFFFFFF);
}

void NTVSelector::onDraw(GraphContext&canvas){
     Selector::onDraw(canvas);
     RefPtr<Gradient>pat=LinearGradient::create(0,0,getWidth(),getHeight());
     pat->add_color_stop_rgba(.0,.2,.2,.2,.2);
     pat->add_color_stop_rgba(.5,1.,1.,1.,1.);
     pat->add_color_stop_rgba(1.,.2,.2,.2,.2);
     canvas.set_source(pat);//color(0xFF888888);
     canvas.move_to(0,0);canvas.line_to(getWidth(),0);
     canvas.move_to(0,getHeight());canvas.move_to(getWidth(),getHeight());
     canvas.stroke();
}

NTVProgressBar::NTVProgressBar(int w,int h):ProgressBar(w,h){

}
void NTVProgressBar::onDraw(GraphContext&canvas){
     RECT r=getClientRect();
     RefPtr<Gradient>pat=LinearGradient::create(0,0,0,r.height);
     pat->add_color_stop_rgba(.0,.2,.2,.2,1.);
     pat->add_color_stop_rgba(.5,.0,.0,.0,1.);
     pat->add_color_stop_rgba(1.,.2,.2,.2,1.);
     canvas.set_source(pat);//color(0xFF888888);
     canvas.rectangle(r);
     canvas.fill();
     if(getWidth()>getHeight()){
         r.width=r.width*progress_/(max_-min_);
     }else{
         int h=r.height;
         r.height=r.height*progress_/(max_-min_);
         r.offset(0,h-r.height);
     }
     RefPtr<Gradient>pat1=LinearGradient::create(r.x,r.y,r.x,r.height);//height);
     pat1->add_color_stop_rgba(.0,.2,.2,.2,1.);
     pat1->add_color_stop_rgba(.5,1.,1.,1.,1.);
     pat1->add_color_stop_rgba(1.,.2,.2,.2,1.);
     canvas.set_source(pat1);
     canvas.rectangle(r);
     canvas.fill();
}
class NTVSeekBar:public SeekBar{
public:
     NTVSeekBar(int w,int h):SeekBar(w,h){}
     void onDraw(GraphContext&canvas){
         RECT r=getClientRect();
         RefPtr<Gradient>pat=LinearGradient::create(0,0,0,r.height);
         pat->add_color_stop_rgba(.0,.2,.2,.2,1.);
         pat->add_color_stop_rgba(.5,.0,.0,.0,1.);
         pat->add_color_stop_rgba(1.,.2,.2,.2,1.);
         canvas.set_source(pat);//color(0xFF888888);
         canvas.rectangle(r);
         canvas.fill();
         if(getWidth()>getHeight()){
             r.width=r.width*progress_/(max_-min_);
         }else{
             int h=r.height;
             r.height=r.height*progress_/(max_-min_);
             r.offset(0,h-r.height);
         }
         RefPtr<Gradient>pat1=LinearGradient::create(r.x,r.y,r.x,r.height);//height);
         pat1->add_color_stop_rgba(.0,.2,.2,.2,1.);
         pat1->add_color_stop_rgba(.5,1.,1.,1.,1.);
         pat1->add_color_stop_rgba(1.,.2,.2,.2,1.);
         canvas.set_source(pat1);
         canvas.rectangle(r);
         canvas.fill();
     }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DrawSplitter(GraphContext&canvas,RECT&r){
     RefPtr<Gradient>pat=LinearGradient::create(r.x,r.y,r.x+r.width,r.y+r.height);
     pat->add_color_stop_rgba(.0 ,1.,0,0,.1);
     pat->add_color_stop_rgba(.15,1.,0,0,.6);
     pat->add_color_stop_rgba(1. ,1,0,0,.0);
     canvas.set_source(pat);//color(0xFF888888);

#if 0
     rect.set(r.x,r.y+r.height,r.width,1);
     canvas.rectangle(rect);
     canvas.fill();
#else
     //canvas.set_source_rgb(.6,0,0);
     canvas.move_to(r.x,r.y+r.height);
     canvas.rel_line_to(25,-6);
     canvas.rel_line_to(15,5);
     canvas.rel_line_to(r.width-20,0);
     canvas.rel_line_to(0,1);
     canvas.line_to(r.x,r.y+r.height);
     canvas.fill();
#endif
}
void SettingPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    if(state){
        canvas.set_source_rgba(0.6,0,0,.6);
    }else
        canvas.set_source(lv.getBgPattern());//Color());
    RECT rect=itm.rect;
    canvas.rectangle(rect);
    canvas.fill();
    canvas.set_color(lv.getFgColor());
    rect.inflate(-50,0);
    canvas.draw_text(rect,itm.getText(),DT_LEFT|DT_VCENTER);
    rect.inflate(50,0);
    DrawSplitter(canvas,rect);
}

static void ChannelPainterInner(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas,bool hasid){
    ChannelItem&ch=(ChannelItem&)itm;
    RECT r=itm.rect;
    r.inflate(0,-1);
    canvas.save();
    if(state){
        canvas.set_source_rgba(0.6,0,0,.6);
        canvas.rectangle(r);
        canvas.fill();
    }
    canvas.restore();
    r=itm.rect;
    canvas.set_color(lv.getFgColor());
    
    r.width=hasid?80:20;
    r.x=20;
    if(hasid){
        canvas.draw_text(r,std::to_string(itm.getId()),DT_RIGHT|DT_VCENTER);
        r.x=r.x+r.width+20;
    }
    r.width=itm.rect.width-r.x;
    
    canvas.draw_text(r,itm.getText(),DT_LEFT|DT_VCENTER);
    r=itm.rect;
    r.inflate(6,0);
    DrawSplitter(canvas,r);

    r.set(r.width-r.height,r.y,r.height,r.height);
    if(ch.camode)canvas.draw_text(r,"$",DT_LEFT|DT_VCENTER);
}

void  ChannelPainterLCN(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    ChannelPainterInner(lv,itm,state,canvas,true);
}
void  ChannelPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    ChannelPainterInner(lv,itm,state,canvas,false);
}

ToolBar*CreateNTVToolBar(int w,int h){
    ToolBar*toolbar=new ToolBar(w,h);
    RefPtr<LinearGradient>pat=LinearGradient::create(0,0,w,h);
    pat->add_color_stop_rgba(.3,.0,.0,.0,.2);
    pat->add_color_stop_rgba(.5,.6,.6,.6,1.);
    pat->add_color_stop_rgba(1.,.0,.0,.3,.2);
    toolbar->setBgPattern(pat);
    return toolbar;
}

void GetTPString(const TRANSPONDER*tp,std::string&s){
    char buf[128]={0};
    const char*polars[]={"H","V","L","R"};
    const int qam[]={0,16,32,64,128,256,512,1025};
    switch(tp->delivery_type){
    case DELIVERY_S:
    case DELIVERY_S2:
         sprintf(buf,"%d/%s/%d",tp->u.s.frequency/1000,polars[tp->u.s.polar],tp->u.s.symbol_rate);
         break;
    case DELIVERY_C:
         sprintf(buf,"%d/%d/%d",tp->u.c.frequency,qam[tp->u.c.modulation],tp->u.c.symbol_rate);
         break;
    default:break;
    }
    s=buf;
}

class VolumeWindow:public ToastWindow{
protected:
       SeekBar*p;
public:
   VolumeWindow(int w,int h):ToastWindow(w,h){
       setFlag(View::FOCUSABLE);
       p=new NTVSeekBar(w-2,h-2);
       addChildView(p)->setPos(1,1);
       p->setMin(0);p->setMax(100);
       NGLOG_DEBUG("volume=%d",nglSndGetColume(0));
       p->setProgress(nglSndGetColume(0));
       p->setOnSeekBarChangeListener([](AbsSeekBar&seek,int progress,bool fromuser){
            nglSndSetVolume(0,progress);
       });
   }
   bool onKeyUp(KeyEvent&k){
       switch(k.getKeyCode()){
       case KEY_VOLUMEDOWN:k.setKeyCode(KEY_LEFT);break;
       case KEY_VOLUMEUP  :k.setKeyCode(KEY_RIGHT);break;
       default:break;
       }
       return ToastWindow::onKeyUp(k);
   }
};

void ShowVolumeWindow(int timeout){
    ToastWindow::makeWindow([]()->ToastWindow*{
        VolumeWindow*w=new VolumeWindow(680,40);
        return w;
    },timeout)->setPos(300,600);
}

void ShowAudioSelector(int estype,int timeout){//ST_AUDIO
    ToastWindow::makeWindow([estype]()->ToastWindow*{
             ToastWindow*w=new ToastWindow(400,100);
             SERVICELOCATOR svc;
             ELEMENTSTREAM es[16];
             DtvGetCurrentService(&svc);
             int cnt=DtvGetElementsByCategory(&svc,estype,es);
             w->setFlag(View::FOCUSABLE);
             ListView*lst=new ListView(390,80);
             for(int i=0;i<cnt;i++){
                NGLOG_DEBUG("audio[%d] pid=%d type=%d lan=%s",i,es[i].pid,es[i].getType(),es[i].iso639lan);
                if(es[i].iso639lan[0])
                   lst->addItem(new ListView::ListItem((const char*)es[i].iso639lan));
             }
             w->addChildView(lst)->setPos(2,2);
             w->setPos(400,600);
             return w;
	},timeout);
}

}//namespace
