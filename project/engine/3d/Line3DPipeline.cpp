#include "Line3DPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

void Line3DPipeline::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	CreateGraphicsPipeline(BlendMode::kNone);
	CreateGraphicsPipeline(BlendMode::kNormal);
	/*CreateGraphicsPipeline(BlendMode::kAdd);
	CreateGraphicsPipeline(BlendMode::kSubtract);
	CreateGraphicsPipeline(BlendMode::kMultiply);
	CreateGraphicsPipeline(BlendMode::kScreen);*/
}

void Line3DPipeline::DrawSetting(BlendMode blendMode) {
	//読み込み済みブレンドモードを検索
	assert(graphicsPipelineState_.contains(blendMode));

	// コマンドリストの取得
	ComPtr<ID3D12GraphicsCommandList> commandList = dxCommon_->GetCommandList();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(graphicsPipelineState_[blendMode].Get());//PSOを設定
	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておくとよい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Line3DPipeline::CreateRootSignature() {
	HRESULT hr;

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	//座標と色
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	//rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	//rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド

	descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //配列の長さ


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイナリフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリをもとに生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void Line3DPipeline::CreateGraphicsPipeline(BlendMode blendMode) {
	//読み込み済みブレンドモードを検索
	if (graphicsPipelineState_.contains(blendMode)) {
		//読み込み済みなら早期return
		return;
	}
	HRESULT hr;

	CreateRootSignature();


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "COLOR";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//カリングしない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶさない
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//Shaderをコンパイルする
	ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Line3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Line3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };//PixeLShader
	graphicsPipelineStateDesc.BlendState = BlendModeHelper::BlendModeFunctionTable[static_cast<size_t>(blendMode)]();//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。線
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	////DepthStencilStateの設定
	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	////Depthの機能を有効化する
	//depthStencilDesc.DepthEnable = true;
	////書き込みします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	////比較関数はLessEqual。つまり、近ければ描画される
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	////DepthStencilの設定
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[blendMode]));
	assert(SUCCEEDED(hr));
}