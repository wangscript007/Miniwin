#pragma once
#include <windows.h>
#include "../Core/IRenderingDevice.h"
#include "../Utilities/SimpleLock.h"
#include "../Core/EmulationSettings.h"
#include "../Core/VideoRenderer.h"
#include "../Core/BaseRenderer.h"

class Console;

class NGRenderer : public Window,IRenderingDevice, public BaseRenderer{
private:
	VideoResizeFilter _resizeFilter = VideoResizeFilter::NearestNeighbor;

	static SimpleLock _frameLock;
	uint32_t* _frameBuffer = nullptr;

	const uint32_t _bytesPerPixel = 4;
	uint32_t _screenBufferSize = 0;

	bool _frameChanged = true;
	uint32_t _noUpdateCount = 0;

	uint32_t _requiredHeight = 0;
	uint32_t _requiredWidth = 0;
	
	uint32_t _nesFrameHeight = 0;
	uint32_t _nesFrameWidth = 0;
	uint32_t _newFrameBufferSize = 0;

	bool _vsyncEnabled = false;

	bool Init();
	void Cleanup();
	void SetScreenSize(uint32_t width, uint32_t height);

	void DrawPauseScreen(bool disableOverlay);

	float MeasureString(std::wstring text) override;
	bool ContainsCharacter(wchar_t character) override;

public:
	NGRenderer(shared_ptr<Console> console,bool registerAsMessageManager);
	virtual ~NGRenderer();

	void UpdateFrame(void *frameBuffer, uint32_t width, uint32_t height) override;
	void Render() override;
	void Reset() override;
	void DrawString(std::wstring message, int x, int y, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t opacity = 255) override;
	void SetFullscreenMode(bool fullscreen, void* windowHandle, uint32_t monitorWidth, uint32_t monitorHeight) override;
        void onDraw(GraphContext&canvas)override;
        bool onMessage(DWORD msg,DWORD wp,ULONG lp)override;
};
