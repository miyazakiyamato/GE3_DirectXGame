#include "Logger.h"
#include <dxgidebug.h>

namespace Logger {
	void Log(const std::string& message) {
		//出力ウィンドウへの文字出力
		OutputDebugStringA(message.c_str());
	}
}
