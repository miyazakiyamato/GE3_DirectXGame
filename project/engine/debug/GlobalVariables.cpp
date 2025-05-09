#include "GlobalVariables.h"
#include "imgui.h"
#include <WinApp.h>
#include <iostream>
#include <fstream>

GlobalVariables* GlobalVariables::GetInstance() { 
	static GlobalVariables instance;
	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトがなければ追加する
	datas_[groupName];
}

void GlobalVariables::Update() {
	if (!ImGui::Begin("Global Variables",nullptr,ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
	}
	if (!ImGui::BeginMenuBar()) {return;}
	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();++itGroup) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {continue;}
		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.begin(); itItem != group.end();++itItem) {
			//各項目を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			// 制御文字を削除した表示用の名前を生成
			std::string displayName = itemName;
			displayName.erase(std::remove_if(displayName.begin(), displayName.end(),
				[](unsigned char c) { return std::iscntrl(c); }),
				displayName.end());

			// bool型の値を保持していれば
			if (std::holds_alternative<bool>(item)) {
				bool* ptr = std::get_if<bool>(&item);
				ImGui::Checkbox(displayName.c_str(), ptr);
			}
			//int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::DragInt(displayName.c_str(), ptr, 1.0f);
			}
			// float型の値を保持していれば
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(displayName.c_str(), ptr,0.01f);
			}
			//Vector2型の値を保持していれば
			else if (std::holds_alternative<Vector2>(item)) {
				Vector2* ptr = std::get_if<Vector2>(&item);
				ImGui::DragFloat2(displayName.c_str(), reinterpret_cast<float*>(ptr),0.01f);
			}
			//Vector3型の値を保持していれば
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::DragFloat3(displayName.c_str(), reinterpret_cast<float*>(ptr),0.01f);
			}
			//Vector4型の値を保持していれば
			else if (std::holds_alternative<Vector4>(item)) {
				Vector4* ptr = std::get_if<Vector4>(&item);
				ImGui::ColorEdit4(displayName.c_str(), reinterpret_cast<float*>(ptr));
			}
			//std::string型の値を保持していれば
			else if (std::holds_alternative<std::string>(item)) {
				std::string* ptr = std::get_if<std::string>(&item);
				ImGui::Text((displayName + " " + *ptr).c_str());
			}
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}
void GlobalVariables::ShowCombo(const std::string& label, const std::vector<std::string>& items,
	int& selectedIndex, std::function<void(const std::string&)> onSelect) {
	if (items.empty()) return;
	const char* currentItem = items[selectedIndex].c_str();
	if (ImGui::BeginCombo(label.c_str(), currentItem)) {
		for (int i = 0; i < items.size(); ++i) {
			bool isSelected = (selectedIndex == i);
			if (ImGui::Selectable(items[i].c_str(), isSelected)) {
				selectedIndex = i;
				onSelect(items[i]); // 選択時にコールバックを呼び出す
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}
void GlobalVariables::ShowCombo(const std::string& label, const std::vector<std::string>& items,
	int& selectedIndex, std::function<void(const int&)> onSelect) {
	if (items.empty()) return;
	const char* currentItem = items[selectedIndex].c_str();
	if (ImGui::BeginCombo(label.c_str(), currentItem)) {
		for (int i = 0; i < items.size(); ++i) {
			bool isSelected = (selectedIndex == i);
			if (ImGui::Selectable(items[i].c_str(), isSelected)) {
				selectedIndex = i;
				onSelect(i); // 選択時にコールバックを呼び出す
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}
void GlobalVariables::SaveFile(const std::string& groupName) {
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	//未登録チェック
	assert(itGroup != datas_.end());

	json root;

	root = json::object();

	//jsonオブジェクト登録
	root[groupName] = json::object();

	//各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin(); itItem != itGroup->second.end(); ++itItem) {
		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		Item& item = itItem->second;

		// bool型の値を保持していれば
		if (std::holds_alternative<bool>(item)) {
			// bool型の値を登録
			root[groupName][itemName] = std::get<bool>(item);
		}
		// int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item)) {
			//int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		// float型の値を保持していれば
		else if (std::holds_alternative<float>(item)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item);
		}
		// Vector2型の値を保持していれば
		else if (std::holds_alternative<Vector2>(item)) {
			// Vector2型の値を登録
			Vector2 value = std::get<Vector2>(item);
			root[groupName][itemName] = json::array({value.x, value.y});
		}
		// Vector3型の値を保持していれば
		else if (std::holds_alternative<Vector3>(item)) {
			// Vector3型の値を登録
			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = json::array({value.x, value.y, value.z});
		}
		// Vector4型の値を保持していれば
		else if (std::holds_alternative<Vector4>(item)) {
			// Vector4型の値を登録
			Vector4 value = std::get<Vector4>(item);
			root[groupName][itemName] = json::array({value.x, value.y, value.z,value.w});
		}
		// std::string型の値を保持していれば
		else if (std::holds_alternative<std::string>(item)) {
			// std::string型の値を登録
			root[groupName][itemName] = std::get<std::string>(item);
		}
	}
	
	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}
	//書き込むJSONファイルのフルパスを合成
	std::string filePath = kDirectoryPath + groupName + ".json";
	//書き込む用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込みように開く
	ofs.open(filePath);

	//ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

}

void GlobalVariables::LoadFiles() {
	// ディレクトリがなければスキップ
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(dir);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();
		
		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}
		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	//読み込むJSONファイルのフルパスを合成
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		// bool型の値を保持していれば
		if (itItem->is_boolean()) {
			// bool型の値を登録
			bool value = itItem->get<bool>();
			SetValue(groupName, itemName, value);
		}
		//int32_t型の値を保持していれば
		else if (itItem->is_number_integer()) {
			//int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		// float型の値を保持していれば
		else if (itItem->is_number_float()) {
			// float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName,static_cast<float>(value));
		} 
		//要素数2の配列であれば
		else if (itItem->is_array() && itItem->size() == 2) {
			//float型のjson配列登録
			Vector2 value = {itItem->at(0), itItem->at(1)};
			SetValue(groupName, itemName, value);
		}
		//要素数3の配列であれば
		else if (itItem->is_array() && itItem->size() == 3) {
			//float型のjson配列登録
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
		//要素数4の配列であれば
		else if (itItem->is_array() && itItem->size() == 4) {
			//float型のjson配列登録
			Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2),itItem->at(3) };
			SetValue(groupName, itemName, value);
		}
		// std::string型の値を保持していれば
		else if (itItem->is_string()) {
			// std::string型の値を登録
			std::string value = itItem->get<std::string>();
			SetValue(groupName, itemName, static_cast<std::string>(value));
		}
	}
}

template<typename T>
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, T value) {
	//グループの参照を取得
	Group& group = datas_[groupName];
	
	// 設定した項目を追加
	group[key] = value;
}
// 明示的インスタンス化
template void GlobalVariables::SetValue<bool>(const std::string&, const std::string&, bool);
template void GlobalVariables::SetValue<int>(const std::string&, const std::string&, int);
template void GlobalVariables::SetValue<float>(const std::string&, const std::string&, float);
template void GlobalVariables::SetValue<Vector2>(const std::string&, const std::string&, Vector2);
template void GlobalVariables::SetValue<Vector3>(const std::string&, const std::string&, Vector3);
template void GlobalVariables::SetValue<Vector4>(const std::string&, const std::string&, Vector4);
template void GlobalVariables::SetValue<std::string>(const std::string&, const std::string&, std::string);

template<typename T>
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, T value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());
	
	std::map<std::string, Item>::iterator itItem = itGroup->second.begin();

	if (!itGroup->second.contains(key)) {
		SetValue(groupName, key, value);
	}
}
// 明示的インスタンス化
template void GlobalVariables::AddItem<bool>(const std::string&, const std::string&, bool);
template void GlobalVariables::AddItem<int>(const std::string&, const std::string&, int);
template void GlobalVariables::AddItem<float>(const std::string&, const std::string&, float);
template void GlobalVariables::AddItem<Vector2>(const std::string&, const std::string&, Vector2);
template void GlobalVariables::AddItem<Vector3>(const std::string&, const std::string&, Vector3);
template void GlobalVariables::AddItem<Vector4>(const std::string&, const std::string&, Vector4);
template void GlobalVariables::AddItem<std::string>(const std::string&, const std::string&, std::string);

template<typename T>
T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) const{
	assert(datas_.contains(groupName));

	// グループの参照を取得
	const Group& group = datas_.at(groupName);

	if (group.find(key) == group.end()) {
		return T();
	}

	return std::get<T>(group.at(key));
}
// 明示的インスタンス化
template bool GlobalVariables::GetValue<bool>(const std::string&, const std::string&) const;
template int GlobalVariables::GetValue<int>(const std::string&, const std::string&) const;
template float GlobalVariables::GetValue<float>(const std::string&, const std::string&) const;
template Vector2 GlobalVariables::GetValue<Vector2>(const std::string&, const std::string&) const;
template Vector3 GlobalVariables::GetValue<Vector3>(const std::string&, const std::string&) const;
template Vector4 GlobalVariables::GetValue<Vector4>(const std::string&, const std::string&) const;
template std::string GlobalVariables::GetValue<std::string>(const std::string&, const std::string&) const;