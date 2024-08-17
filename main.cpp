//#pragma warning(push)
//#pragma warning(disable:4023)
//#pragma warning(pop)
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <cassert>
#include <dxcapi.h>
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <format>
#include <cmath>
#include <wrl.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Microsoft::WRL;
//
//ComPtr<IDxcBlob> CompileShader(
//	//CompilerするShaderファイルへのパス
//	const std::wstring& filePath,
//	//Compilerに使用するProfile
//	const wchar_t* profile,
//	//初期化で生成したものを3つ
//	ComPtr<IDxcUtils> dxcUtils,
//	ComPtr<IDxcCompiler3> dxcCompiler,
//	ComPtr<IDxcIncludeHandler> includeHandler)
//{
//	//これからシェーダーをコンパイルする旨をログに出す
//	Log(ConvertString(std::format(L"Begin CompileSher, path:{},profile:{}\n", filePath, profile)));
//	//hlslファイルを読む
//	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
//	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
//	//読めながったら止める
//	assert(SUCCEEDED(hr));
//	//読み込んだファイルの内容を設定する
//	DxcBuffer shaderSourceBuffer;
//	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
//	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
//	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
//
//	LPCWSTR arguments[] = {
//		filePath.c_str(), //コンパイル対象のhlslファイル名
//		L"-E", L"main", //エントリーポイントの指定。基本的にmain以外にはしない
//		L"-T", profile, //ShaderProfileの設定
//		L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
//		L"-Od",  //最適化を外しておく
//		L"-Zpr", //メモリレイアウトは行優先
//	};
//	//実際にShaderをコンパイルする
//	ComPtr<IDxcResult> shaderResult = nullptr;
//	hr = dxcCompiler->Compile(
//		&shaderSourceBuffer, //読み込んだファイル
//		arguments,           //コンパイルオプション
//		_countof(arguments), //コンパイルオプションの数
//		includeHandler.Get(),		 //includeが含まれた諸々
//		IID_PPV_ARGS(&shaderResult)//コンパイル結果
//	);
//	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
//	assert(SUCCEEDED(hr));
//	//警告・エラーが出たらログに出して止める
//	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
//	shaderResult->GetOutput(_Notnull_ DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
//	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
//		Log(shaderError->GetStringPointer());
//		//警告・エラーダメゼッタイ
//		assert(false);
//	}
//	//コンパイル結果から実行用のバイナリ部分を取得
//	ComPtr<IDxcBlob> shaderBlob = nullptr;
//	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
//	assert(SUCCEEDED(hr));
//	//成功したログを出す
//	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
//	//もう使わないリソースを解放
//	shaderSource->Release();
//	shaderResult->Release();
//	//実行用のバイナリを返却
//	return shaderBlob;
//
//}
//
//ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, const size_t& sizeInbytes) {
//	//頂点リソース用のヒープの設定
//	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
//	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
//	//頂点リソースの設定
//	D3D12_RESOURCE_DESC vertexResourceDesc{};
//	//バッファリソース。テクスチャの場合はまた別の設定をする
//	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	vertexResourceDesc.Width = sizeInbytes;//リソースのサイズ。
//	//バッファの場合はこれらは1にする決まり
//	vertexResourceDesc.Height = 1;
//	vertexResourceDesc.DepthOrArraySize = 1;
//	vertexResourceDesc.MipLevels = 1;
//	vertexResourceDesc.SampleDesc.Count = 1;
//	//バッファの場合はこれにする決まり
//	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//	ComPtr<ID3D12Resource> vertexResource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
//		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
//		IID_PPV_ARGS(&vertexResource));
//	assert(SUCCEEDED(hr));
//
//	return vertexResource;
//}
//
//DirectX::ScratchImage LoadTexture(const std::string& filePath) {
//	//テクスチャファイルを読んでプログラムで扱えるようにする
//	DirectX::ScratchImage image{};
//	std::wstring filePathW = ConvertString(filePath);
//	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_DEFAULT_SRGB, nullptr, image);
//	assert(SUCCEEDED(hr));
//
//	//ミップマップの作成
//	DirectX::ScratchImage mipImages{};
//	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//	assert(SUCCEEDED(hr));
//
//	//ミップマップ付きのデータを返す
//	return mipImages;
//}
//
//ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {
//	//metadataを基にResourceの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = UINT(metadata.width);//Textureの幅
//	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
//	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
//	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き or 配列Textureの配列数
//	resourceDesc.Format = metadata.format;//TextureのFormat
//	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元
//
//	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
//
//	//Resourceの生成
//	ComPtr<ID3D12Resource> resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,//Heapの設定
//		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
//		&resourceDesc,//Resourceの設定
//		D3D12_RESOURCE_STATE_COPY_DEST,//データ転送される設定
//		nullptr,//Clear最適値。使わないのでnullptr
//		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
//	assert(SUCCEEDED(hr));
//	return resource;
//}
//
//[[nodiscard]]
//ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ComPtr<ID3D12Device> device,
//	ComPtr<ID3D12GraphicsCommandList> commandList) {
//	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
//	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);
//	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subResources.size()));
//	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
//	UpdateSubresources(commandList.Get(), texture, intermediateResource.Get(), 0, 0, UINT(subResources.size()), subResources.data());
//	//Tetureへの転送後は利用できるよう、D3D12_ReSOURCE_STATE_COPY\DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
//	D3D12_RESOURCE_BARRIER barrier{};
//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//	barrier.Transition.pResource = texture;
//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
//	commandList->ResourceBarrier(1, &barrier);
//	return intermediateResource;
//}
//
//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
//	//中で必要な関数の宣言
//	MaterialData materialData;//構築するMaterialData
//	std::string line;//ファイルから読んんだ1行を格納するもの
//	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
//	assert(file.is_open());//開けなかったら止める
//	materialData.mtlFilePath = directoryPath + "/" + filename;
//	//実際にファイルを読み、MaterialDataを構築する
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//		//identifierに応じた処理
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			//連結してファイルパスにする
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//		}
//	}
//	//MaterialDataを返す
//	return materialData;
//}
//
//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
//	//中で必要となる変数の宣言
//	ModelData modelData;//構築するModelData
//	std::vector<Vector4> positions;//位置
//	std::vector<Vector3> normals;//法線
//	std::vector<Vector2> texcoords;//テクスチャ座標
//	std::string line;//ファイルから読んだ1行を格納するもの
//	//ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());//開けなかったら止める
//	//実際にファイルを読み、ModelDataを構築する
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;//先頭の識別子を読む
//		if (identifier == "v") {
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt") {
//			Vector2 texcood;
//			s >> texcood.x >> texcood.y;
//			texcoords.push_back(texcood);
//		}
//		else if (identifier == "vn") {
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normals.push_back(normal);
//		}
//		else if (identifier == "f") {
//			//面は三角限定。その他は未対応
//			VertexData triangle[3];
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element) {
//					std::string index;
//					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//				}
//				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				position.x *= -1.0f;
//				normal.x *= -1.0f;
//				texcoord.y = 1.0f - texcoord.y;
//				triangle[faceVertex] = {position,texcoord,normal};
//			}
//			//頂点を逆順で登録することで周り順を逆にする
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//			
//		}
//		else if (identifier == "mtllib") {
//			//materialTemplateLibraryファイルの名前を取得
//			std::string materialFilename;
//			s >> materialFilename;
//			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//	//ModelDataを返す
//	return modelData;
//}
//Vector4 LoadColor(const std::string& filePath) {
//	Vector4 color = {};
//	std::string line;//ファイルから読んだ1行を格納するもの
//	std::ifstream file(filePath);//ファイルを開く
//	assert(file.is_open());//開けなかったら止める
//	//実際にファイルを読む
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;//先頭の識別子を読む
//		if (identifier == "Kd") {
//			s >> color.x >> color.y >> color.z >> color.w;
//		}
//	}
//	return color;
//}

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リソースリークチェック
		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);
	D3DResourceLeakChecker leakCheck;

	//ポインタ
	WinApp* winApp = nullptr;
	Input* input = nullptr;
	DirectXCommon* dxCommon = nullptr;

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//DirecXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	////DepthStencilStateの設定
	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	////Depthの機能を有効化する
	//depthStencilDesc.DepthEnable = true;
	////書き込みします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	////比較関数はLessEqual。つまり、近ければ描画される
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	////RootSignature作成
	//D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//
	//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	//descriptorRange[0].NumDescriptors = 1;//数は1つ
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	////RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
	//D3D12_ROOT_PARAMETER rootParameters[4] = {};
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	//rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	//rootParameters[1].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	//rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	//rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	//rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	//rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	//rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	//rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	//rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
	//descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	//descriptionRootSignature.NumParameters = _countof(rootParameters); //配列の長さ

	//
	//D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイナリフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	//staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	//staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	//staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	//descriptionRootSignature.pStaticSamplers = staticSamplers;
	//descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	////シリアライズしてバイナリにする
	//ComPtr<ID3DBlob> signatureBlob = nullptr;
	//ComPtr<ID3DBlob> errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr)) {
	//	Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	////バイナリをもとに生成
	//ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//hr = device->CreateRootSignature(0,
	//	signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
	//	IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(hr));

	////InputLayout
	//D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//inputElementDescs[0].SemanticName = "POSITION";
	//inputElementDescs[0].SemanticIndex = 0;
	//inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[1].SemanticName = "TEXCOORD";
	//inputElementDescs[1].SemanticIndex = 0;
	//inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[2].SemanticName = "NORMAL";
	//inputElementDescs[2].SemanticIndex = 0;
	//inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	//inputLayoutDesc.pInputElementDescs = inputElementDescs;
	//inputLayoutDesc.NumElements = _countof(inputElementDescs);

	////BlendStateの設定
	//D3D12_BLEND_DESC blendDesc{};
	////すべての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	////RasterizerStateの設定
	//D3D12_RASTERIZER_DESC rasterizerDesc{};
	////裏面(時計回り)を表示しない
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	////三角形の中を塗りつぶす
	//rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	////Shaderをコンパイルする
	//ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resources/shaders/Object3D.VS.hlsl",
	//	L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(vertexShaderBlob != nullptr);

	//ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/Object3D.PS.hlsl",
	//	L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(pixelShaderBlob != nullptr);

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();//RootSignature
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	//vertexShaderBlob->GetBufferSize() };//VertexShader
	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	//pixelShaderBlob->GetBufferSize() };//PixeLShader
	//graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	//graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	////書き込むRTVの情報
	//graphicsPipelineStateDesc.NumRenderTargets = 1;
	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	////利用するトポロジ(形状)のタイプ。三角形
	//graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	////どのように画面に色を打ち込むかの設定
	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	////DepthStencilの設定
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	////実際に生成
	//ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
	//	IID_PPV_ARGS(&graphicsPipelineState));
	//assert(SUCCEEDED(hr));

	//////Sphereの頂点数
	////const uint32_t kSubdivision = 16;
	////const uint32_t kVertexNum = kSubdivision * kSubdivision * 4;
	////ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * kVertexNum);
	//////頂点バッファビューを作成する
	////D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//////リソースの先頭のアドレスから使う
	////vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//////使用するリソースのサイズは頂点3つのサイズ
	////vertexBufferView.SizeInBytes = sizeof(VertexData) * kVertexNum;
	//////1頂点当たりのサイズ
	////vertexBufferView.StrideInBytes = sizeof(VertexData);

	//////頂点リソースにデータを書き込む
	////VertexData* vertexData = nullptr;
	//////書き込むためのアドレスを取得
	////vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//////経度分割1つ分の角度
	////const float KLonEvery = (float)M_PI * 2.0f / float(kSubdivision);
	//////緯度分割1つ分の角度
	////const float KLatEvery = (float)M_PI / float(kSubdivision);
	//////緯度の方向に分割
	////for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
	////	float lat = -(float)M_PI / 2.0f + KLatEvery * latIndex;//0
	////	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
	////		uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
	////		float lon = lonIndex * KLonEvery;//
	////		float u = float(lonIndex) / float(kSubdivision);
	////		float v = 1.0f - float(latIndex) / float(kSubdivision);
	////		//頂点にデータを入力する。
	////		//左下
	////		vertexData[start].position.x = cos(lat) * cos(lon);
	////		vertexData[start].position.y = sin(lat);
	////		vertexData[start].position.z = cos(lat) * sin(lon);
	////		vertexData[start].position.w = 1.0f;
	////		vertexData[start].texcoord = { u - 1.0f / float(kSubdivision), v};
	////		vertexData[start].normal.x = vertexData[start].position.x;
	////		vertexData[start].normal.y = vertexData[start].position.y;
	////		vertexData[start].normal.z = vertexData[start].position.z;
	////		//左上
	////		vertexData[start + 1].position.x = cos(lat + KLatEvery) * cos(lon);
	////		vertexData[start + 1].position.y = sin(lat + KLatEvery);
	////		vertexData[start + 1].position.z = cos(lat + KLatEvery) * sin(lon);
	////		vertexData[start + 1].position.w = 1.0f;
	////		vertexData[start + 1].texcoord = { u - 1.0f / float(kSubdivision), v - 1.0f / float(kSubdivision) };
	////		vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
	////		vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
	////		vertexData[start + 1].normal.z = vertexData[start + 1].position.z;
	////		//右下
	////		vertexData[start + 2].position.x = cos(lat) * cos(lon + KLonEvery);
	////		vertexData[start + 2].position.y = sin(lat);
	////		vertexData[start + 2].position.z = cos(lat) * sin(lon + KLonEvery);
	////		vertexData[start + 2].position.w = 1.0f;
	////		vertexData[start + 2].texcoord = { u, v};
	////		vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
	////		vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
	////		vertexData[start + 2].normal.z = vertexData[start + 2].position.z;
	////		//右上
	////		vertexData[start + 3].position.x = cos(lat + KLatEvery) * cos(lon + KLonEvery);
	////		vertexData[start + 3].position.y = sin(lat + KLatEvery);
	////		vertexData[start + 3].position.z = cos(lat + KLatEvery) * sin(lon + KLonEvery);
	////		vertexData[start + 3].position.w = 1.0f;
	////		vertexData[start + 3].texcoord = { u, v - 1.0f / float(kSubdivision) };
	////		vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
	////		vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
	////		vertexData[start + 3].normal.z = vertexData[start + 3].position.z;
	////	}
	////}

	//////Sphere用のインデックスリソースを作る
	////const uint32_t kIndexNum = kSubdivision * kSubdivision * 6;
	////ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * kIndexNum);
	//////IndexBufferView(IBV)の作成
	////D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//////リソースの先頭のアドレスから使う
	////indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//////使用するリソースのサイズはインデックス6つ分のサイズ
	////indexBufferView.SizeInBytes = sizeof(uint32_t) * kIndexNum;
	//////インデックスはuint32_tとする
	////indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//////インデックスリソースにデータを書き込む
	////uint32_t* indexData = nullptr;
	////indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	////for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
	////	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
	////		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
	////		uint32_t vertex = (latIndex * kSubdivision + lonIndex) * 4;
	////		indexData[start] = vertex; indexData[start + 1] = vertex + 1; indexData[start + 2] = vertex + 2;
	////		indexData[start + 3] = vertex + 1; indexData[start + 4] = vertex + 3; indexData[start + 5] = vertex + 2;
	////	}
	////}

	////モデル読み込み
	//ModelData modelData = LoadObjFile("Resources", "axis.obj");
	////頂点リソースを作る
	//ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点3つのサイズ
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * UINT(modelData.vertices.size());
	////1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	//vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());//頂点データをリソースにコピー

	////Sprite用の頂点リソースを作る
	//ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	////リソースの先頭のアドレスから使う
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite.Get()->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点6つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	////1頂点当たりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	////頂点リソースにデータを書き込む
	//VertexData* vertexDataSprite = nullptr;
	//vertexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	////三角形の頂点
	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	//vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	//vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	//vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	//vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };
	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	//vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };
	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	//vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	//vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	////IndexSprite用のインデックスリソースを作る
	//ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);
	////IndexBufferView(IBV)の作成
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	////リソースの先頭のアドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite.Get()->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	////インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	////インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;
	//
	////Sprite用のTransformationMatirx用のリソースを作る。Marix4x4 1つ分のサイズを用意する
	//ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	////書き込むためのアドレスを取得
	//transformationMatrixResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	////単位行列を書き込んどく
	//transformationMatrixDataSprite->WVP = Matrix4x4::MakeIdentity4x4();
	//transformationMatrixDataSprite->World = Matrix4x4::MakeIdentity4x4();

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//ComPtr<ID3D12Resource> materialResource = CreateBufferResource(device, sizeof(Material));
	////マテリアルにデータを書き込む
	//Material* materialData = nullptr;
	////書き込むためのアドレスを取得
	//materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	////色を書き込む
	//materialData->color = LoadColor(modelData.material.mtlFilePath);
	////Lightingを有効にする
	//materialData->enableLighting = true;
	////UVTransform単位行列で初期化
	//materialData->uvTransform = Matrix4x4::MakeIdentity4x4();

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	////マテリアルにデータを書き込む
	//Material* materialDataSprite = nullptr;
	////書き込むためのアドレスを取得
	//materialResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	////今回は白を書き込んで見る
	//materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	////Lightingを無効にする
	//materialDataSprite->enableLighting = false;
	////UVTransform単位行列で初期化
	//materialDataSprite->uvTransform = Matrix4x4::MakeIdentity4x4();

	////WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* wvpData;
	////書き込むためのアドレスを取得
	//wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	////単位行列を書き込んでおく
	//wvpData->WVP = Matrix4x4::MakeIdentity4x4();
	//wvpData->World = Matrix4x4::MakeIdentity4x4();

	////DirectionalLightのリソースを作る。
	//ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	//DirectionalLight* directionalLightData;
	//directionalLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	//directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	//directionalLightData->intensity = 1.0f;

	////Transform変数を作る。
	//Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	////SpriteのTransform変数を作る。
	//Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	////SpriteのUVTransform変数を作る。
	//Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	////カメラのTransform変数を作る。
	//Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	////Textureを読んで転送する
	//DirectX::ScratchImage mipImages = LoadTexture("Resources/uvChecker.png");
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device, metadata);
	//ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages, device, commandList);

	////2枚目のTextureを読んで転送する
	//DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//ComPtr<ID3D12Resource> textureResource2 = CreateTextureResource(device, metadata2);
	//ComPtr<ID3D12Resource> intermediateResource2 = UploadTextureData(textureResource2.Get(), mipImages2, device, commandList);

	//commandList->Close();
	//ComPtr<ID3D12CommandList> commandLists[] = { commandList.Get()};
	//commandQueue->ExecuteCommandLists(1,commandLists->GetAddressOf());
	////Fenceの値を更新
	//fenceValue++;
	////GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	//commandQueue->Signal(fence.Get(), fenceValue);
	////Fenceの値が指定したSignal値にたどり着いているかを確認する
	////GetCompletedValueの初期値はFence作成時に渡した初期値
	//if (fence->GetCompletedValue() < fenceValue) {
	//	//指定したSignalにたどり着いてないので、たどり着くまで待つようにイベントを設定する
	//	fence->SetEventOnCompletion(fenceValue, fenceEvent);
	//	//イベント待つ
	//	WaitForSingleObject(fenceEvent, INFINITE);
	//}
	////次のフレーム用のコマンドリストを準備
	//hr = commandAllocator->Reset();
	//assert(SUCCEEDED(hr));
	//hr = commandList->Reset(commandAllocator.Get(), nullptr);
	//assert(SUCCEEDED(hr));
	///*intermediateResource->Release();
	//intermediateResource2->Release();*/

	////metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	//srvDesc2.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////SRVを作成するDescriptorHeapの場所を決める
	////先頭はImGuiが作っているのでその次を使う
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
	////SRVの生成
	//device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	////SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	////SRVの生成
	//device->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	//bool useMonsterBall = true;

	
	MSG msg{};
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		//Windowsメッセージ処理
		if (winApp->ProcessMessage()) {
			//ゲームループをぬける
			break;
		}
		//ゲームの処理
		input->Update();

		//ImGui_ImplDX12_NewFrame();
		//ImGui_ImplWin32_NewFrame();
		//ImGui::NewFrame();

		////開発用のUIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		//ImGui::Begin("Settings");
		//ImGui::ColorEdit4("Model.Color", &(*materialData).color.x);
		//ImGui::ColorEdit4("Sprite.Color", &(*materialDataSprite).color.x);
		//ImGui::DragFloat3("Transform.Rotate", &transform.rotate.x, 0.01f, -2.0f * float(M_PI), 2.0f * float(M_PI));
		//ImGui::DragFloat2("TransformSprite.Scale", &transformSprite.scale.x, 0.01f, 0.0f, 2.0f);
		//ImGui::DragFloat("TransformSprite.Rotate", &transformSprite.rotate.z, 0.01f, 0.0f, 2.0f * float(M_PI));
		//ImGui::DragFloat3("TransformSprite.Translate", &transformSprite.translate.x, 1.0f, 0.0f, 640.0f);
		//if (transformSprite.translate.y > 360.0f) {
		//	transformSprite.translate.y = 360.0f;
		//}
		//ImGui::DragFloat3("Camera.Rotate", &cameraTransform.rotate.x, 0.001f);
		//ImGui::DragFloat3("Camera.Translate", &cameraTransform.translate.x, 0.01f);
		//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::ColorEdit4("DirectionalLightData.Color", &directionalLightData->color.x);
		//ImGui::DragFloat3("DirectionalLightData.Direction", &directionalLightData->direction.x, 0.01f, -1.0f, 1.0f);
		//directionalLightData->direction = Vector3::Normalize(directionalLightData->direction);
		//ImGui::DragFloat("DirectionalLightData.Intensity", &directionalLightData->intensity, 0.01f, 0.0f, 1.0f);
		//ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		//ImGui::End();

		//if (input->PushKey(DIK_A)) {
		//	cameraTransform.translate.x -= 0.1f;
		//}
		//else if (input->PushKey(DIK_D)) {
		//	cameraTransform.translate.x += 0.1f;
		//}
		//if (input->PushKey(DIK_W)) {
		//	cameraTransform.translate.y += 0.1f;
		//}
		//else if (input->PushKey(DIK_S)) {
		//	cameraTransform.translate.y -= 0.1f;
		//}

		////3D
		////transform.rotate.y += 0.03f;
		//Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		//Matrix4x4 cameraMatrix = Matrix4x4::MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		//Matrix4x4 viewMatrix = Matrix4x4::Inverse(cameraMatrix);
		//Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
		//wvpData->WVP = worldViewProjectionMatrix;
		//wvpData->World = worldMatrix;

		////Sprite用のWorldViewProjectionMtrixを作る
		//Matrix4x4 worldMatrixSprite = Matrix4x4::MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		//Matrix4x4 viewMatrixSprite = Matrix4x4::MakeIdentity4x4();
		//Matrix4x4 projectionMatrixSprite = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * viewMatrixSprite * projectionMatrixSprite;
		//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
		//transformationMatrixDataSprite->World = worldMatrixSprite;

		////SpriteのuvTransformを作る
		//Matrix4x4 uvTransformMatrix = Matrix4x4::MakeAffineMatrix(uvTransformSprite.scale, { 0.0f,0.0f,uvTransformSprite.rotate.z }, uvTransformSprite.translate);
		//materialDataSprite->uvTransform = uvTransformMatrix;

		////ImGuiの内部コマンドを生成する
		//ImGui::Render();

		//描画前処理
		dxCommon->PreDraw();

		////RootSignatureを設定。PSOに設定しているけど別途設定が必要
		//commandList->SetGraphicsRootSignature(rootSignature.Get());
		//commandList->SetPipelineState(graphicsPipelineState.Get());//PSOを設定
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
		////commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
		////形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておくとよい
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		////マテリアルCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
		////wvp用のCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
		////SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
		//commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
		////Lighting
		//commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());
		////描画！(DrawCall/ドローコール)3頂点で1つのインスタンス。インスタンスについては
		//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
		////commandList->DrawIndexedInstanced(kIndexNum, 1, 0, 0, 0);

		////Spriteの描画。変更が必要なものだけ変更する
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);//VBVを設定
		//commandList->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
		////マテリアルCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite.Get()->GetGPUVirtualAddress());
		////TransformationMatrixCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite.Get()->GetGPUVirtualAddress());
		////SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
		//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
		////Lighting
		//commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());
		////描画！(DrawColl/ドローコール)
		////commandList->DrawInstanced(6, 1, 0, 0);
		//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		////実際のcommandListのImGuiの描画コマンドを積む
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		//描画後処理
		dxCommon->PostDraw();
	}

	////ImGuiの終了処理。
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();

	winApp->Finalize();

	//解放
	delete input;
	delete winApp;
	delete dxCommon;

	//CloseHandle(fenceEvent);

	return 0;
}