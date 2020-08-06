#include <windows.h>
#include <appmenus.h>
#include <ngl_log.h>
#include <ngl_timer.h>
#include <app.h>
#include <imageframeview.h>
#include <clockview.h>
#include <fstream>

#ifdef HAVE_IRDETOCCA
#include <irdccaui.h>
using namespace cca;
#endif

NGL_MODULE(MAINMENU)

namespace ntvplus{

static void onButtonClick(View&v){
    NGLOG_DEBUG("onButtonClick %d",v.getId());
    switch(v.getId()){
    case 0:CreateSettingWindow();break;
    case 1:CreateTVGuide();break;
    case 2:CreateChannelList();break;
    case 3:CreateMultiMedia()/*CreateMediaWindow()*/;break;
#ifdef HAVE_IRDETOCCA
    case 4:CreateIrdCcaMenuWindow();break;
#endif
    default:break;
    }
}
class IconButton:public ImageButton{
protected:
    int rad,dir;
    int count;
public:
    IconButton(const std::string&res,int w,int h):ImageButton(res,w,h){
        rad=w/4;count=.0;dir=1; 
        setBgColor(0); 
    }
    bool onMessage(DWORD msg,DWORD wp,ULONG lp)override{
        if(msg==WM_TIMER && hasFlag(FOCUSED)){
            rad=(float)getWidth()/3;
            rad+=rad*count*0.1;
            if(dir>0&&count>=8)dir*=-1;
            if(dir<0&&count==0)dir*=-1;
            count+=dir;
            NGLOG_VERBOSE("dir=%d rad=%d count=%d",dir,rad,count);
            sendMessage(msg,wp,lp,100);invalidate(nullptr);
            return true;
        }return ImageButton::onMessage(msg,wp,lp);
    }
    void onFocusChanged(bool focus)override{
        RECT r=getBound();
        int delta=focus?30:-30;
        r.inflate(delta,delta);
        setBound(r);
        if(focus)sendMessage(View::WM_TIMER,0,0);
    }
    void onDraw(GraphContext&canvas){
        int cx=getWidth()/2;
        int cy=getHeight()/2;
        View::onDraw(canvas);
        RefPtr<RadialGradient>rp=RadialGradient::create(cx,cy,rad/8,cx,cy,rad);
       
        rp->add_color_stop_rgba(.0,1,0,0,1);
        rp->add_color_stop_rgba(1.,.0,0,0,.0);

        if(hasFlag(FOCUSED)){
           canvas.save();
           canvas.set_source(rp);
           canvas.arc(cx,cy,rad,0,M_PI*2);//
           canvas.fill();
           canvas.restore();
        }
        RECT rect=getClientRect();
        canvas.draw_image(img_,&rect,nullptr,ST_FIT_START);
        canvas.set_source_rgb(1,1,1);
        canvas.set_font_size(getFontSize());
        rect=getClientRect();
        std::string txt=App::getInstance().getString(mText);
        canvas.draw_text(rect,txt,DT_CENTER|DT_BOTTOM);
    }
};


Window*CreateMainMenu(){
   const char* btnname[]={"Setting","SatTV","Channels","multimedia","game"};
   const char*imgname[]={
          "im_setting.png","im_setting_select.png",
          "im_sat.png","im_sat_select.png",
          "im_channel.png","im_channel_select.png",
          "im_multimedia.png","im_multimedia_select.png",
          "im_game.png","im_game_select.png"
   };
   NTVWindow*w=new NTVWindow(0,0,1280,720);
   //RefPtr<SurfacePattern>pat=SurfacePattern::create(App::getInstance().loadImage("im_background.png"));
   //w->setBgPattern(pat);
   w->clearFlag(ATTR_ANIMATE_FOCUS);/*enable focus animation*/
   w->initContent(NWS_TITLE/*|NWS_TOOLTIPS*/);
   w->setText("mainmenu");

   for(int i=0;i<sizeof(btnname)/sizeof(char*);i++){
       ImageButton *btn=new IconButton(btnname[i],160,160);
       //if(i==0)btn->setFlag(View::FOCUSED);
       btn->setPos(70+i*230,550);
       btn->setFgColor(0xFFFFFFFF).setBgColor(0).setFontSize(30);
       btn->setImage(imgname[2*i]);
       //btn->setHotImage(imgname[2*i+1]);
       btn->setId(i);
       btn->setClickListener(onButtonClick);
       w->addChildView(btn);
   }
   NGLOG_DEBUG("show mainmenu  %p cplusplus=%x/%d",w,__cplusplus,__cplusplus);
   w->show();
   return w;
}


}//namespace
