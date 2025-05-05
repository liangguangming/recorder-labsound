cmake_minimum_required(VERSION 3.10)
project(MyLabSoundProject)

# 打印 CMake 版本信息
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 添加编译选项来禁用 labsound 的日志
add_definitions(-DLABSOUND_DISABLE_LOGGING)

# 添加 spdlog 作为本地子项目
add_subdirectory(third_party/spdlog)

# 添加 spdlog 作为子项目（注释掉，保留以便后续回退）
# include(FetchContent)
# FetchContent_Declare(
#     spdlog
#     GIT_REPOSITORY https://github.com/gabime/spdlog.git
#     GIT_TAG v1.14.1
# )
# FetchContent_MakeAvailable(spdlog)

# 添加 labsound 作为本地子项目
add_subdirectory(third_party/labsound)

# 添加 labsound 作为子项目（注释掉，保留以便后续回退）
# FetchContent_Declare(
#     labsound
#     GIT_REPOSITORY https://github.com/LabSound/LabSound.git
#     GIT_TAG v1.2.0
# )
# FetchContent_MakeAvailable(labsound)

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
    # ${labsound_SOURCE_DIR}/include
    # ${labsound_BINARY_DIR}/include
    # ${labsound_SOURCE_DIR}/src
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