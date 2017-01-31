#include <iostream>
#include <vector>

class FileCollection;

class FileProperties
{
public:

	void Init( const char* fullPath, const char* pathNoExt  );

	const char* GetPath() const			{ return m_FullPath.c_str(); }
	const char* GetPathNoExt() const	{ return m_PathNoExt.c_str(); }

private:

	std::string  m_FullPath;
	std::string  m_PathNoExt;
};

class FileCollection
{
public:
	
	enum DirFilter
	{
		kCurrentDirOnly,
		kIncludeSubDirs
	};

	void AddFilesInDir( const char* dir, const char* ext, DirFilter dirFilter );

	FileProperties* GetFileProperties() { return m_Data.data(); }
	
	FileProperties Get( int i ) const { return m_Data[i]; }

	unsigned int Count() { return unsigned int( m_Data.size()); }

private:
	
	std::vector<FileProperties> m_Data;
};

