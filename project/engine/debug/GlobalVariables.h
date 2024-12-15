#pragma once
#include <variant>
#include <map>
#include <string>
#include <json.hpp>
#include "Matrix4x4.h"


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

	void ShowCombo(const std::string& label, const std::vector<std::string>& items, int& selectedIndex, std::function<void(const std::string&)> onSelect);
	void ShowCombo(const std::string& label, const std::vector<std::string>& items, int& selectedIndex, std::function<void(const int&)> onSelect);

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
	using Item = std::variant<bool, int32_t, float, Vector2, Vector3, Vector4, std::string>;
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
	
	template<typename T>
	//値を取得
	T GetValue(const std::string& groupName, const std::string& key) const; 
};