/*********************************************************************
 * \file   Parser.h
 * \brief  This is a file.
 *
 * \author Evan O'Bryant (evan.o)
 * \date   9/17/21
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once

#pragma warning(push, 0) //disable warnings from header files that can't be fixed
#include <rapidjson/document.h>
#pragma warning(pop)

#include "Debug.h"
#include "FunctionTypedefs.h"
#include "ID.h"
#include <map>
#include <string>
#include <vector>

class IComponent;
class ISystem;
class PlayerFactory;

class Parser {
	//---functions---
public:
	using Val = const rapidjson::Value&;
	using pVal = const rapidjson::Value*;

	Parser();
	~Parser() {}

	//simple parser
	bool GetDataList(const std::string& filename); //simply loads a file for specialized use rather than reading all data at once
	bool LoadData(const std::string& category, const std::string& data);

	//getters
	std::string& LoadedData();

	//get array
	Val GetArray(const char* arrayName, pVal subObject = nullptr);

	//reading individual PODs
	bool ReadBool(const char* boolName, pVal subObject = nullptr);
	int ReadInt(const char* intName, pVal subObject = nullptr);
	float ReadFloat(const char* floatName, pVal subObject = nullptr);
	glm::vec4 ReadVector(const char* vectorName, pVal subObject = nullptr);
	std::string ReadString(const char* stringName, pVal subObject = nullptr);

	//reading in mass data
	void ReadWallData(Val& wall, glm::vec3& pos, float& length, float& rotation, bool(&colliderWalls)[4]);
	void ReadPitData(Val& pit, glm::vec3& pos, float& length, float& rotation);
	PlayerFactory* ReadEntity(UID factoryID, ISystem* parentSys, std::string filename);
	std::string ReadImagePath(const char* imageName);
	std::string ReadImagePath(const std::string& imageName);
	std::string ReadAnimationPath(const std::string& aniName);

private:
	//---funcs---
	bool GenerateDOMTree(rapidjson::Document& doc, const char* filename);
	bool CheckValidData();
	
	//---data---
	rapidjson::Document baseDoc; //json for whole scene
	rapidjson::Document dataDoc; //json for data in scene
	std::string baseFile;
	std::string dataFile;
	std::string dataSubFolder;

	std::string recentImagePath; //member variable to prevent errors in "readimagefile"

	Debug::Logger parseLog; //logger
};