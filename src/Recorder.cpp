#include "Recorder.h"
#include "logger/Logger.h"
#include <fstream>

Recorder::Recorder() 
    : state_(State::Stopped)
{
    Logger::info("Recorder constructed");
}

Recorder::~Recorder() {
    if (state_ != State::Stopped) {
        stop();
    }
    Logger::info("Recorder destroyed");
}

void Recorder::setOutputPath(const std::string& path) {
    outputPath_ = path;
    Logger::info("Output path set to: {}", path);
}

bool Recorder::start() {
    if (state_ != State::Stopped) {
        Logger::warn("Cannot start: recorder is not stopped");
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
        Logger::warn("Cannot pause: recorder is not recording");
        return;
    }

    state_ = State::Paused;
    Logger::info("Recording paused");
}

void Recorder::resume() {
    if (state_ != State::Paused) {
        Logger::warn("Cannot resume: recorder is not paused");
        return;
    }

    state_ = State::Recording;
    Logger::info("Recording resumed");
}

void Recorder::stop() {
    if (state_ == State::Stopped) {
        Logger::warn("Recorder is already stopped");
        return;
    }

    state_ = State::Stopped;
    writeToFile();
    Logger::info("Recording stopped and saved");
}

Recorder::State Recorder::state() const {
    return state_;
}

bool Recorder::initializeNodes() {
    // TODO: 初始化 LabSound 节点
    // 1. 创建 AudioContext
    // 2. 创建并配置 micNode_
    // 3. 创建并配置 sysNode_
    // 4. 创建并配置 gainNode_
    // 5. 创建并配置 recordNode_
    // 6. 连接节点
    return true;
}

void Recorder::writeToFile() {
    if (pcmBuffer_.empty()) {
        Logger::warn("No audio data to write");
        return;
    }

    std::ofstream file(outputPath_, std::ios::binary);
    if (!file) {
        Logger::error("Failed to open file for writing: {}", outputPath_);
        return;
    }

    // TODO: 写入 PCM 数据
    // 1. 写入采样率
    // 2. 写入通道数
    // 3. 写入数据大小
    // 4. 写入 PCM 数据

    Logger::info("Saved {} samples to {}", pcmBuffer_.size(), outputPath_);
} 