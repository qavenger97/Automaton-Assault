/*!
 * \file Texture.cpp
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Texture class for render system
 */

#include "pch.h"

#include "Texture.h"
#include "Core\FileUtils.h"
#include "Core\DDSTextureLoader.h"
#include "RenderDevice.h"

Hourglass::TextureManager Hourglass::g_TextureManager;

void Hourglass::TextureManager::Init()
{
	LoadTextures("Assets/Texture/");
	LoadCubeMap("Assets/Environment/");
}

void Hourglass::TextureManager::Shutdown()
{
	for (std::vector<Texture*>::iterator iter = m_Textures.begin(); iter != m_Textures.end(); iter++)
	{
		delete *iter;
	}
	m_Textures.clear();

	for (std::vector<Texture*>::iterator iter = m_Environmental.begin(); iter != m_Environmental.end(); iter++)
	{
		delete *iter;
	}
	m_Environmental.clear();
}

Hourglass::Texture* Hourglass::TextureManager::GetTexture(const char* path) const
{
	std::string texPath(path);
	FileUtils::TrimPathDelimiter(texPath);

	for (int i = 0; i < m_Textures.size(); i++)
	{
		if (FileUtils::iequals(texPath, m_Textures[i]->GetPath()))
			return m_Textures[i];
	}

	return nullptr;
}

Hourglass::Texture * Hourglass::TextureManager::GetEnvirnmental(int i) const
{
	return m_Environmental[i];
}

void Hourglass::TextureManager::LoadTextures(const char* path)
{
	char searchPath[MAX_PATH];
	strcpy_s(searchPath, path);
	strcat_s(searchPath, "\\*.*");

	FileUtils::IterateFolderForFiles(searchPath, ".dds", [&](const char* filename) {
		size_t char_len;
		wchar_t wszName[1024];
		mbstowcs_s(&char_len, wszName, filename, strlen(filename));

		ComPtr<ID3D11Resource> pResource;
		ComPtr<ID3D11ShaderResourceView> srv;
		char msg_buf[1024];

		HRESULT hr = CreateDDSTextureFromFile(g_RenderDev.device, wszName, &pResource, &srv);
		if (FAILED(hr))
		{
			sprintf_s(msg_buf, sizeof(msg_buf), "*** Failed to load texture [%s] ***\n", filename);
			OutputDebugStringA(msg_buf);
		}

		if (pResource)
		{
			ComPtr<ID3D11Texture2D> pTexture;
			pResource.As<ID3D11Texture2D>(&pTexture);

			assert(pTexture);

			// Unify delimiter in path
			std::string strFilename = filename;
			FileUtils::TrimPathDelimiter(strFilename);

			D3D11_TEXTURE2D_DESC desc;
			pTexture->GetDesc(&desc);

			Texture* texture = new Texture();
			texture->ShaderResourceView = srv;
			texture->Width = desc.Width;
			texture->Height = desc.Height;
			texture->SetPath(strFilename);

			m_Textures.push_back(texture);
		}
	});
}

void Hourglass::TextureManager::LoadCubeMap(const char * path)
{
	char searchPath[MAX_PATH];
	strcpy_s(searchPath, path);
	strcat_s(searchPath, "\\*.*");

	FileUtils::IterateFolderForFiles(searchPath, ".dds", [&](const char* filename) {
		size_t char_len;
		wchar_t wszName[1024];
		mbstowcs_s(&char_len, wszName, filename, strlen(filename));

		ComPtr<ID3D11Resource> pResource;
		ComPtr<ID3D11ShaderResourceView> srv;
		char msg_buf[1024];

		HRESULT hr = CreateDDSTextureFromFile(g_RenderDev.device, wszName, &pResource, &srv);
		if (FAILED(hr))
		{
			sprintf_s(msg_buf, sizeof(msg_buf), "*** Failed to load texture [%s] ***\n", filename);
			OutputDebugStringA(msg_buf);
		}

		if (pResource)
		{
			ComPtr<ID3D11Texture2D> pTexture;
			pResource.As<ID3D11Texture2D>(&pTexture);

			assert(pTexture);

			// Unify delimiter in path
			std::string strFilename = filename;
			FileUtils::TrimPathDelimiter(strFilename);

			D3D11_TEXTURE2D_DESC desc;
			pTexture->GetDesc(&desc);

			Texture* texture = new Texture();
			texture->ShaderResourceView = srv;
			texture->Width = desc.Width;
			texture->Height = desc.Height;
			texture->SetPath(strFilename);

			m_Environmental.push_back(texture);
		}
	});
}
