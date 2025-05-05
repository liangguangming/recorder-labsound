#include <LabSound/LabSound.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>

// 添加 ffmpeg 头文件
extern "C" {
#include <libavutil/avutil.h>
}

int main() {
    spdlog::info("Starting LabSound example...");

    // 测试 ffmpeg
    spdlog::info("FFmpeg version: {}", av_version_info());

    using namespace lab;
    
    try {
        // 获取音频设备列表
        spdlog::info("Getting audio device list...");
        auto devices = AudioDevice::MakeAudioDeviceList();
        for (const auto& device : devices) {
            spdlog::info("Device {}: {}", device.index, device.identifier);
            spdlog::info("  Output channels: {}", device.num_output_channels);
            spdlog::info("  Input channels: {}", device.num_input_channels);
            spdlog::info("  Default output: {}", device.is_default_output ? "yes" : "no");
            spdlog::info("  Default input: {}", device.is_default_input ? "yes" : "no");
            spdlog::info("  Nominal sample rate: {}", device.nominal_samplerate);
        }

        AudioDeviceInfo inputDeviceInfo,outputDeviceInfo;
        for (const auto& device : devices) {
            if (device.is_default_input) {
                inputDeviceInfo = device;
            }
            if (device.is_default_output) {
                outputDeviceInfo = device;
            }
        }

        // 获取默认输出设备
        auto defaultOutputDevice = AudioDevice::GetDefaultOutputAudioDeviceIndex();
        if (!defaultOutputDevice.valid) {
            spdlog::error("No default output device found");
            return 1;
        }

        // 创建一个非离线模式的音频上下文
        spdlog::info("Creating audio context...");
        std::unique_ptr<AudioContext> ctx = std::make_unique<AudioContext>(false);
        if (!ctx) {
            spdlog::error("Failed to create audio context");
            return 1;
        }

        // 配置音频流
        AudioStreamConfig outputConfig;
        outputConfig.device_index = defaultOutputDevice.index;  // 使用默认输出设备
        outputConfig.desired_channels = outputDeviceInfo.num_output_channels;  // 立体声
        outputConfig.desired_samplerate = outputDeviceInfo.nominal_samplerate;  // 标准采样率

        AudioStreamConfig inputConfig;
        inputConfig.device_index = inputDeviceInfo.index;  // 使用默认输入设备
        inputConfig.desired_channels = 0; // 禁用 input
        inputConfig.desired_samplerate = inputDeviceInfo.nominal_samplerate;  // 标准采样率

        // 创建并设置硬件设备节点
        spdlog::info("Creating hardware device node...");
        auto deviceNode = std::make_shared<AudioHardwareDeviceNode>(*ctx, outputConfig, inputConfig);
        if (!deviceNode) {
            spdlog::error("Failed to create hardware device node");
            return 1;
        }

        // 设置设备节点
        spdlog::info("Setting device node...");
        ctx->setDeviceNode(deviceNode);
        
        // 启动音频流
        spdlog::info("Starting audio stream...");
        deviceNode->start();
        
        // 确保上下文已初始化
        if (!ctx->isInitialized()) {
            spdlog::info("Initializing audio context...");
            ctx->lazyInitialize();
        }

        // resume() 返回 void，不需要检查返回值
        spdlog::info("Resuming audio context...");
        ctx->resume();
        spdlog::info("Audio context resumed.");

        // 创建音频节点
        spdlog::info("Creating audio nodes...");
        
        // 创建两个振荡器，产生不同频率的声音
        auto oscillator1 = std::make_shared<OscillatorNode>(*ctx);
        auto oscillator2 = std::make_shared<OscillatorNode>(*ctx);
        
        // 创建增益节点（用作混合器）
        auto gainNode = std::make_shared<GainNode>(*ctx);

        if (!oscillator1 || !oscillator2 || !gainNode) {
            spdlog::error("Failed to create audio nodes");
            return 1;
        }

        spdlog::info("Setting up audio nodes...");
        // 设置振荡器1：440Hz 正弦波
        oscillator1->setType(OscillatorType::SINE);
        oscillator1->frequency()->setValue(440.0f);
        
        // 设置振荡器2：880Hz 正弦波
        oscillator2->setType(OscillatorType::SINE);
        oscillator2->frequency()->setValue(880.0f);
        
        // 设置增益
        gainNode->gain()->setValue(0.2f);

        // 使用 AudioContext 的 connect 方法连接节点
        spdlog::info("Connecting audio nodes...");
        
        // 将两个振荡器连接到增益节点
        ctx->connect(gainNode, oscillator1);
        ctx->connect(gainNode, oscillator2);
        
        // 将增益节点连接到设备节点
        ctx->connect(deviceNode, gainNode);

        // 等待连接完成
        spdlog::info("Synchronizing connections...");
        ctx->synchronizeConnections();

        // 立即开始播放
        spdlog::info("Starting oscillators...");
        oscillator1->start(0.0f);
        oscillator2->start(0.0f);
        spdlog::info("Sine waves started.");

        std::this_thread::sleep_for(std::chrono::seconds(5));
        spdlog::info("Waiting for 5 seconds...");

        // 立即停止播放
        spdlog::info("Stopping oscillators...");
        oscillator1->stop(0.0f);
        oscillator2->stop(0.0f);
        spdlog::info("Sine waves stopped.");

        // 停止音频流
        spdlog::info("Stopping audio stream...");
        deviceNode->stop();

        // suspend() 返回 void，不需要检查返回值
        spdlog::info("Suspending audio context...");
        ctx->suspend();
        spdlog::info("Audio context suspended.");

    } catch (const std::exception& e) {
        spdlog::error("Exception caught: {}", e.what());
        return 1;
    } catch (...) {
        spdlog::error("Unknown exception caught");
        return 1;
    }

    spdlog::info("LabSound example finished.");
    return 0;
}