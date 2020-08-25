#include <ntvwindow.h>
#include <ngl_log.h>
#include <irdccaui.h>
#include <CCA_UCMsgProc.h>
#include <MApp_ZUI_ACTCCACasCmd.h>

NGL_MODULE(CCASTAUTSCOM);

#define STATUS_LIST_ITEM_HEIGHT			  40
#define PRODUCT_ITEM_MAX_LINES            16  // Lines

using namespace ntvplus;

namespace cca{

extern INT8  gau8Services[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
extern INT8  gau8Product[PRODUCT_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];
extern INT8  gau8Client[STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH];

#define S3_IRD_SERVIEE_TEXT_BUF_SIZE                   (STATUS_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH)

class IrdCcaStatusCom:public Window{
protected:
	ListView* inflst;
	INT8* buffer= NULL;
	UINT8 u16LineCount=0;
	UINT8 osdId=0xFF;
	UINT8 serIndex=0xFF;
public:
	IrdCcaStatusCom(int x,int y,int w,int h,UINT8 menuflag,int index);
	bool updateStatusCom(UINT8 menuflag,UINT8 index);
	UINT16 GetServiceStatus(INT8 *pu8Text, UINT8 u8LineCount, UINT16 u16LineLength,UINT8 index);
	virtual bool onMessage(DWORD msgid, DWORD wParam, ULONG lParam) override{
		NGLOG_DEBUG("msgid(%d) wParam (%d) osdId(%d)",msgid,wParam,osdId);
		if(WM_CCAMSG==msgid&&wParam==osdId){//osdid need define first
			this->updateStatusCom(wParam,serIndex);
			this->show();
		   return true;
		}		
		return Window::onMessage(msgid,wParam,lParam);
	};
};

static UINT32 _atohex(UINT8 u8Char)
{
    if (u8Char >= 'a')
    {
        return  ((u8Char - 'a') + 10);
    }
    else if (u8Char >= 'A')
    {
        return  ((u8Char - 'A') + 10);
    }
    else if (u8Char >= '0')
    {
        return  (u8Char - '0');
    }

    return 0;
}

UINT16 _MApp_ZUI_ACT_GetCCASerivesStatus_CurrentHandle(UINT8 index)
{
    INT8* pCurrLine = gau8Services+index*STATUS_ITEM_MAX_LINE_LENGTH;
    UINT16 u16Handle = 0;    
    u16Handle = u16Handle + (_atohex(pCurrLine[6]) << 12);
    u16Handle = u16Handle + (_atohex(pCurrLine[7]) << 8);
    u16Handle = u16Handle + (_atohex(pCurrLine[8]) << 4);
    u16Handle = u16Handle + (_atohex(pCurrLine[9]));
    NGLOG_DEBUG("u16Handle = 0x%08x\n", u16Handle);

    return u16Handle;
}

UINT16 IrdCcaStatusCom :: GetServiceStatus(INT8 *pu8Text, UINT8 u8LineCount, UINT16 u16LineLength,UINT8 index)
{
    UINT8  u8TotalLines = 0;
    UINT8  u8SplitLines = 0;
    INT8* pu8Str       = pu8Text;
    UINT32 u32Handle    = _MApp_ZUI_ACT_GetCCASerivesStatus_CurrentHandle(index);
    UINT32 u32Index     = MApp_CCAUI_CasCmd_GetServiceIndexByHandle(u32Handle);
    INT8  au8Buf[STATUS_ITEM_MAX_LINE_LENGTH];

    if (u32Index == 0xfffffff)
    {
        NGLOG_DEBUG("This handle is not found = %d\n", u32Handle);
        return 0;
    }
    else
    {
        NGLOG_DEBUG("Handle = %d, Index = %ld\n", u32Handle, u32Index);
    }

    //enable Monitor
    if(MApp_CCAUI_CasCmd_EnableServiceMonitorStatus(u32Index, TRUE) == FALSE)
    {
        NGLOG_DEBUG("Enable Monitor for handle %d FAIL!!\n", u32Handle);
    }

    // Service name
    memset(au8Buf, '\0', sizeof(au8Buf));
    if (MApp_CCAUI_CasCmd_GetServiceItemName(u32Index, au8Buf, sizeof(au8Buf)) == TRUE)
    {
        snprintf((char *)pu8Str, u16LineLength, "%s", au8Buf);
    }
    else
    {
        snprintf((char *)pu8Str, u16LineLength, "%s", "FAIL to get the service name!");
    }
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    // Handle
    snprintf((char *)pu8Str, u16LineLength, "  Service Handle:  0x%08lX", u32Handle);
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Service status
    memset(au8Buf, '\0', sizeof(au8Buf));
    if (MApp_CCAUI_CasCmd_GetServiceItemStatus(u32Index, au8Buf, u16LineLength) == TRUE)
    {
        //FIXME next line : if(strlen(au8Buf)>34)
        snprintf((char *)pu8Str, u16LineLength, "  Service Status:   %s", au8Buf);
    }
    else
    {
        snprintf((char *)pu8Str, u16LineLength, "%s", "FAIL to get the service status!");
    }
    u8TotalLines++;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength;

    //Service Pids status
    u8SplitLines = MApp_CCAUI_CasCmd_GetServiceItemCaStatus(u32Index, pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;

    //Monitoring
    u8SplitLines = MApp_CCAUI_CasCmd_GetServiceItemMonitorStatus(u32Index, pu8Str, u8LineCount - u8TotalLines, u16LineLength);
    u8TotalLines += u8SplitLines;
    if (u8LineCount <= u8TotalLines)
    {
        return u8TotalLines;
    }
    pu8Str += u16LineLength * u8SplitLines;
    
    return u8TotalLines;
}
bool IrdCcaStatusCom:: updateStatusCom(UINT8 menuflag,UINT8 index){

	if(inflst){
		UINT16 u16ArraySize  = PRODUCT_ITEM_MAX_LINES * STATUS_ITEM_MAX_LINE_LENGTH;
		INT8  gau8Str[S3_IRD_SERVIEE_TEXT_BUF_SIZE];
		memset(gau8Str,0,sizeof(gau8Str));
		switch(menuflag)
		{
			case E_APP_CMD_CCA_CA_SERVICE_STATUS:{
					u16LineCount =GetServiceStatus(gau8Str, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH,index);
					buffer = (INT8*)gau8Str;
				};break;
			case E_APP_CMD_CCA_CA_PRODUCT_STATUS:{
					u16LineCount = MApp_CCAUI_CasCmd_GetProductCount()+2;
					memset(gau8Product, 0, u16ArraySize);
					MApp_CCAUI_CasCmd_GetProductStatus(gau8Product, PRODUCT_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH, 0);
					buffer = (INT8*)gau8Product;
				};break;
			case E_APP_CMD_CCA_CA_CLIENT_STATUS:{
				memset(gau8Client, 0, u16ArraySize);
				u16LineCount = MApp_CCAUI_CasCmd_GetClientStatus(gau8Client, STATUS_ITEM_MAX_LINES, STATUS_ITEM_MAX_LINE_LENGTH);
				buffer = (INT8*)gau8Client;
				};break;
			default:return false;
		}
		
		inflst->clear();
		for(INT8 i=0;i<u16LineCount;i++){
			inflst->addItem(new ListView::ListItem((const char*)(buffer+i*STATUS_ITEM_MAX_LINE_LENGTH),i));
		}

		return true;
	}

	return false;
}

IrdCcaStatusCom::IrdCcaStatusCom(int x,int y,int w,int h,UINT8 menuflag,int index): Window(x,y,w,h){
	osdId = menuflag;
	serIndex = index;
	inflst = new ListView(w,h);
	inflst->setPos(0,0);
    inflst->setItemSize(-1,STATUS_LIST_ITEM_HEIGHT);
    inflst->setBgColor(0);
    inflst->setFgColor(getFgColor());
    inflst->setFlag(View::SCROLLBARS_VERTICAL);
    inflst->setFlag(View::BORDER);
    inflst->setItemPainter(ChannelPainter);
    addChildView(inflst);
	updateStatusCom(menuflag,index);	
}	
bool CreatIrdCcaStatusComWindow(UINT8 menuflag,int index){
	Window *w  = new IrdCcaStatusCom(420,80,800,400,menuflag, index);
	w->show();
	return true;
	
}
}
