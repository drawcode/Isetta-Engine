/*
 * Copyright (c) 2018 Isetta
 */
#pragma once

// TODO put in the Filesystem Module
#define __FILENAME__ \
  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#include <afx.h>
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <string>
#include "Core/Color.h"
#include "Core/IModule.h"
#include "Core/Math/Math.h"

namespace Isetta {

namespace Debug {
enum Verbosity {
  Off = 0,
  Error = (1u << 1),
  Warning = (1u << 2),
  Info = (1u << 3),
  All = ~0
};

static inline const std::string ToString(Debug::Verbosity v) {
  switch (v) {
    case Verbosity::Off:
      return "Off";
    case Verbosity::Error:
      return "Error";
    case Verbosity::Warning:
      return "Warning";
    case Verbosity::Info:
      return "Info";
    default:
      return "Unknown";
  }
}

enum Channel {
  General = (1u << 0),
  Memory = (1u << 1),
  Networking = (1u << 2),
  Graphics = (1u << 3),
  Physics = (1u << 4),
  Gameplay = (1u << 5),
  Sound = (1u << 6),
};

static inline const std::string ToString(Debug::Channel c) {
  switch (c) {
    case Debug::General:
      return "General";
    case Debug::Memory:
      return "Memory";
    case Debug::Networking:
      return "Networking";
    case Debug::Graphics:
      return "Graphics";
    case Debug::Physics:
      return "Physics";
    case Debug::Gameplay:
      return "Gameplay";
    case Debug::Sound:
      return "Sound";
    default:
      return "Unknown";
  }
}
}  // namespace Debug

class Logger {
 public:
  static uint8_t gVerbosityMask;
  static uint32_t gChannelMask;
  static bool gAlwaysFlush;

  static void SetLoggerFiles(std::ofstream* inEngineStream,
                             std::ofstream* inChannelStream = nullptr);
  static void SetLoggerChannelFile(std::ofstream* inChannelStream);

  static int PrintF(const std::string format, ...);
  static int PrintF(const Debug::Channel channel, const std::string format,
                    ...);
  static int PrintF(const Debug::Channel channel,
                    const Debug::Verbosity verbosity, const std::string format,
                    ...);

  static void Log(const Debug::Channel channel, const std::string format, ...);
  static void LogWarning(const Debug::Channel channel, const std::string format,
                         ...);
  static void LogError(const Debug::Channel channel, const std::string format,
                       ...);

  // static void DrawLine(Math::Vector3 start, Math::Vector3 end,
  //                     Color color = Color::white, float duration = 0.0f,
  //                     bool depthTest = true);
  // static void DrawRay(Math::Vector3 start, Math::Vector3 dir,
  //                    Color color = Color::white, float duration = 0.0f,
  //                    bool depthTest = true);

 protected:
  static int VDebugPrintF(const Debug::Channel channel,
                          const Debug::Verbosity verbosity,
                          const std::string format, va_list argList);

 private:
  static std::ofstream* engineStream;
  static std::ofstream* channelStream;
  static bool CheckChannelMask(const Debug::Channel channel);
  static bool CheckVerbosity(const Debug::Verbosity verbosity);

  static Debug::Verbosity defaultVerbosity;
  static Debug::Channel defaultChannel;
};
}  // namespace Isetta
