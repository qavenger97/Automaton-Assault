/*!
* \file FileUtils.h
*
* \author Shiyang Ao
* \date October 2016
*
* File utility functions
*/
#pragma once

namespace Hourglass
{
	class FileUtils
	{
	public:
		// Case insensitive string comparison
		static bool iequals(const std::string& str1, const std::string& str2);

		// Replace all slashes with correct delimiter
		//    eg: '/Assets//Mesh' becomes '\Asset\Mesh'
		static void TrimPathDelimiter(std::string& path);

		// Iterate through all files with specified extension in the folder and call a function on them
		static void IterateFolderForFiles(const char* path, const char* ext, std::function<void(const char* filename)> fileOpFunc, bool searchSubfolders=true);

		// Allocate a buffer with file content
		static char* ReadBinaryFileBuffer(const char* filename, int* fileSize);
	};
}