#include "pch.h"

#include "AnimationManager.h"
#include "Core\FileArchive.h"
#include "Component\Animation.h"

Hourglass::AnimationManager Hourglass::g_AnimationManager;

void Hourglass::AnimationManager::Init()
{
	LoadAnimations( "Assets/Animation/*.*" );
}

void Hourglass::AnimationManager::Shutdown()
{
	for (std::vector<Clip*>::iterator iter = m_Clips.begin(); iter != m_Clips.end(); iter++)
	{
		delete *iter;
	}
	m_Clips.clear();
	m_Lengths.clear();
	m_Skeletons.clear();
}

Hourglass::Clip* Hourglass::AnimationManager::GetClip( const char* path )
{
	std::string animPath( path );
	std::replace( animPath.begin(), animPath.end(), '/', '\\' );

	for (int i = 0; i < animPath.size(); i++)
		animPath[i] = (char)tolower( animPath[i] );

	for (int i = 0; i < m_Clips.size(); i++)
	{
		if (animPath == m_Clips[i]->GetPath())
			return m_Clips[i];
	}

	return nullptr;
}

Hourglass::LoadingSkeleton* Hourglass::AnimationManager::GetSkeleton( const char* path )
{
	assert( m_Skeletons.size() == m_Lengths.size() );
	std::string animPath( path );
	std::replace( animPath.begin(), animPath.end(), '/', '\\' );

	for (int i = 0; i < animPath.size(); i++)
		animPath[i] = (char)tolower( animPath[i] );

	for (int i = 0; i < m_Clips.size(); i++)
	{
		if (animPath == m_Clips[i]->GetPath())
		{
			return &m_Skeletons[i];
		}
	}

	return nullptr;
}

float Hourglass::AnimationManager::GetLength( const char* path )
{
	assert( m_Clips.size() == m_Lengths.size() );
	std::string animPath( path );
	std::replace( animPath.begin(), animPath.end(), '/', '\\' );

	for (int i = 0; i < animPath.size(); i++)
		animPath[i] = (char)tolower( animPath[i] );

	for (int i = 0; i < m_Clips.size(); i++)
	{
		if (animPath == m_Clips[i]->GetPath())
		{
			return m_Lengths[i];
		}
	}

	return float();
}

void Hourglass::AnimationManager::LoadAnimations( const char* path )
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	std::string resFindingPath = path;
	hFind = FindFirstFileA( resFindingPath.data(), &FindFileData );

	std::string relPath( path );
	relPath = relPath.substr( 0, relPath.find_last_of( "\\/" ) );
	relPath += "\\";

	do
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			// TODO
			std::string resName = relPath + FindFileData.cFileName;

			std::string lowerExt = resName.substr( resName.find_last_of( '.' ) );
			for (UINT i = 0; i < lowerExt.size(); i++)
			{
				lowerExt[i] = tolower( lowerExt[i] );
			}

			if (lowerExt == ".hani")
			{
				FileArchive archive;

				if (archive.Open( resName.data(), kFileOpenMode_Read ))
				{
					LoadingSkeleton skeleton;
					float length;
					Clip* pClip = new Clip;

					archive.Serialize( skeleton );
					archive.Serialize( length );
					pClip->Serialize( &archive );

					std::replace( resName.begin(), resName.end(), '/', '\\' );

					// Store resource name in lower cases
					for (int i = 0; i < resName.size(); i++)
						resName[i] = (char)tolower( resName[i] );

					pClip->SetPath( resName.c_str() );
					m_Skeletons.push_back( skeleton );
					m_Lengths.push_back( length );
					m_Clips.push_back( pClip );
					archive.Close();
				}
				else
				{
					char buf[1024];
					sprintf_s( buf, "Unable to load mesh: %s\n", resName.data() );
					OutputDebugStringA( buf );
				}
			}
		}
		else
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char subDirName[4096];
				strcpy_s( subDirName, relPath.data() );
				strcat_s( subDirName, FindFileData.cFileName );
				strcat_s( subDirName, "\\*.*" );
				LoadAnimations( subDirName );
			}
		}

	} while (FindNextFileA( hFind, &FindFileData ) != 0);
}
