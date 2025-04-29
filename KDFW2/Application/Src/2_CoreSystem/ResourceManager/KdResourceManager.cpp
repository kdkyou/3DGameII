#include "KdResourceManager.h"

std::shared_ptr<KdObject> KdResourceManager::LoadAsset(const std::string& assetPath)
{
	// ���ɓǂݍ��ݍς݂Ȃ�A�����Ԃ��B
	{
		auto it = m_loadedAssetMap.find(assetPath);
		if (it != m_loadedAssetMap.end())
		{
			return (*it).second;
		}
	}

	// ���^�t�@�C��������ꍇ�́A���擾
	nlohmann::json metaData;
	std::ifstream ifs(assetPath + ".kdfwmeta");
	if (ifs)
	{
		// ���^�t�@�C���ǂݍ���
		ifs >> metaData;

		// �t�@�C���^�C�v����A�K�؂ȓǂݍ��݊֐������s����
		std::string type = metaData["Type"].get<std::string>();

		// Type����A�ǂݍ��ݏ�����I��
		auto itFound = m_loadFunctions.find(type);
		if (itFound == m_loadFunctions.end())return nullptr;

		// �ǂݍ��ݎ��s
		auto loadedAsset = itFound->second(assetPath);
		if (loadedAsset != nullptr)
		{
			// �ǂݍ��ݍς݂֓o�^
			m_loadedAssetMap[assetPath] = loadedAsset;

			return loadedAsset;
		}
	}
	else
	{
		// ���^�t�@�C����񂪂Ȃ��Ȃ�A��������œǂݍ��݊֐������s
		for (auto&& func : m_loadFunctions)
		{
			// �ǂݍ��ݎ��s
			auto loadedAsset = func.second(assetPath);
			if (loadedAsset != nullptr)
			{
				// �ǂݍ��ݍς݂֓o�^
				m_loadedAssetMap[assetPath] = loadedAsset;

				return loadedAsset;
			}
		}
	}



	return nullptr;
}
