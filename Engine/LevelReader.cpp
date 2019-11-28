#include "LevelReader.h"
#include "FileUtilities.h"
#include "Mesh.h"
#include "Model.h"


LevelReader::LevelReader() {}



bool LevelReader::loadLevel(const std::string& levelPath)
{
	rapidjson::Document sceneDef;
	sceneDef.Parse(FileUtils::loadFileContents(_projectPath + levelPath).c_str());

	if (!sceneDef.IsObject())
		return false;
	
	if(!loadLevelDef(sceneDef))
		return false;
	
	if (!loadResourceDefs(sceneDef))
		return false;

	return true;
}



bool LevelReader::loadLevelDef(const rapidjson::Document& levelDef)
{
#pragma push_macro("GetObject")	//THANK YOU WINDOWS, THANKS SO MUCH WOWWWWW
#undef GetObject

	_ld.id = levelDef.FindMember("id")->value.GetInt();
	_ld.name = levelDef.FindMember("name")->value.GetString();
	_ld.description = levelDef.FindMember("description")->value.GetString();
	_ld.projName = levelDef.FindMember("project")->value.GetString();
	_ld.jsonPath = levelDef.FindMember("jsonDefPath")->value.GetString();
	_ld.createdAt = levelDef.FindMember("createdAt")->value.GetString();
	_ld.updatedAt = levelDef.FindMember("updatedAt")->value.GetString();

	return true;

#pragma pop_macro("GetObject")
}



bool LevelReader::loadResourceDefs(const rapidjson::Document& levelDef)
{
	if (!levelDef.FindMember("assets")->value.IsArray())
		return false;

	auto assDefArr = levelDef.FindMember("assets")->value.GetArray();

	ResourceDef ad;
	for (rapidjson::Value::ConstValueIterator itr = assDefArr.Begin(); itr != assDefArr.End(); ++itr)
	{
		ad.id = itr->FindMember("id")->value.GetInt();
		ad.path = itr->FindMember("path")->value.GetString();
		ad.name = itr->FindMember("name")->value.GetString();
		_resourceDefs.push_back(ad);
	}
	return true;
}



void LevelReader::setProjectData(const std::string & projectPath)	//, const std::vector<std::string>& levelPaths
{
	_projectPath = projectPath;
}



const std::vector<ResourceDef>& LevelReader::getLevelResourceDefs()
{
	return _resourceDefs;
}



void LevelReader::clearLevelResourceDefs()
{
	_resourceDefs.clear();
}