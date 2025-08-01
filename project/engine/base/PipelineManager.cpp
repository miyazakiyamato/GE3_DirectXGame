#include "PipelineManager.h"
#include "Logger.h"
#include "StringUtility.h"
#include "PipelineFactory.h"

using namespace Microsoft::WRL;
using namespace Logger;
using namespace StringUtility;

PipelineManager* PipelineManager::instance = nullptr;

PipelineManager* PipelineManager::GetInstance(){
	if (instance == nullptr) {
		instance = new PipelineManager;
	}
	return instance;
}

void PipelineManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

void PipelineManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void PipelineManager::DrawSetting(const std::string& stateName){
	if (nowStateName_ == stateName) {
		//読み込み済みなら早期return
		return;
	}

	GraphicsPipelineData* pipelineData = graphicsPipelineDates_[stateName].get();
	// コマンドリストの取得
	ComPtr<ID3D12GraphicsCommandList> commandList = dxCommon_->GetCommandList();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(pipelineData->rootSignature.Get());
	commandList->SetPipelineState(pipelineData->pipelineState.Get());//PSOを設定
	if (pipelineData->state.isOffScreen == false) {
		//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておくとよい
		switch (pipelineData->state.primitiveTopologyType) {
		default:
		case PrimitiveTopologyType::kTriangleList:
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case PrimitiveTopologyType::kLineList:
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		case PrimitiveTopologyType::kPointList:
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;
		}
	}
	nowStateName_ = stateName;
}

void PipelineManager::DrawSettingCS(const std::string& stateName){
	ComputePipelineData* pipelineData = computePipelineDates_[stateName].get();
	// コマンドリストの取得
	ComPtr<ID3D12GraphicsCommandList> commandList = dxCommon_->GetCommandList();
	//RootSignatureを設定。
	commandList->SetComputeRootSignature(pipelineData->rootSignature.Get());
	commandList->SetPipelineState(pipelineData->pipelineState.Get());//コンピュートPSOを設定

	nowStateName_ = stateName;
}

std::string  PipelineManager::CreatePipelineState(const PipelineState& pipelineState) {
	std::string setStateName = pipelineState.shaderName;
	int id = 0;
	//読み込み済みなら
	if (graphicsPipelineDates_.count(setStateName) > 0) {
		for (const auto& [name, pipelineData] : graphicsPipelineDates_) {
			if (name.rfind(setStateName, 0) == 0) {
				if (pipelineData->state.shaderName == pipelineState.shaderName &&
					pipelineData->state.cullMode == pipelineState.cullMode &&
					pipelineData->state.fillMode == pipelineState.fillMode &&
					pipelineData->state.blendMode == pipelineState.blendMode &&
					pipelineData->state.depthMode == pipelineState.depthMode &&
					pipelineData->state.staticSamplersMode == pipelineState.staticSamplersMode &&
					pipelineData->state.primitiveTopologyType == pipelineState.primitiveTopologyType &&
					pipelineData->state.isOffScreen == pipelineState.isOffScreen) {
					//同じパイプライン状態なら早期return
					if (id == 0) {
						//IDが0ならそのまま返す
						return setStateName;
					}
					return setStateName + std::to_string(id);
				}
				id++;
			}
		}
		setStateName += std::to_string(id);
	}

	GraphicsPipelineData data;
	data.state = pipelineState;

	//ルートシグネチャの作成
	CreateGraphicsRootSignature(data);
	//グラフィックスパイプラインの生成
	CreateGraphicsPipeline(data);

	std::unique_ptr<GraphicsPipelineData> state = std::make_unique<GraphicsPipelineData>(data);
	graphicsPipelineDates_[setStateName] = std::move(state);
	return setStateName;
}

std::string PipelineManager::CreateComputePipelineState(const std::string& shaderName){
	if (computePipelineDates_.count(shaderName) > 0) {
		//読み込み済みなら
		return shaderName;
	}
	ComputePipelineData data;
	data.shaderName = shaderName;
	//コンピュートルートシグネチャの作成
	CreateComputeRootSignature(data);
	//コンピュートシェーダーの生成
	CreateComputePipeline(data);
	std::unique_ptr<ComputePipelineData> state = std::make_unique<ComputePipelineData>(data);
	computePipelineDates_[shaderName] = std::move(state);
	return shaderName;
}

void PipelineManager::CreateGraphicsRootSignature(GraphicsPipelineData& pipeline){
	HRESULT hr;

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	std::unique_ptr<BasePipeline> basePipeline(PipelineFactory::ChangePipeline(pipeline.state.shaderName));
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = basePipeline->DescriptorRanges();
	std::vector<D3D12_ROOT_PARAMETER> rootParametersVector = basePipeline->RootParameters(descriptorRanges);
	const uint32_t rootParametersCount = static_cast<uint32_t>(rootParametersVector.size());
	
	// 動的配列を作成しvectorからコピー
	std::unique_ptr<D3D12_ROOT_PARAMETER[]> rootParameters(new D3D12_ROOT_PARAMETER[rootParametersCount]);
	std::copy_n(rootParametersVector.begin(), rootParametersCount, rootParameters.get());

	descriptionRootSignature.pParameters = rootParameters.get();
	descriptionRootSignature.NumParameters = rootParametersCount;

	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplersVector = basePipeline->StaticSamplers(pipeline.state.staticSamplersMode);
	const uint32_t staticSamplersCount = static_cast<uint32_t>(staticSamplersVector.size());

	// 動的配列を作成しvectorからコピー
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> staticSamplers(new D3D12_STATIC_SAMPLER_DESC[staticSamplersCount]);
	std::copy_n(staticSamplersVector.begin(), staticSamplersCount, staticSamplers.get());

	descriptionRootSignature.pStaticSamplers = staticSamplers.get();
	descriptionRootSignature.NumStaticSamplers = staticSamplersCount;

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
		IID_PPV_ARGS(&pipeline.rootSignature));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreateGraphicsPipeline(GraphicsPipelineData& pipeline){
	HRESULT hr;
	
	//InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	std::unique_ptr<BasePipeline> basePipeline(PipelineFactory::ChangePipeline(pipeline.state.shaderName));
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescsVector = basePipeline->InputElementDesc();
	const uint32_t inputElementDescsCount = static_cast<uint32_t>(inputElementDescsVector.size());

	// 動的配列を作成しvectorからコピー
	std::unique_ptr<D3D12_INPUT_ELEMENT_DESC[]> inputElementDescs(new D3D12_INPUT_ELEMENT_DESC[inputElementDescsCount]);
	std::copy_n(inputElementDescsVector.begin(), inputElementDescsCount, inputElementDescs.get());

	inputLayoutDesc.pInputElementDescs = inputElementDescs.get();
	inputLayoutDesc.NumElements = inputElementDescsCount;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//カリング
	switch (pipeline.state.cullMode){
	default:
	case CullMode::kNone://カリングなし
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case CullMode::kFront://フロントカリング
		rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case CullMode::kBack://バックカリング
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		break;
	}
	switch (pipeline.state.fillMode){
	default:
	case FillMode::kSolid://ソリッド描画
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		break;
	case FillMode::kWireframe://ワイヤーフレーム描画
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		break;
	}
	//Shaderをコンパイルする
	ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(shaderFilePath_ + ConvertString(pipeline.state.shaderName) + vsFilePath_, L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(shaderFilePath_ + ConvertString(pipeline.state.shaderName) + psFilePath_, L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = pipeline.rootSignature.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };//PixeLShader
	graphicsPipelineStateDesc.BlendState = BlendModeHelper::BlendModeFunctionTable[static_cast<size_t>(pipeline.state.blendMode)]();//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	switch (pipeline.state.primitiveTopologyType) {
	default:
	case PrimitiveTopologyType::kTriangleList:
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	case PrimitiveTopologyType::kLineList:
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		break;
	case PrimitiveTopologyType::kPointList:
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		break;
	}
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	switch (pipeline.state.depthMode) {
	default:
	case DepthMode::kEnable:
		//Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		//DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case DepthMode::kDisable:
		//Depthの機能を無効化する
		depthStencilDesc.DepthEnable = false;
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		break;
	case DepthMode::kReadOnly:
		//Depthの機能を読み取り専用にする
		depthStencilDesc.DepthEnable = true;
		//書き込みしない
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		//比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		//DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	}
	//実際に生成
	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pipeline.pipelineState));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreateComputeRootSignature(ComputePipelineData& pipeline) {
	HRESULT hr;
	//RootSignature作成
	std::unique_ptr<BasePipeline> basePipeline(PipelineFactory::ChangePipeline(pipeline.shaderName));
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = basePipeline->ComputeDescriptorRanges();
	std::vector<D3D12_ROOT_PARAMETER> rootParametersVector = basePipeline->ComputeRootParameters(descriptorRanges);
	const uint32_t rootParametersCount = static_cast<uint32_t>(rootParametersVector.size());
	
	// 動的配列を作成しvectorからコピー
	std::unique_ptr<D3D12_ROOT_PARAMETER[]> rootParameters(new D3D12_ROOT_PARAMETER[rootParametersCount]);
	std::copy_n(rootParametersVector.begin(), rootParametersCount, rootParameters.get());

	// ルートシグネチャ全体のデスクリプション
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters = rootParameters.get();
	descriptionRootSignature.NumParameters = rootParametersCount;

	descriptionRootSignature.pStaticSamplers = nullptr;
	descriptionRootSignature.NumStaticSamplers = 0;

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
		IID_PPV_ARGS(&pipeline.rootSignature));
}

void PipelineManager::CreateComputePipeline(ComputePipelineData& pipeline){
	HRESULT hr;
	// シェーダーをコンパイルする
	// コンピュートシェーダーのファイルパスを指定
	ComPtr<IDxcBlob> computeShaderBlob = dxCommon_->CompileShader(shaderFilePath_ + ConvertString(pipeline.shaderName) + csFilePath_,L"cs_6_0");
	assert(computeShaderBlob != nullptr);
	
	// ComputePipelineStateDescの作成
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	// Compute Shaderの設定
	computePipelineStateDesc.CS = {
		.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
		.BytecodeLength = computeShaderBlob->GetBufferSize()
	};
	// NodeMask (通常は0で問題ありません)
	computePipelineStateDesc.NodeMask = 0;
	// CachedPSO (今回は使用しない)
	computePipelineStateDesc.CachedPSO = {};
	// Flags (通常はnone)
	computePipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	computePipelineStateDesc.pRootSignature = pipeline.rootSignature.Get();
	// 実際に生成
	hr = dxCommon_->GetDevice()->CreateComputePipelineState(
		&computePipelineStateDesc,
		IID_PPV_ARGS(&pipeline.pipelineState) // 結果を GraphicsPipelineData に格納
	);
	assert(SUCCEEDED(hr));
}