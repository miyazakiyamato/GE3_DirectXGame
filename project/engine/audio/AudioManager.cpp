#include "AudioManager.h"
#include <cassert>
#include <mfapi.h>          // Media Foundationの基本ヘッダー
#include <mfidl.h>          // Media FoundationのCOMインターフェイスのヘッダー
#include <mfreadwrite.h>    // Media Foundationのファイル読み書き用ヘッダー

#pragma comment(lib, "Uuid.Lib")       // GUIDを扱うためのライブラリ
#pragma comment(lib, "Mfplat.lib")     // Media Foundationのライブラリ
#pragma comment(lib, "Mf.lib")         // Media Foundationのメインライブラリ
#pragma comment(lib, "mfreadwrite.lib")// Media Foundationのファイル読み書き用ライブラリ

#include <initguid.h>  // GUIDの初期化を有効にするために必要

// MF_MT_MAJOR_TYPE の定義
DEFINE_GUID(MF_MT_MAJOR_TYPE,
	0x48eba18e, 0xf8c9, 0x4687, 0xbf, 0x11, 0x0a, 0x74, 0xc9, 0xf9, 0x6a, 0x8f);

// MFMediaType_Audio の定義
DEFINE_GUID(MFMediaType_Audio,
	0x73647561, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// MF_MT_SUBTYPE の定義
DEFINE_GUID(MF_MT_SUBTYPE,
	0xf7e34c9a, 0x42e8, 0x4714, 0xb7, 0x35, 0x82, 0xa8, 0x43, 0xb6, 0xe2, 0x3d);

// MFAudioFormat_PCM の定義
DEFINE_GUID(MFAudioFormat_PCM,
	0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// MF_MT_AUDIO_SAMPLES_PER_SECOND の定義
DEFINE_GUID(MF_MT_AUDIO_SAMPLES_PER_SECOND,
	0x4A4E8647, 0xB66B, 0x4C6D, 0x9E, 0xB1, 0x7E, 0xF7, 0xBB, 0xA5, 0x0C, 0xE4);

// MF_MT_AUDIO_NUM_CHANNELS の定義
DEFINE_GUID(MF_MT_AUDIO_NUM_CHANNELS,
	0x7D85B7C2, 0x1F30, 0x4625, 0xBA, 0x9B, 0x45, 0x65, 0xC3, 0x2B, 0x8E, 0xFF);

AudioManager* AudioManager::instance = nullptr;

AudioManager* AudioManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new AudioManager;
	}
	return instance;
}


void AudioManager::Initialize(){
	HRESULT result;

	//xAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	// Media Foundationの初期化
	result = MFStartup(MF_VERSION);
	assert(SUCCEEDED(result));
}

void AudioManager::Finalize(){
	//xAudio2解放
	xAudio2.Reset();
	//音声データ解放
	soundDatas.clear();

	// Media Foundationの終了
	MFShutdown();

	delete instance;
	instance = nullptr;
}

void AudioManager::LoadWave(const std::string& filePath)
{
	//読み込み済みテクスチャを検索
	if (soundDatas.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}

	const std::string filename = audioFilePath + filePath;

	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//RIFFヘッダーの読み込み
	RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format{};
	while (file.good()) {
		// チャンクヘッダーの確認
		file.read((char*)&format, sizeof(ChunkHeader));
		if (file.eof()) {
			assert(0); // チャンクが見つからずファイルの終わりに到達した場合
		}
		if (strncmp(format.chunk.id, "fmt ", 4) == 0) {
			// チャンク本体の読み込み（16バイト、18バイト、または40バイトの場合の処理）
			if (format.chunk.size == 16 || format.chunk.size == 18) {
				// 16バイトまたは18バイトのフォーマットを読み込む
				assert(format.chunk.size <= sizeof(format.fmt));
				file.read((char*)&format.fmt, format.chunk.size);
			}
			else if (format.chunk.size == 40) {
				// 40バイトのフォーマットの場合、最初の18バイトのみ読み込む
				file.read((char*)&format.fmt, 18);
				// 残りの22バイトを無視する（スキップ）
				file.ignore(static_cast<std::streamsize>(format.chunk.size) - 18);
			}
			else {
				// サポートしていないフォーマットサイズの場合
				assert(0);
			}
			break; // 正常にfmtチャンクを読み込んだらループを抜ける
		}
		else {
			// fmtチャンクでない場合はスキップ
			file.ignore(format.chunk.size);
		}
	}

	// Dataチャンクの読み込み
	ChunkHeader data{};
	while (file.good()) {
		file.read((char*)&data, sizeof(data));
		if (file.eof()) {
			assert(0); // チャンクが見つからずファイルの終わりに到達した場合
		}
		// JUNKやその他の無視すべきチャンクを検出した場合
		if (strncmp(data.id, "data", 4) == 0) {
			break; // data チャンクが見つかったらループを抜ける
		}
		else {
			// 不要なチャンクはスキップ
			file.ignore(data.size);
		}
	}

	//Dataチャンクのデータ部(波型データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//Waveファイルを閉じる
	file.close();

	//追加した音声データの参照を取得
	SoundData& soundData = soundDatas[filePath];

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
}

void AudioManager::LoadMP3(const std::string& filePath){
	// 読み込み済みなら早期return
	if (soundDatas.contains(filePath)) {
		return;
	}

	const std::string filename = audioFilePath + filePath;

	IMFSourceReader* pSourceReader = nullptr;
	IMFMediaType* pAudioType = nullptr;

	// Media FoundationでMP3ファイルを読み込む
	HRESULT result = MFCreateSourceReaderFromURL(std::wstring(filename.begin(), filename.end()).c_str(), nullptr, &pSourceReader);
	assert(SUCCEEDED(result));

	// PCM形式へのデコード設定
	// Media Typeの作成
	result = MFCreateMediaType(&pAudioType);
	assert(SUCCEEDED(result));

	// ストリーム情報を取得する
	UINT32 sampleRate = 44100;
	UINT32 channelCount = 2;
	UINT32 bytesPerSample{};

	// 最初のオーディオストリームのフォーマットを取得
	result = pSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pAudioType);
	if (SUCCEEDED(result)) {
		// サンプルレートを取得
		result = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
		if (FAILED(result)) {
			pSourceReader->Release();
			pAudioType->Release();
			return;
		}

		// チャンネル数を取得
		result = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_NUM_CHANNELS, channelCount);
		if (FAILED(result)) {
			pSourceReader->Release();
			pAudioType->Release();
			return;
		}

		// IMFMediaTypeの設定
		result = pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		if (SUCCEEDED(result)) {
			result = pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM); // PCM形式に設定
		}
		if (SUCCEEDED(result)) {
			result = pAudioType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
		}
		if (SUCCEEDED(result)) {
			result = pAudioType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channelCount);
		}
	}

	//// メジャータイプをオーディオに設定
	//result = pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	//assert(SUCCEEDED(result));
	//// サブタイプをPCMに設定
	//result = pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	//assert(SUCCEEDED(result));
	//// サンプルレートを44100に設定（MP3ファイルのサンプルレートが異なる場合は適宜変更）
	//result = pAudioType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
	//assert(SUCCEEDED(result));
	//// チャネル数を2（ステレオ）に設定
	//result = pAudioType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
	//assert(SUCCEEDED(result));

	result = pSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pAudioType);
	assert(SUCCEEDED(result));

	// デコードされたPCMデータの読み込み
	IMFSample* pSample = nullptr;
	DWORD streamIndex, flags;
	LONGLONG llTimeStamp;
	std::vector<BYTE> pcmData;

	while (true) {
		result = pSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
		if (FAILED(result) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
			break;  // ストリームの終わり
		}

		if (pSample) {
			IMFMediaBuffer* pBuffer = nullptr;
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pAudioData = nullptr;
			DWORD bufferSize = 0;
			pBuffer->Lock(&pAudioData, NULL, &bufferSize);

			pcmData.insert(pcmData.end(), pAudioData, pAudioData + bufferSize);

			pBuffer->Unlock();
			pBuffer->Release();
			pSample->Release();
		}
	}

	// 読み込んだPCMデータをXAudio2用に設定
	SoundData soundData;
	soundData.pBuffer = new BYTE[pcmData.size()];
	std::copy(pcmData.begin(), pcmData.end(), soundData.pBuffer);
	soundData.bufferSize = static_cast<unsigned int>(pcmData.size());

	// WAVEFORMATEXの設定
	WAVEFORMATEX wfex = {};
	wfex.wFormatTag = WAVE_FORMAT_PCM;        // PCMフォーマット
	wfex.nChannels = channelCount;            // MP3ファイルから取得したチャンネル数
	wfex.nSamplesPerSec = sampleRate;         // MP3ファイルから取得したサンプルレート
	wfex.wBitsPerSample = 16;                  // 16ビットPCMを使用
	wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	// soundData.wfexに設定
	soundData.wfex = wfex;

	soundDatas[filePath] = soundData;

	// クリーンアップ
	pSourceReader->Release();
	pAudioType->Release();
}

void AudioManager::PlayWave(const std::string& filePath, float volume, bool loop){
	//音声データの参照を取得
	SoundData& soundData = soundDatas[filePath];

	HRESULT result;

	//波型フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//生成する波型データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// ループの設定
	buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

	//波型データの生成
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
	result = pSourceVoice->SetVolume(volume);

	if (loop)
	{
		if (playSoundDatas.contains(filePath)) {
			return;
		}
		playSoundDatas[filePath] = pSourceVoice;
	}
}

void AudioManager::PlayMP3(const std::string& filePath){
	SoundData& soundData = soundDatas[filePath];

	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = xAudio2->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	XAUDIO2_BUFFER buffer = {};
	buffer.pAudioData = soundData.pBuffer;
	buffer.AudioBytes = soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	result = sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(result));

	sourceVoice->Start();
}

void AudioManager::StopWave(const std::string& filePath)
{
	IXAudio2SourceVoice* pSourceVoice = playSoundDatas[filePath];

	//波型データの生成
	pSourceVoice->Stop();

	playSoundDatas.erase(filePath);
}
