#pragma once
#include <string>

class Resource
{
protected:
	unsigned int handle;
	unsigned int refCount;
	std::string _path;
	std::string _name;

public:

	Resource() {}

	Resource(std::string path, std::string name) : _path(path), _name(name) {}

	~Resource() {}

	inline void incRef() { ++refCount; }
	
	inline void decRef() { --refCount; }

	inline const std::string& getPath() { return _path; }

	inline const std::string& getName() { return _name; }
};