/*!
* \file FileUtils.cpp
*
* \author Shiyang Ao
* \date October 2016
*
* File utility functions
*/
#include "pch.h"

#include "FileUtils.h"

bool Hourglass::FileUtils::iequals(const std::string& str1, const std::string& str2)
{
	if (str1.length() != str2.length())
		return false;

	for (int i = 0; i < str1.length(); i++)
	{
		if (tolower(str1[i]) != tolower(str2[i]))
			return false;
	}

	return true;
}

void Hourglass::FileUtils::TrimPathDelimiter(std::string& path)
{
	std::replace(path.begin(), path.end(), '/', '\\');

	size_t pos = path.find("\\\\");
	while (pos != std::string::npos)
	{
		path.replace(pos, 2, "\\");
		pos = path.find("\\\\");
	}
}

void Hourglass::FileUtils::IterateFolderForFiles(const char* path, const char* ext, std::function<void(const char* filename)> fileOpFunc, bool searchSubfolders/*=true*/)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	std::string resFindingPath = path;
	hFind = FindFirstFileA(resFindingPath.data(), &FindFileData);

	std::string relPath(path);
	relPath = relPath.substr(0, relPath.find_last_of("\\/"));
	relPath += "\\";

	do
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			std::string resName = relPath + FindFileData.cFileName;

			std::string fileExt = resName.substr(resName.find_last_of('.'));

			if (iequals(fileExt, ext))
			{
				fileOpFunc(resName.data());
			}
		}
		else if (searchSubfolders)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char subDirName[4096];
				strcpy_s(subDirName, relPath.data());
				strcat_s(subDirName, FindFileData.cFileName);
				strcat_s(subDirName, "\\*.*");
				IterateFolderForFiles(subDirName, ext, fileOpFunc);
			}
		}

	} while (FindNextFileA(hFind, &FindFileData) != 0);
}


char* Hourglass::FileUtils::ReadBinaryFileBuffer(const char* filename, int* fileSize)
{
	std::ifstream fin;
	fin.open(filename, std::ios_base::binary);

	if (!fin.is_open())
	{
		assert(0 && "Unable to open file");
		return nullptr;
	}

	fin.seekg(0, std::ios_base::end);
	*fileSize = (int)fin.tellg();
	char* pBuffer = new char[*fileSize];

	fin.seekg(0);
	fin.read(pBuffer, *fileSize);

	fin.close();

	return pBuffer;
}
