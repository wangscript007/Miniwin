#include "NGSoundManager.h"
#include "../Core/EmulationSettings.h"
#include "../Core/MessageManager.h"
#include "../Core/SoundMixer.h"
#include "../Core/Console.h"
#include <cmath>

NGSoundManager::NGSoundManager(shared_ptr<Console> console) {
    _console = console;

    if(InitializeAudio(48000, true)) {
        _console->GetSoundMixer()->RegisterAudioDevice(this);
    }
}

NGSoundManager::~NGSoundManager() {
    Release();
}

void NGSoundManager::FillAudioBuffer(void *userData, uint8_t *stream, int len) {
    NGSoundManager* soundManager = (NGSoundManager*)userData;

    soundManager->ReadFromBuffer(stream, len);
}

void NGSoundManager::Release() {
    if(_audioDeviceID != 0) {
        Stop();
        //SDL_CloseAudioDevice(_audioDeviceID);
    }
    if(_buffer) {
        delete[] _buffer;
        _buffer = nullptr;
        _bufferSize = 0;
    }
}

bool NGSoundManager::InitializeAudio(uint32_t sampleRate, bool isStereo) {
#if 0
    int isCapture = 0;

    _sampleRate = sampleRate;
    _isStereo = isStereo;
    _previousLatency = _console->GetSettings()->GetAudioLatency();

    int bytesPerSample = 2 * (isStereo ? 2 : 1);
    int32_t requestedByteLatency = (int32_t)((float)(sampleRate * _console->GetSettings()->GetAudioLatency()) / 1000.0f * bytesPerSample);
    _bufferSize = (int32_t)std::ceil((double)requestedByteLatency * 2 / 0x10000) * 0x10000;
    _buffer = new uint8_t[_bufferSize];
    memset(_buffer, 0, _bufferSize);

    SDL_AudioSpec audioSpec;
    SDL_memset(&audioSpec, 0, sizeof(audioSpec));
    audioSpec.freq = sampleRate;
    audioSpec.format = AUDIO_S16SYS; //16-bit samples
    audioSpec.channels = isStereo ? 2 : 1;
    audioSpec.samples = 1024;
    audioSpec.callback = &NGSoundManager::FillAudioBuffer;
    audioSpec.userdata = this;

    _audioDeviceID = SDL_OpenAudioDevice(_deviceName.empty() ? nullptr : _deviceName.c_str(), isCapture, &audioSpec, &obtainedSpec, 0);
    if(_audioDeviceID == 0 && !_deviceName.empty()) {
        _audioDeviceID = SDL_OpenAudioDevice(nullptr, isCapture, &audioSpec, &obtainedSpec, 0);
    }
    _writePosition = 0;
    _readPosition = 0;
    _needReset = false;
    return _audioDeviceID != 0;
#else 
    return true;
#endif
}

string NGSoundManager::GetAvailableDevices() {
    string deviceString;
    for(string device : GetAvailableDeviceInfo()) {
        deviceString += device + std::string("||");
    }
    return deviceString;
}

vector<string> NGSoundManager::GetAvailableDeviceInfo() {
    vector<string> deviceList;
#if 0
    int isCapture = 0;
    int deviceCount = -1;//SDL_GetNumAudioDevices(isCapture);

    if(deviceCount == -1) {
        //No devices found
    } else {
        for(int i = 0; i < deviceCount; i++) {
            //deviceList.push_back(SDL_GetAudioDeviceName(i, isCapture));
        }
    }
#else
    deviceList.push_back("NGAudio Device");
#endif
    return deviceList;
}

void NGSoundManager::SetAudioDevice(string deviceName) {
    if(deviceName.compare(_deviceName) != 0) {
        _deviceName = deviceName;
        _needReset = true;
    }
}

void NGSoundManager::ReadFromBuffer(uint8_t* output, uint32_t len) {
    if(_readPosition + len < _bufferSize) {
        memcpy(output, _buffer+_readPosition, len);
        _readPosition += len;
    } else {
        int remainingBytes = (_bufferSize - _readPosition);
        memcpy(output, _buffer+_readPosition, remainingBytes);
        memcpy(output+remainingBytes, _buffer, len - remainingBytes);
        _readPosition = len - remainingBytes;
    }

    if(_readPosition >= _writePosition && _readPosition - _writePosition < _bufferSize / 2) {
        _bufferUnderrunEventCount++;
    }
}

void NGSoundManager::WriteToBuffer(uint8_t* input, uint32_t len) {
    if(_writePosition + len < _bufferSize) {
        memcpy(_buffer+_writePosition, input, len);
        _writePosition += len;
    } else {
        int remainingBytes = _bufferSize - _writePosition;
        memcpy(_buffer+_writePosition, input, remainingBytes);
        memcpy(_buffer, ((uint8_t*)input)+remainingBytes, len - remainingBytes);
        _writePosition = len - remainingBytes;
    }
}

void NGSoundManager::UpdateSoundSettings() {
    uint32_t sampleRate = _console->GetSettings()->GetSampleRate();
    uint32_t latency = _console->GetSettings()->GetAudioLatency();
    if(_sampleRate != sampleRate || _needReset || _previousLatency != latency) {
        Release();
        InitializeAudio(sampleRate, true);
    }
}

void NGSoundManager::PlayBuffer(int16_t *soundBuffer, uint32_t sampleCount, uint32_t sampleRate, bool isStereo) {
    uint32_t bytesPerSample = (SoundMixer::BitsPerSample / 8) * (isStereo ? 2 : 1);
    UpdateSoundSettings();

    WriteToBuffer((uint8_t*)soundBuffer, sampleCount * bytesPerSample);

    int32_t byteLatency = (int32_t)((float)(sampleRate * _previousLatency) / 1000.0f * bytesPerSample);
    int32_t playWriteByteLatency = _writePosition - _readPosition;
    if(playWriteByteLatency < 0) {
        playWriteByteLatency = _bufferSize - _readPosition + _writePosition;
    }

    if(playWriteByteLatency > byteLatency) {
        //Start playing
        //SDL_PauseAudioDevice(_audioDeviceID, 0);
    }
}

void NGSoundManager::Pause() {
    //SDL_PauseAudioDevice(_audioDeviceID, 1);
}

void NGSoundManager::Stop() {
    Pause();

    _readPosition = 0;
    _writePosition = 0;
    ResetStats();
}

void NGSoundManager::ProcessEndOfFrame() {
    ProcessLatency(_readPosition, _writePosition);

    uint32_t emulationSpeed = _console->GetSettings()->GetEmulationSpeed();
    if(_averageLatency > 0 && emulationSpeed <= 100 && emulationSpeed > 0 && std::abs(_averageLatency - _console->GetSettings()->GetAudioLatency()) > 50) {
        //Latency is way off (over 50ms gap), stop audio & start again
        Stop();
    }
}
