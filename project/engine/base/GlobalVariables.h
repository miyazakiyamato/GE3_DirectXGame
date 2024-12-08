#pragma once
#include <variant>
#include <map>
#include <string>
#include <json.hpp>
#include "Vector3.h"


class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

	/// <summary>
	/// グループの作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// ファイルに書き出し
	/// </summary>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルから読み込む 
	/// </summary>
	void LoadFile(const std::string& groupName);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;
	
	using json = nlohmann::json;
	using Item = std::variant<bool,int32_t, float, Vector3>;
	using Group = std::map<std::string, Item>;
	// 全データ
	std::map<std::string, Group> datas_;

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "resources/GlobalVariables/";

public:
	template <typename T>
	//値のセット
	void SetValue(const std::string& groupName, const std::string& key, T value);

	template<typename T>
	//項目の追加
	void AddItem(const std::string& groupName, const std::string& key, T value);
	
	bool GetBoolValue(const std::string& groupName, const std::string& key) const;
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;

	//template<typename T>
	////値を取得
	//T GetValue(const std::string& groupName, const std::string& key) const; 
};

template<typename T> inline void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, T value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	
	// 設定した項目を追加
	group[key] = value;
}

template<typename T> inline void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, T value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());
	
	std::map<std::string, Item>::iterator itItem = itGroup->second.begin();

	if (!itGroup->second.contains(key)) {
		SetValue(groupName, key, value);
	}
}

//template<typename T> inline T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) const {
//	assert(datas_.contains(groupName));
//
//	// グループの参照を取得
//	const Group& group = datas_.at(groupName);
//
//	assert(group.items.contains(key));
//
//	//
//	return std::get<T>(group.items.at(key).value);
//}
