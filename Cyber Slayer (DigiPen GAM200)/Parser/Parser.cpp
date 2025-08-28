/*********************************************************************
 * \file   Parser.cpp
 * \brief  The wrapper for our JSON parser. To get into gritty detail, please
 *		   refer to https://rapidjson.org/md_doc_tutorial.html
 *
 * \author Evan O'Bryant
 * \date   9/17/21
 Copyright © 2022 DigiPen (USA) Corporation.
 *********************************************************************/

#pragma warning(push, 0) //disable warnings from header files that shouldn't be fixed
#include <rapidjson/filereadstream.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include "Parser.h"

#include "Component.h"
#include "FactoryComponent.h"
#include <string>

//Systems -- for function pointers
#include "AudioSystem.h"
#include "MeshSystem.h"
#include "PhysicsSystem.h"
#include "Transform.h"

#define BUFFERSIZE 16384 //16k

Parser::Parser() :
baseDoc(),
parseLog("./ParserDebug.txt"),
testLogger("./testSerialization.txt")
{}

bool Parser::GenerateDOMTree(rapidjson::Document& doc, const char* filename) {
	std::string fileLoaded; //gets currently loaded file
	if (&doc == &baseDoc)
		fileLoaded = baseFile;
	else
		fileLoaded = dataFile;
	
	if (filename == fileLoaded) //do not reload currently loaded file
		return true;

	FILE* jsonFile;
	fopen_s(&jsonFile, filename, "rb");

	Debug::ConsoleWrite("File path: %s\n", filename);
	parseLog.LogAssert(jsonFile, "file invalid");

	if (jsonFile) {
		char* buffer = new char[BUFFERSIZE];
		rapidjson::FileReadStream jsonStream(jsonFile, buffer, sizeof(buffer));

		bool noParseError = !doc.ParseStream(jsonStream).HasParseError();
		parseLog.LogAssert(noParseError, "There is an error in the JSON file!");

		fclose(jsonFile);

		if (doc.IsObject()) {
			if (fileLoaded == baseFile)
				baseFile = filename;
			else
				dataFile = filename;

			return true;
		}
	}

	return false;
}

std::string& Parser::LoadedData() {
	return dataFile;
}

PlayerFactory* Parser::ReadEntity(UID factoryID, ISystem* parentSys, std::string filename) {
	if (dataFile != filename) //generates document tree if the data file isn't loaded yet
		GenerateDOMTree(dataDoc, filename.c_str());

	testLogger.LogAssert(dataDoc.HasMember("initial position"), "initPos");
	Val pos = dataDoc["initial position"];
	testLogger.LogAssert(pos.IsArray(), "pos is arr");
	float posX = pos[0].GetFloat();
	float posY = pos[1].GetFloat();
	float posZ = pos[2].GetFloat();

	glm::vec3 posVec = { posX, posY, posZ };

	testLogger.LogAssert(dataDoc.HasMember("scale"), "scale");
	Val scale = dataDoc["scale"];
	testLogger.LogAssert(scale.IsArray(), "scale is arr");

	float scaleX = scale[0].GetFloat();
	float scaleY = scale[1].GetFloat();
	float scaleZ = scale[2].GetFloat();

	glm::vec3 scaleVec = { scaleX, scaleY, scaleZ };
	
	testLogger.LogAssert(dataDoc.HasMember("rotation"), "rotation");
	float rotation = dataDoc["rotation"].GetFloat();
	
	testLogger.LogAssert(dataDoc.HasMember("max speed"), "max speed");
	float maxSpeed = dataDoc["max speed"].GetFloat();
	
	testLogger.LogAssert(dataDoc.HasMember("collision radius"), "collision radius");
	float colRad = dataDoc["collision radius"].GetFloat();
	
	testLogger.LogAssert(dataDoc.HasMember("color"), "color");
	Val color = dataDoc["color"];
	testLogger.LogAssert(color.IsArray(), "color is arr");

	float colorR = color[0].GetFloat();
	float colorG = color[1].GetFloat();
	float colorB = color[2].GetFloat();
	float colorA = color[3].GetFloat();

	glm::vec4 colorVec = { colorR, colorG, colorB, colorA };

	return new PlayerFactory(factoryID, parentSys, scaleVec, rotation, posVec, maxSpeed, colRad, colorVec);
}

/*!
 * @brief Loads a new file to do custom parsing. (Used in factory system)
 * @param filename The name of the file to read.
 * @return If the operation was successful.
		   If the file doesn't exists, or an error occured, returns false.
*/
bool Parser::GetDataList(const std::string& filename) {
	parseLog.Write("Reading file: %s", filename.c_str());
	return GenerateDOMTree(baseDoc, filename.c_str());
}

/*!
 * @brief Loads a master json file that contains lists of other json files for data
 *		  in a scene.
 * @param category
 * @param data
 * @return If the operation was successful.
		   If the file doesn't exists, or an error occured, returns false.
*/
bool Parser::LoadData(const std::string& category, const std::string& data) {
	if (baseDoc.HasMember(category.c_str())) {
		parseLog.LogAssert(baseDoc[category.c_str()].IsObject(), "category");
		
		Val type = baseDoc[category.c_str()].GetObj();

		if (type.HasMember(data.c_str())) {
			Val dataLocation = type[data.c_str()];
			
			parseLog.LogAssert(dataLocation.IsString(), VARNAME(dataLocation));

			parseLog.Write("Getting file %s...", dataLocation.GetString());

			std::string fileLocation("./Assets/");
			fileLocation += baseDoc["data folder"].GetString(); //will crash if this doesn't exist
			fileLocation += "/";
			fileLocation += dataLocation.GetString();

			parseLog.Write("File location: %s", fileLocation.c_str());

			return GenerateDOMTree(dataDoc, fileLocation.c_str());
		}
	}

	return false;
}

bool Parser::CheckValidData() {
	if (dataFile == "") {
		parseLog.Write("No data loaded! Reading data failed!");
		return false;
	}

	return true;
}

bool Parser::ReadBool(const char* boolName, pVal subObject) {
	if (!CheckValidData())
		return false;

	bool val = false;

	if (subObject) {
		if (subObject->HasMember(boolName)) {
			parseLog.LogAssert((*subObject)[boolName].IsBool(), "bool");
			val = (*subObject)[boolName].GetBool();
			parseLog.Write("Bool read: %s", val ? "true" : "false");
		}
	}
	else if (dataDoc.HasMember(boolName)) {
		parseLog.LogAssert(dataDoc[boolName].IsBool(), "bool");
		val = dataDoc[boolName].GetBool();
		parseLog.Write("Bool read: %s", val ? "true" : "false");
	}

	return val;
}

int Parser::ReadInt(const char* intName, pVal subObject) {
	if (!CheckValidData())
		return -1;

	int val = -1;

	if (subObject) {
		if (subObject->HasMember(intName)) {
			parseLog.LogAssert((*subObject)[intName].IsInt(), "int");
			val = (*subObject)[intName].GetInt();
			parseLog.Write("Int read: %d", val);
		}
	}
	else if (dataDoc.HasMember(intName)) {
		parseLog.LogAssert(dataDoc[intName].IsInt(), "int");
		val = dataDoc[intName].GetInt();
		parseLog.Write("Int read: %d", val);
	}

	return val;
}

float Parser::ReadFloat(const char* floatName, pVal subObject) {
	if (!CheckValidData())
		return -1;

	float val = -1;

	if (subObject) {
		if (subObject->HasMember(floatName)) {
			parseLog.LogAssert((*subObject)[floatName].IsFloat(), "float");
			val = (*subObject)[floatName].GetFloat();
			parseLog.Write("Float read: %f", val);
		}
	}
	else if (dataDoc.HasMember(floatName)) {
		parseLog.LogAssert(dataDoc[floatName].IsFloat(), "float");
		val = dataDoc[floatName].GetFloat();
		parseLog.Write("Float read: %f", val);
	}

	return val;
}

/*!
 * @brief Reads in a vector and returns it. Result can be casted/converted to desired vector size.
 * @param vectorName the name of the vector to look for in the file (file given through LoadNewFile).
 * @return A 4D vector.
*/
glm::vec4 Parser::ReadVector(const char* vectorName, pVal subObject) {
	if (!CheckValidData())
		return {-1, -1, -1, -1};

	float vals[4] = { -1,-1,-1,-1 };

	if (subObject) { //reads from a subvalue inside the document if provided
		if (subObject->HasMember(vectorName)) {
			parseLog.LogAssert((*subObject)[vectorName].IsArray(), "vector");
			Val vector = (*subObject)[vectorName];
			rapidjson::SizeType size = vector.Size();

			parseLog.Write("Vector:");

			for (rapidjson::SizeType i = 0; i < size; ++i) {
				vals[i] = vector[i].GetFloat();
				parseLog.Write("%f, ", vals[i]);
			}
		}
	}
	else if (dataDoc.HasMember(vectorName)) {
		parseLog.LogAssert(dataDoc[vectorName].IsArray(), "vector");
		Val vector = dataDoc[vectorName];
		rapidjson::SizeType size = vector.Size();

		parseLog.Write("Vector:");
		
		for (rapidjson::SizeType i = 0; i < size; ++i) {
			vals[i] = vector[i].GetFloat();
			parseLog.Write("%f, ", vals[i]);
		}
	}

	return glm::make_vec4(vals);
}

std::string Parser::ReadString(const char* stringName, pVal subObject) {
	if (!CheckValidData())
		return "";

	std::string val{""};

	if (subObject) {
		if (subObject->HasMember(stringName)) {
			parseLog.LogAssert((*subObject)[stringName].IsString(), "string");
			val = (*subObject)[stringName].GetString();
			parseLog.Write("String read: %s", val.c_str());
		}
	}
	else if (dataDoc.HasMember(stringName)) {
		parseLog.LogAssert(dataDoc[stringName].IsString(), "string");
		val = dataDoc[stringName].GetString();
		parseLog.Write("String read: %s", val.c_str());
	}

	return val;
}

Parser::Val Parser::GetArray(const char* arrayName, pVal subObject) {
	if (!CheckValidData()) {
		Val v = baseDoc; //fake assignment
		return v;
	}

	if (subObject) {
		if (subObject->HasMember(arrayName)) {
			parseLog.LogAssert((*subObject)[arrayName].IsArray(), "array");
			Val v = (*subObject)[arrayName];
			
			return v;
		}
	}
	else if (dataDoc.HasMember(arrayName)) {
		parseLog.LogAssert(dataDoc[arrayName].IsArray(), "array");
		Val v = dataDoc[arrayName];
		
		return v;
	}

	Debug::Assert(false, "Failure to get array from file.");
		
	Val v = dataDoc; //dummy assignment for fake return. program will have already crashed on debug mode
	return v;
}

/*!
 * @brief Reads data for a wall into reference parameters.
 * @param wall The wall in an array of walls.
 * @param pos Output: gives position for the wall 
 * @param length Output: gives length for the wall
 * @param rotation Output: gives rotation for the wall
 * @param colliderWalls Output: gives collider bool array for the wall
*/
void Parser::ReadWallData(Val& wall, glm::vec3& pos, float& length, float& rotation, bool (&colliderWalls)[4]) {	
	pos = glm::vec3(ReadVector("position", &wall));
	length = ReadFloat("length", &wall);
	rotation = ReadFloat("rotation", &wall);

	if (wall.HasMember("collider bools")) {
		parseLog.LogAssert(wall["collider bools"].IsArray(), "collider bools");
		
		Val colliderBools = wall["collider bools"];
		rapidjson::SizeType size = colliderBools.Size();

		parseLog.Write("Bools: ");

		for (rapidjson::SizeType i = 0; i < size; ++i) {
			colliderWalls[i] = colliderBools[i].GetBool();
			parseLog.Write("%f, ", colliderWalls[i]);
		}
	}
}

/*!
 * @brief Reads data for a pit into reference parameters. 
 * @param pit The pit in an array of pits to be read in.
 * @param pos Output: gives position for the pit
 * @param radius Output: gives the radius for the pit
*/
void Parser::ReadPitData(Val& pit, glm::vec3& pos, float& length, float& rotation) {
	pos = glm::vec3(ReadVector("position", &pit));
	length = ReadFloat("length", &pit);
	rotation = ReadFloat("rotation", &pit);
}

std::string Parser::ReadImagePath(const char* imageName) {
	std::string filePath = ReadString(imageName);
	return "./Assets/images/" + filePath;
}

std::string Parser::ReadImagePath(const std::string& imageName) {
	return "./Assets/images/" + ReadString(imageName.c_str());
}

std::string Parser::ReadAnimationPath(const std::string& aniName) {
	return "./Assets/images/animations/" + ReadString(aniName.c_str());
}
