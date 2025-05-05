cmake_minimum_required(VERSION 3.10)
project(MyLabSoundProject)

# 打印 CMake 版本信息
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")

# 检查 labsound 目录是否存在
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/labsound")
    message(FATAL_ERROR "labsound directory not found at ${CMAKE_SOURCE_DIR}/third_party/labsound")
endif()

# 检查 spdlog 目录是否存在
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/spdlog")
    message(FATAL_ERROR "spdlog directory not found at ${CMAKE_SOURCE_DIR}/third_party/spdlog")
endif()

# 检查 ffmpeg 目录是否存在
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/ffmpeg/build")
    message(FATAL_ERROR "ffmpeg build directory not found at ${CMAKE_SOURCE_DIR}/third_party/ffmpeg/build. Please run ./third_party/build_ffmpeg.sh first")
endif()

# 添加编译选项来禁用 labsound 的日志和调试输出
add_definitions(-DLABSOUND_DISABLE_LOGGING)
add_definitions(-DLABSOUND_LOG_LEVEL=5)  # 设置日志级别为最高级别（FATAL）
add_definitions(-DNDEBUG)  # 禁用调试输出

# 添加 spdlog 作为本地子项目
add_subdirectory(third_party/spdlog)

# 添加 labsound 作为本地子项目
add_subdirectory(third_party/labsound)

# 设置 ffmpeg 的路径
set(FFMPEG_ROOT ${CMAKE_SOURCE_DIR}/third_party/ffmpeg/build)
set(FFMPEG_INCLUDE_DIR ${FFMPEG_ROOT}/include)
set(FFMPEG_LIB_DIR ${FFMPEG_ROOT}/lib)

# 添加 ffmpeg 头文件路径
include_directories(${FFMPEG_INCLUDE_DIR})

# 查找 ffmpeg 静态库
find_library(AVCODEC_LIB avcodec PATHS ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(AVFORMAT_LIB avformat PATHS ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(AVUTIL_LIB avutil PATHS ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)
find_library(SWRESAMPLE_LIB swresample PATHS ${FFMPEG_LIB_DIR} NO_DEFAULT_PATH)

if(NOT AVCODEC_LIB OR NOT AVFORMAT_LIB OR NOT AVUTIL_LIB OR NOT SWRESAMPLE_LIB)
    message(FATAL_ERROR "Failed to find ffmpeg libraries")
endif()

add_executable(MyLabSoundApp src/main.cpp)

# 链接 labsound 库和 spdlog
target_link_libraries(MyLabSoundApp PRIVATE 
    LabSound
    spdlog::spdlog
    ${CMAKE_DL_LIBS}  # 添加动态链接库支持
)

# 设置包含目录
target_include_directories(MyLabSoundApp PRIVATE 
    ${CMAKE_SOURCE_DIR}/third_party/labsound/include
    ${CMAKE_SOURCE_DIR}/third_party/labsound/src
    ${FFMPEG_INCLUDE_DIR}
)

# 设置编译器标志
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加必要的编译选项
if(APPLE)
    target_link_libraries(MyLabSoundApp PRIVATE 
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework Accelerate"
    )
endif()

# 链接 ffmpeg 静态库
target_link_libraries(MyLabSoundApp PRIVATE 
    ${AVCODEC_LIB}
    ${AVFORMAT_LIB}
    ${AVUTIL_LIB}
    ${SWRESAMPLE_LIB}
)

# 打印最终配置信息
message(STATUS "Final include directories for MyLabSoundApp:")
get_target_property(INCLUDES MyLabSoundApp INCLUDE_DIRECTORIES)
foreach(INCLUDE ${INCLUDES})
    message(STATUS "  ${INCLUDE}")
endforeach()

message(STATUS "Final link libraries for MyLabSoundApp:")
get_target_property(LIBS MyLabSoundApp LINK_LIBRARIES)
foreach(LIB ${LIBS})
    message(STATUS "  ${LIB}")
endforeach()