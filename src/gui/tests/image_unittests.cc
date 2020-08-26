#include <gtest/gtest.h>
#include <ngl_types.h>
#include <ngl_graph.h>
#include <graph_context.h>
#include <ngl_os.h>
#include <ngl_timer.h>
#include <sys/time.h>
#include <assets.h>
#include <ngl_misc.h>
#include <ngl_input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <svg-cairo.h>
#include <fstream>
#include <curl/curl.h>
#include <unistd.h>
#include <uriparser/Uri.h>

using namespace nglui;

class IMAGE:public testing::Test{
protected:
   unsigned long long ts;
   std::vector<std::string>images;
public :
   static nglui::Assets *rm;
   static void SetUpTestCase(){
       nglSysInit();
       nglGraphInit();
       nglInputInit();
       rm=new nglui::Assets("ntvplus.pak");
   }
   static void TearDownCase(){
       delete rm; 
   }
   virtual void SetUp(){
       UriParserStateA state;
       UriUriA baseuri,uri,absuri;
       char string[128];
       int res,len;

       memset(&baseuri,0,sizeof(UriUriA));
       memset(&uri,0,sizeof(UriUriA));
       memset(&absuri,0,sizeof(UriUriA));

       state.uri=&baseuri;
       uriParseUriA(&state,"file:///home/zhhou");
       uriToStringA(string,&baseuri,sizeof(string),&len);
       printf("baseuri=%s len=%d \r\n",string,len);

       state.uri=&uri;
       res=uriParseUriA(&state,"#innerref");
       uriToStringA(string,&uri,sizeof(string),&len);
       printf("renativeuri(%s)=%d  len=%d\r\n",string,res,len);

       uriAddBaseUriA(&absuri,&baseuri,&uri);
       uriToStringA(string,&absuri,sizeof(string),&len);
       printf("absuri=%s len=%d\r\n",string,len);
       images.clear();
   }
   virtual void TearDown(){
   }
   void tmstart(){
      ts=gettime();
   }
   void tmend(const char*txt){
      printf("%s:used time %lldms\r\n",txt,gettime()-ts);
   }
   unsigned long long gettime(){
       struct timeval tv;
       gettimeofday(&tv,NULL);
       return tv.tv_sec*1000+tv.tv_usec/1000;
   }
   void loadImages(const std::string&path,const std::string&filter){
       struct stat st;
       if(stat(path.c_str(),&st)==-1)
           return ;
       if(S_ISREG(st.st_mode)){
           size_t pt=path.rfind('.');
           if(pt!= std::string::npos){
              std::string ext=path.substr(pt+1);
              if(filter.compare(ext))images.push_back(path);
           }
           return ;
       }
       DIR *dir=opendir(path.c_str());
       struct dirent *entry;
       while(dir&&(entry=readdir(dir))){
           std::string s=path;
           if(entry->d_name[0]=='.')
              continue;
           s.append("/");
           s.append(entry->d_name);
           loadImages(s,filter);
       }
       if(dir)closedir(dir);
   }
};
nglui::Assets *IMAGE::rm=nullptr;

TEST_F(IMAGE,Bitmap){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    loadImages("./","bmp");
    for(int i=0;i<images.size();i++){
       std::ifstream fs(images[i].c_str());
       RefPtr<ImageSurface>img=Assets::loadImage(fs);
       RECT rect={0,0,800,600};
       for(int i=0;i<10;i++){
          ctx->set_color(0xFF000000|(i*20<<16));
          ctx->rectangle(rect);
          ctx->fill();
          ctx->draw_image(img,10,10);//,&rect,NULL,ST_FITXY);//ST_CENTER_INSIDE);
          ctx->invalidate(rect);
          GraphDevice::getInstance().ComposeSurfaces();
          nglSleep(500);
       }
    }
    nglSleep(2000);
}
TEST_F(IMAGE,Image_PNG){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    loadImages("./","png");
    for(int i=0;i<images.size();i++){
        tmstart();
        std::ifstream fs(images[i].c_str());
        RefPtr<ImageSurface>img=Assets::loadImage(fs);
        tmend("decodepng");
        RECT rect={0,0,800,600};
        ctx->rectangle(rect);
        ctx->fill();
        tmstart();
        ctx->draw_image(img,&rect,NULL,ST_CENTER_INSIDE);
        tmend("drawimage");
        ctx->invalidate(rect);
    }
    //ctx->dump2png("test2.png");
    nglSleep(2500);
}

TEST_F(IMAGE,Image_JPG){
    RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    loadImages("./","jpg");
    for(int i=0;i<images.size();i++){
        tmstart();
       std::ifstream fs(images[i].c_str());
        RefPtr<ImageSurface>imgj=Assets::loadImage(fs);
        tmend("decodejpg");
        RECT rect={0,0,800,600};
        ctx->rectangle(rect);ctx->fill();
        tmstart();
        ctx->draw_image(imgj,&rect,NULL,ST_CENTER_INSIDE);
        tmend("drawimage");
        ctx->invalidate(rect);
        GraphDevice::getInstance().ComposeSurfaces();
        nglSleep(1000);
    }
    nglSleep(2500);
}

#ifdef ENABLE_CAIROSVG
TEST_F(IMAGE,SVG){
     svg_cairo_t *svg;
     CURL *hcurl;
     RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
     const static char*svgs[]={
        "https://www.w3school.com.cn/svg/rect1.svg",
        "https://www.w3school.com.cn/svg/rect2.svg",
        "https://www.w3school.com.cn/svg/rect3.svg",
        "https://www.w3school.com.cn/svg/rect4.svg",
        "https://www.w3school.com.cn/svg/circle1.svg",
        "https://www.w3school.com.cn/svg/ellipse1.svg",
        "https://www.w3school.com.cn/svg/ellipse2.svg",
        "https://www.w3school.com.cn/svg/ellipse3.svg",
        "https://www.w3school.com.cn/svg/line1.svg",
        "https://www.w3school.com.cn/svg/polygon1.svg",
        "https://www.w3school.com.cn/svg/polygon2.svg",
        "https://www.w3school.com.cn/svg/polyline1.svg",
        "https://www.w3school.com.cn/svg/path1.svg",
        "https://www.w3school.com.cn/svg/path2.svg",
        "https://www.w3school.com.cn/svg/filter1.svg",
        "https://www.w3school.com.cn/svg/filter2.svg",
        "https://www.w3school.com.cn/svg/feblend_1.svg",
        "https://www.w3school.com.cn/svg/fecolormatrix_1.svg",
        "https://www.w3school.com.cn/svg/fecomponenttransfer_1.svg",
        "https://www.w3school.com.cn/svg/feoffset_1.svg",
        "https://www.w3school.com.cn/svg/femerge_1.svg",
        "https://www.w3school.com.cn/svg/femorphology_1.svg",
        "https://www.w3school.com.cn/svg/filter_1.svg",
        "https://www.w3school.com.cn/svg/filter_2.svg",
        "https://www.w3school.com.cn/svg/filter_3.svg",
        "https://www.w3school.com.cn/svg/filter_4.svg",
        "https://www.w3school.com.cn/svg/filter_5.svg",
        "https://www.w3school.com.cn/svg/filter_6.svg",
        "https://www.w3school.com.cn/svg/linear1.svg",
        "https://www.w3school.com.cn/svg/linear3.svg",
        "https://www.w3school.com.cn/svg/radial1.svg",
        "https://www.w3school.com.cn/svg/radial2.svg",
        "https://www.w3school.com.cn/svg/a_1.svg",
        "https://www.w3school.com.cn/svg/a_2.svg",
        "https://www.w3school.com.cn/svg/animate_1.svg",
        "https://www.w3school.com.cn/svg/animate_2.svg",
        "https://www.w3school.com.cn/svg/animatecolor_1.svg",
        "https://www.w3school.com.cn/svg/animatemotion_0.svg",
        "https://www.w3school.com.cn/svg/animatemotion_1.svg",
        "https://www.w3school.com.cn/svg/animatemotion_2.svg",
        "https://www.levien.com/svg/tiger.svg"
     };
     unsigned int width,height;
     RECT rect={0,0,800,600};
     system("mkdir -p svgs pngs");
     for(int i=0;i<sizeof(svgs)/sizeof(svgs[0]);i++){
         svg_cairo_create(&svg);
         const char*file=strrchr(svgs[i],'/');
         char command[256],url[128],fpng[128];
         sprintf(command,"curl %s -s -o svgs/%s",svgs[i],file+1);
         int rc=system(command);
         ctx->set_source_rgb(1,1,1);
         ctx->rectangle(0,0,800,600);
         ctx->fill();
#if 1 
         sprintf(url,"file://./svgs/%s",file+1);
         svg_cairo_parse(svg,url);
      
         svg_cairo_get_size(svg,&width,&height);
         printf("%s  imagesize=%dx%d\r\n",svgs[i],width,height);
         if(width*height==0)printf("command:%s =%d\r\n",command ,rc);
         svg_cairo_render(svg,(cairo_t*)ctx->cobj());
         svg_cairo_destroy(svg);
         ctx->move_to(100,540);
         ctx->set_source_rgb(0,.5,1);
         ctx->set_font_size(24);
         ctx->show_text(svgs[i]);
         ctx->invalidate(rect);
#else
         sprintf(url,"./svgs/%s",file+1);
         std::ifstream fs(url);
         RefPtr<ImageSurface>img=Assets::loadImage(fs);
         RECT rect={0,0,800,600};
         ctx->draw_image(img,&rect,NULL,ST_CENTER_INSIDE);
         printf("url %s img=%p\r\n",url,img);
         printf("image %s size=%dx%d\r\n",svgs[i],img->get_width(),img->get_height());
#endif
         strcpy(fpng,file+1);
         sprintf(fpng,"pngs/%s",file+1);
         strcpy((char*)strrchr(fpng,'.'),".png");
         ctx->invalidate(rect);
         GraphDevice::getInstance().ComposeSurfaces();
         
         ctx->dump2png(fpng);
         nglSleep(500);
     }
     nglSleep(3000);
}
#endif

TEST_F(IMAGE,Resource_Image){
   RefPtr<GraphContext>ctx(GraphDevice::getInstance().createContext(800,600));
    tmstart();
    RefPtr<ImageSurface>img=rm->loadImage("hs-400.png");//"hd_mainmenu.bmp");
    tmend("decodejpg");
    RECT rect={0,0,800,600};
    ctx->rectangle(rect);ctx->fill();
    tmstart();
    ctx->draw_image(img,&rect,NULL,ST_CENTER_INSIDE);
    tmend("drawimage");
    ctx->invalidate(rect);
    nglSleep(2500);
}

