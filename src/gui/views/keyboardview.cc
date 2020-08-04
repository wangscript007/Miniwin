#include<keyboardview.h>
#include<ngl_log.h>

NGL_MODULE(KEYBOARD);

namespace nglui{


KeyboardView::KeyboardView(int w,int h):View(w,h){
    key_bg_color=0xFF222222;
    kx=ky=0;
    kcol=krow=0;
    key_index=0;
    setFlag(FOCUSABLE|ENABLED);
    m_editbox=nullptr;
    kbd_lines.push_back(std::vector<int>());
}

void KeyboardView::addKey(UINT code,UINT uper,USHORT w,USHORT h,const char*txt){
    KEY k;
    k.unicode=code;
    k.upcode=uper;
    k.x=kx;
    k.y=ky;
    k.width=w;
    k.height=h;
    if(NULL!=txt)k.text=txt;
    if(KC_NONE==code){
         if(0==w){
            ky+=h+2;
            kbd_lines.push_back(std::vector<int>());
            kx=0;
         }else{
             kx+=w;
         }
    }else{   
         kbd_lines.back().push_back(keys.size()); 
         keys.push_back(k);
         kx+=w+2;
    }
}

void KeyboardView::addNewKeyLine(UINT line_height){
    kbd_lines.push_back(std::vector<int>());
    kx=0;
    if(0==line_height&&keys.size())
        line_height=keys[0].height;
    ky+=line_height;
}

void KeyboardView::setBuddy(EditText*edt){
    m_editbox=edt;
}

EditText*KeyboardView::getBuddy(){
    return m_editbox;
}

void KeyboardView::setKeyBgColor(UINT cl){
    key_bg_color=cl;
}

UINT KeyboardView::getKeyBgColor(){
    return key_bg_color;
}

void KeyboardView::setKeyIndex(UINT idx){
    if(key_index!=idx){
        key_index=idx;
        invalidate(NULL);
    }
}
void KeyboardView::onDraw(GraphContext&canvas){

    View::onDraw(canvas);
    canvas.set_color(getKeyBgColor());
    for(int i=0;i<keys.size();i++){
        KEY&k=keys[i];
        canvas.rectangle(k.x,k.y,k.width,k.height);
    }    
    canvas.fill();
    if(key_index>=0&&key_index<keys.size()){
        canvas.set_color(0xFF00FF00); 
        KEY&k=keys[key_index];
        canvas.rectangle(k.x,k.y,k.width,k.height);
        canvas.fill();
    }
    canvas.set_color(getFgColor());
    canvas.set_font_size(getFontSize());
    for(int i=0;i<keys.size();i++){
         KEY&k=keys[i];
         RECT r={k.x,k.y,k.width,k.height};
         std::string txt;
         if(k.text.empty())
             EditText::Unicode2UTF8(k.unicode,txt);
         else txt=k.text;
         canvas.draw_text(r,txt,DT_VCENTER|DT_CENTER);
    }
    canvas.fill();
}

bool KeyboardView::onKeyUp(KeyEvent&k){
    switch(k.getKeyCode()){
    case KEY_LEFT: 
             kcol=(kcol-1+kbd_lines[krow].size())%kbd_lines[krow].size();
             break;
    case KEY_RIGHT:
             kcol=(kcol+1)%kbd_lines[krow].size();
             break;
    case KEY_UP:   
             krow=(krow-1+kbd_lines.size())%kbd_lines.size();
             if(kbd_lines[krow].size()<kcol)
                 kcol=kbd_lines[krow].size()-1;
             break;
    case KEY_DOWN: 
             krow=(krow+1)%kbd_lines.size();
             if(kbd_lines[krow].size()<kcol)
                 kcol=kbd_lines[krow].size()-1;      
             break;
    case KEY_ENTER:
             if(m_editbox){
                 m_editbox->sendMessage(WM_CHAR,keys[key_index].unicode,0);
             }break; 
    default: break;
    }
    setKeyIndex(kbd_lines[krow][kcol]);
    return false;
}

}//namespace
