#include "Framework.h"
#include "MyGame.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Framework* game = new MyGame;

	game->Run();

	delete game;

	return 0;
}