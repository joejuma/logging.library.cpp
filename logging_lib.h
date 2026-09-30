#pragma once
/*
	# Logging (Library)
	By Joseph Juma

	## Version
	v1.2.0

	## About
	A single-header C++14 compatible logging library.

	## To Do
	-	Implement a static-sized circular buffer structure.
	-	Bring in file functions library into here for file-logging.
	-	Implement file-logger.
	-	Create statically sized logs for stack allocation rather than heap
		allocation on everything(?)

	## Copyright
	Copyright 2024-2026, Joseph Juma. All rights reserved.
*/
#ifndef LOGGING_LIBRARY__H
#define LOGGING_LIBRARY__H
/* Deps */
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

/* Macros */
// Pointer Deletion Macros
#ifndef SAFE_DELETE_POINTER
#define SAFE_DELETE_POINTER(ptr) if(ptr != 0)\
{\
	delete ptr;\
	ptr = 0;\
}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(arr) if(arr != 0)\
{\
	delete[] arr;\
	arr = 0;\
}
#endif

namespace lg
{
	/* Constants */

	// Version Constants
	static const uint8_t	LOGGING_LIBRARY_VERSION_MAJOR = 1;
	static const uint8_t	LOGGING_LIBRARY_VERSION_MINOR = 2;
	static const uint16_t	LOGGING_LIBRARY_VERSION_PATCH = 0;

	/* Functions */

	// Versioning Functions
	inline constexpr const uint8_t get_logging_library_version_major()
	{
		return LOGGING_LIBRARY_VERSION_MAJOR;
	};
	inline constexpr const uint8_t get_logging_library_version_minor()
	{
		return LOGGING_LIBRARY_VERSION_MINOR;
	};
	inline constexpr const uint16_t get_logging_library_version_patch()
	{
		return LOGGING_LIBRARY_VERSION_PATCH;
	};

	inline constexpr const uint32_t get_logging_library_version()
	{
		uint32_t version = 0x00000000;
		version ^= get_logging_library_version_major();
		version = (version << 8) ^ get_logging_library_version_minor();
		version = (version << 8) ^ get_logging_library_version_patch();

		return version;
	};

	/* Structures */

#ifndef CIRCULAR_BUFFER_STRUCT__H
#define CIRCULAR_BUFFER_STRUCT__H
// Circular Buffer Structure
// @placeholder.
#endif

// Log Message Structures
	struct LogMessage
	{
		/* Static Elements */
		// Logging Message Level Constants
		static const uint8_t MESSAGE_LEVEL_MESSAGE = 0x00;
		static const uint8_t MESSAGE_LEVEL_WARNING = 0x01;
		static const uint8_t MESSAGE_LEVEL_ERROR = 0x02;

		/* Elements */
		std::string message;
		std::string functionName;
		uint8_t level;

		/* Static Methods */
		static std::string getSeverityString(const uint8_t level)
		{
			switch (level)
			{
			case MESSAGE_LEVEL_MESSAGE:
				return "Message";
				break;
			case MESSAGE_LEVEL_WARNING:
				return "Warning";
				break;
			case MESSAGE_LEVEL_ERROR:
				return "Error";
				break;
			default:
				return "Message";
				break;
			};
		};

		/* Methods */

		// Constructors & Destructor
		LogMessage(const std::string& _msg = "", const std::string& _fn = "", const uint8_t _lv = 0) : message(_msg), functionName(_fn), level(_lv)
		{};
		LogMessage(std::string&& _msg, std::string&& _fn, const uint8_t _lv = 0) : message(std::move(_msg)), functionName(std::move(_fn)), level(_lv)
		{};
		LogMessage(const LogMessage& src)
		{
			this->message = src.message;
			this->functionName = (src.functionName != "") ? src.functionName : "";
			this->level = src.level;
		};
		~LogMessage()
		{
			this->message.clear();
			this->functionName.clear();
			this->level = 0;
		};

		// Serialization Methods
		inline std::string toCSV() const
		{
			return ("" +
				("\"" + getSeverityString(this->level) + "\",") +
				("\"" + this->message + "\",") +
				("\"" + this->functionName + "\"") +
				"\n");
		};
		inline std::string toJSON() const
		{
			return ("{" +
				("\"level\":\"" + getSeverityString(this->level) + "\",") +
				("\"message\":\"" + this->message + "\",") +
				("\"functionName\":\"" + this->functionName + "\"") +
				"}");
		};
		inline std::string toXML() const
		{
			return ("<LogMessage>" + 
				("<Level>" + getSeverityString(this->level) + "</Level>") +
				("<Message>" + this->message + "</Message>") +
				("<FunctionName>" + this->functionName + "< / FunctionName>") + 
				"</LogMessage>");
		};
	};

	// Logger Structures
	struct Logger
	{
		/*
			# Logger (struct)

			## About
			A log managing structure. Used as the central logging mechanism, and
			repository. This is a base-class implementation, which should be
			derived from for more specialized use cases.

			## To Do
			-	Improve the implementation here to be more modular. A circ-buffer
				logger isn't feasible from this base-class as-is. So a breakout
				into a more sparse pure-virtual base-class and implementation of
				this logger as just one variant would be a stronger design.

			## Notes
			-	I've swapped the log-level checks to runtime from compile-time as
				it gives us multiple benefits for a small runtime hit. Though
				given this might be called a large number of very frequent times
				it may need reversion. So I made this note as a reminder if you
				are running into logging slowing down your app.
		*/

		/* Static Elements */

		// Logging Behavior Constants
		static const uint8_t LOG_LEVEL_INVALID = 0x00;
		static const uint8_t LOG_LEVEL_INTERNAL = 0x01;
		static const uint8_t LOG_LEVEL_EXPLICIT = 0x02;

		/* Elements */
		uint8_t loggingLevel;
		std::vector<LogMessage> logs;

		/* Methods */

		// Constructors & Destructor
		Logger(const uint8_t _level = LOG_LEVEL_INTERNAL) : loggingLevel(_level)
		{};
		Logger(const Logger& src) : loggingLevel(src.loggingLevel)
		{
			this->logs.clear();
			for (const LogMessage& _log : src.logs)
			{
				this->logs.push_back(_log);
			};
		};
		virtual ~Logger()
		{
			this->logs.clear();
			this->loggingLevel = LOG_LEVEL_INVALID;
		};

		// Utility Methods
		inline void clear() noexcept
		{
			this->logs.clear();
		};
		inline uint64_t size() const
		{
			return this->logs.size();
		};

		// Logging Methods
		inline void logMessage(const std::string& _msg, const std::string& _fn)
		{
			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(_msg, _fn, LogMessage::MESSAGE_LEVEL_MESSAGE));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << "[Message] in " << _fn << "():" << _msg << "\n";
			};
		};
		inline void logWarning(const std::string& _msg, const std::string& _fn)
		{
			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(_msg, _fn, LogMessage::MESSAGE_LEVEL_WARNING));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << "[Warning] in " << _fn << "():" << _msg << "\n";
			};
		};
		inline void logError(const std::string& _msg, const std::string& _fn)
		{
			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(_msg, _fn, LogMessage::MESSAGE_LEVEL_ERROR));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << "[Error] in " << _fn << "():" << _msg << "\n";
			};
		};

		// Serialization Methods
		inline std::string toCSV(const bool header = true) const
		{
			std::string csv = (header) ? "level, message, functionName\n" : "";
			for (const LogMessage& _log : this->logs)
			{
				csv += _log.toCSV();
			};
			return csv;
		};
		inline std::string toJSON() const
		{
			std::string json = "[";
			for (const LogMessage& _log : this->logs)
			{
				json += _log.toJSON() + ",";
			};
			json[json.size()] = ']';
			return json;
		};
		inline std::string toXML() const
		{
			std::string xml = "<Logs>";
			for (const LogMessage& _log : this->logs)
			{
				xml += _log.toXML();
			};
			xml += "</Logs>";
			return xml;
		};
	};

	// Logging Interface
	struct LoggingInterface
	{
		/*
			# Logging Interface (struct)

			## About
			A class that implements basic logging functions and adds a logger
			pointer. Adding this as a parent to a structure will add logging
			behavior to it automatically.
		*/

		/* Elements */
		Logger* log;

		/* Methods */

		// Constructors & Destructor
		LoggingInterface(Logger* _log = 0) : log(_log)
		{};
		~LoggingInterface()
		{
			this->log = 0;
		};

		// Log Methods
		inline void setLog(Logger* log)
		{
			this->log = log;
		};
		inline Logger* getLog() const
		{
			return this->log;
		};

		// Logging Methods
		inline void logMessage(const std::string& msg, const std::string& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logMessage(msg, fn);
		};
		inline void logWarning(const std::string& msg, const std::string& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logWarning(msg, fn);
		};
		inline void logError(const std::string& msg, const std::string& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logError(msg, fn);
		};
	};
};
#endif