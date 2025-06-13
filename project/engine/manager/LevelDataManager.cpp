#include "LevelDataManager.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <json.hpp>

void LevelDataManager::LoadJsonFile(const std::string& filePath){
	std::string fullpath = kDirectoryFilePath + filePath + kExtension;

	std::ifstream file(fullpath);
	if (file.fail()) {
		std::cerr << "ファイルを開けませんでした: " << fullpath << std::endl;
		assert(0);
	}

	nlohmann::json deserialized;
	try {
		file >> deserialized;
	}
	catch (const std::exception& e) {
		std::cerr << "JSONのパースに失敗しました: " << e.what() << std::endl;
		assert(0);
	}

	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	std::string name = deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();
	levelData->reserve(100);
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));
		std::string type = object["type"].get<std::string>();

		if (type.compare("MESH") == 0) {
			levelData->emplace_back(ObjectData{});
			ObjectData& objectData = levelData->back();

			nlohmann::json& transform = object["transform"];
			objectData.translation.x = (float)transform["translation"][0];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = (float)transform["translation"][1];

			objectData.rotation.x = -(float)transform["rotation"][0];
			objectData.rotation.y = -(float)transform["rotation"][2];
			objectData.rotation.z = -(float)transform["rotation"][1];

			objectData.scaling.x = (float)transform["scaling"][0];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][1];
			if (object.contains("file_name")) {
				objectData.fileName = object["file_name"];
			}
			if (object.contains("children")) {

			}
		}
	}
	levelDates_[filePath] = std::move(levelData);
}