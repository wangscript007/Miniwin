#include<desktop.h>
#include<toolbar.h>
#include<windowmanager.h>
#include<ngl_log.h>

NGL_MODULE(DESKTOP)

namespace nglui{

Desktop::Desktop():Window(0,0,
   GraphDevice::getInstance().getScreenWidth(),
   GraphDevice::getInstance().getScreenHeight()){
   onkeypress=nullptr;
   clearFlag(BORDER);
   setBgColor(0);
   setText("Desktop");
   statusbar_=new ToolBar(GraphDevice::getInstance().getScreenWidth(),40);
   RefPtr<LinearGradient>linpat(LinearGradient::create(0, 0, 1280, 0));
   linpat->add_color_stop_rgba ( .0, 1, 1, 1, 0);
   linpat->add_color_stop_rgba ( .2, 0, 1, 0, 0.5);
   linpat->add_color_stop_rgba ( .4, 1, 1, 1, 0);
   linpat->add_color_stop_rgba ( .6, 0, 0, 1, 0.5);
   linpat->add_color_stop_rgba ( .8, 1, 1, 1, 0);

   statusbar_->setItemSize(100,0);
   statusbar_->setBgColor(0xFFAAAAAA);
   statusbar_->setBgPattern(linpat);
   statusbar_->addItem(new ToolBar::Button("Network",0));
   statusbar_->addItem(new ToolBar::Button("Settings",1));
   statusbar_->addItem(new ToolBar::Button("Menu",2));
   addChildView(statusbar_);
}

Desktop::~Desktop(){
   NGLOG_DEBUG("%p Destroied",this);
}

GraphContext*Desktop::getCanvas(){
   canvas=GraphDevice::getInstance().getPrimaryContext();
   return canvas;
}

void Desktop::setKeyListener(KeyHandler fun){
    onkeypress=fun;
}

bool Desktop::onKeyUp(KeyEvent&k){
    if(onkeypress!=nullptr)
        return  onkeypress(k.getKeyCode());
    if(k.getKeyCode()==KEY_POWER)
        exit(0);
    return false;
}

}//namespace
