#pragma once

#include "KdObject.h"

// �A�Z�b�g�Ǘ��N���X
class KdResourceManager
{
public:

	// �A�Z�b�g�����[�h����(�^�w��o�[�W����)
	template<class AssetType>
	std::shared_ptr<AssetType> LoadAsset(const std::string& assetPath)
	{
		return std::dynamic_pointer_cast<AssetType>( LoadAsset(assetPath) );
	}

	// �A�Z�b�g�����[�h����
	std::shared_ptr<KdObject> LoadAsset(const std::string& assetPath);

	// �A�Z�b�g�����[�h����֐���o�^����
	void RegisterAssetLoadFunction(const std::string& typeName, std::function<std::shared_ptr<KdObject>(const std::string&)> func)
	{
		m_loadFunctions[typeName] = func;
	}

	// �ǂݍ��ݍς݂̃A�Z�b�g�Ǘ����N���A����
	void ReleaseAllLoadedAssets()
	{
		m_loadedAssetMap.clear();
	}

private:

	// �ǂݍ��ݍς݃A�Z�b�g�Ǘ��}�b�v
	std::unordered_map<std::string, std::shared_ptr<KdObject>> m_loadedAssetMap;

	// �A�Z�b�g���[�h�֐��o�^�}�b�v
	// ��)"Texture" -> �e�N�X�`�������[�h����֐���o�^���Ă���
	// ��)"Model" -> ���f�������[�h����֐���o�^���Ă���
	std::unordered_map<
		std::string,
		std::function<std::shared_ptr<KdObject>(const std::string&)>
	> m_loadFunctions;

//-------------------------------
// �V���O���g��
//-------------------------------
private:

	KdResourceManager()
	{
	}

public:
	static KdResourceManager& GetInstance() {
		static KdResourceManager instance;
		return instance;
	}

};