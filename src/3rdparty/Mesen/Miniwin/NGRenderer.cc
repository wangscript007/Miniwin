#include "NGRenderer.h"
#include "../Core/Console.h"
#include "../Core/Debugger.h"
#include "../Core/VideoRenderer.h"
#include "../Core/VideoDecoder.h"
#include "../Core/EmulationSettings.h"
#include <ngl_types.h>
#include <ngl_log.h>

SimpleLock NGRenderer::_frameLock;

NGL_MODULE(Mesen);

#define WM_NES 1000

NGRenderer::NGRenderer(shared_ptr<Console> console,bool registerAsMessageManager)
    : Window(240,40,800,640),BaseRenderer(console, registerAsMessageManager) {
    _frameBuffer = nullptr;
    _requiredWidth = 256;
    _requiredHeight = 240;

    shared_ptr<VideoRenderer> videoRenderer = _console->GetVideoRenderer();
    if(videoRenderer) {
        _console->GetVideoRenderer()->RegisterRenderingDevice(this);
    }
}

NGRenderer::~NGRenderer() {
    shared_ptr<VideoRenderer> videoRenderer = _console->GetVideoRenderer();
    if(videoRenderer) {
        videoRenderer->UnregisterRenderingDevice(this);
    }

    auto lock = _frameLock.AcquireSafe();
    Cleanup();
    delete[] _frameBuffer;
}

void NGRenderer::SetFullscreenMode(bool fullscreen, void* windowHandle, uint32_t monitorWidth, uint32_t monitorHeight) {
    //TODO: Implement exclusive fullscreen for Linux
}

bool NGRenderer::Init() {
    auto log = [](const char* msg) {
        MessageManager::Log(msg);
        //MessageManager::Log(SDL_GetError());
    };

    return true;
}

void NGRenderer::Cleanup() {
}

void NGRenderer::Reset() {
    Cleanup();
    if(Init()) {
        _console->GetVideoRenderer()->RegisterRenderingDevice(this);
    } else {
        Cleanup();
    }
}

void NGRenderer::SetScreenSize(uint32_t width, uint32_t height) {
    ScreenSize screenSize;
    _console->GetVideoDecoder()->GetScreenSize(screenSize, false);

    if(_screenHeight != (uint32_t)screenSize.Height || _screenWidth != (uint32_t)screenSize.Width || _nesFrameHeight != height || _nesFrameWidth != width || _resizeFilter != _console->GetSettings()->GetVideoResizeFilter() || _vsyncEnabled != _console->GetSettings()->CheckFlag(EmulationFlags::VerticalSync)) {
        _vsyncEnabled = _console->GetSettings()->CheckFlag(EmulationFlags::VerticalSync);

        _nesFrameHeight = height;
        _nesFrameWidth = width;
        _newFrameBufferSize = width*height;

        _screenHeight = screenSize.Height;
        _screenWidth = screenSize.Width;

        _resizeFilter = _console->GetSettings()->GetVideoResizeFilter();

        _screenBufferSize = _screenHeight*_screenWidth;

        Reset();
    }
}

void NGRenderer::UpdateFrame(void *frameBuffer, uint32_t width, uint32_t height) {
    _frameLock.Acquire();
    if(_frameBuffer == nullptr || _requiredWidth != width || _requiredHeight != height) {
        _requiredWidth = width;
        _requiredHeight = height;

        delete[] _frameBuffer;
        _frameBuffer = new uint32_t[width*height];
        memset(_frameBuffer, 0, width*height*4);
    }

    memcpy(_frameBuffer, frameBuffer, width*height*_bytesPerPixel);
    _frameChanged = true;
    _frameLock.Release();
    invalidate(nullptr);
}

void NGRenderer::Render() {
    SetScreenSize(_requiredWidth, _requiredHeight);

    bool paused = _console->IsPaused() && _console->IsRunning();
    bool disableOverlay = _console->GetSettings()->CheckFlag(EmulationFlags::HidePauseOverlay);
    shared_ptr<Debugger> debugger = _console->GetDebugger(false);
    if(debugger && debugger->IsExecutionStopped()) {
        paused = debugger->IsPauseIconShown();
        disableOverlay = true;
    }

    if(_noUpdateCount > 10 || _frameChanged || paused || IsMessageShown()) {

        uint8_t *textureBuffer;
        int rowPitch;
        auto frameLock = _frameLock.AcquireSafe();
        if(_frameBuffer && _nesFrameWidth == _requiredWidth && _nesFrameHeight == _requiredHeight) {
            uint32_t* ppuFrameBuffer = _frameBuffer;
            for(uint32_t i = 0, iMax = _nesFrameHeight; i < iMax; i++) {
                memcpy(textureBuffer, ppuFrameBuffer, _nesFrameWidth*_bytesPerPixel);
                ppuFrameBuffer += _nesFrameWidth;
                textureBuffer += rowPitch;
            }
        }

        if(_frameChanged) {
            _renderedFrameCount++;
            _frameChanged = false;
        }

        if(paused && !_console->GetSettings()->CheckFlag(EmulationFlags::HidePauseOverlay)) {
            DrawPauseScreen(disableOverlay);
        } else if(_console->GetVideoDecoder()->IsRunning()) {
            DrawCounters();
        }
        DrawToasts();
    } else {
        _noUpdateCount++;
    }
}

void NGRenderer::onDraw(GraphContext&canvas){
    Window::onDraw(canvas);
    RefPtr<ImageSurface>img=ImageSurface::create((BYTE*)_frameBuffer,Surface::Format::RGB24,
         _requiredWidth, _requiredHeight, _requiredWidth*4);
    RECT rect={2,2,_requiredWidth*2,_requiredHeight*2};
    canvas.draw_image(img,&rect,nullptr,ST_FIT_XY);
}

bool NGRenderer::onMessage(DWORD msg,DWORD wp,ULONG lp){
    if(msg==WM_NES){
        sendMessage(msg,wp,lp,25);
        _console->RunFrame();
        return true;
    }
    return Window::onMessage(msg,wp,lp);
}
void NGRenderer::DrawPauseScreen(bool disableOverlay) {
    if(disableOverlay) {
        DrawString(L"I", 15, 15, 106, 90, 205, 168);
        DrawString(L"I", 23, 15, 106, 90, 205, 168);
    } else {
    }
}

void NGRenderer::DrawString(std::wstring message,int x,int y,uint8_t r,uint8_t g,uint8_t b,uint8_t opacity) {
    const wchar_t *text = message.c_str();
    //_spriteFont->DrawString(_sdlRenderer, text, x, y, r, g, b);
}

float NGRenderer::MeasureString(std::wstring text) {
    return 0;//return _spriteFont->MeasureString(text.c_str()).x;
}

bool NGRenderer::ContainsCharacter(wchar_t character) {
    return false;//_spriteFont->ContainsCharacter(character);
}


