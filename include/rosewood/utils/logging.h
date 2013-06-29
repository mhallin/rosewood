#ifndef __ROSEWOOD_UTILS_LOGGING_H__
#define __ROSEWOOD_UTILS_LOGGING_H__

#define DEBUG rosewood::utils::LogLevel::kDebugLevel
#define INFO rosewood::utils::LogLevel::kInfoLevel
#define WARNING rosewood::utils::LogLevel::kWarningLevel
#define ERROR rosewood::utils::LogLevel::kErrorLevel

#define LOG(level, ...) rosewood::utils::LogWriter(level, __FILE__, __LINE__, ##__VA_ARGS__)

#include <string>
#include <sstream>

namespace rosewood { namespace utils {

		enum class LogLevel {
			kDebugLevel = 10,
			kInfoLevel = 20,
			kWarningLevel = 30,
			kErrorLevel = 40
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
