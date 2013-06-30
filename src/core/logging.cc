#include "rosewood/core/logging.h"

#include <mutex>
#include <map>
#include <iostream>

using rosewood::core::LogLevel;
using rosewood::core::LogWriter;

static std::string basename(const std::string &path) {
    size_t pos = path.rfind('/');

    if (pos == std::string::npos) {
        return path;
    }

    return path.substr(pos);
}

static const std::map<LogLevel, std::string> kLogLevelNames {
	{ LogLevel::kDebugLevel,   "DEBUG" },
	{ LogLevel::kInfoLevel,    "INFO" },
	{ LogLevel::kWarningLevel, "WARNING" },
	{ LogLevel::kErrorLevel,   "ERROR" },
};

static std::mutex gWriterMutex;

LogLevel LogWriter::gLogLevel = LogLevel::kDebugLevel;

LogWriter::LogWriter(LogLevel level, const std::string &file, int line, const std::string &message)
	: _enabled(level >= gLogLevel)
	, _level(level)
	, _file(file)
	, _line(line)
	, _stream(message)
{ }

LogWriter::~LogWriter() {
	if (!_enabled) return;

	std::lock_guard<std::mutex> lock(gWriterMutex);
	std::cerr << "[" << kLogLevelNames.at(_level) << " " << basename(_file.c_str()) << ":" << _line << "] " << _stream.str() << std::endl;
}
