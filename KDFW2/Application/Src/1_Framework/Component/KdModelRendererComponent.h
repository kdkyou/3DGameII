#pragma once

#include "KdComponent.h"
#include "KdTransformComponent.h"

#if 0
class KdMeshRenderer : public KdRendererComponent
{
public:

	// KdModelを設定
	// model		 : KdModel
	// meshNodeName	 : メッシュノード名
	void SetModel(std::shared_ptr<KdModel> model, const std::string& meshNodeName)
	{
		Release();
		if (model == nullptr)return;

		// モデルデータ
		m_sourceModel = model;

		// モデル内のメッシュノード名
		m_nodeName = meshNodeName;

		// ノード名からメッシュノードを検索
		if (model != nullptr)
		{
			const KdModel::Node* node = model->GetNode(m_nodeName);
			if (node != nullptr && node->Mesh)
			{
				// メッシュを設定
				SetMesh(node->Mesh, &model->GetMaterials());

				// スキンメッシュ
				if (node->Mesh->IsSkinMesh())
				{

				}
			}
		}

	}

	// KdMeshとマテリアルを設定
	void SetMesh(std::shared_ptr<KdMesh> mesh, const std::vector<std::shared_ptr<KdMaterial>>* srcMaterials)
	{
		// メッシュ
		m_mesh = mesh;

		// サブセット数ぶん、マテリアル配列を用意
		m_materials.resize(m_mesh->GetSubsets().size());
		if (srcMaterials != nullptr)
		{
			// サブセットに対応したマテリアルを取得する
			for (uint32_t iSubset = 0; iSubset < m_mesh->GetSubsets().size(); iSubset++)
			{
				// サブセット情報
				const KdMeshSubset& subset = m_mesh->GetSubsets()[iSubset];

				// マテリアルIndexが範囲外なら、スキップ
				if (subset.MaterialNo >= srcMaterials->size())continue;

				// マテリアル登録
				m_materials[iSubset] = (*srcMaterials)[subset.MaterialNo];
			}
		}
	}

	// 描画前処理
	virtual void PreDraw() override;

	// 描画処理
	virtual void Draw(bool opaque, KdShader::PassTags passTag) override;

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// 継承元の関数も実行
		KdComponent::Deserialize(jsonObj);

		// アセットパスからモデルをロード
		std::string assetPath;
		GetFromJson(jsonObj, "m_assetPath", assetPath);

		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(assetPath);

		std::string nodeName;
		GetFromJson(jsonObj, "m_nodeName", nodeName);

		SetModel(model, nodeName);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// 継承元の関数も実行
		KdComponent::Serialize(outJson);

		// アセットパス
		if (m_sourceModel != nullptr)
		{
			outJson["m_assetPath"] = m_sourceModel->GetAssetPath();
		}
		else
		{
			outJson["m_assetPath"] = "";
		}

		// ノード名
		outJson["m_nodeName"] = m_nodeName;

		// マテリアル情報

	}

	virtual void Editor_ImGui() override;

	void Release()
	{
		m_sourceModel = nullptr;
		m_mesh = nullptr;
		m_nodeName = "";
		m_materials.clear();

		m_bones.clear();

//		m_cbBoneMatrices = nullptr;
	}

private:

	// 元モデルデータ(アセットからロード時)
	std::shared_ptr<KdModel>		m_sourceModel;

	// メッシュのあるノード名
	std::string						m_nodeName;

	// メッシュ
	std::shared_ptr<KdMesh>			m_mesh;

	// このメッシュ用のマテリアルリスト
	std::vector<std::shared_ptr<KdMaterial>>	m_materials;

	// (SkinMesh)ボーンTransform配列
	std::vector<std::shared_ptr<KdTransformComponent>>	m_bones;

};
#endif

class KdModelRendererComponent : public KdRendererComponent
{
public:

	// ノード名から特定のノードインデックスを返す
	int GetNodeIndexFromName(const std::string& nodeName)const;


	void Load();

	void SetMode(const std::shared_ptr<KdModel>& model);
	const std::shared_ptr<KdModel> GetModel(){return m_sourceModel;}

	// 描画前処理
	virtual void PreDraw() override;

	// 描画処理
	virtual void Draw(bool opaque, KdShader::PassTags passTag) override;

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// 継承元の関数も実行
		KdComponent::Deserialize(jsonObj);

		// アセットパスからモデルをロード
		std::string assetPath;
		KdJsonUtility::GetValue(jsonObj, "m_assetPath", &assetPath);

		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(assetPath);

		SetMode(model);

		// マテリアル情報
		KdJsonUtility::GetArray(jsonObj, "Materials", [this](uint32_t idx, nlohmann::json jsonObj)
			{
				// マテリアルを複製
				m_materials[idx] = m_materials[idx]->Clone();
				// データ復元
				m_materials[idx]->Deserialize(jsonObj);
			});
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// 継承元の関数も実行
		KdComponent::Serialize(outJson);

		// アセットパス
		if (m_sourceModel != nullptr)
		{
			outJson["m_assetPath"] = m_sourceModel->GetAssetPath();
		}
		else
		{
			outJson["m_assetPath"] = "";
		}

		// マテリアル情報
		auto materialAry = nlohmann::json::array();
		for (auto&& mate : m_materials)
		{
			if (mate->IsInstanced())
			{
				nlohmann::json js;
				mate->Serialize(js);

				materialAry.push_back(js);
			}
		}
		outJson["Materials"] = materialAry;
	}

	// ImGui
	virtual void Editor_ImGui() override;

	// 
	void Release()
	{
		m_sourceModel = nullptr;
		m_allNodeTransforms.clear();
		m_materials.clear();
		m_cbBoneMatrices = nullptr;

		m_gpuSkinningData.clear();

	}


	const std::vector<KdTransform>& GetAllNodeTransforms()const {return m_allNodeTransforms;}
	void SetAnimatedTransform( const std::shared_ptr<std::vector<KdTransform>>& animatedTransform){m_animatedTransform = animatedTransform;}

	const std::shared_ptr<KdModel> GetSourceModel() const {return m_sourceModel;}


private:

	// モデルデータ
	std::shared_ptr<KdModel>					m_sourceModel;

	// 全ノード座標リスト
	std::vector<KdTransform>					m_allNodeTransforms;

	// アニメーションしたTransform(毎回nullクリア)
	std::weak_ptr<std::vector<KdTransform>>		m_animatedTransform;

	// このメッシュ用のマテリアルリスト
	std::vector<std::shared_ptr<KdMaterial>>	m_materials;

	// (SkinMesh)ボーン行列用 定数バッファ
	std::shared_ptr<KdConstantBufferType<KdSkinningShader::CBufferData>>	m_cbBoneMatrices;

	// (SkinMesh)ボーン事前スキニング用 頂点バッファ集
	std::vector<std::shared_ptr<KdMeshGPUSkinningData>>	m_gpuSkinningData;

};