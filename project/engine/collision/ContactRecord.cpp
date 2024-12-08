#include "ContactRecord.h"

void ContactRecord::AddRecord(uint32_t number) {
	//履歴に登録する
	history_.push_back(number);
}

bool ContactRecord::CheckRecord(uint32_t number) {
	for (uint32_t num : history_) {
		if (num == number) {
			return true;
		}
	}
	return false;
}

void ContactRecord::Clear() { history_.clear(); }
