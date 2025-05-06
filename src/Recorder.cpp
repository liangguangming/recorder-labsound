#include "Recorder.h"
#include "logger/Logger.h"
#include <fstream>
#include <LabSound/LabSound.h>

Recorder::Recorder() 
    : state_(State::Stopped)
    , sampleRate_(44100)
    , channels_(1)  // 修改为单声道，因为设备只支持单声道输入
{
    Logger::info("Recorder constructed");
}

Recorder::~Recorder() {
    stop();
    Logger::info("Recorder destroyed");
}

void Recorder::setOutputPath(const std::string& path) {
    outputPath_ = path;
}

bool Recorder::start() {
    if (state_ == State::Recording) {
        Logger::warn("Already recording");
        return false;
    }

    if (!initializeNodes()) {
        Logger::error("Failed to initialize audio nodes");
        return false;
    }

    state_ = State::Recording;
    Logger::info("Recording started");
    return true;
}

void Recorder::pause() {
    if (state_ != State::Recording) {
        Logger::warn("Not recording");
        return;
    }

    state_ = State::Paused;
    Logger::info("Recording paused");
}

void Recorder::resume() {
    if (state_ != State::Paused) {
        Logger::warn("Not paused");
        return;
    }

    state_ = State::Recording;
    Logger::info("Recording resumed");
}

void Recorder::stop() {
    if (state_ == State::Stopped) {
        return;
    }

    state_ = State::Stopped;
    
    if (!pcmBuffer_.empty()) {
        writeToFile();
    }

    // 清理资源
    if (ctx_) {
        ctx_->suspend();
        ctx_.reset();
    }
    
    micNode_.reset();
    gainNode_.reset();
    functionNode_.reset();
    
    Logger::info("Recording stopped");
}

Recorder::State Recorder::state() const {
    return state_;
}

bool Recorder::initializeNodes() {
    try {
        // 1. 创建 AudioContext
        lab::AudioStreamConfig outputConfig;
        outputConfig.desired_samplerate = sampleRate_;
        outputConfig.desired_channels = 2;  // 输出保持双声道
        outputConfig.device_index = 0;
        
        lab::AudioStreamConfig inputConfig;
        inputConfig.desired_samplerate = sampleRate_;
        inputConfig.desired_channels = 1;  // 输入使用单声道
        inputConfig.device_index = 0;
        
        Logger::info("Creating audio context with sample rate: {}, input channels: {}, output channels: {}", 
                    sampleRate_, inputConfig.desired_channels, outputConfig.desired_channels);
        
        ctx_ = lab::MakeRealtimeAudioContext(outputConfig, inputConfig);
        if (!ctx_) {
            Logger::error("Failed to create audio context");
            return false;
        }
        
        // 2. 创建并配置 micNode_
        lab::ContextRenderLock r(ctx_.get(), "Recorder::initializeNodes");
        micNode_ = lab::MakeAudioHardwareInputNode(r);
        if (!micNode_) {
            Logger::error("Failed to create microphone node");
            return false;
        }
        
        // 3. 创建并配置 gainNode_
        gainNode_ = std::make_shared<lab::GainNode>(*ctx_);
        gainNode_->gain()->setValue(1.0f);
        
        // 4. 创建并配置 functionNode_
        functionNode_ = std::make_shared<lab::FunctionNode>(*ctx_, channels_);
        functionNode_->setFunction([this](lab::ContextRenderLock& r, lab::FunctionNode* me, int channel, float* buffer, int bufferSize) {
            if (state_ == State::Recording) {
                std::lock_guard<std::mutex> lock(bufferMutex_);
                pcmBuffer_.insert(pcmBuffer_.end(), buffer, buffer + bufferSize);
            }
        });
        
        // 5. 连接节点
        ctx_->connect(gainNode_, micNode_, 0, 0);
        ctx_->connect(functionNode_, gainNode_, 0, 0);
        
        // 6. 启动音频上下文
        ctx_->resume();
        
        Logger::info("Audio nodes initialized successfully");
        return true;
    } catch (const std::exception& e) {
        Logger::error("Exception in initializeNodes: {}", e.what());
        return false;
    }
}

void Recorder::writeToFile() {
    if (pcmBuffer_.empty()) {
        Logger::warn("No audio data to write");
        return;
    }

    std::ofstream file(outputPath_, std::ios::binary);
    if (!file) {
        Logger::error("Failed to open output file: {}", outputPath_);
        return;
    }

    // 写入 WAV 头
    struct WAVHeader {
        char riff[4] = {'R', 'I', 'F', 'F'};
        uint32_t chunkSize;
        char wave[4] = {'W', 'A', 'V', 'E'};
        char fmt[4] = {'f', 'm', 't', ' '};
        uint32_t fmtChunkSize = 16;
        uint16_t audioFormat = 3;  // 3 表示浮点 PCM
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample = 32;  // 32 位浮点
        char data[4] = {'d', 'a', 't', 'a'};
        uint32_t dataChunkSize;
    } header;

    header.numChannels = channels_;
    header.sampleRate = sampleRate_;
    header.byteRate = sampleRate_ * channels_ * sizeof(float);
    header.blockAlign = channels_ * sizeof(float);
    header.dataChunkSize = pcmBuffer_.size() * sizeof(float);
    header.chunkSize = 36 + header.dataChunkSize;

    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    file.write(reinterpret_cast<const char*>(pcmBuffer_.data()), pcmBuffer_.size() * sizeof(float));

    if (!file) {
        Logger::error("Failed to write audio data to file");
        return;
    }

    Logger::info("Audio data written to file: {}", outputPath_);
    pcmBuffer_.clear();
} 