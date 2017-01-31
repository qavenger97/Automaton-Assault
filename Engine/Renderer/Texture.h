/*!
 * \file Texture.h
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Texture class for render system
 */
#pragma once

namespace Hourglass
{
	class Texture
	{
	public:
		ComPtr<ID3D11ShaderResourceView>	ShaderResourceView;
		UINT								Width, Height;

		void SetPath(const std::string path) { m_Path = path; }
		const std::string& GetPath() const { return m_Path; }

	private:
		std::string					m_Path;
	};

	class TextureManager
	{
	public:
		// Initialize texture manager
		void Init();

		// Shutdown texture manager
		void Shutdown();

		// Get texture by path
		Texture* GetTexture(const char* path) const;
		Texture* GetEnvirnmental(int i) const;
	private:
		void LoadTextures(const char* path);
		void LoadCubeMap(const char* path);
		std::vector<Texture*>		m_Textures;
		std::vector<Texture*>		m_Environmental;
	};

	extern TextureManager g_TextureManager;
}
