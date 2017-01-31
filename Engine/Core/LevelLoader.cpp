#include "pch.h"

#include "LevelLoader.h"
#include "System\EntityManager.h"
#include "System\AISystem.h"
#include "System\PhysicsSystem.h"

void Hourglass::LevelLoader::Init(AISystem* aiSys)
{
	aiSys->LoadBehaviors();
	LoadAssembledXML();
	InitLevel(aiSys);
}

void Hourglass::LevelLoader::InitLevel(AISystem * aiSys)
{
	LoadStartLevelXML(aiSys);
	g_Physics.BuildQuadtree();
}

void Hourglass::LevelLoader::LoadLevelXML( const char* path, AISystem* aiSys )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( path );

	// Get the level root
	tinyxml2::XMLElement * xmlLevel = doc.FirstChildElement();
	assert( strcmp( xmlLevel->Value(), "Level" ) == 0 );

	// Get the Entities
	tinyxml2::XMLElement* xmlEntities = xmlLevel->FirstChildElement( "Entities" );

	// If there are top-level elements, process them and their children
	tinyxml2::XMLElement* xmlElement = xmlEntities->FirstChildElement();
	int counter = 0;
	while (xmlElement != nullptr)
	{
		const char* name = xmlElement->Name();

		if (strcmp(name, "Entity") == 0)
		{
			counter++;
			Entity* entity = Entity::Create();
			g_EntityManager.LoadFromXML(entity, xmlElement);
			entity->GetTransform()->Reset();
		}
		else if (strcmp(name, "Assembled") == 0)
		{
			const char* assembledName = xmlElement->Attribute("name");
			Entity* assembledEntity = Entity::Assemble(WSID(assembledName));
			g_EntityManager.LoadFromXML(assembledEntity, xmlElement);
		}

		xmlElement = xmlElement->NextSiblingElement();
	}

	tinyxml2::XMLElement* xmlWaypoints = xmlLevel->FirstChildElement( "Waypoints" );

	if (xmlWaypoints)
		aiSys->LoadWaypoints( xmlWaypoints );
}
	

void Hourglass::LevelLoader::LoadStartLevelXML(AISystem* aiSys)
{
	char configPath[FILENAME_MAX] = "Assets\\XML\\Config.XML";
	char levelFolder[FILENAME_MAX] = "Assets\\XML\\Level\\";

	// We will be reading in this path from the Config File
	char levelPath[FILENAME_MAX];

	// The file name for the start level
	char levelFileName[FILENAME_MAX];

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError errorResult = doc.LoadFile(configPath);
	assert(errorResult == tinyxml2::XML_SUCCESS);

	// Get the config root
	tinyxml2::XMLElement * xmlRoot = doc.FirstChildElement();
	assert(strcmp(xmlRoot->Value(), "Config") == 0);

	tinyxml2::XMLElement* levelXML = xmlRoot->FirstChildElement("Level");
	do
	{
		strcpy_s(levelFileName, levelXML->FirstChild()->Value());

		// Construct the full level path
		strcpy_s(levelPath, levelFolder);
		strcat_s(levelPath, levelFileName);

		LoadLevelXML(levelPath, aiSys);
	} while (levelXML = levelXML->NextSiblingElement("Level"));
}

void Hourglass::LevelLoader::LoadAssembledXML()
{
	char folder[FILENAME_MAX] = "Assets\\XML\\Assembled\\";
	char filter[FILENAME_MAX];
	strcpy_s( filter, folder );
	strcat_s( filter, "*.xml" );

	WIN32_FIND_DATAA fileData;
	HANDLE searchHandle = FindFirstFileA( filter, &fileData );
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Piece together the full path from folder + file name
			char fullpath[FILENAME_MAX];
			strcpy_s( fullpath, folder );
			strcat_s( fullpath, fileData.cFileName );

			// Load a single document from the Assembled folder
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLError errorResult = doc.LoadFile( fullpath );
			assert( errorResult == tinyxml2::XML_SUCCESS );
			tinyxml2::XMLElement* xmlRoot = doc.FirstChildElement();
			assert( strcmp( xmlRoot->Value(), "Assembled" ) == 0 );
			g_EntityManager.CreateAssembledFromXML( xmlRoot );

		} while (FindNextFileA( searchHandle, &fileData ));

		FindClose( searchHandle );
	}
}
