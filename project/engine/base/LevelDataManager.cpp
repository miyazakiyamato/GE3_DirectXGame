#include "LevelDataManager.h"
#include <iostream>
#include <fstream>
#include <cassert>

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
		std::unique_ptr<ObjectData> objectData = LoadObjectData(object);
		if (!objectData) {
			continue; // nullptrが返された場合はスキップ
		}
		levelData->push_back(std::move(objectData));
	}
	levelDates_[filePath] = std::move(levelData);
}

std::unique_ptr<ObjectData> LevelDataManager::LoadObjectData(const nlohmann::json& object){
	//assert(object.contains("type"));
	if (!object.contains("type")) {
		return nullptr; // typeが存在しない場合はnullptrを返す
	}
	std::string type = object["type"].get<std::string>();
	
	std::unique_ptr<ObjectData> objectData = std::make_unique<ObjectData>();

	objectData->typeName = type;
	if (object.contains("transform")) {
		const nlohmann::json& transform = object["transform"];
		//位置
		objectData->translation.x = (float)transform["translation"][0];
		objectData->translation.y = (float)transform["translation"][2];
		objectData->translation.z = (float)transform["translation"][1];
		//回転
		objectData->rotation.x = -(float)transform["rotation"][0];
		objectData->rotation.y = -(float)transform["rotation"][2];
		objectData->rotation.z = -(float)transform["rotation"][1];
		//大きさ
		objectData->scaling.x = (float)transform["scaling"][0];
		objectData->scaling.y = (float)transform["scaling"][2];
		objectData->scaling.z = (float)transform["scaling"][1];
		if (object.contains("disabled")) {
			bool disabled = object["disabled"];
			if (disabled) {
				return nullptr; // disabledがtrueの場合はnullptrを返す
			}
		}
		if (object.contains("file_name")) {
			objectData->fileName = object["file_name"];
		}
		if (object.contains("children")) {
			for (const auto& childJson : object["children"]) {
				auto child = LoadObjectData(childJson);
				if (child) {
					objectData->children.push_back(std::move(child));
				}
			}
		}
		return std::move(objectData);
	}
	return nullptr;
}