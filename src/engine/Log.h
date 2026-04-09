#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Engine {

enum class LogLevel {
    Info,
    Warn,
    Error
};

inline std::string GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm buf;
#ifdef _WIN32
    localtime_s(&buf, &in_time_t);   // thread-safe on Windows
#else
    buf = *std::localtime(&in_time_t); // Linux/macOS
#endif

    std::stringstream ss;
    ss << std::put_time(&buf, "%H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}


inline void Log(LogLevel level, const std::string& message) {
    std::string prefix;
    switch (level) {
        case LogLevel::Info:  prefix = "[INFO] "; break;
        case LogLevel::Warn:  prefix = "[WARN] "; break;
        case LogLevel::Error: prefix = "[ERROR] "; break;
    }

    std::cout << GetTimestamp() << " " << prefix << message << std::endl;
}

#define ENGINE_LOG_INFO(msg) Engine::Log(Engine::LogLevel::Info, msg)
#define ENGINE_LOG_WARN(msg) Engine::Log(Engine::LogLevel::Warn, msg)
#define ENGINE_LOG_ERROR(msg) Engine::Log(Engine::LogLevel::Error, msg)

} // namespace Engine
