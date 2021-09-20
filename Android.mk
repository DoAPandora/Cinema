# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)
TARGET_ARCH_ABI := $(APP_ABI)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Creating prebuilt for dependency: modloader - version: 1.2.3
include $(CLEAR_VARS)
LOCAL_MODULE := modloader
LOCAL_EXPORT_C_INCLUDES := extern/modloader
LOCAL_SRC_FILES := extern/libmodloader.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: beatsaber-hook - version: 2.3.1
include $(CLEAR_VARS)
LOCAL_MODULE := beatsaber-hook_2_3_1
LOCAL_EXPORT_C_INCLUDES := extern/beatsaber-hook
LOCAL_SRC_FILES := extern/libbeatsaber-hook_2_3_1.so
LOCAL_CPP_FEATURES += exceptions
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: codegen - version: 0.14.0
include $(CLEAR_VARS)
LOCAL_MODULE := codegen
LOCAL_EXPORT_C_INCLUDES := extern/codegen
LOCAL_SRC_FILES := extern/libcodegen.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: questui - version: 0.11.1
include $(CLEAR_VARS)
LOCAL_MODULE := questui
LOCAL_EXPORT_C_INCLUDES := extern/questui
LOCAL_SRC_FILES := extern/libquestui.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: custom-types - version: 0.12.7
include $(CLEAR_VARS)
LOCAL_MODULE := custom-types
LOCAL_EXPORT_C_INCLUDES := extern/custom-types
LOCAL_SRC_FILES := extern/libcustom-types.so
include $(PREBUILT_SHARED_LIBRARY)

#FFmpeg Libraries:

# Creating prebuilt for dependency: ffmpeg - avcodec
include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := ffmpeg/libavcodec.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - avdevice
include $(CLEAR_VARS)
LOCAL_MODULE := avdevice
LOCAL_SRC_FILES := ffmpeg/libavdevice.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - avfilter
include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := ffmpeg/libavfilter.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - avformat
include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := ffmpeg/libavformat.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - avutil
include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := ffmpeg/libavutil.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - swresample
include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := ffmpeg/libswresample.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - swscale
include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := ffmpeg/libswscale.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - mobileffmpeg
include $(CLEAR_VARS)
LOCAL_MODULE := ffmpegkit
LOCAL_SRC_FILES := ffmpeg/libffmpegkit.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: ffmpeg - mobileffmpegabidetect
include $(CLEAR_VARS)
LOCAL_MODULE := ffmpegkitabidetect
LOCAL_SRC_FILES := ffmpeg/libffmpegkit_abidetect.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := cinema
LOCAL_SRC_FILES += $(call rwildcard,src/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.c)
LOCAL_SHARED_LIBRARIES += modloader
LOCAL_SHARED_LIBRARIES += beatsaber-hook_2_3_1
LOCAL_SHARED_LIBRARIES += codegen
LOCAL_SHARED_LIBRARIES += questui
LOCAL_SHARED_LIBRARIES += custom-types

LOCAL_SHARED_LIBRARIES += avcodec
LOCAL_SHARED_LIBRARIES += avformat
LOCAL_SHARED_LIBRARIES += avutil
LOCAL_SHARED_LIBRARIES += swscale
LOCAL_SHARED_LIBRARIES += ffmpegkit
LOCAL_SHARED_LIBRARIES += ffmpegkitabidetect

LOCAL_CFLAGS += -I'extern/libil2cpp/il2cpp/libil2cpp' -DID='"cinema"' -DVERSION='"0.1.0"' -I'./shared' -I'./extern' -isystem'extern/codegen/include' ./ffmpeg
LOCAL_LDLIBS += -llog -lGLESv3 -lGLESv2
LOCAL_CPPFLAGS += -std=c++2a -O3 -frtti
LOCAL_C_INCLUDES += ./include ./src ./ffmpeg ./shared
include $(BUILD_SHARED_LIBRARY)