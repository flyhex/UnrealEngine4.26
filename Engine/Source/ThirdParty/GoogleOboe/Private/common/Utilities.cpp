/*
 * Copyright 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "oboe/Utilities.h"
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#ifdef __ANDROID__
#include <sys/system_properties.h>
#endif

#include <oboe/AudioStream.h>
#include "oboe/Definitions.h"


namespace oboe {

constexpr float kScaleI16ToFloat = (1.0f / 32768.0f);

void convertFloatToPcm16(const float *source, int16_t *destination, int32_t numSamples) {
    for (int i = 0; i < numSamples; i++) {
        float fval = source[i];
        fval += 1.0; // to avoid discontinuity at 0.0 caused by truncation
        fval *= 32768.0f;
        auto sample = static_cast<int32_t>(fval);
        // clip to 16-bit range
        if (sample < 0) sample = 0;
        else if (sample > 0x0FFFF) sample = 0x0FFFF;
        sample -= 32768; // center at zero
        destination[i] = static_cast<int16_t>(sample);
    }
}

void convertPcm16ToFloat(const int16_t *source, float *destination, int32_t numSamples) {
    for (int i = 0; i < numSamples; i++) {
        destination[i] = source[i] * kScaleI16ToFloat;
    }
}

int32_t convertFormatToSizeInBytes(AudioFormat format) {
    int32_t size = 0;
    switch (format) {
        case AudioFormat::I16:
            size = sizeof(int16_t);
            break;
        case AudioFormat::Float:
            size = sizeof(float);
            break;
        default:
            break;
    }
    return size;
}

template<>
const char *convertToText<Result>(Result returnCode) {
    switch (returnCode) {
        case Result::OK:                    return "OK";
        case Result::ErrorDisconnected:     return "ErrorDisconnected";
        case Result::ErrorIllegalArgument:  return "ErrorIllegalArgument";
        case Result::ErrorInternal:         return "ErrorInternal";
        case Result::ErrorInvalidState:     return "ErrorInvalidState";
        case Result::ErrorInvalidHandle:    return "ErrorInvalidHandle";
        case Result::ErrorUnimplemented:    return "ErrorUnimplemented";
        case Result::ErrorUnavailable:      return "ErrorUnavailable";
        case Result::ErrorNoFreeHandles:    return "ErrorNoFreeHandles";
        case Result::ErrorNoMemory:         return "ErrorNoMemory";
        case Result::ErrorNull:             return "ErrorNull";
        case Result::ErrorTimeout:          return "ErrorTimeout";
        case Result::ErrorWouldBlock:       return "ErrorWouldBlock";
        case Result::ErrorInvalidFormat:    return "ErrorInvalidFormat";
        case Result::ErrorOutOfRange:       return "ErrorOutOfRange";
        case Result::ErrorNoService:        return "ErrorNoService";
        case Result::ErrorInvalidRate:      return "ErrorInvalidRate";
        case Result::ErrorClosed:           return "ErrorClosed";
        default:                            return "Unrecognized result";
    }
}

template<>
const char *convertToText<AudioFormat>(AudioFormat format) {
    switch (format) {
        case AudioFormat::Invalid:      return "Invalid";
        case AudioFormat::Unspecified:  return "Unspecified";
        case AudioFormat::I16:          return "I16";
        case AudioFormat::Float:        return "Float";
        default:                        return "Unrecognized format";
    }
}

template<>
const char *convertToText<PerformanceMode>(PerformanceMode mode) {
    switch (mode) {
        case PerformanceMode::LowLatency:   return "LowLatency";
        case PerformanceMode::None:         return "None";
        case PerformanceMode::PowerSaving:  return "PowerSaving";
        default:                            return "Unrecognized performance mode";
    }
}

template<>
const char *convertToText<SharingMode>(SharingMode mode) {
    switch (mode) {
        case SharingMode::Exclusive:    return "Exclusive";
        case SharingMode::Shared:       return "Shared";
        default:                        return "Unrecognized sharing mode";
    }
}

template<>
const char *convertToText<DataCallbackResult>(DataCallbackResult result) {
    switch (result) {
        case DataCallbackResult::Continue:  return "Continue";
        case DataCallbackResult::Stop:      return "Stop";
        default:                            return "Unrecognized data callback result";
    }
}

template<>
const char *convertToText<Direction>(Direction direction) {
    switch (direction) {
        case Direction::Input:  return "Input";
        case Direction::Output: return "Output";
        default:                return "Unrecognized direction";
    }
}

template<>
const char *convertToText<StreamState>(StreamState state) {
    switch (state) {
        case StreamState::Closed:           return "Closed";
        case StreamState::Closing:          return "Closing";
        case StreamState::Disconnected:     return "Disconnected";
        case StreamState::Flushed:          return "Flushed";
        case StreamState::Flushing:         return "Flushing";
        case StreamState::Open:             return "Open";
        case StreamState::Paused:           return "Paused";
        case StreamState::Pausing:          return "Pausing";
        case StreamState::Started:          return "Started";
        case StreamState::Starting:         return "Starting";
        case StreamState::Stopped:          return "Stopped";
        case StreamState::Stopping:         return "Stopping";
        case StreamState::Uninitialized:    return "Uninitialized";
        case StreamState::Unknown:          return "Unknown";
        default:                            return "Unrecognized stream state";
    }
}

template<>
const char *convertToText<AudioApi >(AudioApi audioApi) {

    switch (audioApi) {
        case AudioApi::Unspecified: return "Unspecified";
        case AudioApi::OpenSLES:    return "OpenSLES";
        case AudioApi::AAudio:      return "AAudio";
        default:                    return "Unrecognised audio API";
    }
}

template<>
const char* convertToText<AudioStream*>(AudioStream* stream) {
    static std::string streamText;
    std::stringstream s;

    s<<"StreamID: "<< static_cast<void*>(stream)<<std::endl
     <<"DeviceId: "<<stream->getDeviceId()<<std::endl
     <<"Direction: "<<oboe::convertToText(stream->getDirection())<<std::endl
     <<"API type: "<<oboe::convertToText(stream->getAudioApi())<<std::endl
     <<"BufferCapacity: "<<stream->getBufferCapacityInFrames()<<std::endl
     <<"BufferSize: "<<stream->getBufferSizeInFrames()<<std::endl
     <<"FramesPerBurst: "<< stream->getFramesPerBurst()<<std::endl
     <<"FramesPerCallback: "<<stream->getFramesPerCallback()<<std::endl
     <<"SampleRate: "<<stream->getSampleRate()<<std::endl
     <<"ChannelCount: "<<stream->getChannelCount()<<std::endl
     <<"Format: "<<oboe::convertToText(stream->getFormat())<<std::endl
     <<"SharingMode: "<<oboe::convertToText(stream->getSharingMode())<<std::endl
     <<"PerformanceMode: "<<oboe::convertToText(stream->getPerformanceMode())
     <<std::endl
     <<"CurrentState: "<<oboe::convertToText(stream->getState())<<std::endl
     <<"XRunCount: "<<stream->getXRunCount()<<std::endl
     <<"FramesRead: "<<stream->getFramesRead()<<std::endl
     <<"FramesWritten: "<<stream->getFramesWritten()<<std::endl;

    streamText = s.str();
    return streamText.c_str();
}

int getSdkVersion() {
#ifdef __ANDROID__
    static int sCachedSdkVersion = -1;
    if (sCachedSdkVersion == -1) {
        char sdk[PROP_VALUE_MAX] = {0};
        if (__system_property_get("ro.build.version.sdk", sdk) != 0) {
            sCachedSdkVersion = atoi(sdk);
        }
    }
    return sCachedSdkVersion;
#endif
    return -1;
}

}// namespace oboe
