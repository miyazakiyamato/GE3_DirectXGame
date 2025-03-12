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

	//Line用の頂点リソースを作る
	vertexResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 2);
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 2;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//IndexLine用のインデックスリソースを作る
	indexResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 2);
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 2;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//Line用のTransformationMatirx用のリソースを作る
	wvpResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix) * kMaxInstance_);
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kMaxInstance_; index++) {
		wvpData[index].WVP = Matrix4x4::MakeIdentity4x4();
		wvpData[index].color = { 1.0f,1.0f,1.0f,1.0f };//色を書き込む
	}

	srvIndexForInstancing_ = srvManager_->ALLocate();
	srvManager_->CreateSRVforStructuredBuffer(srvIndexForInstancing_, wvpResource.Get(), kMaxInstance_, sizeof(TransformationMatrix));

	//頂点リソースにデータを書き込む
	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点座標
	vertexData[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[1].position = { 1.0f,0.0f,0.0f,1.0f };

	vertexData[0].color = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[1].color = { 1.0f,1.0f,1.0f,1.0f };
	//インデックスリソースにデータを書き込む
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1;
}

void Line3dManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void Line3dManager::Update() {
	kNumInstance_ = 0;
	lines_.clear();
}

void Line3dManager::Draw() {
	int index = 0;
	for (std::list<Line>::iterator it = lines_.begin(); it != lines_.end();) {
		Line& line = *it;
		//Line用のWorldViewProjectionMtrixを作る
		Matrix4x4 worldMatrix = Matrix4x4::MakeScaleMatrix({ line.diff.Length(),1.0f,1.0f }) * Quaternion::DirectionToDirection({ 1.0f,0.0f,0.0f }, line.diff) * Matrix4x4::MakeTranslateMatrix(line.origin);
		Matrix4x4 worldViewProjectionMatrix;

		if (CameraManager::GetInstance()->GetCamera()) {
			const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
			worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
		} else {
			worldViewProjectionMatrix = worldMatrix;
		}
		wvpData[index].WVP = worldViewProjectionMatrix;
		wvpData[index].color = line.color;

		++it;
		++index;
	}

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = PipelineManager::GetInstance()->GetDxCommon()->GetCommandList();
	//パイプラインを設定
	PipelineManager::GetInstance()->DrawSetting(PipelineState::kLine3D, blendMode_);
	//Lineの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
	// インスタンシングデータのSRVのDescriptorTableを設定
	srvManager_->SetGraphicsRootDescriptorTable(0, srvIndexForInstancing_);
	//ドローコール
	commandList->DrawIndexedInstanced(2, kNumInstance_, 0, 0, 0);
}

void Line3dManager::DrawLine(const Vector3& pos1, const Vector3& pos2, const Vector4& color){
	Vector3 origin = pos1;
	Vector3 diff = pos2;
	diff -= pos1;
	kNumInstance_++;
	if (kNumInstance_ >= kMaxInstance_) {
		kNumInstance_ = kMaxInstance_;
	}
	lines_.push_back({ origin,diff,color});
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

void Line3dManager::DrawGrid() {
	const float kGridHalfwidth = 50.0f;
	const uint32_t kSubdivision = int(kGridHalfwidth * 2.0f);
	const float kGridEvery = (kGridHalfwidth * 2.0f) / float(kSubdivision);
	//
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 screenVertices = { kGridEvery * xIndex - kGridEvery * kSubdivision / 2,0.0f, -kGridEvery * (kSubdivision / 2) };
		Vector3 screenVertices2 = { kGridEvery * xIndex - kGridEvery * kSubdivision / 2,0.0f, kGridEvery * kSubdivision / 2 };
		if (xIndex == kSubdivision / 2) {
			DrawLine(screenVertices, screenVertices2, {0.0f,0.0f,0.0f,1.0f});
		} else {
			DrawLine(screenVertices, screenVertices2, { 0.66f,0.66f,0.66f,1.0f });
		}
	}
	//
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 screenVertices = { -kGridEvery * (kSubdivision / 2),0.0f,kGridEvery * zIndex - kGridEvery * kSubdivision / 2 };
		Vector3 screenVertices2 = { kGridEvery * kSubdivision / 2,0.0f,kGridEvery * zIndex - kGridEvery * kSubdivision / 2 };
		if (zIndex == kSubdivision / 2) {
			DrawLine(screenVertices, screenVertices2, { 0.0f,0.0f,0.0f,1.0f });
		} else {
			DrawLine(screenVertices, screenVertices2, { 0.66f,0.66f,0.66f,1.0f });
		}

	}
}
//void  Line3dManager::DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
//	Vector3 center = Vector3::Multiply(plane.distance, plane.normal);
//	Vector3 perpendiculars[4];
//	perpendiculars[0] = Vector3::Normalize(Vector3::Perpendicular(plane.normal));
//	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
//	perpendiculars[2] = Vector3::Cross(plane.normal, perpendiculars[0]);
//	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };
//	//
//	Vector3 points[4];
//	for (int32_t index = 0; index < 4; ++index) {
//		Vector3 extend = Vector3::Multiply(2.0f, perpendiculars[index]);
//		Vector3 point = Vector3::Add(center, extend);
//		points[index] = Matrix4x4::Transform(Matrix4x4::Transform(point, viewProjectionMatrix), viewportMatrix);
//	}
//	DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
//	DrawLine(int(points[0].x), int(points[0].y), int(points[3].x), int(points[3].y), color);
//	DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
//	DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
//
//}
//void  Line3dManager::DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
//	Vector3 points[3];
//	for (int32_t index = 0; index < 3; ++index) {
//
//		points[index] = Matrix4x4::Transform(Matrix4x4::Transform(triangle.Vertices[index], viewProjectionMatrix), viewportMatrix);
//	}
//	DrawTriangle(int(points[0].x), int(points[0].y), int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color, kFillModeWireFrame);
//}
//void  Line3dManager::DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
//	Vector3 perpendiculars[8]{};
//	perpendiculars[0] = aabb.min;
//	perpendiculars[1] = { aabb.max.x,aabb.min.y,aabb.min.z };
//	perpendiculars[2] = { aabb.min.x,aabb.max.y,aabb.min.z };
//	perpendiculars[3] = { aabb.max.x,aabb.max.y,aabb.min.z };
//
//	perpendiculars[4] = { aabb.min.x,aabb.min.y,aabb.max.z };
//	perpendiculars[5] = { aabb.max.x,aabb.min.y,aabb.max.z };
//	perpendiculars[6] = { aabb.min.x,aabb.max.y,aabb.max.z };
//	perpendiculars[7] = aabb.max;
//
//	Vector3 points[8];
//	for (int32_t index = 0; index < 8; ++index) {
//		points[index] = Matrix4x4::Transform(Matrix4x4::Transform(perpendiculars[index], viewProjectionMatrix), viewportMatrix);
//	}
//	DrawLine((int)points[0].x, (int)points[0].y, (int)points[1].x, (int)points[1].y, color);
//	DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, color);
//	DrawLine((int)points[3].x, (int)points[3].y, (int)points[2].x, (int)points[2].y, color);
//	DrawLine((int)points[3].x, (int)points[3].y, (int)points[1].x, (int)points[1].y, color);
//
//	DrawLine((int)points[4].x, (int)points[4].y, (int)points[5].x, (int)points[5].y, color);
//	DrawLine((int)points[4].x, (int)points[4].y, (int)points[6].x, (int)points[6].y, color);
//	DrawLine((int)points[7].x, (int)points[7].y, (int)points[6].x, (int)points[6].y, color);
//	DrawLine((int)points[7].x, (int)points[7].y, (int)points[5].x, (int)points[5].y, color);
//
//	DrawLine((int)points[0].x, (int)points[0].y, (int)points[4].x, (int)points[4].y, color);
//	DrawLine((int)points[1].x, (int)points[1].y, (int)points[5].x, (int)points[5].y, color);
//	DrawLine((int)points[2].x, (int)points[2].y, (int)points[6].x, (int)points[6].y, color);
//	DrawLine((int)points[3].x, (int)points[3].y, (int)points[7].x, (int)points[7].y, color);
//}
//void  Line3dManager::DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
//	Matrix4x4 obbWorldMatrix{
//		obb.orientations[0].x,obb.orientations[1].x,obb.orientations[2].x,0,
//		obb.orientations[0].y,obb.orientations[1].y,obb.orientations[2].y,0,
//		obb.orientations[0].z,obb.orientations[1].z,obb.orientations[2].z,0,
//		obb.center.x,obb.center.y,obb.center.z,1
//	};
//	Vector3 perpendiculars[8]{};
//	perpendiculars[0] = { -obb.size.x,-obb.size.y,-obb.size.z };
//	perpendiculars[1] = { obb.size.x,-obb.size.y,-obb.size.z };
//	perpendiculars[2] = { -obb.size.x,obb.size.y,-obb.size.z };
//	perpendiculars[3] = { obb.size.x,obb.size.y,-obb.size.z };
//
//	perpendiculars[4] = { -obb.size.x,-obb.size.y,obb.size.z };
//	perpendiculars[5] = { obb.size.x,-obb.size.y,obb.size.z };
//	perpendiculars[6] = { -obb.size.x,obb.size.y,obb.size.z };
//	perpendiculars[7] = { obb.size.x,obb.size.y,obb.size.z };
//
//	Vector3 points[8];
//	for (int32_t index = 0; index < 8; ++index) {
//		points[index] = Matrix4x4::Transform(Matrix4x4::Transform(perpendiculars[index], Matrix4x4::Multiply(obbWorldMatrix, viewProjectionMatrix)), viewportMatrix);
//	}
//	DrawLine((int)points[0].x, (int)points[0].y, (int)points[1].x, (int)points[1].y, color);
//	DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, color);
//	DrawLine((int)points[3].x, (int)points[3].y, (int)points[2].x, (int)points[2].y, color);
//	DrawLine((int)points[3].x, (int)points[3].y, (int)points[1].x, (int)points[1].y, color);
//
//	DrawLine((int)points[4].x, (int)points[4].y, (int)points[5].x, (int)points[5].y, color);
//	DrawLine((int)points[4].x, (int)points[4].y, (int)points[6].x, (int)points[6].y, color);
//	DrawLine((int)points[7].x, (int)points[7].y, (int)points[6].x, (int)points[6].y, color);
//	DrawLine((int)points[7].x, (int)points[7].y, (int)points[5].x, (int)points[5].y, color);
//
//	DrawLine((int)points[0].x, (int)points[0].y, (int)points[4].x, (int)points[4].y, color);
//	DrawLine((int)points[1].x, (int)points[1].y, (int)points[5].x, (int)points[5].y, color);
//	DrawLine((int)points[2].x, (int)points[2].y, (int)points[6].x, (int)points[6].y, color);
//	DrawLine((int)points[3].x, (int)points[3].y, (int)points[7].x, (int)points[7].y, color);
//}
//void  Line3dManager::DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
//	const size_t segmentCount = 100;
//	for (size_t i = 0; i < segmentCount; i++) {
//		float t = 1.0f / segmentCount * i;
//		float t2 = t + 1.0f / segmentCount;
//		Vector3 p2 = Vector3::Lerp(controlPoint0, controlPoint1, t),
//			p3 = Vector3::Lerp(controlPoint1, controlPoint2, t);
//		Vector3 pos[2];
//		pos[0] = Vector3::CatmullRomInterpolation(controlPoint0, p2, p3, controlPoint2, t);
//		p2 = Vector3::Lerp(controlPoint0, controlPoint1, t2);
//		p3 = Vector3::Lerp(controlPoint1, controlPoint2, t2);
//		pos[1] = Vector3::CatmullRomInterpolation(controlPoint0, p2, p3, controlPoint2, t2);
//		Vector3 points[2];
//		for (int32_t index = 0; index < 2; ++index) {
//			points[index] = Matrix4x4::Transform(Matrix4x4::Transform(pos[index], viewProjectionMatrix), viewportMatrix);
//		}
//		DrawLine((int)points[0].x, (int)points[0].y, (int)points[1].x, (int)points[1].y, color);
//	}
//}
//void  Line3dManager::DrawCotmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
//	const size_t segmentCount = 100;
//	for (size_t j = 0; j < 3; j++) {
//		Vector3 controlPoint[4] = { controlPoint0, controlPoint1, controlPoint2, controlPoint3 };
//		if (j == 0) {
//			controlPoint[1] = controlPoint0;
//			controlPoint[2] = controlPoint1;
//			controlPoint[3] = controlPoint2;
//		}
//		if (j == 2) {
//			controlPoint[0] = controlPoint1;
//			controlPoint[1] = controlPoint2;
//			controlPoint[2] = controlPoint3;
//		}
//		for (size_t i = 0; i < segmentCount; i++) {
//			float t = 1.0f / segmentCount * i;
//			float t2 = t + 1.0f / segmentCount;
//			Vector3 pos[2];
//			pos[0] = Vector3::CatmullRomInterpolation(controlPoint[0], controlPoint[1], controlPoint[2], controlPoint[3], t);
//			pos[1] = Vector3::CatmullRomInterpolation(controlPoint[0], controlPoint[1], controlPoint[2], controlPoint[3], t2);
//			Vector3 points[2];
//			for (int32_t index = 0; index < 2; ++index) {
//				points[index] = Matrix4x4::Transform(Matrix4x4::Transform(pos[index], viewProjectionMatrix), viewportMatrix);
//			}
//			DrawLine((int)points[0].x, (int)points[0].y, (int)points[1].x, (int)points[1].y, color);
//		}
//	}
//}