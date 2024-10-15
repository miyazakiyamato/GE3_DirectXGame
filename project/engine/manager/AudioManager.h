#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <unordered_map>
#include <mfapi.h>          // Media Foundationのヘッダファイル追加
#include <mfplay.h>
#include <mfreadwrite.h>

#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib, "mfplat.lib")  // Media Foundationのリンク設定追加
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfreadwrite.lib")

class AudioManager{
public://namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;	

public://構造体
	struct ChunkHeader {//チャンクヘッダ
		char id[4];//チャンク毎のID
		int32_t size;//チャンクサイズ
	};
	struct RiffHeader {//RIFFヘッダチャンク
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};
	struct FormatChunk {//FMTチャンク
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//波型フォーマット
	};
	struct SoundData {//音声データ
		WAVEFORMATEX wfex;//波型フォーマット
		BYTE* pBuffer;//バッファの先頭アドレス
		unsigned int bufferSize;//バッファのサイズ
	};
public://メンバ関数
	//シングルインスタンスの取得
	static AudioManager* GetInstance();
	//初期化
	void Initialize();
	//終了
	void Finalize();
	//音声データ読み込み
	void LoadWave(const std::string& filePath); // WAVE読み込み用関数
	void LoadMP3(const std::string& filePath);  // MP3読み込み用関数
	//音声再生
	void PlayWave(const std::string& filePath); // WAVE再生用関数
	void PlayMP3(const std::string& filePath);  // MP3再生用関数
private://メンバ変数
	static AudioManager* instance;

	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(AudioManager&) = delete;
	AudioManager& operator=(AudioManager&) = delete;

	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;

	const std::string audioFilePath = "resources/";

	// 音声格納
	std::unordered_map<std::string, SoundData> soundDatas;
};

