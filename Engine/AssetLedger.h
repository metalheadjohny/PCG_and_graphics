#pragma once
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/unordered_set.hpp>
#include <unordered_set>
#include <string>
#include <fstream>
#include "ResourceHandle.h"	// TBD, should act as a proxy as well?
#include "ResourceDef.h"



class AssetLedger
{
	// Gigabrain implementation with 0 effort, 0 cache locality and maximum fragmentation
	std::unordered_set<ResourceDef> _assetMap;	// _assSet just sounds weird doesn't it?

	// This will write it all out at once, something I don't like one bit (might use sqlite)
	template <typename Archive>
	void serialize(Archive& ar)
	{
		ar(_assetMap);
	}

public:


	uint32_t add(const std::string& assName, const std::string& path, ResType resType)
	{
		ResourceDef rd{ fnv1hash(assName), assName, path, resType };

		if (!_assetMap.insert(rd).second)
		{
			assert(false && "HASH COLLISION! Asset name: %s", assName);
		}
	}



	const ResourceDef& get(uint32_t ID)
	{

	}



	void remove(uint32_t ID)
	{
		_assetMap.erase(ResourceDef{ID, 0});
	}
	

	void load(const std::string& path)
	{
		std::ifstream ifs(path);
		cereal::JSONInputArchive jiArch(ifs);
		serialize(jiArch);
	}


	void save(const std::string& path)
	{
		std::ofstream ofs(path);
		cereal::JSONOutputArchive joArch(ofs);
		serialize(joArch);
	}
};