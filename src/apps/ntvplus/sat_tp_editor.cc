#include<ntvwindow.h>
#include<appmenus.h>

namespace ntvplus{

class SatEditorWindow:public Window{
protected:
   TextView*title;
   EditText*sat;
   EditText*pos;
   Selector*dir;
   SATELLITE satellite;
   UpdateSatelliteListener listener;
   void*userdata;
public:
   SatEditorWindow(int x,int y,int w,int h);
   void setSatellite(const SATELLITE&s);
   void setListener(UpdateSatelliteListener ls,void*data){
        listener=ls;
        userdata=data;
   }
};

class TPEditorWindow:public Window{
protected:
   TextView*title;
   EditText*freq;
   EditText*symbol;
   Selector*polar;
   Selector*broadcast;
   Selector*modulation;
   Selector*fec;
   TRANSPONDER transponder;
   UpdateTransponderListener listener;
   void*userdata;
public:
   TPEditorWindow(int x,int y,int w,int h);
   void setTransponder(const TRANSPONDER&tp);
   void setListener(UpdateTransponderListener ls,void*data){
        listener=ls;
        userdata=data;
   }
};

#define NEWCTRL(txt,w,x) x=new NTVSelector(txt,w-10,40);\
        (x)->setLabelWidth(200);\
        addChildView(x);

SatEditorWindow::SatEditorWindow(int x,int y,int w,int h):Window(x,y,w,h){
    setLayout(new LinearLayout());
    title=new TextView("Satellite",w-10,56);
    addChildView(title)->setBgColor(0xFF222222).setFgColor(0xFFFFFFFF).setFontSize(40);
    
    sat=new NTVEditText(w-10,40);
    sat->setLabelWidth(200);
    sat->setLabel("Name");
    sat->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    addChildView(sat);

    pos=new NTVEditText(w-10,40);
    pos->setLabelWidth(200);
    pos->setLabel("Position");
    pos->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    addChildView(pos);
    NEWCTRL("Direction",w,dir);
    dir->addItem(new Selector::ListItem("East",0));
    dir->addItem(new Selector::ListItem("West",1));
    listener=nullptr;
}

void SatEditorWindow::setSatellite(const SATELLITE&s){
    satellite=s;

    sat->setText(s.name);
    pos->setText(std::to_string(s.position));
    dir->setIndex(s.direction);
}

TPEditorWindow::TPEditorWindow(int x,int y,int w,int h):Window(x,y,w,h){
    setLayout(new LinearLayout());
    listener=nullptr;
    title=new TextView("Transponder",w-10,56);
    addChildView(title)->setBgColor(0xFF222222).setFgColor(0xFFFFFFFF).setFontSize(40);
    freq=new NTVEditText(w-10,40);
    freq->setLabelWidth(200);
    freq->setLabel("Frequency");
    freq->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    addChildView(freq);
    
    symbol=new NTVEditText(w-10,40);
    symbol->setLabelWidth(200);
    symbol->setLabel("SymbolRate");
    symbol->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    addChildView(symbol);

    NEWCTRL("Polarsation",w,polar);
    NEWCTRL("Broadcast",w,broadcast);
    NEWCTRL("Modulation",w,modulation);
    NEWCTRL("FEC",w,fec);

    //"horizontal",0},{"vertical",1},{"left",2},{"right",3
    polar->addItem(new Selector::ListItem("Horizontal",0));
    polar->addItem(new Selector::ListItem("Vertical",1));
    polar->addItem(new Selector::ListItem("Left Circular",2));
    polar->addItem(new Selector::ListItem("Right Circular",3));
   
    broadcast->addItem(new Selector::ListItem("DVB-S",0));
    broadcast->addItem(new Selector::ListItem("DVB-S2",1));
    
    modulation->addItem(new Selector::ListItem("QPSK",0));
    modulation->addItem(new Selector::ListItem("8PSK",1));

    fec->addItem(new Selector::ListItem("Auto",0));
    fec->addItem(new Selector::ListItem("1/2",1));
    fec->addItem(new Selector::ListItem("2/3",2));
    fec->addItem(new Selector::ListItem("3/4",3));
    fec->addItem(new Selector::ListItem("5/6",4));
    fec->addItem(new Selector::ListItem("7/8",5));
    fec->addItem(new Selector::ListItem("8/9",6));
    fec->addItem(new Selector::ListItem("3/5",7));
    fec->addItem(new Selector::ListItem("4/5",8));
    fec->addItem(new Selector::ListItem("9/10",9));
}

void TPEditorWindow::setTransponder(const TRANSPONDER&tp){
    transponder=tp;

    freq->setText(std::to_string(tp.u.s.frequency));
    symbol->setText(std::to_string(tp.u.s.symbol_rate));
    polar->setIndex(tp.u.s.polar);
    broadcast->setIndex(tp.delivery_type==DELIVERY_S?0:1);
    modulation->setIndex(tp.u.s.modulation);
    fec->setIndex(tp.u.s.fec);
}

Window*CreateSatEditorWindow(const SATELLITE* sat,const UpdateSatelliteListener ls,void*userdata){
    SatEditorWindow*w=new SatEditorWindow(400,200,480,206);
    if(sat)w->setSatellite(*sat);
    w->setListener(ls,userdata);
    w->show();
    return w;
}

Window*CreateTPEditorWindow(const TRANSPONDER*tp,const UpdateTransponderListener ls,void*userdata){
    TPEditorWindow*w=new TPEditorWindow(400,200,480,336);
    if(tp)w->setTransponder(*tp);
    w->setListener(ls,userdata);
    w->show();
    return w;
}

}//namespace
