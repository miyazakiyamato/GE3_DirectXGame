#include "Line3D.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include <CameraManager.h>
#include "SrvManager.h"
#include "numbers"

Line3dManager* Line3dManager::instance = nullptr;

Line3dManager* Line3dManager::GetInstance() {
	if (instance == nullptr) {
		instance = new Line3dManager;
	}
	return instance;
}

void Line3dManager::Initialize(SrvManager* srvManager) {
	srvManager_ = srvManager;

	//Line用のTransformationMatirx用のリソースを作る
	wvpResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = Matrix4x4::MakeIdentity4x4();
}

void Line3dManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void Line3dManager::Update() {
	//kNumVertex_ = 0;
	vertices.clear();
	//Line用のWorldViewProjectionMtrixを作る
	Matrix4x4 worldViewProjectionMatrix;

	if (CameraManager::GetInstance()->GetCamera()) {
		const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
		worldViewProjectionMatrix = viewProjectionMatrix;
	} else {
		worldViewProjectionMatrix = Matrix4x4::MakeIdentity4x4();
	}
	wvpData->WVP = worldViewProjectionMatrix;
}

void Line3dManager::Draw() {
	if (vertices.empty()) {
		return;
	}

	// Line用の頂点リソースを作る
	vertexResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * UINT(vertices.size() * 2));
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * UINT(vertices.size() * 2);
	// 1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// IndexLine用のインデックスリソースを作る
	indexResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * UINT(vertices.size() * 2));
	// リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * UINT(vertices.size() * 2);
	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// 頂点リソースにデータを書き込む
	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 頂点座標
	uint32_t index = 0;
	for (auto& lineData : vertices) {
		// 頂点座標
		vertexData[index].position = lineData.pos1;
		vertexData[index + 1].position = lineData.pos2;

		vertexData[index].color = lineData.color;
		vertexData[index + 1].color = lineData.color;

		index += 2;
	}
	vertexResource.Get()->Unmap(0, nullptr);

	// インデックスリソースにデータを書き込む
	index = 0;
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	for (uint32_t i = 0; i < vertices.size() * 2; i += 2) {
		indexData[i] = i;
		indexData[i + 1] = i + 1;
	}
	indexResource.Get()->Unmap(0, nullptr);

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = PipelineManager::GetInstance()->GetDxCommon()->GetCommandList();
	// パイプラインを設定
	PipelineManager::GetInstance()->DrawSetting(PipelineState::kLine3D, blendMode_);
	// Lineの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView); // IBVを設定
	// TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, wvpResource.Get()->GetGPUVirtualAddress());
	// ドローコール
	commandList->DrawIndexedInstanced(UINT(vertices.size() * 2), 1, 0, 0, 0);
}

void Line3dManager::DrawLine(const Vector3& pos1, const Vector3& pos2, const Vector4& color){
	LineData lineData;
	lineData.pos1 = { pos1.x, pos1.y, pos1.z, 1.0f };
	lineData.pos2 = { pos2.x, pos2.y, pos2.z, 1.0f };
	lineData.isConnected = false;
	lineData.color = color;
	vertices.push_back(lineData);
	//indices.push_back(kNumVertex_);
}

void Line3dManager::DrawSphere(const Sphere& sphere, const Vector4& color) {
	const uint32_t kSubdivision = 20;
	const float kLonEvery = std::numbers::pi_v<float> *2 / kSubdivision;
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;
	//
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			Vector3 a, b, c;
			a = Vector3::Add(sphere.center, Vector3::Multiply(sphere.radius, { cosf(lat) * cosf(lon),sinf(lat),cosf(lat) * sinf(lon) }));
			b = Vector3::Add(sphere.center, Vector3::Multiply(sphere.radius, { cosf(lat + kLatEvery) * cosf(lon),sinf(lat + kLatEvery),cosf(lat + kLatEvery) * sinf(lon) }));
			c = Vector3::Add(sphere.center, Vector3::Multiply(sphere.radius, { cosf(lat) * cosf(lon + kLonEvery),sinf(lat),cosf(lat) * sinf(lon + kLonEvery) }));

			DrawLine(a, b, color);
			DrawLine(a, c, color);
		}
	}
}

void Line3dManager::DrawGrid(const Vector2 gridHalfLength) {
	float kGridHalfwidth = gridHalfLength.x;
	float kGridHalfheight = gridHalfLength.y;
	const uint32_t kSubdivisionW = int(kGridHalfwidth * 2.0f);
	const uint32_t kSubdivisionH = int(kGridHalfheight * 2.0f);
	kGridHalfwidth = (kGridHalfwidth * 2.0f) / float(kSubdivisionW);
	kGridHalfheight = (kGridHalfheight * 2.0f) / float(kSubdivisionH);
	//
	for (uint32_t xIndex = 0; xIndex <= kSubdivisionW; ++xIndex) {
		Vector3 screenVertices = { kGridHalfwidth * xIndex - kGridHalfwidth * kSubdivisionW / 2,0.0f, -kGridHalfheight * (kSubdivisionH / 2) };
		Vector3 screenVertices2 = { kGridHalfwidth * xIndex - kGridHalfwidth * kSubdivisionW / 2,0.0f, kGridHalfheight * kSubdivisionH / 2 };
		if (xIndex == kSubdivisionW / 2) {
			DrawLine(screenVertices, screenVertices2, {0.0f,0.0f,0.0f,1.0f});
		} else {
			DrawLine(screenVertices, screenVertices2, { 0.66f,0.66f,0.66f,1.0f });
		}
	}
	//
	for (uint32_t zIndex = 0; zIndex <= kSubdivisionH; ++zIndex) {
		Vector3 screenVertices = { -kGridHalfwidth * (kSubdivisionW / 2),0.0f,kGridHalfheight * zIndex - kGridHalfheight * kSubdivisionH / 2 };
		Vector3 screenVertices2 = { kGridHalfwidth * kSubdivisionW / 2,0.0f,kGridHalfheight * zIndex - kGridHalfheight * kSubdivisionH / 2 };
		if (zIndex == kSubdivisionH / 2) {
			DrawLine(screenVertices, screenVertices2, { 0.0f,0.0f,0.0f,1.0f });
		} else {
			DrawLine(screenVertices, screenVertices2, { 0.66f,0.66f,0.66f,1.0f });
		}
	}
}
void  Line3dManager::DrawPlane(const Plane& plane, const Vector4& color) {
	Vector3 center = Vector3::Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Vector3::Normalize(Vector3::Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = Vector3::Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };
	//
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Vector3::Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Vector3::Add(center, extend);
		points[index] = point;
	}
	DrawLine(points[0],points[2], color);
	DrawLine(points[0],points[3], color);
	DrawLine(points[1],points[2], color);
	DrawLine(points[1],points[3], color);
}
void  Line3dManager::DrawTriangle(const Triangle& triangle, const Vector4& color) {
	DrawLine(triangle.Vertices[0], triangle.Vertices[1], color);
	DrawLine(triangle.Vertices[1], triangle.Vertices[2], color);
	DrawLine(triangle.Vertices[2], triangle.Vertices[0], color);
}
void  Line3dManager::DrawAABB(const AABB& aabb, const Vector4& color) {
	Vector3 points[8]{};
	points[0] = aabb.min;
	points[1] = { aabb.max.x,aabb.min.y,aabb.min.z };
	points[2] = { aabb.min.x,aabb.max.y,aabb.min.z };
	points[3] = { aabb.max.x,aabb.max.y,aabb.min.z };

	points[4] = { aabb.min.x,aabb.min.y,aabb.max.z };
	points[5] = { aabb.max.x,aabb.min.y,aabb.max.z };
	points[6] = { aabb.min.x,aabb.max.y,aabb.max.z };
	points[7] = aabb.max;

	DrawLine(points[0],points[1], color);
	DrawLine(points[0],points[2], color);
	DrawLine(points[3],points[2], color);
	DrawLine(points[3],points[1], color);

	DrawLine(points[4], points[5], color);
	DrawLine(points[4], points[6], color);
	DrawLine(points[7], points[6], color);
	DrawLine(points[7], points[5], color);

	DrawLine(points[0], points[4], color);
	DrawLine(points[1], points[5], color);
	DrawLine(points[2], points[6], color);
	DrawLine(points[3], points[7], color);
}
void  Line3dManager::DrawOBB(const OBB& obb, const Vector4& color) {
	Matrix4x4 obbWorldMatrix{
		obb.orientations[0].x,obb.orientations[1].x,obb.orientations[2].x,0,
		obb.orientations[0].y,obb.orientations[1].y,obb.orientations[2].y,0,
		obb.orientations[0].z,obb.orientations[1].z,obb.orientations[2].z,0,
		obb.center.x,obb.center.y,obb.center.z,1
	};
	Vector3 perpendiculars[8]{};
	perpendiculars[0] = { -obb.size.x,-obb.size.y,-obb.size.z };
	perpendiculars[1] = { obb.size.x,-obb.size.y,-obb.size.z };
	perpendiculars[2] = { -obb.size.x,obb.size.y,-obb.size.z };
	perpendiculars[3] = { obb.size.x,obb.size.y,-obb.size.z };

	perpendiculars[4] = { -obb.size.x,-obb.size.y,obb.size.z };
	perpendiculars[5] = { obb.size.x,-obb.size.y,obb.size.z };
	perpendiculars[6] = { -obb.size.x,obb.size.y,obb.size.z };
	perpendiculars[7] = { obb.size.x,obb.size.y,obb.size.z };

	Vector3 points[8];
	for (int32_t index = 0; index < 8; ++index) {
		points[index] = Matrix4x4::Transform(perpendiculars[index], obbWorldMatrix);
	}
	DrawLine(points[0], points[1], color);
	DrawLine(points[0], points[2], color);
	DrawLine(points[3], points[2], color);
	DrawLine(points[3], points[1], color);

	DrawLine(points[4], points[5], color);
	DrawLine(points[4], points[6], color);
	DrawLine(points[7], points[6], color);
	DrawLine(points[7], points[5], color);

	DrawLine(points[0], points[4], color);
	DrawLine(points[1], points[5], color);
	DrawLine(points[2], points[6], color);
	DrawLine(points[3], points[7], color);
}
void  Line3dManager::DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector4& color) {
	const size_t segmentCount = 100;
	for (size_t i = 0; i < segmentCount; i++) {
		float t = 1.0f / segmentCount * i;
		float t2 = t + 1.0f / segmentCount;
		Vector3 p2 = Vector3::Lerp(controlPoint0, controlPoint1, t),
			p3 = Vector3::Lerp(controlPoint1, controlPoint2, t);
		Vector3 pos[2];
		pos[0] = Vector3::CatmullRomInterpolation(controlPoint0, p2, p3, controlPoint2, t);
		p2 = Vector3::Lerp(controlPoint0, controlPoint1, t2);
		p3 = Vector3::Lerp(controlPoint1, controlPoint2, t2);
		pos[1] = Vector3::CatmullRomInterpolation(controlPoint0, p2, p3, controlPoint2, t2);
		DrawLine(pos[0], pos[1], color);
	}
}
void  Line3dManager::DrawCotmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, const Vector4& color) {
	const size_t segmentCount = 100;
	for (size_t j = 0; j < 3; j++) {
		Vector3 controlPoint[4] = { controlPoint0, controlPoint1, controlPoint2, controlPoint3 };
		if (j == 0) {
			controlPoint[1] = controlPoint0;
			controlPoint[2] = controlPoint1;
			controlPoint[3] = controlPoint2;
		}
		if (j == 2) {
			controlPoint[0] = controlPoint1;
			controlPoint[1] = controlPoint2;
			controlPoint[2] = controlPoint3;
		}
		for (size_t i = 0; i < segmentCount; i++) {
			float t = 1.0f / segmentCount * i;
			float t2 = t + 1.0f / segmentCount;
			Vector3 pos[2];
			pos[0] = Vector3::CatmullRomInterpolation(controlPoint[0], controlPoint[1], controlPoint[2], controlPoint[3], t);
			pos[1] = Vector3::CatmullRomInterpolation(controlPoint[0], controlPoint[1], controlPoint[2], controlPoint[3], t2);
			DrawLine(pos[0], pos[1], color);
		}
	}
}