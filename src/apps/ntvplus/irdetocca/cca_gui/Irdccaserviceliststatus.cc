#include <windows.h>
#include <ntvwindow.h>
#include <ntvwindow.h>
#include <ngl_log.h>
#include <irdccaui.h>
#include <MApp_ZUI_ACTCCACasCmd.h>

NGL_MODULE(IRDCCASERVICELISTSTATUS);
using namespace ntvplus;


namespace cca{
	
extern INT8  gau8Services[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];

class IrdCcaServiceListStatus:public Window{
protected:
	TextView *sltitle=nullptr;
	ListView *sllv=nullptr;//servicelist listview
public:
	IrdCcaServiceListStatus(int x,int y,int w,int h);
	bool updateStatus();
	bool virtual onMessage(DWORD msgid, DWORD wParam, ULONG lParam) override{
		NGLOG_DEBUG("msgid(%d) wParam (%d) ",msgid,wParam);
		if(WM_CCAMSG==msgid&&E_APP_CMD_CCA_CA_SERVICE_LIST_STATUS==wParam){
			updateStatus();
			show();
			return true;
		}
		return Window::onMessage(msgid,wParam,lParam);
	};

};

bool IrdCcaServiceListStatus::updateStatus()
{
	UINT16    u16ServiceNum = 0;
    UINT16 u16ArraySize  = STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH;

    memset(gau8Services, 0, u16ArraySize);
    u16ServiceNum = MApp_CCAUI_CasCmd_GetDvbClientServices(gau8Services, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH);

    if (u16ServiceNum > 0)
    {
		if(sllv) sllv->clear();
		else{
			sllv = new ListView(790,480);
			addChildView(sllv)->setPos(10,80);
			sllv->setBgColor(0);
		    sllv->setFgColor(getFgColor());
		}
		for ( int i=0; i<u16ServiceNum; i++ ){
			sllv->addItem(new ListView::ListItem((const char*)(gau8Services+i * STATUS_ITEM_MAX_LINE_LENGTH),i));
			sllv->setItemClickListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
				CreatIrdCcaStatusComWindow(E_APP_CMD_CCA_CA_SERVICE_STATUS,index);
		    });
		}	
}
	
}
IrdCcaServiceListStatus::IrdCcaServiceListStatus(int x,int y,int w,int h):Window(x,y,w,h)
{
	sltitle=new TextView("IrdCcaServiceListStatus",796,40);
	addChildView(sltitle)->setPos(2, 2);
	updateStatus();
}


bool CreateIrdCcaServiceListStatus(){
	Window * w= new IrdCcaServiceListStatus(420,80,800,200);
	w->show();
	return true;
}
}
