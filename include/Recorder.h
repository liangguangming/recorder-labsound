#pragma once

#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <LabSound/LabSound.h>

class Recorder {
public:
    Recorder();
    ~Recorder();

    // 设置输出音频文件路径（如 data/output.pcm）
    void setOutputPath(const std::string& path);

    // 启动录制
    bool start();

    // 暂停录制
    void pause();

    // 恢复录制
    void resume();

    // 停止录制并保存文件
    void stop();

    // 录制状态
    enum class State {
        Stopped,
        Recording,
        Paused
    };
    State state() const;

private:
    std::string outputPath_;
    std::vector<float> pcmBuffer_; // 录制的 PCM 数据
    std::atomic<State> state_;
    std::mutex bufferMutex_;

    // LabSound 相关
    std::unique_ptr<lab::AudioContext> ctx_;
    std::shared_ptr<lab::AudioHardwareInputNode> micNode_;
    std::shared_ptr<lab::FunctionNode> sysNode_;
    std::shared_ptr<lab::GainNode> gainNode_;
    std::shared_ptr<lab::FunctionNode> recordNode_;

    // 内部方法：写入文件
    void writeToFile();

    // 内部方法：初始化音频节点
    bool initializeNodes();

    // 禁止拷贝
    Recorder(const Recorder&) = delete;
    Recorder& operator=(const Recorder&) = delete;
}; 