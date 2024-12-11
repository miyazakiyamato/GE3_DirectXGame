#include "D3DResourceLeakChecker.h"
#include "Framework.h"
#include "MyGame.h"
#include <memory>

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//リークチェック
	D3DResourceLeakChecker leakCheck;
	//ゲーム
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	game->Run();

	return 0;
}