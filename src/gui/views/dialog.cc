#include <dialog.h>
#include <ngl_log.h>
NGL_MODULE(Dialog);

namespace nglui{
Dialog::Dialog(){
    window=nullptr;
    messageView=nullptr;
    listview=nullptr;
    onCreateContent=nullptr;
}

void Dialog::setContentCreateListener(ContentListener ls){
    onCreateContent=ls;
}

Dialog& Dialog::setTitle(const std::string&txt){
    title=txt;
    return *this;
}

Dialog& Dialog::setMessage(const std::string&txt){
    message=txt;
    return *this;
}

Button* Dialog::getButton(int whichButton){
    return buttons[whichButton];
}

Dialog& Dialog::setButton(int whichButton,const std::string&txt,View::ClickListener onClick){
    Button*btn=buttons[whichButton];
    if(nullptr==btn){
       btn=new Button(txt);//,120,38);
       buttons[whichButton]=btn;
       btn->setId(whichButton);
    }
    NGLOG_DEBUG("btn[%d]=%p size=%dx%d",whichButton,btn,btn->getWidth(),btn->getHeight());
    btn->setText(txt);
    btn->setClickListener([onClick](View&v){
        if(onClick)onClick(v);
        v.getParent()->sendMessage(View::WM_DESTROY,0,0);
        NGLOG_DEBUG("Button[%d]%s is clicked",v.getId(),v.getText().c_str());
    });
    return *this;
}

Dialog& Dialog::setPositiveButton(const std::string&txt,View::ClickListener onClick){
    return setButton(BUTTON_POSITIVE,txt,onClick);
}

Dialog& Dialog::setNegativeButton(const std::string&txt,View::ClickListener onClick){
    return setButton(BUTTON_NEGATIVE,txt,onClick);
}

Dialog& Dialog::setNeutralButton(const std::string&txt,View::ClickListener onClick){
    return setButton(BUTTON_NEUTRAL,txt,onClick);
}

Dialog& Dialog::setSingleChoiceItems(const std::vector<std::string>&items,int checkeditem,AbsListView::ItemClickListener onItemClick){
    if(nullptr==listview)
       listview=new ListView(100,100);
    listview->clear();
    for(auto i:items)
       listview->addItem(new ListView::ListItem(i));
    listview->setIndex(checkeditem);
    return *this;
}

void Dialog::createContent(){
    if(nullptr==window){
        window=new Window(0,0,600,280);
        window->setText(title).setBgColor(0x80000000);
        window->addChildView(new TextView(title,598,40))->setPos(1,1).setBgColor(0x80222222);
        window->setVisible(0);
    }
    if(onCreateContent!=nullptr)onCreateContent(window);
    int x=10;
    int mw=0;
    int interval=window->getWidth();;
    for(auto b:buttons){
        mw=std::max(mw,b.second->getWidth());
        mw=std::max(mw,b.second->getHeight()*2);
    }
    interval=(window->getWidth()-mw*buttons.size())/(1+buttons.size());
    x=interval*2-6;

    if(!message.empty()){
        messageView=new TextView(message,window->getWidth()-10,window->getHeight()-100);
        messageView->setMultiLine(true);
        window->addChildView(messageView)->setPos(5,45);
    }  
    if(listview){
       listview->setSize(window->getWidth()-10,window->getHeight()-100);
       listview->setFlag(View::BORDER);
       window->addChildView(listview)->setPos(5,45); 
    }
    for(auto it=buttons.rbegin();it!=buttons.rend();it++){
        Button*b=it->second;
        b->setSize(mw,b->getHeight());
        window->addChildView(b)->setPos(x,window->getHeight()-b->getHeight()-6);
        x=b->getBound().right()+interval;
    } 
}

void Dialog::show(){
    if(window==nullptr)
       createContent();
    window->show();
}

}/*namspace*/

