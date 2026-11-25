#pragma once
/*
	# Logging Library (C++)
	By Joseph Juma

	## Version
	v1.2.3

	## About
	A single-header C++14 compatible logging library.

	## To Do
	-	Implement a static-sized circular buffer structure.
	-	Bring in file functions library into here for file-logging.
	-	Implement file-logger.
	-	Create statically sized logs for stack allocation rather than heap
		allocation on everything(?)

	## Copyright
	Copyright 2024 to 2026, Joseph Juma. All rights reserved.
*/
#ifndef LOGGING_LIBRARY__H
#define LOGGING_LIBRARY__H
/* Deps */
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

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
	static const uint16_t	LOGGING_LIBRARY_VERSION_PATCH = 3;

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
	inline const std::string get_logging_library_version_string()
	{
		return (
			"v" +
			std::to_string(LOGGING_LIBRARY_VERSION_MAJOR) + "." +
			std::to_string(LOGGING_LIBRARY_VERSION_MINOR) + "." +
			std::to_string(LOGGING_LIBRARY_VERSION_PATCH)
		);
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
		uint8_t level;
		std::string message;
		std::string functionName;

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
				return std::to_string(level);
				break;
			};
		};

		/* Methods */

		// Constructors & Destructor
		LogMessage() : level(0), message(""), functionName("")
		{};
		LogMessage(std::string&& _msg, std::string&& _fn, const uint8_t _lv = 0) : level(_lv), message(std::move(_msg)), functionName(std::move(_fn))
		{};
		LogMessage(const std::string& _msg, const std::string& _fn, uint8_t _lv = 0) : message(_msg), functionName(_fn), level(_lv)
		{};
		LogMessage(const LogMessage& src) : level(src.level), message(src.message), functionName(src.functionName)
		{};
		LogMessage(LogMessage&& src) noexcept : level(src.level), message(std::move(src.message)), functionName(std::move(src.functionName))
		{
			src.level = 0;
			src.message.clear();
			src.functionName.clear();
		};
		~LogMessage()
		{
			this->functionName.clear();
			this->message.clear();
			this->level = 0;
		};

		// Assignment Operator
		inline LogMessage& operator=(LogMessage&& B) noexcept
		{
			if (this != &B)
			{
				this->level = B.level;
				this->message = std::move(B.message);
				this->functionName = std::move(B.functionName);

				B.level = 0;
				B.message.clear();
				B.functionName.clear();
			};

			return (*this);
		};
		inline LogMessage& operator=(const LogMessage& B)
		{
			if (this != &B)
			{
				this->level = B.level;
				this->message = B.message;
				this->functionName = B.functionName;
			};
			
			return (*this);
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
				("<FunctionName>" + this->functionName + "</FunctionName>") + 
				"</LogMessage>");
		};
	};

	// Logger Structures
	struct Logger
	{
		/*
			# Logger (interface)

			## About
			An abstract interface for log managing structures. Classes that
			derive from this are employed as the central logging mechanism
			and repository.
		*/

		/* Methods */

		// Metadata Methods
		virtual size_t size() const = 0;

		// Utility Methods
		virtual void clear() noexcept = 0;

		// Message Logging Methods
		virtual void logMessage(std::string&& _msg, std::string&& _fn) = 0;
		virtual void logMessage(const std::string& _msg, const std::string& _fn) = 0;

		// Warning Logging Methods
		virtual void logWarning(std::string&& _msg, std::string&& _fn) = 0;
		virtual void logWarning(const std::string& _msg, const std::string& _fn) = 0;

		// Error Logging Methods
		virtual void logError(std::string&& _msg, std::string&& _fn) = 0;
		virtual void logError(const std::string& _msg, const std::string& _fn) = 0;

		// Serialization Methods
		virtual std::string toCSV(const bool _header = true) const = 0;
		virtual std::string toJSON() const = 0;
		virtual std::string toXML() const = 0;
	};
	struct BaseLogger : Logger
	{
		/*
			# Base Logger (struct)

			## About
			A base logger class. Implements generic, shared logging behavior
			and structures.

			## Notes

			-	[Reminder] Anything you implement in here needs to be thread
				safe if not overriden downstream in the `ThreadSafeLogger`!
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
		BaseLogger(const uint8_t _level = LOG_LEVEL_INTERNAL) : loggingLevel(_level)
		{};
		BaseLogger(const BaseLogger& src) : loggingLevel(src.loggingLevel)
		{
			this->logs.clear();
			for (const LogMessage& _log : src.logs)
			{
				this->logs.push_back(_log);
			};
		};
		BaseLogger(BaseLogger&& src) noexcept : loggingLevel(src.loggingLevel), logs(std::move(src.logs))
		{
			src.loggingLevel = LOG_LEVEL_INVALID;
			src.logs.clear();
		};
		virtual ~BaseLogger()
		{
			this->logs.clear();
			this->loggingLevel = LOG_LEVEL_INVALID;
		};

	};
	
	struct SimpleLogger : BaseLogger
	{
		/*
			# Simple Logger (struct)

			## About
			A simple logging structure. This should be considered a base class
			for simple, single-threaded application loggers.

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

		/* Methods */

		// Constructors & Destructor
		SimpleLogger(const uint8_t _level = LOG_LEVEL_INTERNAL) : BaseLogger(_level)
		{};
		SimpleLogger(const SimpleLogger& src) : BaseLogger(src)
		{};
		SimpleLogger(SimpleLogger&& src) noexcept : BaseLogger(std::move(src))
		{};
		virtual ~SimpleLogger()
		{};

		// Assignment Operators
		inline SimpleLogger& operator=(const SimpleLogger& B)
		{
			if (this != &B)
			{
				this->loggingLevel = B.loggingLevel;
				this->logs = B.logs;
			};

			return (*this);
		};
		inline SimpleLogger& operator=(SimpleLogger&& B) noexcept
		{
			if (this != &B)
			{
				this->loggingLevel = std::move(B.loggingLevel);
				this->logs = std::move(B.logs);

				this->loggingLevel = LOG_LEVEL_INVALID;
				this->logs.clear();
			};

			return (*this);
		};

		// Metadata Methods
		inline size_t size() const override
		{
			return this->logs.size();
		};

		// Utility Methods
		inline void clear() noexcept override
		{
			this->logs.clear();
		};
		
		// Logging Methods
		inline void logMessage(std::string&& _msg, std::string&& _fn) override
		{
			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(std::move(_msg), std::move(_fn), LogMessage::MESSAGE_LEVEL_MESSAGE));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << "[Message] in " << _fn << "():" << _msg << "\n";
			};
		};
		inline void logMessage(const std::string& _msg, const std::string& _fn) override
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
		
		inline void logWarning(std::string&& _msg, std::string&& _fn) override
		{
			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(std::move(_msg), std::move(_fn), LogMessage::MESSAGE_LEVEL_WARNING));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << "[Warning] in " << _fn << "():" << _msg << "\n";
			};
		};
		inline void logWarning(const std::string& _msg, const std::string& _fn) override
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
		
		inline void logError(std::string&& _msg, std::string&& _fn) override
		{
			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(std::move(_msg), std::move(_fn), LogMessage::MESSAGE_LEVEL_ERROR));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << "[Error] in " << _fn << "():" << _msg << "\n";
			};
		};
		inline void logError(const std::string& _msg, const std::string& _fn) override
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
		inline std::string toCSV(const bool header = true) const override
		{
			std::string csv = (header) ? "level, message, functionName\n" : "";
			for (const LogMessage& _log : this->logs)
			{
				csv += _log.toCSV();
			};
			return csv;
		};
		inline std::string toJSON() const override
		{
			std::string json = "[";
			for (const LogMessage& _log : this->logs)
			{
				json += _log.toJSON() + ",";
			};
			json += ']';
			return json;
		};
		inline std::string toXML() const override
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
	struct ThreadSafeLogger : BaseLogger
	{
		/*
			# Thread Safe Logger (struct)

			## About
			A thread-safe logging structure. Employs mutexes for multithreading
			safe logging. Derive from this class if you want to implement
			thread safe logging.
		*/

		/* Elements */
		mutable std::mutex loggerMutex;

		/* Methods */

		// Constructors & Destructor
		ThreadSafeLogger(const uint8_t _level = LOG_LEVEL_INTERNAL) : BaseLogger(_level)
		{};
		
		ThreadSafeLogger(const ThreadSafeLogger& src, const std::lock_guard<std::mutex>& _guard) : BaseLogger(src)
		{
			/*
				@note
				This is used to do cross-thread safe copying. It works by
				passing in a guard on src.loggerMutex which is kept in
				scope during the various constructor calls.
			*/
		};
		ThreadSafeLogger(const ThreadSafeLogger& src) : ThreadSafeLogger(src, std::lock_guard<std::mutex>(src.loggerMutex))
		{};
		
		ThreadSafeLogger(ThreadSafeLogger&& src, const std::lock_guard<std::mutex>& _guard) noexcept : BaseLogger(std::move(src))
		{
			/*
				@note
				This is used to do cross-thread safe copying. It works by
				passing in a guard on src.loggerMutex which is kept in
				scope during the various constructor calls.
			*/
		};
		ThreadSafeLogger(ThreadSafeLogger&& src) noexcept : ThreadSafeLogger(std::move(src), std::lock_guard<std::mutex>(src.loggerMutex))
		{};

		virtual ~ThreadSafeLogger()
		{
			this->logs.clear();
			this->loggingLevel = LOG_LEVEL_INVALID;
		};

		// Metadata Methods
		inline size_t size() const override
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);
			return this->logs.size();
		};

		// Utility Methods
		inline void clear() noexcept override
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);
			this->logs.clear();
		};

		// Internal Message Logging Methods
		void _logMessage(std::string&& _msg, std::string&& _fn, uint8_t _messageLevel)
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);

			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(std::move(_msg), std::move(_fn), _messageLevel));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << ("[Message] in " + _fn + "():" + _msg + "\n"); // each call to `<<` isn't guaranteed thread-safe.
			};
		};
		void _logMessage(const std::string& _msg, const std::string& _fn, uint8_t _messageLevel)
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);

			if ((loggingLevel & LOG_LEVEL_INTERNAL) != 0)
			{
				this->logs.push_back(LogMessage(_msg, _fn, _messageLevel));
			}
			else if ((loggingLevel & LOG_LEVEL_EXPLICIT) != 0)
			{
				std::cout << ("[Message] in " + _fn + "():" + _msg + "\n"); // each call to `<<` isn't guaranteed thread-safe.
			};
		};

		// Message Logging Methods
		inline void logMessage(std::string&& _msg, std::string&& _fn) override
		{
			this->_logMessage(std::move(_msg), std::move(_fn), LogMessage::MESSAGE_LEVEL_MESSAGE);
		};
		inline void logMessage(const std::string& _msg, const std::string& _fn) override
		{
			this->_logMessage(_msg, _fn, LogMessage::MESSAGE_LEVEL_MESSAGE);
		};

		// Warning Logging Methods
		inline void logWarning(std::string&& _msg, std::string&& _fn) override
		{
			this->_logMessage(std::move(_msg), std::move(_fn), LogMessage::MESSAGE_LEVEL_WARNING);
		};
		inline void logWarning(const std::string& _msg, const std::string& _fn) override
		{
			this->_logMessage(_msg, _fn, LogMessage::MESSAGE_LEVEL_WARNING);
		};

		// Error Logging Methods
		inline void logError(std::string&& _msg, std::string&& _fn) override
		{
			this->_logMessage(std::move(_msg), std::move(_fn), LogMessage::MESSAGE_LEVEL_ERROR);
		};
		inline void logError(const std::string& _msg, const std::string& _fn) override
		{
			this->_logMessage(_msg, _fn, LogMessage::MESSAGE_LEVEL_ERROR);
		};

		// Serialization Methods
		inline std::string toCSV(const bool _header = true) const override
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);
			std::string csv = (_header) ? "level, message, functionName\n" : "";
			for (const LogMessage& _log : this->logs)
			{
				csv += _log.toCSV();
			};
			return csv;
		};
		inline std::string toJSON() const override
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);
			std::string json = "[";
			for (const LogMessage& _log : this->logs)
			{
				json += _log.toJSON() + ",";
			};
			json += ']';
			return json;
		};
		inline std::string toXML() const override
		{
			std::lock_guard<std::mutex> _guard(loggerMutex);
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
		LoggingInterface(LoggingInterface& src) : log(src.log)
		{};
		virtual ~LoggingInterface()
		{
			this->log = 0;
		};

		// Assignment Operator
		inline LoggingInterface& operator=(const LoggingInterface& B)
		{
			this->log = B.log; // I feel like this might not need a check atm.
			return (*this);
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
		inline void logMessage(std::string&& msg, std::string&& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logMessage(std::move(msg), std::move(fn));
		};
		inline void logMessage(const std::string& msg, const std::string& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logMessage(msg, fn);
		};
		
		inline void logWarning(std::string&& msg, std::string&& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logWarning(std::move(msg), std::move(fn));
		};
		inline void logWarning(const std::string& msg, const std::string& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logWarning(msg, fn);
		};
		
		inline void logError(std::string&& msg, std::string&& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logError(std::move(msg), std::move(fn));
		};
		inline void logError(const std::string& msg, const std::string& fn)
		{
			// @note: For speed I didn't ptr check; will crash if log is invalid.
			log->logError(msg, fn);
		};
	};
};
#endif