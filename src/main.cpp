#include "Recorder.h"
#include "logger/Logger.h"
#include <spdlog/spdlog.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // 初始化日志系统
    Logger::getInstance().init("logs/recorder.log", spdlog::level::debug);
    
    Logger::info("Application started");
    
    // 创建录音器实例
    Recorder recorder;
    
    // 设置输出路径
    recorder.setOutputPath("output.pcm");
    
    // 开始录音
    if (!recorder.start()) {
        Logger::error("Failed to start recording");
        return 1;
    }
    
    // 录音 5 秒
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    // 停止录音
    recorder.stop();
    
    Logger::info("Recording completed");
    return 0;
}