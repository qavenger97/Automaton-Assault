#pragma once

namespace Hourglass
{
	enum FileArchiveOpenMode
	{
		kFileOpenMode_Read,
		kFileOpenMode_Write
	};

	class FileArchive
	{
	public:
		~FileArchive();

		// Open file for read or write
		bool Open(const char* filename, FileArchiveOpenMode mode);

		// Close file
		void Close();

		// Is file open for operation
		bool IsOpen() const;

		// Is file open for reading
		bool IsReading() const;

		// Make sure file contains a header
		//		Read Mode: return false if file doesn't contain the header
		//		Write Mode: write header into file stream
		bool EnsureHeader(const char* header, UINT size);

		// Serialize a vector
		template<typename T>
		void Serialize( T& data )
		{
			if (m_OpenMode == kFileOpenMode_Write)
			{
				m_FileStream.write( (char*)&data, sizeof( data ) );
			}
			else
			{
				assert( !m_FileStream.eof() );
				m_FileStream.read( (char*)&data, sizeof( data ) );
			}
		}

		// Serialize a string
		template<>
		void Serialize<std::string>( std::string& str )
		{
			if (m_OpenMode == kFileOpenMode_Write)
			{
				unsigned int size = unsigned int( str.size() );
				m_FileStream.write( (char*)&size, sizeof( size ) );
				m_FileStream.write( str.c_str(), str.size() );
			}
			else
			{
				unsigned int size;
				char buffer[255];
				assert( !m_FileStream.eof() );
				m_FileStream.read( (char*)&size, sizeof( size ) );
				m_FileStream.read( buffer, size );
				str = buffer;
			}
		}

		// Serialize a vector
		template<typename T>
		void Serialize(std::vector<T>& vec)
		{			
			if (m_OpenMode == kFileOpenMode_Write)
			{
				UINT size = (UINT)vec.size();
				m_FileStream.write((char*)&size, sizeof(size));
				if (size)
					m_FileStream.write((char*)vec.data(), sizeof(T) * size);
			}
			else
			{
				assert(!m_FileStream.eof());
				UINT size;
				m_FileStream.read((char*)&size, sizeof(size));
				vec.resize(size);
				if (size)
					m_FileStream.read((char*)vec.data(), sizeof(T) * size);
			}
		}

	private:
		std::fstream		m_FileStream;
		FileArchiveOpenMode	m_OpenMode;
	};
}