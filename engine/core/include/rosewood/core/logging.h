#ifndef __ROSEWOOD_CORE_LOGGING_H__
#define __ROSEWOOD_CORE_LOGGING_H__

#define STMT rosewood::core::LogLevel::kStatementLevel
#define DBG rosewood::core::LogLevel::kDebugLevel
#define INFO rosewood::core::LogLevel::kInfoLevel
#define WARNING rosewood::core::LogLevel::kWarningLevel
#define ERROR rosewood::core::LogLevel::kErrorLevel

#define LOG(level, ...) rosewood::core::LogWriter(level, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_STMT(stmt) LOG(STMT, #stmt); stmt

#include <string>
#include <sstream>

namespace rosewood { namespace core {

		enum class LogLevel {
			kStatementLevel = 10,
			kDebugLevel = 20,
			kInfoLevel = 30,
			kWarningLevel = 40,
			kErrorLevel = 50
		};

		class LogWriter {
		public:
			LogWriter(LogLevel level, const std::string &file, int line, const std::string &message = "");

			~LogWriter();

			template<typename T> LogWriter &operator<<(const T &rhs) {
				if (_enabled) _stream << rhs;
				return *this;
			}

			static LogLevel gLogLevel;

		private:
			bool _enabled;
			LogLevel _level;

			std::string _file;
			int _line;

			std::ostringstream _stream;

		};

} }

#endif
