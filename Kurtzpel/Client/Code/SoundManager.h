#pragma once

//#include "Define.h"

enum SoundChannel { BGM, PLAYER, PLAYER_EFFECT, MONSTER, EFFECT, MAXCHANNEL };

class SoundManager
{
public:
	static SoundManager* GetInstance();
	static void Destroy();

private:
	SoundManager();
	~SoundManager();

public:
	static void Initialize();
	static void Update();
	static void Release();
public:
	static void PlaySound(TCHAR* pSoundKey, SoundChannel eID);
	static void PlayOverlapSound(TCHAR* pSoundKey, SoundChannel eID, float offsetVolume = 1.f, float duration = 0.2f);
	static void PlayOverlapSoundWithAmp(TCHAR* pSoundKey, SoundChannel eID);
	static void PlayBGM(TCHAR* pSoundKey);
	static void StopSound(SoundChannel eID);
	static void StopAll();
	static void SetVolume(SoundChannel channel, float per);

private:
	static void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> soundMap;
	map<TCHAR*, pair<bool, float>> soundTimeMap;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* channels[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* pSystem;
	float volume;
};


