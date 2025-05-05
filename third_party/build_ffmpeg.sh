#!/bin/bash

set -e

# 进入 ffmpeg 子模块目录
cd "$(dirname "$0")/ffmpeg"

# 创建输出目录
BUILD_DIR=$(pwd)/build
mkdir -p "$BUILD_DIR"

# 获取 CPU 核心数（macOS 兼容）
if [[ "$OSTYPE" == "darwin"* ]]; then
    CPU_CORES=$(sysctl -n hw.ncpu)
else
    CPU_CORES=$(nproc)
fi

# 设置 pkg-config 路径
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"

# 设置 lame 和 vorbis 的路径
LAME_PREFIX="/opt/homebrew"
VORBIS_PREFIX="/opt/homebrew"

# 配置编译参数
./configure \
    --prefix="$BUILD_DIR" \
    --disable-shared \
    --enable-static \
    --disable-programs \
    --disable-doc \
    --disable-debug \
    --disable-everything \
    --enable-encoder=libmp3lame \
    --enable-encoder=libvorbis \
    --enable-muxer=mp3 \
    --enable-muxer=ogg \
    --enable-protocol=file \
    --enable-avformat \
    --enable-avcodec \
    --enable-avutil \
    --enable-swresample \
    --enable-pic \
    --disable-network \
    --disable-encoders \
    --enable-encoder=libmp3lame \
    --enable-encoder=libvorbis \
    --disable-decoders \
    --disable-muxers \
    --enable-muxer=mp3 \
    --enable-muxer=ogg \
    --disable-demuxers \
    --disable-parsers \
    --disable-bsfs \
    --disable-protocols \
    --enable-protocol=file \
    --disable-indevs \
    --disable-outdevs \
    --disable-filters \
    --enable-libmp3lame \
    --enable-libvorbis \
    --pkg-config-flags="--static" \
    --extra-cflags="-I$LAME_PREFIX/include -I$VORBIS_PREFIX/include" \
    --extra-ldflags="-L$LAME_PREFIX/lib -L$VORBIS_PREFIX/lib"

# 编译并安装
make -j${CPU_CORES}
make install

echo "ffmpeg 静态库已编译并安装到 $BUILD_DIR" 