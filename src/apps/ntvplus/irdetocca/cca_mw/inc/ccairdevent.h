#ifndef __UNIVERSAL_CCAIRD_IRD_EVENT__
#define __UNIVERSAL_CCAIRD_IRD_EVENT__

#include<looper/EventLoop.h>
#include<vector>
#include <ngl_log.h>

using namespace nglui;

/********************************************************/
 /********************************************************/
 
 typedef enum
 {
    E_APP_CMD_CCAIRD_SHOW_ERROR_MSG,
    E_APP_CMD_CCAIRD_PIN_CODE,
    E_APP_CMD_CCAIRD_IPPV,
    E_APP_CMD_CCAIRD_KEY_INPUT,
    E_APP_CMD_CCAIRD_FINGERPRINT,
    E_APP_CMD_CCAIRD_FINGERPRINTING_OPTION,
    E_APP_CMD_CCAIRD_TMS,
    E_APP_CMD_CCAIRD_FORCED_MSG,
    E_APP_CMD_CCAIRD_MAIL,
    E_APP_CMD_CCAIRD_ICON_MSG,
    E_APP_CMD_CCAIRD_PREVIEW,
    E_APP_CMD_CCAIRD_STATUS,
    E_APP_CMD_CCAIRD_PIN_CHANGE,
    E_APP_CMD_CCAIRD_OTA_MSG_SHOW_EVENT,
    E_APP_CMD_CCAIRD_OTA_MSG_DRV_EVENT,
    E_APP_CMD_CCAIRD_OTA_MSG_KEY_EVENT,
    E_APP_CMD_CCAIRD_OTA_MENU_SIGNAL_INFO,
    E_APP_CMD_CCAIRD_END, ///< End of command list.                            
 } tCCAIRD_UI_EventType;
 
 typedef struct
 {
     //tCCAIRD_UI_EventType   eEventType;             
     //DWORD               dwScSlot;               
     //DWORD               dwAcsId;                
     //DWORD               dwStbStreamHandle;      
     UINT8               u8CmdData[20];  
	 UINT8				 *pUserArg;
     //BOOL                bIsScrambledByProgram;  
     //UINT32              uiStreamsNumber;        
 } tCCAIRD_UI_Event;
 
 /********************************************************/
 /********************************************************/
 
 typedef void (*tCCAIRD_UI_EventCallback)(tCCAIRD_UI_Event *pEvent);
  /* End of group : uiEventsTypes */
 
 
 DWORD CCAIRD_UI_AddEventListener(tCCAIRD_UI_EventCallback pEventCallbackFunction, void  *pUserArg);
 
 INT CCAIRD_UI_RemoveEventListener(DWORD dwCCAIRDListenerHandle);
 
 
 
 DWORD CCAIRD_UI_AddProgramEventListener(tCCAIRD_UI_EventCallback pEventCallbackFunction, void * pUserArg);
 
 INT   CCAIRD_UI_RemoveProgramEventListener(DWORD dwCCAIRDListenerHandle);
 
  /* End of group : uiEventsFunctions */
 
  /* End of group : uiEvents */

class CCAIRDEventSource:public nglui::EventSource{
protected:
    std::vector<tCCAIRD_UI_Event>events;
	static CCAIRDEventSource*mInst;
public:
    DWORD svcstate;
public:
    CCAIRDEventSource(){};
	static CCAIRDEventSource*getInstance(){
		if(mInst==nullptr)
		mInst=new CCAIRDEventSource();
		return mInst;
	}
    bool prepare(int&) override { return events.size();}
    bool check(){
        return  events.size();
    }
    void addEvent(tCCAIRD_UI_Event&e){
        events.emplace(events.begin(),e);
    }
    bool getEvent(tCCAIRD_UI_Event&e){
        if(events.size()){
           e=events.back();
           events.pop_back();
           return true;
        }
        return false;
    }
    bool dispatch(EventHandler &func) { return func(*this); }
	void sendIrdCcaCmd( tCCAIRD_UI_Event&e){ 
		addEvent(e);
	}
};



#endif 

