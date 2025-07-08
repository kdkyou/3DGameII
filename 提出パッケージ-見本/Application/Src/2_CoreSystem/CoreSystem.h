#pragma once


// ComPtr短縮名
using Microsoft::WRL::ComPtr;

// 文字コード変換
#include "Utilities/strconv.h"

// GUID
#include "Utilities/KdGuid.h"

#include "KdObject.h"

// クラスアセンブリ
#include "ClassAssembly/KdClassAssembly.h"

// JSON(N・ローマン)
#include "nlohmann/KdJsonUtility.h"

// DirectX
#include "Graphics/KdDirectX.h"

// Audio
#include "Audio/KdAudio.h"

// Math
#include "Math/KdMath.h"

// Collision
#include "Math/KdCollision.h"

// Easing
#include "Math/KdEase.h"

// Graphics 
#include "Graphics/KdGraphics.h"


#include "ResourceManager/KdResourceManager.h"

/*
//====================================================
// 
// コアシステム
// 
//====================================================
class KdCoreSystem
{
public:

//-------------------------------
// シングルトン
//-------------------------------
private:

	KdCoreSystem()
	{
	}


public:
	static KdCoreSystem& GetInstance() {
		static KdCoreSystem instance;
		return instance;
	}
};
*/

inline std::vector<std::string> split(std::string str, char del)
{
	std::vector<std::string> out;
	std::stringstream ss(str);
	std::string buf;
	while (std::getline(ss, buf, del))
	{
		out.push_back(buf);
	}

	return out;
}
