#include "rosewood/utils/logging.h"

#include <libgen.h>

#include <mutex>
#include <map>
#include <iostream>

using rosewood::utils::LogLevel;
using rosewood::utils::LogWriter;

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
