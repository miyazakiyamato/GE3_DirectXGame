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
	void DrawSphere(const Sphere& sphere, const Vector4& color, const uint32_t& subdivision);
	void DrawSphere(const Matrix4x4& worldMatrix, const Vector4& color, const uint32_t& subdivision);
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
	ComPtr<ID3D12Resource> wvpResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	TransformationMatrix* wvpData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	//Line用の頂点リソースを作る
	std::list<VertexData> vertices;
	size_t preVerticesSize = 0;
	//ブレンドモード
	std::string pipelineStateName_ = "";
public://ゲッターセッター
};

