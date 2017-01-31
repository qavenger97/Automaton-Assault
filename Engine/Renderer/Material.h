/*!
 * \file Material.h
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * 
 */
#pragma once

namespace Hourglass
{
	class Texture;

	class Material
	{
	public:
		int			m_BlendMode;
		Color		m_UniformColor;
		Texture*	m_Textures[8];

		void SetPath(const std::string path) { m_Path = path; }
		const std::string& GetPath() const { return m_Path; }
	private:
		std::string					m_Path;
	};

	class MaterialManager
	{
	public:
		// Initialize material manager
		void Init();

		// Shutdown material manager
		void Shutdown();

		// Get a material by its path
		Material* GetMaterial(const char* path) const;

		Material* GetDefaultMaterial() const;
	private:
		// Load materials from 'Assets/Material' folder
		void LoadMateriales(const char* path);

		std::vector<Material*>		m_Materials;
		Material*					m_DefaultMaterial;
	};

	extern MaterialManager g_MaterialManager;
}