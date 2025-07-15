#include "GlobalVariables.h"
#include "imgui.h"
#include <WinApp.h>
#include <iostream>
#include <fstream>
#include <set>

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
		const std::string& groupName = itGroup->first;
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {continue;}

		// --- CollapsingHeaderグループ分け ---
		// 1. ヘッダーごとにアイテムをまとめる
		std::map<std::string, std::vector<std::string>> headerToItems;
		const std::vector<std::string>& order = GetDisplayOrder(groupName);
		for (const std::string& itemName : order) {
			std::string header = GetHeaderGroup(groupName, itemName);
			headerToItems[header].push_back(itemName);
		}
		// 2. 描画済み管理
		std::set<std::string> drawn;
		// 3. 各ヘッダーで描画
		static std::map<std::string, bool> headerOpenStates;
		for (auto& [header, items] : headerToItems) {
			// 空文字列ヘッダーはグループ化しない
			bool useHeader = !header.empty();
			bool open = true;
			std::string headerKey = groupName + "/" + header;
			if (useHeader) {
				// ImGui::CollapsingHeaderの戻り値で開閉状態を管理
				open = ImGui::CollapsingHeader(header.c_str());
				headerOpenStates[headerKey] = open;
			}
			if (open) {
				for (const std::string& itemName : items) {
					auto itItem = group.find(itemName);
					if (itItem == group.end()) continue;
					drawn.insert(itemName);
					Item& item = itItem->second;

					std::string displayName = itemName;
					displayName.erase(std::remove_if(displayName.begin(), displayName.end(),
						[](unsigned char c) { return std::iscntrl(c); }),
						displayName.end());

					if (std::holds_alternative<bool>(item)) {
						bool* ptr = std::get_if<bool>(&item);
						ImGui::Checkbox(displayName.c_str(), ptr);
					}
					if (std::holds_alternative<int32_t>(item)) {
						int32_t* ptr = std::get_if<int32_t>(&item);
						ImGui::DragInt(displayName.c_str(), ptr, 1.0f);
					}
					else if (std::holds_alternative<float>(item)) {
						float* ptr = std::get_if<float>(&item);
						ImGui::DragFloat(displayName.c_str(), ptr,0.01f);
					}
					else if (std::holds_alternative<Vector2>(item)) {
						Vector2* ptr = std::get_if<Vector2>(&item);
						ImGui::DragFloat2(displayName.c_str(), reinterpret_cast<float*>(ptr),0.01f);
					}
					else if (std::holds_alternative<Vector3>(item)) {
						Vector3* ptr = std::get_if<Vector3>(&item);
						ImGui::DragFloat3(displayName.c_str(), reinterpret_cast<float*>(ptr),0.01f);
					}
					else if (std::holds_alternative<Vector4>(item)) {
						Vector4* ptr = std::get_if<Vector4>(&item);
						ImGui::ColorEdit4(displayName.c_str(), reinterpret_cast<float*>(ptr));
					}
					else if (std::holds_alternative<std::string>(item)) {
						std::string* ptr = std::get_if<std::string>(&item);
						ImGui::Text((displayName + " " + *ptr).c_str());
					} 
					else if (std::holds_alternative<Transform>(item)) {
						Transform* ptr = std::get_if<Transform>(&item);
						ImGui::Indent();
						if (ptr && ImGui::CollapsingHeader(displayName.c_str())) {
							ImGui::DragFloat3((displayName + ".Scale").c_str(), &ptr->scale.x, 0.01f);
							ImGui::SliderAngle((displayName + ".Rotate.x").c_str(), &ptr->rotate.x);
							ImGui::SliderAngle((displayName + ".Rotate.y").c_str(), &ptr->rotate.y);
							ImGui::SliderAngle((displayName + ".Rotate.z").c_str(), &ptr->rotate.z);
							ImGui::DragFloat3((displayName + ".Translate").c_str(), &ptr->translate.x, 0.01f);
						}
						ImGui::Unindent();
					}
				}
			}
		}
		// --- CollapsingHeaderグループ分けここまで ---

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
		// Transform型の値を保持していれば
		else if (std::holds_alternative<Transform>(item)) {
			Transform value = std::get<Transform>(item);
			root[groupName][itemName] = json::object({
				{"scale", {value.scale.x, value.scale.y, value.scale.z}},
				{"rotate", {value.rotate.x, value.rotate.y, value.rotate.z}},
				{"translate", {value.translate.x, value.translate.y, value.translate.z}}
			});
		}
	}
	
	// 表示順序も保存
	if (displayOrders_.count(groupName)) {
		root[groupName + "_DisplayOrder"] = displayOrders_[groupName];
	}
	// CollapsingHeaderグループも保存
	if (headerGroups_.count(groupName)) {
		root[groupName + "_HeaderGroups"] = headerGroups_[groupName];
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
		// Transform型の値を保持していれば
		else if (itItem->is_object() && itItem->contains("scale") && itItem->contains("rotate") && itItem->contains("translate")) {
			Transform value;
			auto scaleArr = (*itItem)["scale"];
			auto rotateArr = (*itItem)["rotate"];
			auto translateArr = (*itItem)["translate"];
			value.scale = { scaleArr[0], scaleArr[1], scaleArr[2] };
			value.rotate = { rotateArr[0], rotateArr[1], rotateArr[2] };
			value.translate = { translateArr[0], translateArr[1], translateArr[2] };
			SetValue(groupName, itemName, value);
		}
		// std::string型の値を保持していれば
		else if (itItem->is_string()) {
			// std::string型の値を登録
			std::string value = itItem->get<std::string>();
			SetValue(groupName, itemName, static_cast<std::string>(value));
		}
	}

	// 表示順序の復元
	std::string orderKey = groupName + "_DisplayOrder";
	if (root.contains(orderKey) && root[orderKey].is_array()) {
		std::vector<std::string> order;
		for (const auto& v : root[orderKey]) {
			order.push_back(v.get<std::string>());
		}
		displayOrders_[groupName] = order;
	}
	// CollapsingHeaderグループの復元
	std::string headerKey = groupName + "_HeaderGroups";
	if (root.contains(headerKey) && root[headerKey].is_object()) {
		for (auto it = root[headerKey].begin(); it != root[headerKey].end(); ++it) {
			headerGroups_[groupName][it.key()] = it.value().get<std::string>();
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
template void GlobalVariables::SetValue<Transform>(const std::string&, const std::string&, Transform);

template<typename T>
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, T value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());
	
	// 既に存在しなければ追加
	if (!itGroup->second.contains(key)) {
		SetValue(groupName, key, value);
		// 順序記録
		auto& order = displayOrders_[groupName];
		if (std::find(order.begin(), order.end(), key) == order.end()) {
			order.push_back(key);
		}
		// 現在のヘッダーグループがあれば自動で割り当て
		auto itHeader = currentHeaderGroup_.find(groupName);
		if (itHeader != currentHeaderGroup_.end() && !itHeader->second.empty()) {
			headerGroups_[groupName][key] = itHeader->second;
		}
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
template void GlobalVariables::AddItem<Transform>(const std::string&, const std::string&, Transform);

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
template Transform GlobalVariables::GetValue<Transform>(const std::string&, const std::string&) const;

void GlobalVariables::SetDisplayOrder(const std::string& groupName, const std::vector<std::string>& order) {
	displayOrders_[groupName] = order;
}

const std::vector<std::string>& GlobalVariables::GetDisplayOrder(const std::string& groupName) const {
	static const std::vector<std::string> empty;
	auto it = displayOrders_.find(groupName);
	if (it != displayOrders_.end()) {
		return it->second;
	}
	return empty;
}

void GlobalVariables::SetHeaderGroup(const std::string& groupName, const std::string& itemKey, const std::string& headerName) {
	headerGroups_[groupName][itemKey] = headerName;
}

void GlobalVariables::SetHeaderGroup(const std::string& groupName, const std::string& headerName) {
	currentHeaderGroup_[groupName] = headerName;
}

void GlobalVariables::EndHeaderGroup(const std::string& groupName) {
	currentHeaderGroup_[groupName].clear();
}

std::string GlobalVariables::GetHeaderGroup(const std::string& groupName, const std::string& itemKey) const {
	auto itGroup = headerGroups_.find(groupName);
	if (itGroup != headerGroups_.end()) {
		auto itItem = itGroup->second.find(itemKey);
		if (itItem != itGroup->second.end()) {
			return itItem->second;
		}
	}
	return "";
}