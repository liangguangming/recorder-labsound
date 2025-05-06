#include "logger/Logger.h"
#include <iostream>
#include <cstdlib>
#include <string>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& logFile, spdlog::level::level_enum level) {
    try {
        // 创建日志目录
        std::string cmd = "mkdir -p " + logFile.substr(0, logFile.find_last_of('/'));
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Failed to create log directory" << std::endl;
        }
        
        // 创建控制台输出
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(level);
        
        // 创建文件输出
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFile, 1024*1024*5, 3);
        file_sink->set_level(level);
        
        // 创建多输出日志器
        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        logger_ = std::make_shared<spdlog::logger>("recorder", sinks.begin(), sinks.end());
        logger_->set_level(level);
        
        // 设置日志格式
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
        
        RECORDER_LOG_INFO("Logger initialized successfully");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
}

void Logger::setLevel(spdlog::level::level_enum level) {
    if (logger_) {
        logger_->set_level(level);
        RECORDER_LOG_INFO("Log level set to: {}", spdlog::level::to_string_view(level));
    }
} 