#ifndef GFILE
#define GFILE
/*!
	File: GFile.h
	Purpose: A Gateware interface that handles both binary and textual file io and directory
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
		//! Unique Identifier for this interface. {3BE0F240-C3E7-424B-888E-F2994B6D1E48}
		static const GUUIID GFileUUIID =
		{
			0x3be0f240, 0xc3e7, 0x424b,{ 0x88, 0x8e, 0xf2, 0x99, 0x4b, 0x6d, 0x1e, 0x48 }
		};

		//! Cross platform FileIO/Directory handling
		/*!
		* Handles file input/output operations, as well as, directory information and file information
		*/
		class GFile : public GMultiThreaded
		{
			// All Gateware API interfaces contain no variables & are pure virtual
		public:

			//! Opens a file for binary read.
			/*!
			*  The file name passed into the function should be passed like it is a realitive path.
			*  The function will look in the current working directory for the file.
			*  If the file is not found in the current working directory, the function will fail.
			*
			*  \param [in] _file The file name of the file to open.
			*
			*  \retval SUCCESS  Succesfully opened the file.
			*  \retval FILE_NOT_FOUND  File could not be found.
			*  \retval FAILURE  A file is already opened.
			*  \retval INVALID_ARGUMENT  A null pointer was passed in.
			*/
			virtual GRETURN OpenBinaryRead(const char* const _file) = 0;

			//! Opens a file for binary write truncating the file.
			/*!
			*  The file name passed into the function should be passed like it is a realitive path.
			*  The function will look in the current working directory for the file.
			*  If the file is not found in the current working directory, the file will be created in
			*  the current working directory.
			*
			*  \param [in] _file The file name of the file to open.
			*
			*  \retval SUCCESS  Succesfully opened the file.
			*  \retval FAILURE  A file is already open or file could not be found/created.
			*  \retval INVALID_ARGUEMENT  A nullptr was passed in.
			*/
			virtual GRETURN OpenBinaryWrite(const char* const _file) = 0;

			//! Opens a file for binary write with append.
			/*!
			*  The file name passed into the function should be passed like it is a realitive path.
			*  The function will look in the current working directory for the file.
			*  If the file is not found in the current working directory, the file will be created in
			*  the current working directory.
			*
			*  \param [in] _file The file name of the file to open.
			*
			*  \retval SUCCESS  Succesfully opened the file.
			*  \retval FAILURE  A file is alread open or the file could not be found/created.
			*  \retval INVALID_ARGUMENT  A nullptr was passed in.
			*/
			virtual GRETURN AppendBinaryWrite(const char* const _file) = 0;

			//! Opens a file for text read.
			/*!
			*  The file name passed into the function should be passed like it is a realitive path.
			*  The function will look in the current working directory for the file.
			*  If the file is not found in the current working directory, the function will fail.
			*
			*  \param [in] _file The file name of the file to open.
			*
			*  \retval SUCCESS  Succesfully opened the file.
			*  \retval FILE_NOT_FOUND  File could not be found.
			*  \retval FAILURE  A file is already open.
			*  \retval INVALID_ARGUEMENT  A nullptr was passed in.
			*/
			virtual GRETURN OpenTextRead(const char* const _file) = 0;

			//! Opens a file for text write truncatin the file.
			/*!
			*  The file name passed into the function should be passed like it is a realitive path.
			*  The function will look in the current working directory for the file.
			*  If the file is not found in the current working directory, the file will be created in
			*  the current working directory.
			*
			*  \param [in] _file The file name of the file to open.
			*
			*  \retval SUCCESS  Succesfully opened the file.
			*  \retval FAILURE  A file is alread open or the file could not be found/created.
			*  \retval INVALID_ARGUMENT  A nullptr was passed in.
			*/
			virtual GRETURN OpenTextWrite(const char* const _file) = 0;

			//! Opens a file for text write with append.
			/*!
			*  The file name passed into the function should be passed like it is a realitive path.
			*  The function will look in the current working directory for the file.
			*  If the file is not found in the current working directory, the file will be created in
			*  the current working directory.
			*
			*  \param [in] _file The file name of the file to open.
			*
			*  \retval SUCCESS  Succesfully opened the file.
			*  \retval FAILURE  A file is alread open or the file could not be found/created.
			*  \retval INVALID_ARGUMENT  A nullptr was passed in.
			*/
			virtual GRETURN AppendTextWrite(const char* const _file) = 0;

			//! Writes binary to the currently opened file.
			/*!
			*  \param [in] _inData The data to write out to file.
			*  \param [in] _numBytes The number of bytes to write out to the file.
			*  
			*  \retval SUCCESS  Succesfully wrote out the data.
			*  \retval FAILURE  Either a file is not open or the write failed.
			*  \retval INVALID_ARGUMENT  Either a nullptr was passed in or a size of 0 bytes was passed in.
			*/
			virtual GRETURN Write(const char* const _inData, unsigned int _numBytes) = 0;

			//! Reads binary from the currently opened file.
			/*!
			*  \param [out] _outData The variable to store the read in bytes.
			*  \param [in] _numBytes The number of bytes to read in from the file.
			*
			*  \retval SUCCESS  Successful read.
			*  \retval FAILURE  Either file is not open or read failed. _outData will be null.
			*  \retval INVALID_ARGUMENT A byte size of 0 was passed in.
			*/
			virtual GRETURN Read(char* _outData, unsigned int _numBytes) = 0;

			//! Writes text to the currently opened file.
			/*!
			*  \param [in] _inData Null terminated string to write out.
			*
			*  \retval SUCCESS  Successful write. 
			*  \retval FAILURE  Either file is not open or read failed.
			*  \retval INVALID_ARGUMENT  A nullptr was passed in.
			*/
			virtual GRETURN WriteLine(const char* const _inData) = 0;

			//! Reads text to the currently opened file.
			/*!
			*
			* Reads text from the current file until either the size is reached or delimeter is reached.
			*
			*  \param [out] _outData Null terminated string to write out.
			*  \param [in] _outDataSize The size of _outData.
			*  \param [in] _delimiter The delemiter to stop reading at.
			*
			*  \retval SUCCESS  Successful read.
			*  \retval FAILURE  Either file is not open or read failed.
			*  \retval INVALID_ARGUMENT  Either a nullptr was passed in or the size request is 0.
			*/
			virtual GRETURN ReadLine(char* _outData, unsigned int _outDataSize, char _delimiter) = 0;

			//! Flushes and closes the current file.
			/*!
			*  \retval SUCCESS  File successfully flushed and closed.
			*  \retval FAILURE  A file is not currently open.
			*/
			virtual GRETURN CloseFile() = 0;

			//! Flushes the current file.
			/*!
			*  \retval SUCCESS  File successfully flushed and closed.
			*  \retval FAILURE  A file is not currently open.
			*/
			virtual GRETURN FlushFile() = 0;

			//! Changes the current working directory.
			/*!
			*  This sets the directory where we will look in with any of the open functions or other directory functions.
			*  Paths that are not relitive to the directory the program was ran from should be passed in as absolute paths.
			*
			*  \param [in] _dir An absolute path to the directory to set as the current working directory.
			*
			*  \retval SUCCESS  Succesfully set the current working directory.
			*  \retval FILE_NOT_FOUND  The directory could not be found.
			*  \retval FAILURE  Failed to open directory (Could be because it was not found).
			*  \retval INVALID_ARGUMENT  A nullptr was passed in
			*/
			virtual GRETURN SetCurrentWorkingDirectory(const char* const _dir) = 0;

			//! Retrieves the absolute path of the current working directory
			/*!
			*  This is the directory where we will look for any file open commands.
			*  This is by Windows standard gareenteed to be 255 or less. 
			*
			*  \param [out] _outDir An absolute path to the directory to set as the current working directory.
			*  \param [in] _dirSize The size of _outDir.
			*
			*  \retval SUCCESS  Successfully obtained the working directory
			*  \retval FAILURE  The current working directory is invalid or _outDir was not big enough. _outDir will be null.
			*  \retval INVALID_ARGUMENT  A nullptr was passed in or the size is 0.
			*/
			virtual GRETURN GetCurrentWorkingDirectory(char* _outDir, unsigned int _dirSize) = 0;

			//! Gets the number of files in the current working directory.
			/*!
			*  \param [out] _outSize The number of files in the directory.
			*
			*  \retval SUCCESS  Successfully counted the files in the directory.
			*  \retval FAILURE  Either currently working directory is invalid or count failed. _outSize will be -1;
			*/
			virtual GRETURN GetDirectorySize(unsigned int& _outSize) = 0;

			//! Gets the names of all files in the current working directory.
			/*!
			*  This function will retrieve just the file names and extensions.
			*  Any open function using these names will assume the files are in the current working directory.
			*  Any change of the current working directory will make these names invalid until called again.
			*
			*  \param [out] _outFiles Stores the names of the files retrieved.
			*  \param [in] _numFiles The number of files.
			*  \param [in] _fileNameSize The size of the file names. 
			*
			*  \retval SUCCESS  Successfully retrieved the file names.
			*  \retval FAILURE  Either current working directory is invalid or obtaining file names failed.
			*/
			virtual GRETURN GetFilesFromDirectory(char* _outFiles[], unsigned int _numFiles, unsigned int _fileNameSize) = 0;

			//! Gets the size of the specified file in bytes.
			/*!
			*  The filename passed into this function should be passed as a realitive path.
			*  This function will assume the file passed in is in the current working directory
			*  and will look for it there.
			*
			*  \param [in] _file The file to get the size of.
			*  \param [out] _outSize will store the size of the file.
			*
			*  \retval SUCCESS  Successfully retrieved the file size.
			*  \retval FILE_NOT_FOUND  Could not locate the file. Check that the current working directory is valid.
			*/
			virtual GRETURN GetFileSize(const char* const _file, unsigned int& _outSize) = 0;
		};

		//!Creates a GFile Object
		/*!
		*  The GFile created by this function will have its current working directory defaulted to 
		*  the directory where the program was ran from. Call SetCurrentWorkingDirectory to change it.
		*  No file will be opened in creation of GFile. Call an open function to open one.
		*
		*  \param [out] _outFile The Gfile that was created.
		*
		*  \retval SUCCESS  Gfile successfully created.
		*  \retval FAILURE  GFile could not be created.
		*  \retval INVALID_ARGUMENT  A nullptr was passed in.
		*/
		GRETURN GCreateFile(GFile** _outFile);
	}
}

#endif