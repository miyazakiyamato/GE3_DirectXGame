#include "Input.h"
#include "WinApp.h"
#include <cassert>
#include <cmath>
#include "imgui.h"
#include <string>

Input* Input::instance = nullptr;

Input* Input::GetInstance() {
    if (!instance) {
        instance = new Input();
    }
    return instance;
}

void Input::Initialize(WinApp* winApp) {
    winApp_ = winApp;

    HRESULT result;
    // DirectInput の初期化
    result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
    assert(SUCCEEDED(result));
    //キーボードデバイスの生成
    keyboard = nullptr;
    result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
    assert(SUCCEEDED(result));
    result = keyboard->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(result));
    result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    assert(SUCCEEDED(result));

    // マウスデバイスの作成
    result = directInput->CreateDevice(GUID_SysMouse, &mouse, nullptr);
    assert(SUCCEEDED(result));
    result = mouse->SetDataFormat(&c_dfDIMouse2);
    assert(SUCCEEDED(result));
    result = mouse->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    assert(SUCCEEDED(result));
}

void Input::Finalize() {
    delete instance;
    instance = nullptr;
}

void Input::Update() {
    // キーボード状態を保存
    memcpy(keyPre, key, sizeof(key));
    keyboard->Acquire();
    keyboard->GetDeviceState(sizeof(key), key);

    // マウス状態を保存
    mouseStatePre = mouseState;
    mouse->Acquire();
    mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);

    // コントローラー状態を保存
    controller_.preState = controller_.nowState;
    XInputGetState(0, &controller_.nowState);


    preMousePosition = GetMousePosition();

    // マウス位置の更新（絶対座標）
    GetCursorPos(&mousePosition);
    if (isMouseLockedLange_) {
        Vector2 mousePos = GetMousePosition();
        mousePos = Vector2::Clamp(mousePos, leftTop_, rightBottom_);
        SetMousePosition(mousePos);
    }
    // マウス位置を固定
    if (isMouseLocked_) {
        SetMousePosition(lockPosition);
    }
}

bool Input::PushKey(BYTE keyNumber) {
    return key[keyNumber];
}

bool Input::TriggerKey(BYTE keyNumber) {
    return !keyPre[keyNumber] && key[keyNumber];
}

bool Input::PushControllerButton(WORD button) {
    return (controller_.nowState.Gamepad.wButtons & button) != 0;
}

bool Input::TriggerControllerButton(WORD button) {
    return !(controller_.preState.Gamepad.wButtons & button) && (controller_.nowState.Gamepad.wButtons & button);
}

float Input::GetControllerStickX() {
	float thumbLX = controller_.nowState.Gamepad.sThumbLX;
	if (fabs(thumbLX) <= deadZone_.x) {
        return 0.0f;
	}
    return thumbLX / 32767.0f;
}

float Input::GetControllerStickY() {
    float thumbLY = controller_.nowState.Gamepad.sThumbLY;
    if (fabs(thumbLY) <= deadZone_.y) {
        return 0.0f;
    }
    return thumbLY / 32767.0f;
}

bool Input::PushMouseButton(int button) {
    return mouseState.rgbButtons[button] & 0x80;
}

bool Input::TriggerMouseButton(int button) {
    return !(mouseStatePre.rgbButtons[button] & 0x80) && (mouseState.rgbButtons[button] & 0x80);
}

float Input::GetMouseX() {
    return float(mousePosition.x - winApp_->GetWindowStartPosition().x);
}

float Input::GetMouseY() {
    return float(mousePosition.y - winApp_->GetWindowStartPosition().y);
}

Vector2 Input::GetMousePosition()
{
    return Vector2(GetMouseX(), GetMouseY());
}
void Input::SetMouseSensitivity(float sensitivity) {
    mouseSensitivity = sensitivity;
}

void Input::SetMousePosition(Vector2 v) {
    SetCursorPos((int)v.x + winApp_->GetWindowStartPosition().x, (int)v.y + winApp_->GetWindowStartPosition().y);
}

void Input::LockMousePosition(Vector2 v) {
    lockPosition = v;
    isMouseLocked_ = true;
    isMouseLockedLange_ = false;
}

void Input::LockMouseRangePosition(Vector2 leftTop, Vector2 rightBottom) {
    leftTop_ = leftTop;
    rightBottom_ = rightBottom;
    isMouseLockedLange_ = true;
    isMouseLocked_ = false;
}

void Input::ImGuiUpdate() {
    std::string groupName = "Input";
    if (ImGui::BeginMenu(groupName.c_str())) {
        int WindowStartX = winApp_->GetWindowStartPosition().x;
        int WindowStartY = winApp_->GetWindowStartPosition().y;
        ImGui::Text("Window StartPosition: (%d, %d)", WindowStartX, WindowStartY);
		Vector2 mousePos = GetMousePosition();
		ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
		ImGui::Text("Mouse Moving: (%.1f, %.1f)", mouseMoving.x, mouseMoving.y);
		ImGui::Text("Mouse Sensitivity: %.2f", mouseSensitivity);
		ImGui::DragFloat2("Lock Position", &lockPosition.x);
		ImGui::DragFloat2("Left Top", &leftTop_.x);
		ImGui::DragFloat2("Right Bottom", &rightBottom_.x);
		ImGui::Checkbox("Mouse Locked", &isMouseLocked_);
		ImGui::Checkbox("Mouse Locked Range", &isMouseLockedLange_);
        ImGui::EndMenu();
    }
}
