#include "FileCollection.h"
#include <Windows.h>
#include "Shlwapi.h"

void FileProperties::Init( const char* fullPath, const char* pathNoExt )
{
	m_FullPath = fullPath;
	m_PathNoExt = pathNoExt;
}

void FileCollection::AddFilesInDir( const char* dir, const char* ext, DirFilter dirFilter )
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	std::string resFindingPath = dir;
	hFind = FindFirstFileA( resFindingPath.data(), &FindFileData );

	char fullDirName[4096];
	if (GetFullPathNameA( dir, 4096, fullDirName, NULL ))
	{
		PathRemoveFileSpecA( fullDirName );
		strcat_s( fullDirName, "\\" );
	}

	do
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			std::string resName = std::string( fullDirName ) + FindFileData.cFileName;

			std::string lowerExt = resName.substr( resName.find_last_of( '.' ) + 1 );
			for (UINT i = 0; i < lowerExt.size(); i++)
			{
				lowerExt[i] = tolower( lowerExt[i] );
			}

			if (lowerExt == ext)
			{
				char outputName[MAX_PATH];
				strcpy_s( outputName, resName.data() );
				PathRemoveExtensionA( outputName );

				FileProperties fileProps;
				fileProps.Init( resName.c_str(), outputName );
				m_Data.push_back( fileProps );
			}
		}
		else if (dirFilter == DirFilter::kIncludeSubDirs)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char subDirName[4096];
				strcpy_s( subDirName, fullDirName );
				strcat_s( subDirName, FindFileData.cFileName );
				strcat_s( subDirName, "\\*.*" );
				AddFilesInDir( subDirName, ext, dirFilter );
			}
		}

	} while (FindNextFileA( hFind, &FindFileData ) != 0);
}
