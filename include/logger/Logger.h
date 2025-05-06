#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <memory>
#include <string>

class Logger {
public:
    static Logger& getInstance();
    
    // 初始化日志系统
    void init(const std::string& logFile = "recorder.log", 
              spdlog::level::level_enum level = spdlog::level::debug);
    
    // 获取日志器
    std::shared_ptr<spdlog::logger> getLogger() { return logger_; }
    
    // 设置日志级别
    void setLevel(spdlog::level::level_enum level);
    
    // 静态日志方法
    template<typename... Args>
    static void trace(const char* fmt, const Args&... args) {
        getInstance().getLogger()->trace(fmt, args...);
    }
    
    template<typename... Args>
    static void debug(const char* fmt, const Args&... args) {
        getInstance().getLogger()->debug(fmt, args...);
    }
    
    template<typename... Args>
    static void info(const char* fmt, const Args&... args) {
        getInstance().getLogger()->info(fmt, args...);
    }
    
    template<typename... Args>
    static void warn(const char* fmt, const Args&... args) {
        getInstance().getLogger()->warn(fmt, args...);
    }
    
    template<typename... Args>
    static void error(const char* fmt, const Args&... args) {
        getInstance().getLogger()->error(fmt, args...);
    }
    
    template<typename... Args>
    static void critical(const char* fmt, const Args&... args) {
        getInstance().getLogger()->critical(fmt, args...);
    }
    
    // 禁用拷贝和赋值
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    ~Logger() = default;
    
    std::shared_ptr<spdlog::logger> logger_;
};

// 便捷宏定义
#define RECORDER_LOG_TRACE(...) SPDLOG_LOGGER_TRACE(Logger::getInstance().getLogger(), __VA_ARGS__)
#define RECORDER_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(Logger::getInstance().getLogger(), __VA_ARGS__)
#define RECORDER_LOG_INFO(...) SPDLOG_LOGGER_INFO(Logger::getInstance().getLogger(), __VA_ARGS__)
#define RECORDER_LOG_WARN(...) SPDLOG_LOGGER_WARN(Logger::getInstance().getLogger(), __VA_ARGS__)
#define RECORDER_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(Logger::getInstance().getLogger(), __VA_ARGS__)
#define RECORDER_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Logger::getInstance().getLogger(), __VA_ARGS__) 