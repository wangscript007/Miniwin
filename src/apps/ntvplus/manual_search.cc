#include <windows.h>
#include <stdlib.h>
#include <ngl_log.h>
#include <json/json.h>
#include <appmenus.h>
#include <iostream> 
#include <strstream>
#include <preferences.h>

#include<ntvwindow.h>
#include<appmenus.h>

NGL_MODULE(MANUAL_SEARCH);

namespace ntvplus{

class ManualSearchWindow:public Window{
protected:
   TextView*title;
   EditText*freq;
   EditText*symbol;
   Selector*modulation;
   Selector*searchmode;
   TRANSPONDER transponder;
   UpdateTransponderListener listener;
   void*userdata;
public:
	ManualSearchWindow(int x,int y,int w,int h);
	int getManualSearchTransponders(std::vector<TRANSPONDER>&tps);
	virtual bool onKeyUp(KeyEvent&k)override;
};

#define NEWCTRL(txt,w,x) x=new NTVSelector(txt,w,40);\
        (x)->setLabelWidth(200);\
        addChildView(x);

ManualSearchWindow::ManualSearchWindow(int x,int y,int w,int h):Window(x,y,w,h){
	setLayout(new LinearLayout())->setMargins(2);
	//setVisible(FALSE);
	//listener=nullptr;
        w-=4;
	NGLOG_DEBUG(" onCreate ");
	title=new TextView("ManualSearch",w,56);
	addChildView(title)->setBgColor(0xFF222222).setFgColor(0xFFFFFFFF).setFontSize(40);
	title->setAlignment(DT_VCENTER|DT_CENTER);
	freq=new NTVEditText(w,40);
	freq->setLabelWidth(200);
	freq->setLabel("Frequency");
        freq->setHint("Enter Freqency(K)");
	freq->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
	addChildView(freq);
	freq->setFlag(View::FOCUSED);
	
	symbol=new NTVEditText(w,40);
	symbol->setLabelWidth(200);
	symbol->setLabel("SymbolRate");
        symbol->setHint("Enter SymbolRate(K)");
	symbol->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
	addChildView(symbol);

	NEWCTRL("Modulation",w,modulation);	
	modulation->addItem(new Selector::ListItem("QAM16",1));
	modulation->addItem(new Selector::ListItem("QAM32",2));
	modulation->addItem(new Selector::ListItem("QAM64",3));
	modulation->addItem(new Selector::ListItem("QAM128",4));
	modulation->addItem(new Selector::ListItem("QAM256",5));
	modulation->setIndex(2);
        NEWCTRL("SearchMode",w,searchmode);
        searchmode->addItem(new Selector::ListItem("SingleTP",1));
	searchmode->addItem(new Selector::ListItem("Network",0));
        searchmode->setIndex(0);
}

int ManualSearchWindow::getManualSearchTransponders(std::vector<TRANSPONDER>&tps){
   	TRANSPONDER transponder;
	transponder.u.c.frequency = atoi(freq->getText().c_str());
	transponder.u.c.symbol_rate = atoi(symbol->getText().c_str());
	transponder.u.c.modulation = (NGLQamMode)(modulation->getIndex()+1);
	transponder.delivery_type = DELIVERY_C;
	tps.push_back(transponder);
	//transponder=transponders;
	NGLOG_DEBUG("FRE=%d sym=%d module=%d",transponder.u.c.frequency,transponder.u.c.symbol_rate,transponder.u.c.modulation);
}

bool ManualSearchWindow::onKeyUp(KeyEvent&k){
     if(  (k.getKeyCode()==KEY_ENTER) ){
         std::vector<TRANSPONDER>tps;
         getManualSearchTransponders(tps);
         int mode=searchmode->getIndex()==1?0:tps.size();
         CreateSearchResultWindow(tps,mode);
         return true;
     }

     return Window::onKeyUp(k);
}
	
Window*CreateManualChannelSearch(){
      NGLOG_DEBUG("CreateManualChannelSearch");
	ManualSearchWindow*w=new ManualSearchWindow(400,200,480,240);
	    //if(transponder)w->setTransponder(*transponder);
	    //w->setListener(ls,userdata);
	    w->show();
		return w; 
	}

}
