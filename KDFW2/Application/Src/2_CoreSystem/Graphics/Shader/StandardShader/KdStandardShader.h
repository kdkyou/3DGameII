#pragma once

#include "../../KdDirectX.h"

//============================================
// 
// Lit Shader
// 
//============================================
class KdStandardShader : public KdShader
{
public:

	KdStandardShader();

	~KdStandardShader()
	{
		Release();
	}

	void Initialize(const std::string& folderPath, const std::string& hlslFilename);

private:
	void Release();
};
