#include<theme.h>
namespace nglui{

Theme::Theme(){
   sysColors[COLOR_BACKGROUND]  =0x00000000;
   sysColors[COLOR_SCROLLBAR]   =0x88AAAAAA;
   sysColors[COLOR_WINDOW]      =0x88888888;
   sysColors[COLOR_WINDOW_FRAME]=0x88000000;
   sysColors[COLOR_WINDOW_TEXT] =0xFF000000;
   sysColors[COLOR_HIGHLIGHT]   =0x880000FF;
   sysColors[COLOR_HIGHLIGHTTEXT]=0xFFFFFFFF;
   sysColors[COLOR_BTNFACE]     =0x88AAAAAA;
   sysColors[COLOR_BTNTEXT]     =0xFF000000;
   sysColors[COLOR_BTNSHADOW]   =0x88000000;
   sysColors[COLOR_INFOBK]      =0x88888888;
   sysColors[COLOR_INFOTEXT]    =0xAA000000;
}

UINT Theme::getSysColor(int idx){
   return sysColors[idx];
}

void Theme::setSysColor(int idx,UINT color){
    sysColors[idx]=color;
}

void Theme::setSysColors(int size,INT*eles,UINT *colors){
    for(int i=0;i<size;i++){
        if(eles[i]<0||eles[i]>=COLOR_MAX)continue;
        sysColors[eles[i]]=colors[i];
    } 
}

}//namespace
