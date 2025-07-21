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

	// 表示順序を設定
	void SetDisplayOrder(const std::string& groupName, const std::vector<std::string>& order);

	// 表示順序を取得
	const std::vector<std::string>& GetDisplayOrder(const std::string& groupName) const;

	// CollapsingHeaderグループ設定
	void SetHeaderGroup(const std::string& groupName, const std::string& itemKey, const std::string& headerName);
	// CollapsingHeaderグループ取得
	std::string GetHeaderGroup(const std::string& groupName, const std::string& itemKey) const;

	// CollapsingHeaderグループ設定
	void SetHeaderGroup(const std::string& groupName, const std::string& headerName);
	// CollapsingHeaderグループ解除
	void EndHeaderGroup(const std::string& groupName);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;
	
	using json = nlohmann::json;
	using Item = std::variant<bool, int32_t, float, Vector2, Vector3, Vector4, std::string, Transform>;
	using Group = std::map<std::string, Item>;
	// 全データ
	std::map<std::string, Group> datas_;

	// 表示順序: groupName -> vector of item keys
	std::map<std::string, std::vector<std::string>> displayOrders_;

	// CollapsingHeaderグループ: groupName -> (itemKey -> headerName)
	std::map<std::string, std::map<std::string, std::string>> headerGroups_;
	// 現在のヘッダーグループ: groupName -> headerName (空文字列で無効)
	std::map<std::string, std::string> currentHeaderGroup_;

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "resources/GlobalVariables/";
};