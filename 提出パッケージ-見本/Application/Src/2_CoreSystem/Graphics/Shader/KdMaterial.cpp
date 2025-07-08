#include "KdMaterial.h"

void KdMaterial::SetData(const std::string& name)
{
	m_name = name;

	// 名前に、@Transparent がついてる奴は、半透明マテリアルとする
	auto nameOpts = split(m_name, '@');
	if (nameOpts.size() >= 2 && nameOpts[1] == "Transparent")
	{
//		m_opaque = false;
	}
}

// マテリアルデータやシェーダーをデバイスへセットする

bool KdMaterial::SetToDevice(uint32_t passIdx)
{
	Pass* pass = GetPass(passIdx);
	if (pass == nullptr)return false;

	auto shaderPass = m_shader->GetPass(pass->m_passIndex);

	//---------------------------
	// テクスチャをセット
	//---------------------------
	for (auto&& tex : pass->m_textures)
	{
		D3D.SetTextureToVS(tex.first, tex.second.get());
		D3D.SetTextureToPS(tex.first, tex.second.get());
		D3D.SetTextureToGS(tex.first, tex.second.get());
	}

	//---------------------------
	// マテリアル情報：定数バッファをセット
	//---------------------------
	if (pass->m_cBuffer != nullptr)
	{
		pass->m_cBuffer->WriteWorkData();
		pass->m_cBuffer->SetVS(0);
		pass->m_cBuffer->SetPS(0);
		pass->m_cBuffer->SetGS(0);
	}

	//---------------------------
	// シェーダーをセット
	//---------------------------
	// 頂点シェーダをセット
	if (shaderPass->m_VS != nullptr)
	{
		D3D.GetDevContext()->VSSetShader(shaderPass->m_VS.Get(), 0, 0);
	}

	// 頂点レイアウトをセット
	if (shaderPass->m_inputLayout != nullptr)
	{
		D3D.GetDevContext()->IASetInputLayout(shaderPass->m_inputLayout.Get());
	}

	// ピクセルシェーダー
	if (shaderPass->m_PS != nullptr)
	{
		D3D.GetDevContext()->PSSetShader(shaderPass->m_PS.Get(), 0, 0);
	}

	// ジオメトリシェーダー
	if (shaderPass->m_GS != nullptr)
	{
		D3D.GetDevContext()->GSSetShader(shaderPass->m_GS.Get(), 0, 0);
	}

	return true;
}

void KdMaterial::Deserialize(const nlohmann::json& jsonData)
{
	m_name = jsonData["Name"].get<std::string>();

	auto shaderName = jsonData["Shader"].get<std::string>();
	m_shader = KdShaderManager::GetInstance().GetShader(shaderName);

	// 全シェーダーパスから、パス作成
	CreatePassesFromShader(nullptr);

	auto& pass = m_passes[0];

	// テクスチャ
	{
		m_passes[0].m_textures.clear();

		std::vector<std::string> texAry;
		KdJsonUtility::GetArray(jsonData, "Textures", texAry);

		for(uint32_t i = 0; i < texAry.size(); i++)
		{
			auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(texAry[i]);

			m_passes[0].m_textures[i] = tex;
		}
	}

	// マテリアルデータ
	auto materialData = jsonData["MaterialData"];
	for (auto&& pair : materialData.items())
	{
		std::string paramName = pair.key();

		auto val = pair.value();

		// スカラー
		if (val.is_number())
		{
			pass.m_cBuffer->SetValue(paramName, val.get<float>());
		}
		// 2Dベクトル
		else if (val.is_array() && val.size() == 2)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdVector2&)list[0]);
		}
		// 3Dベクトル
		else if (val.is_array() && val.size() == 3)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdVector3&)list[0]);
		}
		// 4Dベクトル
		else if (val.is_array() && val.size() == 4)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdVector4&)list[0]);
		}
		// 4x4行列
		else if (val.is_array() && val.size() == 16)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdMatrix&)list[0]);
		}
		else
		{
			// ※他の型も実装する必要あり
			// assert(0 && "[KdMatrix] 対応していないフォーマットです");
		}

	}
}

void KdMaterial::Serialize(nlohmann::json& outJson) const
{
	nlohmann::json js = nlohmann::json::object();

	// マテリアル名
	js["Name"] = m_name;

	// シェーダー名
	js["Shader"] = m_shader->GetName();

//	js["Opaque"] = m_opaque;

	// テクスチャ
	{
		std::vector<std::string> ary;

		for (auto&& pair : m_passes[0].m_textures)
		{
			ary.resize(pair.first + 1);

			// ※パスじゃなくGUIDで保存すべき
			ary[pair.first] = pair.second->GetFilepath();
		}

		js["Textures"] = ary;
	}

	// マテリアルデータ
	auto cBuffer = m_passes[0].m_cBuffer;
	if (cBuffer != nullptr)
	{
		auto jsMaterial = nlohmann::json::object();

		// 変数データ
		auto layout = cBuffer->GetLayoutInfo();
		for (auto&& vari : layout.GetVariableMaps())
		{
			if (vari.second.Type == KdConstantBufferLayoutInfo::Types::Float)
			{
				// スカラー
				if (vari.second.Rows == 1 && vari.second.Columns == 1)
				{
					float val;
					cBuffer->GetValue(vari.first, val);
					jsMaterial[vari.first] = val;
				}
				// 2Dベクトル
				else if (vari.second.Rows == 1 && vari.second.Columns == 2)
				{
					KdVector2 val;
					cBuffer->GetValue(vari.first, val);
					jsMaterial[vari.first] = nlohmann::json::array({ val.x, val.y });
				}
				// 3Dベクトル
				else if (vari.second.Rows == 1 && vari.second.Columns == 3)
				{
					KdVector3 val;
					cBuffer->GetValue(vari.first, val);

					jsMaterial[vari.first] = nlohmann::json::array({ val.x, val.y ,val.z });
				}
				// 4Dベクトル
				else if (vari.second.Rows == 1 && vari.second.Columns == 4)
				{
					KdVector4 val;
					cBuffer->GetValue(vari.first, val);

					jsMaterial[vari.first] = nlohmann::json::array({ val.x, val.y ,val.z, val.w });
				}
				// 4x4行列
				else if (vari.second.Rows == 4 && vari.second.Columns == 4)
				{
					KdMatrix val;
					cBuffer->GetValue(vari.first, val);

					float* p = &val._11;

					auto ary = nlohmann::json::array();
					for (int i = 0; i < 16; i++)
					{
						ary.push_back(p[i]);
					}
					jsMaterial[vari.first] = ary;
				}
				else
				{
					// ※他の型も実装する必要あり
					// assert(0 && "[KdMatrix] 対応していないフォーマットです");
				}
			}
		}
		js["MaterialData"] = jsMaterial;
	}

	outJson = js;
}
