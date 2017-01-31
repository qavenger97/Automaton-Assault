/*!
 * \file Material.cpp
 *
 * \author Stanley
 * \date October 2016
 *
 * 
 */

#include "pch.h"

#include "Material.h"
#include "Core\FileUtils.h"
#include "Texture.h"
#include "CommonDef.h"

Hourglass::MaterialManager Hourglass::g_MaterialManager;

void Hourglass::MaterialManager::Init()
{
	LoadMateriales("Assets/Material/");
	m_DefaultMaterial = GetMaterial("Assets/Material/Default-Material.xml");
}

void Hourglass::MaterialManager::Shutdown()
{
	for (std::vector<Material*>::iterator iter = m_Materials.begin(); iter != m_Materials.end(); iter++)
	{
		delete *iter;
	}
	m_Materials.clear();
}

Hourglass::Material* Hourglass::MaterialManager::GetMaterial(const char* path) const
{
	std::string matPath(path);
	FileUtils::TrimPathDelimiter(matPath);

	std::string paths = path;

	for (int i = 0; i < m_Materials.size(); i++)
	{
		if (FileUtils::iequals(matPath, m_Materials[i]->GetPath()))
			return m_Materials[i];
	}

	return nullptr;
}

Hourglass::Material* Hourglass::MaterialManager::GetDefaultMaterial() const
{
	return m_DefaultMaterial;
}

void Hourglass::MaterialManager::LoadMateriales(const char* path)
{
	char searchPath[MAX_PATH];
	strcpy_s(searchPath, path);
	strcat_s(searchPath, "\\*.*");

	FileUtils::IterateFolderForFiles(searchPath, ".xml", [&](const char* filename) {
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename);

		tinyxml2::XMLElement * xmlMat = doc.FirstChildElement("Material");
		if (xmlMat)
		{
			Material* mat = new Material();
			ZeroMemory(mat->m_Textures, sizeof(Texture*) * 8);

			// Get trimmed path name
			std::string matPath = filename;
			FileUtils::TrimPathDelimiter(matPath);

			mat->SetPath(matPath);

			// Get material pass
			mat->m_BlendMode = kBlend_Opaque;
			const char* passName = xmlMat->Attribute("blend");

			if (passName)
			{
				if (strcmp(passName, "AlphaBlend") == 0)
				{
					mat->m_BlendMode = kBlend_AlphaBlend;
				}
				else if (strcmp(passName, "AlphaToCoverage") == 0)
				{
					mat->m_BlendMode = kBlend_AlphaToCoverage;
				}
				else if (strcmp(passName, "Additive") == 0)
				{
					mat->m_BlendMode = kBlend_Additive;
				}
			}


			// Get material color
			mat->m_UniformColor = Color(1, 1, 1, 1);

			tinyxml2::XMLElement* xmlColor = xmlMat->FirstChildElement("UniformColor");
			if (xmlColor)
			{
				xmlColor->QueryFloatAttribute("r", &mat->m_UniformColor.x);
				xmlColor->QueryFloatAttribute("g", &mat->m_UniformColor.y);
				xmlColor->QueryFloatAttribute("b", &mat->m_UniformColor.z);
				xmlColor->QueryFloatAttribute("a", &mat->m_UniformColor.w);
			}

			// Get all textures from child nodes
			tinyxml2::XMLElement* xmlTex = xmlMat->FirstChildElement("Texture");

			while (xmlTex)
			{
				int slot = -1;
				xmlTex->QueryIntAttribute("slot", &slot);

				const char* texName = xmlTex->Attribute("file");

				mat->m_Textures[slot] = g_TextureManager.GetTexture(texName);

				xmlTex = xmlTex->NextSiblingElement("Texture");
			}

			m_Materials.push_back(mat);
		}
	});
}
