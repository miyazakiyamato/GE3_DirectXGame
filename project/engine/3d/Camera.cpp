#include "Camera.h"
#include "WinApp.h"

Camera::Camera() :
	transform({ { 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } }),
	fovAngleY_(0.45f),
	aspectRatio_(float(WinApp::kClientWidth) / float(WinApp::kClientHeight)),
	nearClip_(0.1f),
	farClip_(100.0f),
	worldMatrix(Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate)),
	viewMatrix(Matrix4x4::Inverse(worldMatrix)),
	projectionMatrix(Matrix4x4::MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearClip_, farClip_)),
	viewProjectionMatrix(viewMatrix * projectionMatrix)
{}

void Camera::Update(){
	worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix = Matrix4x4::Inverse(worldMatrix);
	projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(fovAngleY_,aspectRatio_,nearClip_,farClip_);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
}
