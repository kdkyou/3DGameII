#include "KdModelRendererComponent.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"

//SetClassAssembly(KdMeshRenderer, "Component");
SetClassAssembly(KdModelRendererComponent, "Component");

#if 0
void KdMeshRenderer::PreDraw()
{
	if (m_mesh == nullptr)return;
	if (IsEnable() == false)return;


	KdFramework::GetInstance().m_currentRenderingData->m_drawList.push_back(this);

	m_mesh->DrawVetexToSO();
}

void KdMeshRenderer::Draw(bool opaque, KdShader::PassTags passTag)
{
	KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->LocalToWorldMatrix();
	KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

	// ���b�V�����Z�b�g
	m_mesh->SetToDevice(true);

	for (uint32_t iSubset = 0; iSubset < m_mesh->GetSubsets().size(); iSubset++)
	{
		auto& subset = m_mesh->GetSubsets()[iSubset];

		// �ʂ��P���������ꍇ�̓X�L�b�v
		if (subset.FaceCount == 0)continue;

		auto material = m_materials[iSubset];

		if (material->IsOpaque() != opaque)
			continue;


		// �}�e���A�����Z�b�g(�V�F�[�_�[�A�e�N�X�`���A�}�e���A��)
		if (material->SetToDevice(passTag) == false)
		{
			continue;
		}

		// ���b�V���`��
		m_mesh->DrawSubset(iSubset);

	}

	// Test



	/*
	for (auto&& mesh : m_mesh->GetMeshes())
	{
		if (mesh->GetMaterial()->IsOpaque() != opaque)continue;

		// �}�e���A�����Z�b�g(�V�F�[�_�[�A�e�N�X�`���A�}�e���A��)
		if (mesh->GetMaterial()->SetToDevice(passTag) == false)
		{
			continue;
		}

		// ���b�V�����Z�b�g
		mesh->SetToDevice();

		// ���b�V���`��
		mesh->Draw();
	}
	*/
}

void KdMeshRenderer::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//-------------------------------
	// ���f���̃��[�h
	//-------------------------------
	if (ImGui::CollapsingHeader(u8"���f���ǂݍ��݊֌W", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(u8"���f���ǂݍ���"))
		{
			std::string filepath;
			if (KdEditorData::GetInstance().OpenFileDialog(filepath))
			{
				m_sourceModel = KdResourceManager::GetInstance().LoadAsset<KdModel>(filepath);
			}
		}

		if (ImGui::Button(u8"���f������"))
		{
			Release();
		}

		if (m_sourceModel != nullptr)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"���f���ǂݍ��ݍς݁I");

			if (ImGui::Button(u8"���f������A�S�m�[�h���쐬����"))
			{
				//----------------------------------------------------------------
				// ���[�g�m�[�h����S�m�[�h�ɃA�N�Z�X���AGameObject���쐬���Ă���
				//----------------------------------------------------------------

				std::function<void(KdModel::Node&, std::shared_ptr<KdGameObject>) > RecNode = [this, &RecNode](KdModel::Node& node, std::shared_ptr<KdGameObject> parent)
					{
						// GameObject�쐬
						auto go = std::make_shared<KdGameObject>();
						go->Initialize(node.NodeName/* + (node.BoneIndex >= 0 ? "(Bone)" : "")*/);
						go->SetParent(parent, false);

						// �s��Z�b�g
						go->GetTransform()->SetLocalMatrix(node.LocalTransform.GetMatrix());

						// �I��
						if (KdEditorData::GetInstance().SelectObj.expired())
						{
							KdEditorData::GetInstance().SelectObj = go;
						}

						// ���b�V�������_���[�쐬
						if (node.Mesh != nullptr)
						{
							auto meshRenderer = go->AddComponent<KdMeshRenderer>();

							// ���b�V���ƃ}�e���A�����Z�b�g
							meshRenderer->SetModel(m_sourceModel, node.NodeName);
						}

						// �q�ċA
						for (size_t iChild = 0; iChild < node.Children.size(); iChild++)
						{
							RecNode(m_sourceModel->GetAllNodes()[node.Children[iChild]], go);
						}
					};

				// ���[�g�m�[�h����S�m�[�h�ɃA�N�Z�X���AGameObject���쐬���Ă���
				RecNode(m_sourceModel->GetAllNodes()[0], GetGameObject());
			}


			if (ImGui::CollapsingHeader(u8"���b�V���m�[�h�̃��X�g", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginListBox("Mesh Nodes"))
				{
					for (auto&& node : m_sourceModel->GetAllNodes())
					{
						if (node.Mesh != nullptr)
						{
							// ������
							if (ImGui::Button(node.NodeName.c_str()))
							{
								SetModel(m_sourceModel, node.NodeName);
							}
						}
					}
				}
				ImGui::EndListBox();

			}
		}

		ImGui::Unindent();
	}

	//-------------------------------
	// ���f�������[�h����Ă���Ƃ��A���̏���\��
	//-------------------------------
	if (m_sourceModel != nullptr)
	{
		ImGui::Dummy(ImVec2(0, 30));

		if (ImGui::CollapsingHeader(u8"�ݒ�", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �m�[�h�����w��
			if (ImGui::InputText("m_nodeName", &m_nodeName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				// �ύX���A���f�����炻�̃��b�V�����m�[�h���������A���݂���΃��b�V����ݒ肷��
				SetModel(m_sourceModel, m_nodeName);
			}

			// �}�e���A�����X�g
			if (ImGui::CollapsingHeader("Material List", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (uint32_t i = 0; i < m_materials.size(); i++)
				{
					auto mate = m_materials[i];
					ImGui::LabelText("Name", mate->GetName().c_str());

					KdMaterial::Pass* pass = mate->GetPass(0);
					auto& layoutInfo = pass->m_cBuffer->GetLayoutInfo();

					// �S�e�N�X�`��
					for (auto&& tex : pass->m_textures)
					{
						if (tex.second != nullptr)
						{
//							ImGui::Image(
						}
					}

					// �S�p�����[�^
					for (auto&& variable : layoutInfo.GetVariableMaps())
					{
						ImGui::PushID(&variable);

						if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
							variable.second.Rows == 1 && variable.second.Columns == 1)
						{
							float val = 0;
							pass->m_cBuffer->GetValue(variable.second.Name, val);

							if (ImGui::InputFloat(variable.second.Name.c_str(), &val))
							{
								pass->m_cBuffer->SetValue(variable.second.Name, val);
							}
						}

						ImGui::PopID();
					}

				}
			}
		}
	}
}
#endif

void KdModelRendererComponent::Load()
{
	std::string filepath;
	if (KdEditorData::GetInstance().OpenFileDialog(filepath))
	{
		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(filepath);

		SetMode(model);
	}
}

void KdModelRendererComponent::SetMode(const std::shared_ptr<KdModel>& model)
{
	Release();
	
	if (model == nullptr)return;

	// ���f���Z�b�g
	m_sourceModel = model;

	// �m�[�h��Transform�����쐬
	m_sourceModel->CreateNodeTransforms(m_allNodeTransforms);

	// �}�e���A���Q��
	m_materials = m_sourceModel->GetMaterials();

	// �X�L�����b�V���̏ꍇ
	if (m_sourceModel->HasSkinMesh())
	{
		// �{�[���s��萔�o�b�t�@�̍쐬
		m_cbBoneMatrices = std::make_shared<KdConstantBufferType<KdSkinningShader::CBufferData>>();
		m_cbBoneMatrices->CreateConstantBuffer();

		// �{�[�����O�X�L�j���O�p ���_�o�b�t�@�W�̍쐬(���b�V���̐��Ԃ�)
		m_gpuSkinningData.resize(m_sourceModel->GetMeshIndices().size());
		for (uint32_t i = 0; i < m_sourceModel->GetMeshIndices().size(); i++)
		{
			uint32_t meshNodeIdx = m_sourceModel->GetMeshIndices()[i];
			// ���̃��b�V���p�̃X�L�j���O�p���_�o�b�t�@�̍쐬
			m_gpuSkinningData[i] = m_sourceModel->GetAllNodes()[meshNodeIdx].Mesh->CreateGPUSkinningData();
		}
	}
}

// �`��O����
void KdModelRendererComponent::PreDraw()
{
	if (m_sourceModel == nullptr)return;
	if (IsEnable() == false)return;

	auto& allNodes = m_sourceModel->GetAllNodes();

	// �A�j���[�V�����ŊǗ�����Ă���ꍇ�͂�����𗘗p����
	std::vector<KdTransform>* allNodeTransformas = &m_allNodeTransforms;
	if(m_animatedTransform.expired() == false)
	{
		allNodeTransformas = m_animatedTransform.lock().get();
	}

	// �X�L�����b�V�����́A�{�[���s���萔�o�b�t�@�ɏ�������
	if (m_sourceModel->HasSkinMesh())
	{
		// �S�{�[��
		for (uint32_t iBone = 0; iBone < m_sourceModel->GetBoneIndices().size(); iBone++)
		{
			// �S�m�[�h���A���̃{�[����Index
			uint32_t boneNodeIndex = m_sourceModel->GetBoneIndices()[iBone];

			// �{�[���̍ŏI�s�� = �{�[���I�t�Z�b�g�s�� * �{�[���̃��[���h�s��
			m_cbBoneMatrices->EditCB().mBoneMatrix[iBone] = allNodes[boneNodeIndex].BoneOffsetMatrix * (*allNodeTransformas)[boneNodeIndex].GetWorldMatrix();
		}

		// ��������
		m_cbBoneMatrices->WriteWorkData();
	}

	// (SkinMesh����)
	// ���_��GPU�X�L�j���O�����s
	for (uint32_t iMesh = 0; iMesh < m_sourceModel->GetMeshIndices().size(); iMesh++)
	{
		uint32_t nodeIdx = m_sourceModel->GetMeshIndices()[iMesh];

		const KdModel::Node& node = m_sourceModel->GetAllNodes()[nodeIdx];

		if (node.Mesh->IsSkinMesh())
		{
			// ���_��GPU�X�L�j���O�����s
			m_cbBoneMatrices->SetVS(0);
			node.Mesh->GPUSkinning(*m_gpuSkinningData[iMesh]);
		}
	}

	// �A�j���[�V�����͖���w�肵�Ă��炤�̂ň�U�N���A
//	m_animatedTransform.reset();

	// �`��o�^
	KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_drawList.push_back(this);
}

// �`�揈��
void KdModelRendererComponent::Draw(bool opaque, KdShader::PassTags passTag)
{
	// ���[���h�s����Z�b�g
//	KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
//	KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

	auto& allNodes = m_sourceModel->GetAllNodes();

	/*
	// �X�L�����b�V�����́A�{�[���s���萔�o�b�t�@�ɏ�������
	if (m_sourceModel->HasSkinMesh())
	{
		// �S�{�[��
		for (uint32_t iBone = 0; iBone < m_sourceModel->GetBoneIndices().size(); iBone++)
		{
			// �S�m�[�h���A���̃{�[����Index
			uint32_t boneNodeIndex = m_sourceModel->GetBoneIndices()[iBone];

			// �{�[���̍ŏI�s�� = �{�[���I�t�Z�b�g�s�� * �{�[���̃��[���h�s��
			m_cbBoneMatrices->EditCB().mBoneMatrix[iBone] = allNodes[boneNodeIndex].BoneOffsetMatrix * m_allNodeTransforms[boneNodeIndex].GetWorldMatrix();
		}

		// ��������
		m_cbBoneMatrices->WriteWorkData();
	}
	*/

	// �S���b�V����`��
	for (uint32_t iMesh = 0; iMesh < m_sourceModel->GetMeshIndices().size(); iMesh++)
	{
		// �S�m�[�h���A���̃��b�V����Index
		uint32_t nodeIdx = m_sourceModel->GetMeshIndices()[iMesh];
		// ���b�V���f�[�^������m�[�h���擾
		const KdModel::Node& node = allNodes[nodeIdx];

		// �������̓X�L�b�v
//		if (node.Enable == false)continue;

		// ���b�V�����Ȃ��ꍇ�̓X�L�b�v
		if (node.Mesh == nullptr)continue;

		// Skin Mesh�̏ꍇ
		if (node.Mesh->IsSkinMesh())
		{
			// ����GameObject�̃��[���h�s����g�p
			KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
			KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

			// ���b�V�����Z�b�g
			node.Mesh->SetToDevice(m_gpuSkinningData[iMesh].get());
		}
		// Static Mesh�̏ꍇ
		else
		{
			// ���b�V���������Ă���m�[�h�̃��[���h�s����g�p
			KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = m_allNodeTransforms[node.NodeIndex].GetWorldMatrix() * GetGameObject()->GetTransform()->GetWorldMatrix();
			KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

			// ���b�V�����Z�b�g
			node.Mesh->SetToDevice(nullptr);
		}


		// �T�u�Z�b�g�P�ʂ̕`��
		for (uint32_t iSubset = 0; iSubset < node.Mesh->GetSubsets().size(); iSubset++)
		{
			// �T�u�Z�b�g���
			auto& subset = node.Mesh->GetSubsets()[iSubset];

			// �ʂ��P���������ꍇ�̓X�L�b�v
			if (subset.FaceCount == 0)continue;

			// �}�e���A�����
			auto material = m_materials[subset.MaterialNo];

			if (material->IsOpaque() != opaque)
				continue;

			// �}�e���A�����Z�b�g(�V�F�[�_�[�A�e�N�X�`���A�}�e���A��)
			if (material->SetToDevice(passTag) == false)
			{
				continue;
			}

			// ���b�V���`��
			node.Mesh->DrawSubset(iSubset);
		}
	}

}

void KdModelRendererComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//-------------------------------
	// ���f���̃��[�h
	//-------------------------------
	if (ImGui::CollapsingHeader(u8"���f���ǂݍ��݊֌W", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(u8"���f���ǂݍ���"))
		{
			Load();
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"���f������"))
		{
			Release();
		}

		ImGui::Unindent();
	}

	if (m_sourceModel != nullptr)
	{
		if (ImGui::CollapsingHeader(u8"�SNode���", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static int selectNodeIndex = -1;

			{
				//------------------
				// �S�m�[�h
				//------------------
				int index = 0;
				std::function<void(const KdModel::Node&) > RecNode = [this, &RecNode, &index](const KdModel::Node& node)
					{
						ImGui::PushID(&node);

						int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
						if (selectNodeIndex == index)
						{
							flags |= ImGuiTreeNodeFlags_Selected;
						}

						//---------------
						// �c���[����
						//---------------
						// �����F
						if (node.BoneIndex >= 0)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 1, 1));
						else if (node.Mesh != nullptr)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
						else
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

						bool bTreeOpen = ImGui::TreeNodeEx((void*)&node, flags, node.NodeName.c_str());
						ImGui::PopStyleColor(1);

						if (ImGui::IsItemClicked(0))
						{
							selectNodeIndex = index;
						}

						index++;

						if (bTreeOpen)
						{
							// �q�ċA
							for (size_t iChild = 0; iChild < node.Children.size(); iChild++)
							{
								RecNode(m_sourceModel->GetAllNodes()[node.Children[iChild]]);
							}

							ImGui::TreePop();
						}
						ImGui::PopID();
					};

				// 
				ImGui::BeginChild("All Node", ImVec2(0, 200), ImGuiChildFlags_Borders);

				// ���[�g�m�[�h����S�m�[�h�ɃA�N�Z�X���AGameObject���쐬���Ă���
				RecNode(m_sourceModel->GetAllNodes()[0]);

				ImGui::EndChild();

				//------------------
				// �I���m�[�h���
				//------------------
				auto& allNodes = m_sourceModel->GetAllNodes();

				ImGui::BeginChild("Node Info", ImVec2(0, 300), ImGuiChildFlags_Borders);
				if (selectNodeIndex >= 0 && selectNodeIndex < allNodes.size())
				{
					auto& node = allNodes[selectNodeIndex];

					KdVector3 pos;
					KdVector3 angles;
					KdVector3 scale;
					KdMatrix mLocal = m_allNodeTransforms[selectNodeIndex].GetLocalMatrix();
					ImGuizmo::DecomposeMatrixToComponents(&mLocal._11, &pos.x, &angles.x, &scale.x);

					// �L��
//					ImGui::Checkbox(u8"�L��", &node.Enable);

					//++++++++++++++++
					// �g�����X�t�H�[��
					//++++++++++++++++

					if (ImGui::CollapsingHeader(u8"�g�����X�t�H�[��", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Indent();

						// ���W
						if (ImGui::DragFloat3(u8"���W", &pos.x, 0.01f))
						{
							m_allNodeTransforms[selectNodeIndex].SetLocalPosition(pos);
						}
						// ��](�I�C���[�p)
						if (ImGui::DragFloat3(u8"��]", &angles.x, 1))
						{
							// ��]�p�x����N�H�[�^�j�I����
							KdMatrix m;
							ImGuizmo::RecomposeMatrixFromComponents(&KdVector3::Zero.x, &angles.x, &KdVector3::One.x, &m._11);
							m_allNodeTransforms[selectNodeIndex].SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
						}
						// �g��
						if (ImGui::DragFloat3(u8"�g��", &scale.x, 0.01f))
						{
							m_allNodeTransforms[selectNodeIndex].SetLocalScale(scale);
						}

						ImGui::Unindent();
					}

					//++++++++++++++++
					// ���b�V��
					//++++++++++++++++
					if (node.Mesh != nullptr)
					{
						if (ImGui::CollapsingHeader(u8"���b�V�����", ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::Indent();

							// �}�e���A�����X�g
							if (ImGui::CollapsingHeader(u8"�}�e���A�����X�g", ImGuiTreeNodeFlags_DefaultOpen))
							{

								for (uint32_t i = 0; i < m_materials.size(); i++)
								{
									auto mate = m_materials[i];
									ImGui::LabelText("Name", mate->GetName().c_str());

									if (mate->IsInstanced())
									{
										ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4(1.0f, 0.0f, 0.0f, 1));
										ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4(1.0f, 0.3f, 0.3f, 1));
										if (ImGui::Button(u8"���C���X�^���X�폜"))
										{
											m_materials[i] = m_sourceModel->GetMaterials()[i];
										}
										ImGui::PopStyleColor(2);
									}
									else
									{
										if (ImGui::Button(u8"�C���X�^���X��"))
										{
											m_materials[i] = mate->Clone();
										}
									}

									if (mate->IsInstanced() == false) ImGui::BeginDisabled();

									KdMaterial::Pass* pass = mate->GetPass(0);
									auto& layoutInfo = pass->m_cBuffer->GetLayoutInfo();

									// �S�e�N�X�`��
									for (auto&& pair : pass->m_textures)
									{
										ImGui::PushID(pair.first);

										ImGui::Text("[%d]", pair.first);
										if (pair.second != nullptr)
										{
											ImGui::SameLine();
											ImGui::Image((ImTextureID)pair.second->GetSRView().Get(), ImVec2(50, 50));
										}

										ImGui::SameLine();

										ImGui::BeginGroup();
										{
											if (ImGui::Button(u8"�ύX"))
											{
												std::string filepath;
												if (KdEditorData::GetInstance().OpenFileDialog(filepath))
												{
													pair.second = KdResourceManager::GetInstance().LoadAsset<KdTexture>(filepath);
												}

											}
											if (ImGui::Button(u8"����"))
											{
												pair.second = nullptr;
											}
										}
										ImGui::EndGroup();

										ImGui::PopID();
									}

									// �S�p�����[�^
									for (auto&& variable : layoutInfo.GetVariableMaps())
									{
										ImGui::PushID(&variable);

										// Float�^
										if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
											variable.second.Rows == 1 && variable.second.Columns == 1)
										{
											float val = 0;
											pass->m_cBuffer->GetValue(variable.second.Name, val);

											if (ImGui::DragFloat(variable.second.Name.c_str(), &val, 0.01f))
											{
												pass->m_cBuffer->SetValue(variable.second.Name, val);
											}
										}
										// Vector2�^
										else if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
											variable.second.Rows == 1 && variable.second.Columns == 2)
										{
											KdVector2 val;
											pass->m_cBuffer->GetValue(variable.second.Name, val);

											if (ImGui::DragFloat2(variable.second.Name.c_str(), &val.x, 0.01f))
											{
												pass->m_cBuffer->SetValue(variable.second.Name, val);
											}
										}
										// Vector3�^
										else if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
											variable.second.Rows == 1 && variable.second.Columns == 3)
										{
											KdVector3 val;
											pass->m_cBuffer->GetValue(variable.second.Name, val);

											if (ImGui::DragFloat3(variable.second.Name.c_str(), &val.x, 0.01f))
											{
												pass->m_cBuffer->SetValue(variable.second.Name, val);
											}
										}
										// Vector4�^
										else if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
											variable.second.Rows == 1 && variable.second.Columns == 4)
										{
											KdVector4 val;
											pass->m_cBuffer->GetValue(variable.second.Name, val);

											if (ImGui::DragFloat4(variable.second.Name.c_str(), &val.x, 0.01f))
											{
												pass->m_cBuffer->SetValue(variable.second.Name, val);
											}
										}
										// Matrix4x4�^
										else if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
											variable.second.Rows == 4 && variable.second.Columns == 4)
										{

										}

										// �����̑��̌^������Ă�

										ImGui::PopID();
									}

									if (mate->IsInstanced() == false) ImGui::EndDisabled();
								}
							}

							ImGui::Unindent();
						}
					}

				}
				ImGui::EndChild();
			}

		}
	}
}
