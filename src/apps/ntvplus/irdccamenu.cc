#include <ntvwindow.h>
#include <ngl_log.h>
#include <CCA_UCMsgProc.h>
#include <MApp_ZUI_ACTCCACasCmd.h>
#include <irdccaui.h>

NGL_MODULE(IRDCCAMENU);
using namespace ntvplus;

namespace cca{

class IrdccaMenuWindow:public NTVWindow{
protected:
	ListView *mnlv=nullptr;
	ListView *inflv=nullptr;
	TextView *inftitle=nullptr;
	UINT8 osdId=0xFF;//EN_CCA_APPCMD_CA_STATUS
public:
	IrdccaMenuWindow(int x,int y,int w,int h);
	bool CreateIrdCcaCaStatus();
};


#define STATUS_ITEM_MAX_LINES            64  // Lines
#define STATUS_ITEM_MAX_LINE_LENGTH      254  // 80 bytes
#define SERVICE_STATUS_MENU_ITEM_NUM     8
#define SERVICE_STATUS_MENU_ITEM_OFFSET  2
#define STATUS_ITEM_MAX_LINES            64  // Lines
#define STATUS_ITEM_MAX_LINE_LENGTH      254  // 80 bytes
#define PRODUCT_ITEM_MAX_LINES            16  // Lines
#define S3_IRD_SERVICE_STATUS_SCREEN_TEXT_LINE_LENGTH  32
#define S3_IRD_SERVIEE_TEXT_BUF_SIZE                   (STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH)
#define TOTAL_SERVICES_STATUS_PAGE_LINE_EXTEND 1

INT8  gau8Services[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
INT8  gau8Product[PRODUCT_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
INT8  gau8Client[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];

static UINT32  gu32ProductIdx;

IrdccaMenuWindow::IrdccaMenuWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
	const char* btnname[]={"Irdeto CA status","Email information","OAD"};
	initContent(NWS_TITLE);
    setText("IrdCcaMenu");
	NGLOG_DEBUG("%p ",this);
	mnlv=new ListView(200,150);
	mnlv->setItemSize(0,50);
	mnlv->setBgColor(0);
	mnlv->setFlag(View::BORDER);
	mnlv->setItemPainter(SettingPainter);
	int itemcount=sizeof(btnname)/sizeof(char*);
    for(int i=0;i<itemcount;i++){
   		mnlv->addItem(new ListView::ListItem(btnname[i%itemcount],i));
	}
	mnlv->setIndex(0);
	addChildView(mnlv)->setPos(0,80);
    mnlv->setItemClickListener([&](AbsListView&lv,const ListView::ListItem&itm,int index){
		switch(index){
	     case 0: {return CreateIrdCcaCaStatus();}break;
		 default:break;
		}
	});
	addTipInfo("help_icon_4arrow.png","Navigation",420);
    addTipInfo("help_icon_ok.png","Select",420);
    addTipInfo("help_icon_exit.png","Exit",420);
}
bool CreateIrdCcaCaStatus(IrdccaMenuWindow *w);


bool IrdccaMenuWindow::CreateIrdCcaCaStatus(){
	char *serbuffer=nullptr;
	char *prodbuffer=nullptr;
	char *cilbuffer=nullptr;
	const char* btnname[]={"Service status","Loader status","Product status","Client status"};
	ListView*cslv=new ListView(200,200);
	cslv->setItemSize(0,50);
	cslv->setBgColor(0);
	cslv->setItemPainter(SettingPainter);
	int itemcount=sizeof(btnname)/sizeof(char*);
    for(int i=0;i<itemcount;i++){
   		cslv->addItem(new ListView::ListItem(btnname[i%itemcount],i));
	}
	cslv->setIndex(0);
	cslv->setItemClickListener([&](AbsListView&lv,const ListView::ListItem&itm,int index){
		switch(index){
			case 0: {return CreateIrdCcaServiceListStatus();}break;//service  status
			case 1: {return CreateIrdCcaLoaderStatusWindow();}break;//loader status
			case 2: {return CreatIrdCcaStatusComWindow(E_APP_CMD_CCA_CA_PRODUCT_STATUS,0);}break;//product status
			case 3: {return CreatIrdCcaStatusComWindow(E_APP_CMD_CCA_CA_CLIENT_STATUS,0);}break;// client status
			default: break;
		}
    });
	addChildView(cslv)->setPos(210,80);
	return true;
}

Window*CreateIrdCcaMenuWindow(){
	Window*w=new IrdccaMenuWindow(0,0,1280,720);
	w->show();

	return w;
}

}
