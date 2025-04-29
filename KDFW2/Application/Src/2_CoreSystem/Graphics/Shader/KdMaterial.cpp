#include "KdMaterial.h"

void KdMaterial::SetData(const std::string& name)
{
	m_name = name;

	// ���O�ɁA@Transparent �����Ă�z�́A�������}�e���A���Ƃ���
	auto nameOpts = split(m_name, '@');
	if (nameOpts.size() >= 2 && nameOpts[1] == "Transparent")
	{
//		m_opaque = false;
	}
}

// �}�e���A���f�[�^��V�F�[�_�[���f�o�C�X�փZ�b�g����

bool KdMaterial::SetToDevice(uint32_t passIdx)
{
	Pass* pass = GetPass(passIdx);
	if (pass == nullptr)return false;

	auto shaderPass = m_shader->GetPass(pass->m_passIndex);

	//---------------------------
	// �e�N�X�`�����Z�b�g
	//---------------------------
	for (auto&& tex : pass->m_textures)
	{
		D3D.SetTextureToVS(tex.first, tex.second.get());
		D3D.SetTextureToPS(tex.first, tex.second.get());
		D3D.SetTextureToGS(tex.first, tex.second.get());
	}

	//---------------------------
	// �}�e���A�����F�萔�o�b�t�@���Z�b�g
	//---------------------------
	if (pass->m_cBuffer != nullptr)
	{
		pass->m_cBuffer->WriteWorkData();
		pass->m_cBuffer->SetVS(0);
		pass->m_cBuffer->SetPS(0);
		pass->m_cBuffer->SetGS(0);
	}

	//---------------------------
	// �V�F�[�_�[���Z�b�g
	//---------------------------
	// ���_�V�F�[�_���Z�b�g
	if (shaderPass->m_VS != nullptr)
	{
		D3D.GetDevContext()->VSSetShader(shaderPass->m_VS.Get(), 0, 0);
	}

	// ���_���C�A�E�g���Z�b�g
	if (shaderPass->m_inputLayout != nullptr)
	{
		D3D.GetDevContext()->IASetInputLayout(shaderPass->m_inputLayout.Get());
	}

	// �s�N�Z���V�F�[�_�[
	if (shaderPass->m_PS != nullptr)
	{
		D3D.GetDevContext()->PSSetShader(shaderPass->m_PS.Get(), 0, 0);
	}

	// �W�I���g���V�F�[�_�[
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

	// �S�V�F�[�_�[�p�X����A�p�X�쐬
	CreatePassesFromShader(nullptr);

	auto& pass = m_passes[0];

	// �e�N�X�`��
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

	// �}�e���A���f�[�^
	auto materialData = jsonData["MaterialData"];
	for (auto&& pair : materialData.items())
	{
		std::string paramName = pair.key();

		auto val = pair.value();

		// �X�J���[
		if (val.is_number())
		{
			pass.m_cBuffer->SetValue(paramName, val.get<float>());
		}
		// 2D�x�N�g��
		else if (val.is_array() && val.size() == 2)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdVector2&)list[0]);
		}
		// 3D�x�N�g��
		else if (val.is_array() && val.size() == 3)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdVector3&)list[0]);
		}
		// 4D�x�N�g��
		else if (val.is_array() && val.size() == 4)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdVector4&)list[0]);
		}
		// 4x4�s��
		else if (val.is_array() && val.size() == 16)
		{
			auto list = val.get<std::vector<float>>();
			pass.m_cBuffer->SetValue(paramName, (KdMatrix&)list[0]);
		}
		else
		{
			// �����̌^����������K�v����
			// assert(0 && "[KdMatrix] �Ή����Ă��Ȃ��t�H�[�}�b�g�ł�");
		}

	}
}

void KdMaterial::Serialize(nlohmann::json& outJson) const
{
	nlohmann::json js = nlohmann::json::object();

	// �}�e���A����
	js["Name"] = m_name;

	// �V�F�[�_�[��
	js["Shader"] = m_shader->GetName();

//	js["Opaque"] = m_opaque;

	// �e�N�X�`��
	{
		std::vector<std::string> ary;

		for (auto&& pair : m_passes[0].m_textures)
		{
			ary.resize(pair.first + 1);

			// ���p�X����Ȃ�GUID�ŕۑ����ׂ�
			ary[pair.first] = pair.second->GetFilepath();
		}

		js["Textures"] = ary;
	}

	// �}�e���A���f�[�^
	auto cBuffer = m_passes[0].m_cBuffer;
	if (cBuffer != nullptr)
	{
		auto jsMaterial = nlohmann::json::object();

		// �ϐ��f�[�^
		auto layout = cBuffer->GetLayoutInfo();
		for (auto&& vari : layout.GetVariableMaps())
		{
			if (vari.second.Type == KdConstantBufferLayoutInfo::Types::Float)
			{
				// �X�J���[
				if (vari.second.Rows == 1 && vari.second.Columns == 1)
				{
					float val;
					cBuffer->GetValue(vari.first, val);
					jsMaterial[vari.first] = val;
				}
				// 2D�x�N�g��
				else if (vari.second.Rows == 1 && vari.second.Columns == 2)
				{
					KdVector2 val;
					cBuffer->GetValue(vari.first, val);
					jsMaterial[vari.first] = nlohmann::json::array({ val.x, val.y });
				}
				// 3D�x�N�g��
				else if (vari.second.Rows == 1 && vari.second.Columns == 3)
				{
					KdVector3 val;
					cBuffer->GetValue(vari.first, val);

					jsMaterial[vari.first] = nlohmann::json::array({ val.x, val.y ,val.z });
				}
				// 4D�x�N�g��
				else if (vari.second.Rows == 1 && vari.second.Columns == 4)
				{
					KdVector4 val;
					cBuffer->GetValue(vari.first, val);

					jsMaterial[vari.first] = nlohmann::json::array({ val.x, val.y ,val.z, val.w });
				}
				// 4x4�s��
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
					// �����̌^����������K�v����
					// assert(0 && "[KdMatrix] �Ή����Ă��Ȃ��t�H�[�}�b�g�ł�");
				}
			}
		}
		js["MaterialData"] = jsMaterial;
	}

	outJson = js;
}
