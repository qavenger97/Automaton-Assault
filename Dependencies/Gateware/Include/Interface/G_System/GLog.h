#ifndef GLOG
#define GLOG

/*!
File: GLog.h
Purpose: A Gateware interface that handles logging in a thread safe manner
Author: Justin W. Parks
Contributors: N/A
Last Modified: 11/16/2016
Copyright: 7thGate Software LLC.
License: MIT
*/

// GFile Inherits directly from GMultiThreaded 
#include "../G_Core/GMultiThreaded.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong.
	namespace CORE
	{
		//! Unique Identifier for this interface. {79AC4197-1A78-4A86-A4A4-E84773A2F6DF}
		static const GUUIID GLogUUIID =
		{
			0x79ac4197, 0x1a78, 0x4a86,{ 0xa4, 0xa4, 0xe8, 0x47, 0x73, 0xa2, 0xf6, 0xdf }
		};

		//Forward declaration so we can use GFile* in a function
		class GFile;


		//! Cross platform threadsafe logger
		class GLog : public GMultiThreaded
		{
			// All Gateware API interfaces contain no variables & are pure virtual
		public:

			//! Logs a null terminated string.
			/*!
			*  Date, Time, and thread ID will be appended to the front of the message unless specified otherwise (See EnableVerboseLogging).
			*  A new line character will be appended to the end of the string so your log messages do not require a new line.
			*
			*  \param [in] _log The message to log out.
			*
			*  \retval SUCCESS  Successfully queued the message to the log.
			*  \retval FAILURE  The queue has reached maximun size (call flush).
			*  \retval INVALID_ARGUMENT  A nullptr was passed in.
			*/
			virtual GRETURN Log(const char* const _log) = 0;
			
			//! Logs a null terminated string with a category.
			/*!
			*  Date, Time, and thread ID will be appended to the front of the message unless specified otherwise (See EnableVerboseLogging).
			*  A new line character will be appended to the end of the string so your log messages do not require a new line.
			*
			*  \param [in] _category The category the log belongs in. ie. ERROR, WARNING, INFO, etc.
			*  \param [in] _log The message to log out.
			*
			*  \retval SUCCESS  Successfully queued the message to the log.
			*  \retval FAILURE  The queue has reached maximun size (call flush).
			*  \retval INVALID_ARGUMENT  Either _category or _log are nullptr.
			*/
			virtual GRETURN LogCatergorized(const char* const _category, const char* const _log) = 0;
			
			//! Turns verbose logging on or off.
			/*!
			*  Use this function to prevent the addition of date, time, and threadID to your logs.
			*
			*  \param [in] _value true to turn on or false to turn off.
			*
			*/
			virtual void EnableVerboseLogging(bool _value) = 0;
			
			//! Turns console logging on or off.
			/*!
			*  Use this function to prevent the additional console logging.
			*
			*  \param [in] _value true to turn on or false to turn off.
			*
			*/
			virtual void EnableConsoleLogging(bool _value) = 0;
			
			//! Forces a log dump to file.
			/*!
			*  This will force a log dump to the file and clear the log queue.
			*
			*  \retval SUCCESS  Successfully dumped the logs
			*  \retval FAILURE  Most likely a file corruption or a file is not open.
			*/
			virtual GRETURN Flush() = 0;
		};

		//! Creates a GLog object
		/*!
		*  This function will create a GLog object with the log file being created in the directory
		*  the program was ran from. If you want to control where the log file is going to be created at
		*  then use the overridden function to pass in a GFile* that is pre set the way you want it.
		*
		*  \param [in] _fileName The name of the log file.
		*  \param [out] _outLog Will contain the GLog if succefully created.
		*
		*  \retval SUCCESS  GLog was successfully created.
		*  \retval FAILURE  GLog was not created. _outLog will be null
		*  \retval INVALID_ARGUMENT  Either one or both arguments are nullptrs.
		*/
		GRETURN GCreateLog(const char* const _fileName, GLog** _outLog);

		//! Creates a GLog object
		/*!
		*  This function will create a GLog object with the GFile object that was passed in.
		*  This is so you can have more control over your log by setting up a GFile in advance.
		*  The GFile object should already have a file open for text writing.
		*  NOTE: A reference count will be added to the GFile object passed in if the function succeeds.
		*
		*  \param [in] _file The GFile object that this log will use.
		*  \param [out] _outLog Will contain the GLog if succefully created.
		*
		*  \retval SUCCESS  GLog was successfully created.
		*  \retval FAILURE  GLog was not created. _outLog will be null.
		*  \retval INVALID_ARGUMENT Either one or both the arguments are nullptr.
		*/
		GRETURN GCreateLog(GFile* _file, GLog** _outLog);
	}
}

#endif