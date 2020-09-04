#include <gtest/gtest.h>
#include <ngl_types.h>
#include <ngl_graph.h>
#include <graph_context.h>
#include <ngl_os.h>
#include <ngl_timer.h>
#include <sys/time.h>
#include <assets.h>
#include <ngl_misc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <svg-cairo.h>
#include <fstream>

using namespace nglui;

class CONTEXT:public testing::Test{
protected:
public :
   static nglui::Assets *rm;
   static void SetUpTestCase(){
       nglSysInit();
       nglGraphInit();
       rm=new nglui::Assets("ntvplus.pak");
   }
   static void TearDownCase(){
       delete rm; 
   }
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};
nglui::Assets *CONTEXT::rm=nullptr;

TEST_F(CONTEXT,SURFACE_CREATE_1){
   RefPtr<GraphContext>ctx1(GraphDevice::getInstance().createContext(800,600));
   const char piaoyao[]={(char)0xe9,(char)0xa3,(char)0x98,(char)0xe6,(char)0x91,(char)0x87,(char)0};
   ctx1->set_color(0,0,255);
   ctx1->rectangle(0,0,800,600);
   ctx1->fill();
   for(int j=0;j<10;j++){
      for(int i=0;i<10;i++){
         const char *txt[]={"Beijing","Sigapo","ShangHai","Shenzhen",
            "The quick brown fox jumps over a lazy dog",
            "Innovation in China","中国智造，惠及全球 0123456789"};
         ctx1->set_font_size(i==j?40:28);
         ctx1->save();
         ctx1->select_font_face("Khek Sangker",ToyFontFace::Slant::ITALIC,ToyFontFace::Weight::BOLD);
         if(i==j){
             RECT rc={400,i*40,400,40};
             ctx1->set_color(255,0,0);
             ctx1->rectangle(0,i*40,800,40);
             ctx1->fill();
             ctx1->set_color(255,255,255);
             ctx1->draw_text(10,i*40,txt[i%(sizeof(txt)/sizeof(char*))]);
         }else{
             ctx1->rectangle(0,i*40,800,40);
             ctx1->fill();
             ctx1->set_color(255,255,255);
             ctx1->draw_text(10,i*40,txt[i%(sizeof(txt)/sizeof(char*))]);
         }
      RECT rect={0,0,800,600};
      ctx1->set_font_size(40);
      ctx1->draw_text(rect,piaoyao,DT_BOTTOM|DT_LEFT);
      ctx1->invalidate(rect);
         ctx1->restore();
      }
      GraphDevice::getInstance().ComposeSurfaces();
      nglSleep(500);
   }
   ctx1->dump2png("test3.png");
   nglSleep(2000);
}

TEST_F(CONTEXT,TextOutXY){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1080,720));
    const char*str="jump over a dog";
    char s2[2],*p=(char*)str;
    RECT rect={100,100,800,120};
    ctx->set_font_size(80);
    ctx->set_color(0xFFFFFFFF);
    ctx->rectangle(100,100,800,1);
    ctx->fill();
    for(int x=100;*p;p++,x+=55){
       s2[0]=*p;s2[1]=0;
       ctx->draw_text(x,100,s2);
    }
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(10000);
}

TEST_F(CONTEXT,drawtext){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1080,720));
    const char*str[]={"jump","over","a", "dog",nullptr};
    const char*str1="jump over a dog";
    int fmt[]={DT_TOP,DT_VCENTER,DT_BOTTOM};
    int fmth[]={DT_LEFT,DT_CENTER,DT_RIGHT};
    char s2[2],*p=(char*)str;
    RECT rect={100,100,800,120};
    ctx->set_font_size(80);
    for(int i=0;i<3;i++){
      rect.x=100;
      rect.y=100+150*i;
      ctx->set_color(0xFFFFFFFF);
      ctx->rectangle(rect);
      ctx->stroke();
      for(int j=0;j<3;j++){ 
         rect.x=100;
         ctx->set_color(0xFF000000|(255<<j*8));
         if(i)printf("%d,%d\r\n",rect.x,rect.y);
         if(i)ctx->draw_text(rect,str1,fmt[j]|fmth[i]);
         for(int k=0;i==0&&str[k];k++){
            ctx->draw_text(rect,str[k],fmt[j]|fmth[i]);
            rect.x+=strlen(str[k])*52;
         }
      }
    }
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(10000);
}

TEST_F(CONTEXT,drawarraw){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1080,720));
    RECT rect={100,100,400,40};
    RECT rect2={550,100,40,400};
    ctx->set_source_rgb(1,1,1);
    ctx->draw_arraw(rect,2);
    ctx->draw_arraw(rect,3);
    ctx->draw_arraw(rect2,0);
    ctx->draw_arraw(rect2,1);
    ctx->invalidate(rect);
    ctx->invalidate(rect2);
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(10000);
}

TEST_F(CONTEXT,memleak1){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1080,720));
    RECT rect={0,0,1000,80};
    nglSleep(10000);
    for(int i=0;i<1000000;i++){
       ctx->draw_text(rect,"The quick brown fox jumps over a lazy dog",DT_CENTER|DT_VCENTER);
    }
}

TEST_F(CONTEXT,TEXT_ALIGNMENT){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1080,720));
    const char*horz[]={"LEFT","CENTER","RIGHT"};
    const char*vert[]={"TOP","VCENTER","BOTTOM"};
    char alignment[64];
    RECT rect={100,100,800,120};
    ctx->set_font_size(40);
    for(int h=0;h<=2;h++){
        for(int v=0;v<=2;v++){
           ctx->set_color(0xFFFFFFFF);
           ctx->rectangle(0,0,1080,720);
           ctx->fill();
           sprintf(alignment,"%s_%s",horz[h],vert[v]);
           printf("test %s\r\n",alignment);
           ctx->set_color(0xFFFF0000);
           ctx->draw_text(20,20,alignment);
           ctx->rectangle(rect);ctx->fill();
           ctx->set_color(0xFF00FF00);
           ctx->draw_text(rect,"The quick brown fox jump sover the lazy dog.",(v<<4)|h);
           ctx->invalidate(rect);
           strcat(alignment,".png");
           ctx->dump2png(alignment);
           GraphDevice::getInstance().ComposeSurfaces();
           nglSleep(1000);
        }nglSleep(1000); 
    }
    nglSleep(2000);
}


TEST_F(CONTEXT,subcanvas){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    RefPtr<GraphContext>ctx1(ctx->subContext(100,100,300,200));
    RefPtr<GraphContext>ctx2(ctx1->subContext(100,100,100,100));
    RefPtr<GraphContext>ctx3(ctx2->subContext(50,50,50,50));
    RECT rect={0,0,800,600};
    ctx->set_color(0xFFFFFFFF);
    ctx->rectangle(0,0,800,600);ctx->fill();
    ctx1->set_color(0xFF00FF00);
    ctx1->rectangle(0,0,300,200);ctx1->fill();
    ctx2->set_color(0xFF0000FF);
    ctx2->rectangle(0,0,100,100);ctx2->fill();
    ctx3->set_color(0xFFFF0000);
    ctx3->rectangle(0,0,50,50);ctx3->fill();
    ctx->invalidate(rect);
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(2500);
}

TEST_F(CONTEXT,Translate){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    RECT rect={0,0,800,600};
    ctx->set_color(0xFFFFFFFF);
    ctx->rectangle(0,0,800,600);ctx->fill();
    ctx->set_color(0xFFFF0000);
    ctx->rectangle(0,0,200,200);ctx->fill();

    ctx->translate(100,100);
    ctx->set_color(0xFF00FF00);
    ctx->rectangle(0,0,100,100);ctx->fill();

    ctx->translate(-50,-50);
    ctx->set_color(0xFF0000FF);
    ctx->rectangle(0,0,100,100);ctx->fill();
    
    ctx->translate(-50,-50);
    ctx->set_color(0xFFFFFF00);
    ctx->rectangle(0,0,100,100);
    ctx->stroke();

    ctx->invalidate(rect);
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(2500);
}

TEST_F(CONTEXT,Clip){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    RefPtr<ImageSurface>img=ImageSurface::create_from_png("light.png");
    RECT rect={0,0,800,600};
    ctx->set_color(0xFFFFFFFF);
    ctx->rectangle(0,0,800,600);ctx->fill();
    ctx->arc(400,300,100,0,M_PI*2);
    ctx->clip();
    RECT rect1={250,150,200,200};
    ctx->draw_image(img,&rect,NULL,ST_CENTER_INSIDE);
    ctx->invalidate(rect1);
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(1000);
}

TEST_F(CONTEXT,Mask){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    RefPtr<ImageSurface>img=ImageSurface::create_from_png("light.png");
    RECT rect={0,0,800,600};
    ctx->set_color(0xFFFFFFFF);
    ctx->rectangle(0,0,800,600);ctx->fill();
    
    ctx->invalidate(rect);
    GraphDevice::getInstance().ComposeSurfaces();
    nglSleep(1000);
}

TEST_F(CONTEXT,Pattern_Line){
   RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
   int i, j; 
   RefPtr<RadialGradient>radpat(RadialGradient::create(200, 150, 80, 400, 300, 400));
   RefPtr<LinearGradient>linpat(LinearGradient::create(200, 210, 600, 390));
   RECT rect={0,0,800,600};
   ctx->set_color(0xffffffff);
   ctx->rectangle(0,0,800,600);ctx->fill();
   radpat->add_color_stop_rgb ( 0, 1.0, 0.8, 0.8); 
   radpat->add_color_stop_rgb ( 1, 0.9, 0.0, 0.0); 
   for (i=1; i<10; i++) 
        for (j=1; j<10; j++) 
            ctx->rectangle ( i*50, j*50,48 ,48); 
   ctx->set_source ( radpat); 
   ctx->fill (); 
   linpat->add_color_stop_rgba ( .0, 1, 1, 1, 0); 
   linpat->add_color_stop_rgba ( .2, 0, 1, 0, 0.5); 
   linpat->add_color_stop_rgba ( .4, 1, 1, 1, 0); 
   linpat->add_color_stop_rgba ( .6, 0, 0, 1, 0.5); 
   linpat->add_color_stop_rgba ( .8, 1, 1, 1, 0); 
   ctx->rectangle ( 0, 0, 800, 600); 
   ctx->set_source(linpat); 
   ctx->fill ();
   ctx->invalidate(rect);
   GraphDevice::getInstance().ComposeSurfaces();
   ctx->dump2png("pat-line.png");
   nglSleep(200); 
}
TEST_F(CONTEXT,Pattern_Radio){
   RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1200,600));
   ctx->set_color(0,0,0);
   ctx->rectangle(0,0,1200,600);ctx->fill();
   RefPtr<RadialGradient>radpat(RadialGradient::create(200, 200, 10, 200, 200, 150));
   RECT rect={0,0,1200,600};
   radpat->add_color_stop_rgb ( .0, 1., 1., 1.);
   radpat->add_color_stop_rgb ( 1., 1., .0,.0);
   ctx->set_source ( radpat);
   ctx->arc(200,200,150,.0,3.1415*2);
   ctx->fill ();
   
   ctx->invalidate(rect);
   GraphDevice::getInstance().ComposeSurfaces();
   ctx->dump2png("pat-radio.png");
   nglSleep(200);
}

TEST_F(CONTEXT,Font){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1280,720));
    const char *texts[]={
        "The quick brown fox jumps over a lazy dog",
        "Innovation in China 0123456789" 
    };
    ctx->set_color(0xffffffff);
    ctx->rectangle(0,0,1280,720);ctx->fill();
    RECT rect={0,0,1280,720};
    auto lg=LinearGradient::create(10,480,1100,720);
    lg->add_color_stop_rgba(.0,1.0,0,0,0.5);
    lg->add_color_stop_rgba(.5,.0,1.,.0,1.);
    lg->add_color_stop_rgba(1.,.0,0,.1,0.5);
    for(int i=0,y=10;i<10;i++){
       std::string txt=texts[i%(sizeof(texts)/sizeof(char*))];
       int font_size=10+i*5;
       //cant use this select_font_face use defaut font pls.
       //ctx->select_font_face(faces[i],FONT_SLANT_NORMAL, FONT_WEIGHT_BOLD);
       ctx->set_font_size(font_size);
       ctx->move_to(0,y);y+=font_size+10;
       if(i%4==0)ctx->set_color(0,0,0);
       else ctx->set_source(lg);
       if(i%2==0){
           ctx->show_text(txt);
       }else{
           ctx->text_path(txt);ctx->stroke();
       }
       GraphDevice::getInstance().ComposeSurfaces();
        nglSleep(200);
    }
    
    ctx->set_font_size(150);
    for(int i=0;i<20;i++){
       auto lg=LinearGradient::create(20*i,480,300+50*i,720);
       lg->add_color_stop_rgba(.0,1.0,0,0,0.5);
       lg->add_color_stop_rgba(.5,.0,1.,.0,1.);
       lg->add_color_stop_rgba(1.,.0,0,.1,0.5);
       ctx->set_source(lg);
       ctx->move_to(0,600);
       ctx->show_text("Innovation in China!");
       ctx->invalidate(rect);
       GraphDevice::getInstance().ComposeSurfaces();
       nglSleep(100);
    }
    nglSleep(1000);
}
TEST_F(CONTEXT,ALPHA){
     RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1280,720));
     ctx->set_color(0xFFFFFFFF);
     ctx->rectangle(0,0,1280,720);ctx->fill();
     ctx->set_color(0x80FF0000);
     ctx->rectangle(200,200,480,320);ctx->fill();
     ctx->dump2png("alpha.png");
     GraphDevice::getInstance().ComposeSurfaces();
}
TEST_F(CONTEXT,HOLE){
     RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(1280,720));
     ctx->set_color(0xFFFFFFFF);
     ctx->rectangle(0,0,1280,720);ctx->fill();
     ctx->set_color(0);
     ctx->rectangle(200,200,480,320);ctx->fill();
     ctx->dump2png("hole.png");
     GraphDevice::getInstance().ComposeSurfaces();
}
TEST_F(CONTEXT,HOLE2){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().getPrimaryContext());
     ctx->set_color(0xFFFFFFFF);
     ctx->rectangle(0,0,1280,720);ctx->fill();
     ctx->set_source_rgba(0,0,0,0);
     ctx->rectangle(200,200,480,320);ctx->fill();
     GraphDevice::getInstance().ComposeSurfaces();
     ctx->dump2png("hole2.png");
}
TEST_F(CONTEXT,HOLE3){
     RefPtr<ImageSurface>img=ImageSurface::create(Surface::Format::ARGB32,1280,720);
     RefPtr<Context>ctx=Context::create(img);
     ctx->set_source_rgb(1,0.5,1);
     ctx->rectangle(0,0,1280,720);
     ctx->fill();
     ctx->set_source_rgba(0,1,0,0.1);
     ctx->rectangle(200,200,480,320);
     ctx->fill();
     GraphDevice::getInstance().ComposeSurfaces();
     img->write_to_png("hole3.png");
}
