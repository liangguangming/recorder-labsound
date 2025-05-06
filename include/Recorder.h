#pragma once

#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <LabSound/LabSound.h>

class Recorder {
public:
    enum class State {
        Stopped,
        Recording,
        Paused
    };

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
    State state() const;

private:
    bool initializeNodes();
    void writeToFile();

    State state_;
    std::string outputPath_;
    std::vector<float> pcmBuffer_; // 录制的 PCM 数据
    std::mutex bufferMutex_;
    
    // 音频参数
    int sampleRate_;
    int channels_;
    
    // LabSound 相关
    std::shared_ptr<lab::AudioContext> ctx_;
    std::shared_ptr<lab::AudioHardwareInputNode> micNode_;
    std::shared_ptr<lab::GainNode> gainNode_;
    std::shared_ptr<lab::FunctionNode> functionNode_;

    // 禁止拷贝
    Recorder(const Recorder&) = delete;
    Recorder& operator=(const Recorder&) = delete;
}; 