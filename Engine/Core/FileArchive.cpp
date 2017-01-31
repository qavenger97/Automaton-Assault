#include "pch.h"
#include "FileArchive.h"

Hourglass::FileArchive::~FileArchive()
{
	Close();
}

bool Hourglass::FileArchive::Open(const char* filename, FileArchiveOpenMode mode)
{
	m_OpenMode = mode;

	m_FileStream.open(filename, (mode == kFileOpenMode_Read ? std::ios::in : std::ios::out) | std::ios::binary);

	return m_FileStream.is_open();
}

void Hourglass::FileArchive::Close()
{
	if (m_FileStream.is_open())
		m_FileStream.close();
}

bool Hourglass::FileArchive::IsOpen() const
{
	return m_FileStream.is_open();
}

bool Hourglass::FileArchive::IsReading() const
{
	return m_OpenMode == kFileOpenMode_Read;
}

bool Hourglass::FileArchive::EnsureHeader(const char* header, UINT size)
{
	if (m_OpenMode == kFileOpenMode_Write)
	{
		m_FileStream.write(header, size);
	}
	else
	{
		assert(!m_FileStream.eof());
		char* pBuf = new char[size];
		m_FileStream.read(pBuf, size);

		for (UINT i = 0; i < size; i++)
		{
			if (pBuf[i] != header[i])
			{
				delete[] pBuf;
				return false;
			}
		}

		delete[] pBuf;
	}

	return true;
}
