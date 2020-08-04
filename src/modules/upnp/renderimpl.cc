#include <stdlib.h>
#include <renderimpl.h>
#include <PltUPnP.h>
#include <PltMediaRenderer.h>
#include <NptStrings.h>
#include <ngl_mediaplayer.h>
#include <ngl_log.h>
#include <ngl_video.h>

NGL_MODULE(MediaRender);

namespace nglui{

MediaRenderer*MediaRenderer::mInst=nullptr;

class RendererImpl:public PLT_MediaRendererDelegate{
protected:
    HANDLE player;
    PLT_UPnP upnp;
    PLT_DeviceHostReference device;
    std::string current_uri;
public:
    RendererImpl(const std::string&name){
        player=NULL;
        device=new PLT_MediaRenderer("NGL Media Renderer",false,"cb35d433-fd0d-4911-8bed-3d2ba0343540");
        upnp.AddDevice(device);
        upnp.Start();
        ((PLT_MediaRenderer*)device.AsPointer())->SetDelegate(this);
        NGLOG_DEBUG("=============mediarenderer %p started",device.AsPointer());
    }
    ~RendererImpl(){
        //delete device;//auto delete
        upnp.Stop();
        if(player)nglMPClose(player);
        player=nullptr;
    }
    virtual NPT_Result OnGetCurrentConnectionInfo(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    // AVTransport
    virtual NPT_Result OnNext(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnPause(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnPlay(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
        nglAvStop(0);
        if(player)nglMPClose(player);
        player=nglMPOpen(current_uri.c_str());
        nglMPPlay(player);
    }
    virtual NPT_Result OnPrevious(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnSeek(PLT_ActionReference& action){
        NPT_String unit,target;
        action->GetArgumentValue("Unit",unit);
        action->GetArgumentValue("Target",target);
        NGLOG_DEBUG("Action:%s Unit:%s Target:%s",action->GetActionDesc().GetName().GetChars(),unit.GetChars(),target.GetChars());
    }
    virtual NPT_Result OnStop(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
        if(player)nglMPClose(player);
        player=nullptr;
    }
    virtual NPT_Result OnSetAVTransportURI(PLT_ActionReference& action){
        NPT_String uri;
        NPT_Result rc=action->GetArgumentValue("CurrentURI",uri);
        current_uri=uri.GetChars();
        NGLOG_DEBUG("uri=%s",uri.GetChars());
        return rc;
    }
    virtual NPT_Result OnSetNextAVTransportURI(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnSetPlayMode(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }

    // RenderingControl
    virtual NPT_Result OnSetVolume(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnSetVolumeDB(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnGetVolumeDBRange(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
    virtual NPT_Result OnSetMute(PLT_ActionReference& action){
        NGLOG_DEBUG("Action:%s",action->GetActionDesc().GetName().GetChars());
    }
};

MediaRenderer::MediaRenderer(const std::string&name){
    deviceName=name;
    deviceimpl=nullptr;
}
MediaRenderer::~MediaRenderer(){
    delete deviceimpl;
}

MediaRenderer& MediaRenderer::getInstance(){
    if(mInst==nullptr)
       mInst=new MediaRenderer("NGL MediaRender");
    NGLOG_DEBUG("mInst=%p",mInst);
    return *mInst;
}

MediaRenderer& MediaRenderer::setName(const std::string&name){
    deviceName=name;
    NGLOG_DEBUG("Render(%p) %s deviceimpl=%p",this,name.c_str(),deviceimpl);
}

MediaRenderer&MediaRenderer::start(){
    NGLOG_DEBUG("deviceimpl=%p",deviceimpl);
    if(deviceimpl==nullptr)
       deviceimpl=new RendererImpl(deviceName);
}

}//namespace
