#pragma once
#include "d3dx12.h"
#include "Quaternion.h"
#include "BlendMode.h"
#include "Collision.h"

class SrvManager;
class Line3dManager{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct VertexData {
		Vector4 position;
		Vector4 color;
	};
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Vector4 color{ 1.0f,1.0f,1.0f,1.0f };
	};

	struct Line{
		Vector3 origin{0.0f,0.0f,0.0f};
		Vector3 diff{ 1.0f,1.0f,1.0f };
		Vector4 color{ 1.0f,1.0f,1.0f,1.0f };
	};

public://メンバ関数
	//シングルトンインスタンスの取得
	static Line3dManager* GetInstance();
	//初期化
	void Initialize(SrvManager* srvManager);
	//終了
	void Finalize();
	//更新
	void Update();
	//描画
	void Draw();

	void DrawLine(const Vector3& pos1,const Vector3& pos2,const Vector4& color);
	void DrawSphere(const Sphere& sphere, const Vector4& color);
	void DrawGrid(const Vector2 gridHalfLength);
	void DrawPlane(const Plane& plane, const Vector4& color);
	void DrawTriangle(const Triangle& triangle, const Vector4& color);
	void DrawAABB(const AABB& aabb, const Vector4& color);
	void DrawOBB(const OBB& obb, const Vector4& color);
	void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector4& color);
	void DrawCotmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, const Vector4& color);
private://シングルインスタンス
	static Line3dManager* instance;

	Line3dManager() = default;
	~Line3dManager() = default;
	Line3dManager(Line3dManager&) = delete;
	Line3dManager& operator=(Line3dManager&) = delete;
private://メンバ変数
	//ポインタ
	SrvManager* srvManager_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource> vertexResource;
	ComPtr<ID3D12Resource> indexResource;
	ComPtr<ID3D12Resource> materialResource;
	ComPtr<ID3D12Resource> wvpResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	TransformationMatrix* wvpData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	std::list<Line> lines_;

	//インスタンスの最大数
	uint32_t kMaxInstance_ = 10000;
	//今のインスタンス数
	uint32_t kNumInstance_ = 0;
	uint32_t srvIndexForInstancing_ = 0;

	BlendMode blendMode_ = BlendMode::kNormal;

public://ゲッターセッター
	//const BlendMode& GetBlendMode() { return blendMode_; }

	//void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }
	void SetKMaxInstance(uint32_t kMaxInstance);
};

