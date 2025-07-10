#pragma once
#define DIRECTINPUT_VERSION 0x800
#include <Windows.h>
#include <dinput.h>
#include <XInput.h>
#include <wrl.h>
#include <array>
#include "Vector2.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

class WinApp;

class Input {
public:
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    struct ControllerState{// コントローラー状態
        XINPUT_STATE nowState = {};
        XINPUT_STATE preState = {};
    };
public:
    // シングルインスタンスの取得
    static Input* GetInstance();
    // 初期化
    void Initialize(WinApp* winApp);
    // 終了
    void Finalize();
    // 更新
    void Update();

    // キーボード操作
    bool PushKey(BYTE keyNumber);
    bool TriggerKey(BYTE keyNumber);

    // コントローラー操作
    bool PushControllerButton(WORD button);
    bool TriggerControllerButton(WORD button);
    float GetControllerStickX();
    float GetControllerStickY();

    // マウス操作
    bool PushMouseButton(int button);
    bool TriggerMouseButton(int button);

    void LockMousePosition(Vector2 v);
    void LockMouseRangePosition(Vector2 leftTop, Vector2 rightBottom);

    void ImGuiUpdate();
private:
    static Input* instance;

    Input() = default;
    ~Input() = default;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

private:
    WinApp* winApp_ = nullptr;

    // DirectInput
    ComPtr<IDirectInput8> directInput;
    ComPtr<IDirectInputDevice8> keyboard;
    ComPtr<IDirectInputDevice8> mouse;

    // キーボード状態
    BYTE key[256] = {};
    BYTE keyPre[256] = {};

    // マウス状態
    DIMOUSESTATE2 mouseState = {};
    DIMOUSESTATE2 mouseStatePre = {};
    float mouseSensitivity = 1.00f; // マウス感度（デフォルト1.0）
    POINT mousePosition = { 0, 0 }; // マウスの現在の位置（絶対座標）

    Vector2 preMousePosition = {};
    Vector2 mouseMoving = {};

    bool isMouseLocked_ = false;  // マウス固定フラグ
    Vector2 lockPosition{}; // 固定する位置
    bool isMouseLockedLange_ = false;//マウスの範囲固定
    Vector2 leftTop_{};
    Vector2 rightBottom_ = {1280.0f,720.0f + 30.0f};
    // コントローラー状態
	ControllerState controller_ = {};
	Vector2 deadZone_ = { 3000.0f, 3000.0f }; // スティックのデッドゾーン
    //std::array<ControllerState, XUSER_MAX_COUNT> controllers_ = {};
	
public:
    float GetMouseX();
    float GetMouseY();
    // マウス位置（絶対座標）を取得
    Vector2 GetMousePosition();
    // 感度の設定
    void SetMouseSensitivity(float sensitivity);
    void SetMousePosition(Vector2 v);
    bool GetIsLockMouse() const { return isMouseLocked_; }
    void SetIsLockMouse(bool isMouseLock) { isMouseLocked_ = isMouseLock; }
    bool GetIsLockedLange() const { return isMouseLockedLange_; }
    void SetIsLockedLange(bool isMouseLock) { isMouseLockedLange_ = isMouseLock; }
};