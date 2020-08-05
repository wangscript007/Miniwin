#include <ntvwindow.h>
#include <ngl_log.h>
#include <CCA_UCMsgProc.h>
#include <MApp_ZUI_ACTCCACasCmd.h>
#include <irdccaui.h>

NGL_MODULE(LOADERSTATUS);
using namespace ntvplus;

namespace cca{

class IrdccaLoaderStatusWindow:public Window{
protected:
	TextView *testxt1=nullptr;
	TextView *device =nullptr;
	TextView *key =nullptr;
	TextView *cssn =nullptr;
	TextView *dsn =nullptr;
	TextView *testxt2 =nullptr;
	TextView *firmversion =nullptr;
	TextView *loaderversion =nullptr;
	TextView *seriaNo =nullptr;
public:
	IrdccaLoaderStatusWindow(int x,int y,int w,int h);
	bool updateIrdccaLoaderStatus();
	virtual bool onMessage(DWORD msgid,DWORD wParam,ULONG lParam) override;
};

extern BOOL LoaderStauts_result;
bool IrdccaLoaderStatusWindow::updateIrdccaLoaderStatus(){
	INT8 au8Buf[128];
    memset(au8Buf, 0, sizeof(au8Buf));	

	//update loader data.
	LoaderStauts_result = CCA_Status_Get_Device_Status();
	if(LoaderStauts_result == true){ 
		NGLOG_DEBUG("Get LoaderStatus Success!!");
	}
	else{
		NGLOG_DEBUG("Get LoaderStatus Fail!!");
	}
	//device
	MApp_CCAUI_CasCmd_GetLoaderItemDeviceValue(au8Buf, sizeof(au8Buf));
	device->setText((const char*)au8Buf);
	//key
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemKey(au8Buf, sizeof(au8Buf));
	key->setText((const char*)au8Buf);
	//cssn
	memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemCssn(au8Buf, sizeof(au8Buf));
	cssn->setText((const char*)au8Buf);
	//dsn id
	memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((INT8 *)au8Buf, sizeof(au8Buf), "L-                          0x%04lx", MApp_CCAUI_CasCmd_GetLoaderItemDoadloadId());
	dsn->setText((const char*)au8Buf);
	
	//firmware
	memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderFirmwareVersion(au8Buf, sizeof(au8Buf));
	firmversion->setText((const char*)au8Buf);
	//loader version
	memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((INT8 *)au8Buf, sizeof(au8Buf), "Loader version        0x%04x", MApp_CCAUI_CasCmd_GetLoaderItemLoaderVersion());
	loaderversion->setText((const char*)au8Buf);
	//serial no.
	memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemSerialNo(au8Buf, sizeof(au8Buf));
	seriaNo->setText((const char*)au8Buf);
	return true;
}

bool IrdccaLoaderStatusWindow::onMessage(DWORD msgid, DWORD wParam, ULONG lParam){
	NGLOG_DEBUG("msgid(%d) wParam (%d) )",msgid,wParam);
	if((View::WM_CCAMSG==msgid)&&(wParam==E_APP_CMD_CCA_CA_LOADER_STATUS)){
		this->updateIrdccaLoaderStatus();
		this->show();
	   return true;
	}		
	return Window::onMessage(msgid,wParam,lParam);
}

IrdccaLoaderStatusWindow::IrdccaLoaderStatusWindow(int x,int y,int w,int h):Window(x,y,w,h){
	INT8 au8Buf[128];
	
    memset(au8Buf, 0, sizeof(au8Buf));
	testxt1=new TextView("Irdeto Loader Status",780,50);
	testxt1->setFontSize(40);
	testxt1->setFgColor(0xFFFFFFFF);
	addChildView(testxt1)->setPos(20, 5).setBgColor(0x80000000);
	if(parent_!=nullptr){
        setBgColor(parent_->getBgColor());
        setFgColor(parent_->getFgColor());
    }
	//init loader data.
	LoaderStauts_result = CCA_Status_Get_Device_Status();
	if(LoaderStauts_result == true){ 
		NGLOG_DEBUG("Get LoaderStatus Success!!");
	}
	else{
		NGLOG_DEBUG("Get LoaderStatus Fail!!");
	}
	//device
	MApp_CCAUI_CasCmd_GetLoaderItemDeviceValue(au8Buf, sizeof(au8Buf));
	device=new TextView((const char*)au8Buf,790,40);
	device->setFgColor(0xFFFFFFFF);
	addChildView(device)->setPos(10, 55).setBgColor(0x80000000);
	//key
    memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemKey(au8Buf, sizeof(au8Buf));
	key=new TextView((const char*)au8Buf,790,40);
	key->setFgColor(0xFFFFFFFF);
	addChildView(key)->setPos(10, 95).setBgColor(0x80000000);
	//cssn
	memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemCssn(au8Buf, sizeof(au8Buf));
	cssn=new TextView((const char*)au8Buf,790,40);
	cssn->setFgColor(0xFFFFFFFF);
	addChildView(cssn)->setPos(10, 135).setBgColor(0x80000000);
	//dsn id
	memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((INT8 *)au8Buf, sizeof(au8Buf), "L-                          0x%04lx", MApp_CCAUI_CasCmd_GetLoaderItemDoadloadId());
	dsn=new TextView((const char*)au8Buf,790,40);
	dsn->setFgColor(0xFFFFFFFF);
	addChildView(dsn)->setPos(10, 175).setBgColor(0x80000000);
	
	testxt2=new TextView("Manufacture Loader Status",780,50);
	testxt2->setFontSize(40);
	testxt2->setFgColor(0xFFFFFFFF);
	addChildView(testxt2)->setPos(20,200).setBgColor(0x80000000);
	//firmware
	memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderFirmwareVersion(au8Buf, sizeof(au8Buf));
	firmversion=new TextView((const char*)au8Buf,790,40);
	firmversion->setFgColor(0xFFFFFFFF);
	addChildView(firmversion)->setPos(10, 250).setBgColor(0x80000000);
	//loader version
	memset(au8Buf, 0, sizeof(au8Buf));
    snprintf((INT8 *)au8Buf, sizeof(au8Buf), "Loader version        0x%04x", MApp_CCAUI_CasCmd_GetLoaderItemLoaderVersion());
	loaderversion=new TextView((const char*)au8Buf,790,40);
	loaderversion->setFgColor(0xFFFFFFFF);
	addChildView(loaderversion)->setPos(10, 290).setBgColor(0x80000000);
	//serial no.
	memset(au8Buf, 0, sizeof(au8Buf));
    MApp_CCAUI_CasCmd_GetLoaderItemSerialNo(au8Buf, sizeof(au8Buf));
	seriaNo=new TextView((const char*)au8Buf,790,40);
	seriaNo->setFgColor(0xFFFFFFFF);
	addChildView(seriaNo)->setPos(10, 330).setBgColor(0x80000000);

}


bool CreateIrdCcaLoaderStatusWindow(){
	Window*w = new IrdccaLoaderStatusWindow(420,280,800,400);
	w->show();
	return true;
}

}
