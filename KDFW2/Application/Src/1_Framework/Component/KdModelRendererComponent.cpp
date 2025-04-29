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

	// メッシュ情報セット
	m_mesh->SetToDevice(true);

	for (uint32_t iSubset = 0; iSubset < m_mesh->GetSubsets().size(); iSubset++)
	{
		auto& subset = m_mesh->GetSubsets()[iSubset];

		// 面が１枚も無い場合はスキップ
		if (subset.FaceCount == 0)continue;

		auto material = m_materials[iSubset];

		if (material->IsOpaque() != opaque)
			continue;


		// マテリアル情報セット(シェーダー、テクスチャ、マテリアル)
		if (material->SetToDevice(passTag) == false)
		{
			continue;
		}

		// メッシュ描画
		m_mesh->DrawSubset(iSubset);

	}

	// Test



	/*
	for (auto&& mesh : m_mesh->GetMeshes())
	{
		if (mesh->GetMaterial()->IsOpaque() != opaque)continue;

		// マテリアル情報セット(シェーダー、テクスチャ、マテリアル)
		if (mesh->GetMaterial()->SetToDevice(passTag) == false)
		{
			continue;
		}

		// メッシュ情報セット
		mesh->SetToDevice();

		// メッシュ描画
		mesh->Draw();
	}
	*/
}

void KdMeshRenderer::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//-------------------------------
	// モデルのロード
	//-------------------------------
	if (ImGui::CollapsingHeader(u8"モデル読み込み関係", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(u8"モデル読み込み"))
		{
			std::string filepath;
			if (KdEditorData::GetInstance().OpenFileDialog(filepath))
			{
				m_sourceModel = KdResourceManager::GetInstance().LoadAsset<KdModel>(filepath);
			}
		}

		if (ImGui::Button(u8"モデル解除"))
		{
			Release();
		}

		if (m_sourceModel != nullptr)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"モデル読み込み済み！");

			if (ImGui::Button(u8"モデルから、全ノードを作成する"))
			{
				//----------------------------------------------------------------
				// ルートノードから全ノードにアクセスし、GameObjectを作成していく
				//----------------------------------------------------------------

				std::function<void(KdModel::Node&, std::shared_ptr<KdGameObject>) > RecNode = [this, &RecNode](KdModel::Node& node, std::shared_ptr<KdGameObject> parent)
					{
						// GameObject作成
						auto go = std::make_shared<KdGameObject>();
						go->Initialize(node.NodeName/* + (node.BoneIndex >= 0 ? "(Bone)" : "")*/);
						go->SetParent(parent, false);

						// 行列セット
						go->GetTransform()->SetLocalMatrix(node.LocalTransform.GetMatrix());

						// 選択
						if (KdEditorData::GetInstance().SelectObj.expired())
						{
							KdEditorData::GetInstance().SelectObj = go;
						}

						// メッシュレンダラー作成
						if (node.Mesh != nullptr)
						{
							auto meshRenderer = go->AddComponent<KdMeshRenderer>();

							// メッシュとマテリアルをセット
							meshRenderer->SetModel(m_sourceModel, node.NodeName);
						}

						// 子再帰
						for (size_t iChild = 0; iChild < node.Children.size(); iChild++)
						{
							RecNode(m_sourceModel->GetAllNodes()[node.Children[iChild]], go);
						}
					};

				// ルートノードから全ノードにアクセスし、GameObjectを作成していく
				RecNode(m_sourceModel->GetAllNodes()[0], GetGameObject());
			}


			if (ImGui::CollapsingHeader(u8"メッシュノードのリスト", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginListBox("Mesh Nodes"))
				{
					for (auto&& node : m_sourceModel->GetAllNodes())
					{
						if (node.Mesh != nullptr)
						{
							// 押すと
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
	// モデルがロードされているとき、その情報を表示
	//-------------------------------
	if (m_sourceModel != nullptr)
	{
		ImGui::Dummy(ImVec2(0, 30));

		if (ImGui::CollapsingHeader(u8"設定", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ノード名を指定
			if (ImGui::InputText("m_nodeName", &m_nodeName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				// 変更時、モデルからそのメッシュｙノードを検索し、存在すればメッシュを設定する
				SetModel(m_sourceModel, m_nodeName);
			}

			// マテリアルリスト
			if (ImGui::CollapsingHeader("Material List", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (uint32_t i = 0; i < m_materials.size(); i++)
				{
					auto mate = m_materials[i];
					ImGui::LabelText("Name", mate->GetName().c_str());

					KdMaterial::Pass* pass = mate->GetPass(0);
					auto& layoutInfo = pass->m_cBuffer->GetLayoutInfo();

					// 全テクスチャ
					for (auto&& tex : pass->m_textures)
					{
						if (tex.second != nullptr)
						{
//							ImGui::Image(
						}
					}

					// 全パラメータ
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

	// モデルセット
	m_sourceModel = model;

	// ノードのTransform情報を作成
	m_sourceModel->CreateNodeTransforms(m_allNodeTransforms);

	// マテリアル参照
	m_materials = m_sourceModel->GetMaterials();

	// スキンメッシュの場合
	if (m_sourceModel->HasSkinMesh())
	{
		// ボーン行列定数バッファの作成
		m_cbBoneMatrices = std::make_shared<KdConstantBufferType<KdSkinningShader::CBufferData>>();
		m_cbBoneMatrices->CreateConstantBuffer();

		// ボーン事前スキニング用 頂点バッファ集の作成(メッシュの数ぶん)
		m_gpuSkinningData.resize(m_sourceModel->GetMeshIndices().size());
		for (uint32_t i = 0; i < m_sourceModel->GetMeshIndices().size(); i++)
		{
			uint32_t meshNodeIdx = m_sourceModel->GetMeshIndices()[i];
			// このメッシュ用のスキニング用頂点バッファの作成
			m_gpuSkinningData[i] = m_sourceModel->GetAllNodes()[meshNodeIdx].Mesh->CreateGPUSkinningData();
		}
	}
}

// 描画前処理
void KdModelRendererComponent::PreDraw()
{
	if (m_sourceModel == nullptr)return;
	if (IsEnable() == false)return;

	auto& allNodes = m_sourceModel->GetAllNodes();

	// アニメーションで管理されている場合はそちらを利用する
	std::vector<KdTransform>* allNodeTransformas = &m_allNodeTransforms;
	if(m_animatedTransform.expired() == false)
	{
		allNodeTransformas = m_animatedTransform.lock().get();
	}

	// スキンメッシュ時は、ボーン行列を定数バッファに書き込む
	if (m_sourceModel->HasSkinMesh())
	{
		// 全ボーン
		for (uint32_t iBone = 0; iBone < m_sourceModel->GetBoneIndices().size(); iBone++)
		{
			// 全ノード中、このボーンのIndex
			uint32_t boneNodeIndex = m_sourceModel->GetBoneIndices()[iBone];

			// ボーンの最終行列 = ボーンオフセット行列 * ボーンのワールド行列
			m_cbBoneMatrices->EditCB().mBoneMatrix[iBone] = allNodes[boneNodeIndex].BoneOffsetMatrix * (*allNodeTransformas)[boneNodeIndex].GetWorldMatrix();
		}

		// 書き込み
		m_cbBoneMatrices->WriteWorkData();
	}

	// (SkinMesh限定)
	// 頂点をGPUスキニングを実行
	for (uint32_t iMesh = 0; iMesh < m_sourceModel->GetMeshIndices().size(); iMesh++)
	{
		uint32_t nodeIdx = m_sourceModel->GetMeshIndices()[iMesh];

		const KdModel::Node& node = m_sourceModel->GetAllNodes()[nodeIdx];

		if (node.Mesh->IsSkinMesh())
		{
			// 頂点をGPUスキニングを実行
			m_cbBoneMatrices->SetVS(0);
			node.Mesh->GPUSkinning(*m_gpuSkinningData[iMesh]);
		}
	}

	// アニメーションは毎回指定してもらうので一旦クリア
//	m_animatedTransform.reset();

	// 描画登録
	KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_drawList.push_back(this);
}

// 描画処理
void KdModelRendererComponent::Draw(bool opaque, KdShader::PassTags passTag)
{
	// ワールド行列をセット
//	KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
//	KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

	auto& allNodes = m_sourceModel->GetAllNodes();

	/*
	// スキンメッシュ時は、ボーン行列を定数バッファに書き込む
	if (m_sourceModel->HasSkinMesh())
	{
		// 全ボーン
		for (uint32_t iBone = 0; iBone < m_sourceModel->GetBoneIndices().size(); iBone++)
		{
			// 全ノード中、このボーンのIndex
			uint32_t boneNodeIndex = m_sourceModel->GetBoneIndices()[iBone];

			// ボーンの最終行列 = ボーンオフセット行列 * ボーンのワールド行列
			m_cbBoneMatrices->EditCB().mBoneMatrix[iBone] = allNodes[boneNodeIndex].BoneOffsetMatrix * m_allNodeTransforms[boneNodeIndex].GetWorldMatrix();
		}

		// 書き込み
		m_cbBoneMatrices->WriteWorkData();
	}
	*/

	// 全メッシュを描画
	for (uint32_t iMesh = 0; iMesh < m_sourceModel->GetMeshIndices().size(); iMesh++)
	{
		// 全ノード中、このメッシュのIndex
		uint32_t nodeIdx = m_sourceModel->GetMeshIndices()[iMesh];
		// メッシュデータがあるノードを取得
		const KdModel::Node& node = allNodes[nodeIdx];

		// 無効時はスキップ
//		if (node.Enable == false)continue;

		// メッシュがない場合はスキップ
		if (node.Mesh == nullptr)continue;

		// Skin Meshの場合
		if (node.Mesh->IsSkinMesh())
		{
			// このGameObjectのワールド行列を使用
			KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();
			KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

			// メッシュ情報セット
			node.Mesh->SetToDevice(m_gpuSkinningData[iMesh].get());
		}
		// Static Meshの場合
		else
		{
			// メッシュを持っているノードのワールド行列を使用
			KdShaderManager::GetInstance().m_cbPerDraw->EditCB().mW = m_allNodeTransforms[node.NodeIndex].GetWorldMatrix() * GetGameObject()->GetTransform()->GetWorldMatrix();
			KdShaderManager::GetInstance().m_cbPerDraw->WriteWorkData();

			// メッシュ情報セット
			node.Mesh->SetToDevice(nullptr);
		}


		// サブセット単位の描画
		for (uint32_t iSubset = 0; iSubset < node.Mesh->GetSubsets().size(); iSubset++)
		{
			// サブセット情報
			auto& subset = node.Mesh->GetSubsets()[iSubset];

			// 面が１枚も無い場合はスキップ
			if (subset.FaceCount == 0)continue;

			// マテリアル情報
			auto material = m_materials[subset.MaterialNo];

			if (material->IsOpaque() != opaque)
				continue;

			// マテリアル情報セット(シェーダー、テクスチャ、マテリアル)
			if (material->SetToDevice(passTag) == false)
			{
				continue;
			}

			// メッシュ描画
			node.Mesh->DrawSubset(iSubset);
		}
	}

}

void KdModelRendererComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//-------------------------------
	// モデルのロード
	//-------------------------------
	if (ImGui::CollapsingHeader(u8"モデル読み込み関係", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		if (ImGui::Button(u8"モデル読み込み"))
		{
			Load();
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"モデル解除"))
		{
			Release();
		}

		ImGui::Unindent();
	}

	if (m_sourceModel != nullptr)
	{
		if (ImGui::CollapsingHeader(u8"全Node情報", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static int selectNodeIndex = -1;

			{
				//------------------
				// 全ノード
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
						// ツリー項目
						//---------------
						// 文字色
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
							// 子再帰
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

				// ルートノードから全ノードにアクセスし、GameObjectを作成していく
				RecNode(m_sourceModel->GetAllNodes()[0]);

				ImGui::EndChild();

				//------------------
				// 選択ノード情報
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

					// 有効
//					ImGui::Checkbox(u8"有効", &node.Enable);

					//++++++++++++++++
					// トランスフォーム
					//++++++++++++++++

					if (ImGui::CollapsingHeader(u8"トランスフォーム", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Indent();

						// 座標
						if (ImGui::DragFloat3(u8"座標", &pos.x, 0.01f))
						{
							m_allNodeTransforms[selectNodeIndex].SetLocalPosition(pos);
						}
						// 回転(オイラー角)
						if (ImGui::DragFloat3(u8"回転", &angles.x, 1))
						{
							// 回転角度からクォータニオンへ
							KdMatrix m;
							ImGuizmo::RecomposeMatrixFromComponents(&KdVector3::Zero.x, &angles.x, &KdVector3::One.x, &m._11);
							m_allNodeTransforms[selectNodeIndex].SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
						}
						// 拡大
						if (ImGui::DragFloat3(u8"拡大", &scale.x, 0.01f))
						{
							m_allNodeTransforms[selectNodeIndex].SetLocalScale(scale);
						}

						ImGui::Unindent();
					}

					//++++++++++++++++
					// メッシュ
					//++++++++++++++++
					if (node.Mesh != nullptr)
					{
						if (ImGui::CollapsingHeader(u8"メッシュ情報", ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::Indent();

							// マテリアルリスト
							if (ImGui::CollapsingHeader(u8"マテリアルリスト", ImGuiTreeNodeFlags_DefaultOpen))
							{

								for (uint32_t i = 0; i < m_materials.size(); i++)
								{
									auto mate = m_materials[i];
									ImGui::LabelText("Name", mate->GetName().c_str());

									if (mate->IsInstanced())
									{
										ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4(1.0f, 0.0f, 0.0f, 1));
										ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4(1.0f, 0.3f, 0.3f, 1));
										if (ImGui::Button(u8"★インスタンス削除"))
										{
											m_materials[i] = m_sourceModel->GetMaterials()[i];
										}
										ImGui::PopStyleColor(2);
									}
									else
									{
										if (ImGui::Button(u8"インスタンス化"))
										{
											m_materials[i] = mate->Clone();
										}
									}

									if (mate->IsInstanced() == false) ImGui::BeginDisabled();

									KdMaterial::Pass* pass = mate->GetPass(0);
									auto& layoutInfo = pass->m_cBuffer->GetLayoutInfo();

									// 全テクスチャ
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
											if (ImGui::Button(u8"変更"))
											{
												std::string filepath;
												if (KdEditorData::GetInstance().OpenFileDialog(filepath))
												{
													pair.second = KdResourceManager::GetInstance().LoadAsset<KdTexture>(filepath);
												}

											}
											if (ImGui::Button(u8"解除"))
											{
												pair.second = nullptr;
											}
										}
										ImGui::EndGroup();

										ImGui::PopID();
									}

									// 全パラメータ
									for (auto&& variable : layoutInfo.GetVariableMaps())
									{
										ImGui::PushID(&variable);

										// Float型
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
										// Vector2型
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
										// Vector3型
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
										// Vector4型
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
										// Matrix4x4型
										else if (variable.second.Type == KdConstantBufferLayoutInfo::Types::Float &&
											variable.second.Rows == 4 && variable.second.Columns == 4)
										{

										}

										// ※その他の型も作ってね

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
