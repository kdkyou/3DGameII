#pragma once
//===============================================
//
// プリコンパイル済みヘッダー
//  ここに書いたものは初回のみ解析されるため、コンパイル時間が高速になる。
//  全てのcppからインクルードされる必要がある。
//
//===============================================


//===============================================
//
// 基本
//
//===============================================

#pragma comment(lib,"winmm.lib")

#define NOMINMAX
//#include <windows.h>
//#include <stdio.h>

#include <wrl/client.h>

//===============================================
//
// STL
//
//===============================================
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <filesystem>
#include <execution>
#include <any>

//DPI変換
#include<ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")


// 安全にReleaseするための関数
template<class T>
void KdSafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

// 安全にDeleteするための関数
template<class T>
void KdSafeDelete(T*& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

//===============================================
// コアシステム
//===============================================
#include "2_CoreSystem/CoreSystem.h"



/*
//===============================================
//
// Direct3D11
//
//===============================================
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

// DirectX Tool Kit
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DirectXTKAudioWin8.lib")
#include <SimpleMath.h>
#include <Audio.h>

// DirectX Tex
#pragma comment(lib, "DirectXTex.lib")
#include <DirectXTex.h>

// DirectXMesh
#include <DirectXMesh.h>


//===============================================
//
// imgui
//
//===============================================
#include "imgui/imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui/imgui_internal.h"

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

#include "imgui/ImGuizmo.h"

//===============================================
// json
//===============================================
//#include "json11/json11.hpp"
#include "json11KdCustom/json11.hpp"

//===============================================
// 文字コード変換
//===============================================
#include "strconv.h"

//===============================================
//
// 自作System
//
//===============================================
#include "System/KdSystem.h"
*/

