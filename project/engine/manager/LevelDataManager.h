#pragma once
#include <unordered_map>
#include <json.hpp>
#include "Matrix4x4.h"

struct ObjectData {
	Vector3 translation{};
	Vector3 rotation{};
	Vector3 scaling{1.0f,1.0f,1.0f};
	std::string fileName = "";
	std::string typeName = "";
	std::vector<std::unique_ptr<ObjectData>> children;
};

class LevelDataManager{
public:
	using LevelData = std::vector<std::unique_ptr<ObjectData>>;
public:
	LevelDataManager() = default;
	~LevelDataManager() = default;
	void LoadJsonFile(const std::string& filePath);
	std::unique_ptr<ObjectData> LoadObjectData(const nlohmann::json& object);
	void Clear() { levelDates_.clear(); }
	LevelData* GetObjectData(const std::string& filePath) {return levelDates_[filePath].get();}
	
private:
	const std::string kDirectoryFilePath = "./resources/level_data/";
	const std::string kExtension = ".json";
	std::unordered_map<std::string, std::unique_ptr<LevelData>> levelDates_;
};

