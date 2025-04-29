//===============================================================
//
// 音関係のクラスをまとめて書いています。
//
//	KdSoundInstanceクラス	… 実際再生されるサウンドインスタンス。KdSoundEffectクラスから生成される。
//	KdSoundEffectクラス	… サウンドの読み込みや、実際に再生するインスタンスの再生など。
//	KdAudioManagerクラス	… XAudioの初期化・解放や再生中のサウンド管理など。
//
//===============================================================
#pragma once

// DirectX Tool Kit
#pragma comment(lib, "xaudio2_9redist.lib")
#include <Audio.h>

// 
#include "2_CoreSystem/Math/KdMath.h"


class KdSoundInstance;
class KdSoundEffect;

//=======================================================
// 再生用サウンドインスタンスクラス
//
//  実際に再生するサウンドオブジェクト
//  KdSoundEffectクラスのCreateInstance()で作成可能
//
//=======================================================
class KdSoundInstance : public std::enable_shared_from_this<KdSoundInstance>{
	friend class KdSoundEffect;
public:

	//==================================================================
	// 取得
	//==================================================================
	DirectX::AudioEmitter&			GetEmitter() { return m_emitter; }
	std::shared_ptr<KdSoundEffect>	GetSoundEffect() { return m_srcData; }


	//==================================================================
	// 再生
	//==================================================================

	// 2D再生
	// ・loop		… ループ再生する？
	// ・manage		… AudioManagerが再生管理をする？
	void Play(bool loop = false, bool manage = true);

	// 3D再生
	// ・pos		… 座標
	// ・loop		… ループ再生するか？
	// ・manage		… AudioManagerが再生管理をする？
	void Play3D(const Math::Vector3& pos, bool loop = false, bool manage = true);

	// m_Emitterの情報を適用する
	void Apply3D();

	//==================================================================
	// 停止
	//==================================================================
	// 停止
	void Stop() {
		if(m_instance)m_instance->Stop();
	}

	// 一時停止
	void Pause() {
		if (m_instance)m_instance->Pause();
	}

	// 再開
	void Resume() {
		if (m_instance)m_instance->Resume();
	}

	//==================================================================
	// 設定
	//==================================================================
	// ボリューム設定
	// ・vol	… ボリューム設定(1.0が100%)
	void SetVolume(float vol);
	
	// 3Dサウンド座標設定
	// ・pos		… 座標
	void SetPos(const Math::Vector3& pos);

	// 減衰倍率設定 1:通常 FLT_MIN～FLT_MAX
	void SetCurveDistanceScaler(float val);

	//==================================================================
	// 取得
	//==================================================================
	// 再生中？
	bool IsPlay();

	// ポーズ中？
	bool IsPause();

	//==================================================================
	~KdSoundInstance();


private:

	//==================================================================
	// 設定
	//==================================================================
	// 初期設定
	void Init(std::unique_ptr<DirectX::SoundEffectInstance> inst, const std::shared_ptr<KdSoundEffect>& soundEffect, bool enable3D)
	{
		m_instance = std::move(inst);
		m_srcData = soundEffect;
		m_is3D = enable3D;
	}

	// 
	KdSoundInstance(){}

private:

	// サウンドの再生インスタンス
	std::unique_ptr<DirectX::SoundEffectInstance>	m_instance;

	// 再生サウンドの元データ
	std::shared_ptr<KdSoundEffect>					m_srcData;

	// エミッター 主に3Dサウンドソースの定義
	DirectX::AudioEmitter							m_emitter;

	// 3Dサウンドか
	bool											m_is3D = false;

private:
	// コピー禁止用
	KdSoundInstance(const KdSoundInstance& src) = delete;
	void operator=(const KdSoundInstance& src) = delete;
};

//=======================================================
//
// サウンドデータクラス
//
//　サウンドの元データとなるクラス。これ自体は再生できません。
//　CreateInstance()で実際に再生するサウンドKdSoundInstanceクラスのインスタンスを生成。
//
//=======================================================
class KdSoundEffect : public std::enable_shared_from_this<KdSoundEffect> {
public:

	// WAVEサウンド読み込み
	// ・filename	… ファイル名
	// 戻り値		… 成功：true
	bool Load(const std::string& fileName);

	// 再生用のサウンドインスタンス作成
	// ・b3D			… 3Dサウンドにするか？
	// ・戻り値		… 新たに作成した再生用サウンドインスタンス
	std::shared_ptr<KdSoundInstance> CreateInstance(bool b3D);

	~KdSoundEffect() {
		m_soundEffect = nullptr;
		m_fileName = "";
	}

	// 
	KdSoundEffect() {}

private:
	// サウンドエフェクト
	std::unique_ptr<DirectX::SoundEffect>	m_soundEffect;

	// ファイル名
	std::string								m_fileName;

private:
	// コピー禁止用
	KdSoundEffect(const KdSoundEffect& src) = delete;
	void operator=(const KdSoundEffect& src) = delete;
};

//================================================
//
// サウンド管理 シングルトンクラス
//
// 
//================================================
class KdAudioManager{
	friend class KdSoundInstance;
public:
	//==================================================================
	// 取得系
	//==================================================================
	//  AudioEngine取得
	std::unique_ptr<DirectX::AudioEngine>& GetAudioEngine() { return m_audioEng; }

	// 3Dリスナー(聞き手)取得
	DirectX::AudioListener&	GetListener() { return m_listener; }

	// 初期化されているか？
	bool	IsInit() { return (m_audioEng != nullptr); }

	// 現在再生中のサウンド数取得
	int		GetNumPlaying() { return (int)m_playList.size(); }


	//==================================================================
	// 初期化・解放
	//==================================================================

	// 初期化
	bool Init();

	// 解放
	void Release();

	//==================================================================
	// 再生サウンド操作
	//==================================================================

	// すべて停止
	void StopAll();

	//==================================================================
	// 処理
	//==================================================================
	// 処理　定期的に実行してください
	// 再生中のサウンドの管理やリスナーの設定繁栄などを行っています
	void Update();

	//==================================================================
	~KdAudioManager() {
		Release();
	}

private:

	// AudioEngine
	std::unique_ptr<DirectX::AudioEngine>				m_audioEng;

	// 3Dサウンド用リスナー
	DirectX::AudioListener								m_listener;

	// 現在再生中のサウンド管理リスト
	std::map<uint64_t, std::shared_ptr<KdSoundInstance>>		m_playList;

private:

	// 再生リスト追加。実際に再生されているものになる。
	void AddPlayList(const std::shared_ptr<KdSoundInstance>& snd){
		if(snd == nullptr)return;
		m_playList[(uint64_t)snd.get()] = snd;
	}

//===================================================================
// シングルトン
//===================================================================
private:
	KdAudioManager() {}

public:
	static KdAudioManager& GetInstance()
	{
		static KdAudioManager	instance;
		return instance;
	}
};
