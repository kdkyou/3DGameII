#include "Pch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "KdMeshLoader.h"
#include "KdAnimationData.h"

// ���f���f�[�^����A�j���[�V�����f�[�^�݂̂𔲂��o���ēƎ��t�@�C���ɕۑ�
void KdAnimationData::Parse(aiAnimation* src, const std::vector<KdMeshScene::NodeData>& allNodes)
{
	Name = src->mName.C_Str(); // �A�j���[�V������

	LengthInTick = src->mDuration;	// �A�j���[�V�����̃t���[����
	TicksParSecond = src->mTicksPerSecond; // ��b�Ԃ̐i�s�t���[����

	// ���ɂ��w�肳��Ă��Ȃ��\��������炵���̂ŁA�w�肳��Ă��Ȃ��������牼��60FPS�Ƃ���
	if (TicksParSecond == 0)
	{
		TicksParSecond = 60.0f;
	}

	// �A�j���[�V�����̌p������(�b)
	LengthInSecond = LengthInTick / TicksParSecond;

	// �A�j���[�V�����̊���U���Ă��镔�ʂ��Ƃɓǂݍ���
	UINT numChannel = src->mNumChannels;
	for (UINT idx = 0; idx < numChannel; idx++)
	{
		aiNodeAnim* srcChannel = src->mChannels[idx];
		KdAnimationData::PerNode destNode;

		destNode.Name = srcChannel->mNodeName.C_Str(); // �Ή�����p�[�c�̖��O
		
		// ���O����m�[�h��Index������o���Ă���------------------->
		for(UINT nc = 0; nc < allNodes.size(); nc++)
		{
			if(allNodes[nc].NodeName == destNode.Name)
			{
				destNode.NodeIndex = nc;
				break;
			}
		}
		//<--------------------�A�j���[�V�����R���|�[�l���g�ŊǗ����ׂ�

		// Positions
		UINT numKey = srcChannel->mNumPositionKeys;
		for(UINT pCnt = 0; pCnt < numKey; pCnt++)
		{
			aiVectorKey src = srcChannel->mPositionKeys[pCnt];
			KdAnimationData::PerNode::PositionKey dest;

			dest.Time = src.mTime;
			dest.vPos.x = src.mValue.x;
			dest.vPos.y = src.mValue.y;
			dest.vPos.z = src.mValue.z;
			destNode.PositionKeys.push_back(dest);
		}

		//Rotaions
		numKey = srcChannel->mNumRotationKeys;
		for(UINT rCnt = 0; rCnt < numKey; rCnt++)
		{
			aiQuatKey src = srcChannel->mRotationKeys[rCnt];
			KdAnimationData::PerNode::RotationKey dest;

			dest.Time = src.mTime;
			dest.qRot.x = src.mValue.x;
			dest.qRot.y = src.mValue.y;
			dest.qRot.z = src.mValue.z;
		  	dest.qRot.w = src.mValue.w;
			destNode.RotationKeys.push_back(dest);
		}

		// Scales
		numKey = srcChannel->mNumScalingKeys;
		for (UINT sCnt = 0; sCnt < numKey; sCnt++)
		{
			aiVectorKey src = srcChannel->mScalingKeys[sCnt];
			KdAnimationData::PerNode::ScaleKey dest;

			dest.Time = src.mTime;
			dest.vScale.x = src.mValue.x;
			dest.vScale.y = src.mValue.y;
			dest.vScale.z = src.mValue.z;
			destNode.ScaleKeys.push_back(dest);
		}

		PerNodes.push_back(destNode);
	}
}

std::string SwapUnavailableCharacters(const std::string& str, const std::string& swap)
{
	std::string ret = str;
	static std::string chStr[] = { ":","*" ,"?" ,"<" ,">" ,"|" };
	for (auto& ch : chStr)
	{
		if(ch == swap){return "error";}

		auto p = ret.find(ch);
		while (p != std::string::npos)
		{
			ret.replace(p, 1, swap);
			p = ret.find(ch);
		}
	}
	return ret;
}

// �A�j���[�V�����f�[�^����̃A�Z�b�g�Ƃ��ďo��
bool KdAnimationData::Save(const std::string& directory) const
{
	// �s��������S���A���_�[�o�[�ɒu��������
	std::string forFile = SwapUnavailableCharacters(Name, "_");

	// �Z�[�u�t�@�C����=�A�j���[�V������.kdanim
	std::string filename = directory + "/" + forFile + ".kdanim";

	std::ofstream ofs(filename);
	if (!ofs) return false;

	// �A�j���[�V�����f�[�^���V���A���C�Y
	auto json = Serialize();

	// ������
	std::string str = json.dump();

	// �t�@�C����������
	ofs.write(str.c_str(), str.size());

	return true;
}



bool KdAnimationData::Load(const std::string& filepath)
{
	std::ifstream ifs(filepath);
	if (!ifs) return false;

	nlohmann::json jsonObj;
	ifs >> jsonObj;

	Deserialize(jsonObj);

	return true;
}


nlohmann::json KdAnimationData::Serialize() const
{
	nlohmann::json jsonObj = nlohmann::json::object();

	// �A�j���[�V������{�f�[�^
	jsonObj["Name"] = Name;
	jsonObj["LengthInTick"] = LengthInTick;
	jsonObj["TicksParSecond"] = TicksParSecond;
	jsonObj["LengthInSecond"] = LengthInSecond;

	// �A�j���[�V�����̊e�L�[�̏��
	auto nodeArr = nlohmann::json::array();
	for(auto&& perNode : PerNodes)
	{
		auto perJson = perNode.Serialize();
		nodeArr.push_back(perJson);
	}
	jsonObj["AnimationDatas"] = nodeArr;

	return jsonObj;
}

bool KdAnimationData::Deserialize(const nlohmann::json& jsonObj)
{
	// �A�j���[�V������{�f�[�^
	KdJsonUtility::GetValue(jsonObj, "Name", &Name);
	KdJsonUtility::GetValue(jsonObj, "LengthInTick", &LengthInTick);
	KdJsonUtility::GetValue(jsonObj, "TicksParSecond", &TicksParSecond);
	KdJsonUtility::GetValue(jsonObj, "LengthInSecond", &LengthInSecond);

	// �m�[�h���̃f�[�^
	auto nodeDatas = jsonObj["AnimationDatas"];
	for(UINT idx = 0; idx < (UINT)nodeDatas.size(); idx++)
	{
		PerNode node;
		if (node.Deserialize(nodeDatas.at(idx)))
		{
			PerNodes.push_back(node);
		}
	}

	return true;
}

// ���ׂĂ̗v�f��⊮
KdTransform KdAnimationData::Interpolation(const KdAnimationData::PerNode& node, double time)
{
	KdTransform trans;
	double frames = time * TicksParSecond; // ���݂̃t���[����
	trans.SetLocalPosition(node.InterpolationPosition(frames));
	trans.SetLocalRotation(node.InterpolationRotation(frames));
	trans.SetLocalScale(node.InterpolationScale(frames));
	return trans;
}

// ���Ԃ��w�肵�ẴA�j���[�V�����⊮
KdVector3 KdAnimationData::PerNode::InterpolationPosition(double time) const
{
	auto res = GetPositionKeyByTime(time);
	return std::get<0>(res).Interpolation(std::get<1>(res), time);
}
KdQuaternion KdAnimationData::PerNode::InterpolationRotation(double time) const
{
	auto res = GetRotationKeyByTime(time);
	return std::get<0>(res).Interpolation(std::get<1>(res), time);
}
KdVector3 KdAnimationData::PerNode::InterpolationScale(double time) const
{
	auto res = GetScaleKeyByTime(time);
	return std::get<0>(res).Interpolation(std::get<1>(res), time);
}

nlohmann::json KdAnimationData::PerNode::Serialize() const
{
	nlohmann::json ret = nlohmann::json::object();

	ret["Name"] = Name;
	ret["NodeIdx"] = NodeIndex; // ���󖢎g�p

	// PositionKeys
	nlohmann::json posArr = nlohmann::json::array();
	for(auto&& posKey : PositionKeys)
	{
		nlohmann::json posJson = nlohmann::json::object();
		posJson["Time"]= posKey.Time;
		posJson["Position"] = KdJsonUtility::CreateArray(&posKey.vPos.x, 3);
		posArr.push_back(posJson);
	}
	ret["PositionKeys"] = posArr;

	// RotationKeys
	nlohmann::json rotArr = nlohmann::json::array();
	for(auto&& rotKey : RotationKeys)
	{
		nlohmann::json rotJson = nlohmann::json::object();
		rotJson["Time"] = rotKey.Time;
		rotJson["Rotation"] = KdJsonUtility::CreateArray(&rotKey.qRot.x, 4);
		rotArr.push_back(rotJson);
	}
	ret["RotationKeys"] = rotArr;

	// ScaleKeys
	nlohmann::json scaArr = nlohmann::json::array();
	for(auto&& scaKey : ScaleKeys)
	{
		nlohmann::json scaJson = nlohmann::json::object();
		scaJson["Time"] = scaKey.Time;
		scaJson["Scale"] = KdJsonUtility::CreateArray(&scaKey.vScale.x, 3);
		scaArr.push_back(scaJson);
	}
	ret["ScaleKeys"] = scaArr;

	return ret;
}

bool KdAnimationData::PerNode::Deserialize(const nlohmann::json& jsonObj)
{
	KdJsonUtility::GetValue(jsonObj, "Name", &Name);
	KdJsonUtility::GetValue(jsonObj, "NodeIdx", &NodeIndex);

	auto posKeysJson = jsonObj["PositionKeys"];
	UINT idx = 0;
	for(idx = 0; idx < (UINT)posKeysJson.size(); idx++)
	{
		PositionKey pos;
		KdJsonUtility::GetValue(posKeysJson.at(idx), "Time", &pos.Time);
		KdJsonUtility::GetArray(posKeysJson.at(idx), "Position", &pos.vPos.x, 3);
		PositionKeys.push_back(pos);
	}
	//for(auto posJson : posKeysJson.items())
	//{
	//	PositionKey pos;
	//	KdJsonUtility::GetValue(posJson, "Time", &pos.Time);
	//	KdJsonUtility::GetArray(posJson, "Position", &pos.vPos.x, 3);
	//	PositionKeys.push_back(pos);
	//}
	auto rotKeysJson = jsonObj["RotationKeys"];
	for (idx = 0; idx < (UINT)rotKeysJson.size(); idx++)
	{
		RotationKey rot;
		KdJsonUtility::GetValue(rotKeysJson.at(idx), "Time", &rot.Time);
		KdJsonUtility::GetArray(rotKeysJson.at(idx), "Rotation", &rot.qRot.x, 4);
		RotationKeys.push_back(rot);
	}
	
	//for (auto rotJson : rotKeysJson.items())
	//{
	//	RotationKey rot;
	//	KdJsonUtility::GetValue(rotJson, "Time", &rot.Time);
	//	KdJsonUtility::GetArray(rotJson, "Rotation", &rot.qRot.x, 4);
	//	RotationKeys.push_back(rot);
	//}

	auto scaKeysJson = jsonObj["ScaleKeys"];
	for (idx = 0; idx < (UINT)scaKeysJson.size(); idx++)
	{
		ScaleKey sca;
		KdJsonUtility::GetValue(scaKeysJson.at(idx), "Time", &sca.Time);
		KdJsonUtility::GetArray(scaKeysJson.at(idx), "Scale", &sca.vScale.x, 3);
		ScaleKeys.push_back(sca);
	}

	//for (auto scaJson : scaKeysJson.items())
	//{
	//	ScaleKey sca;
	//	KdJsonUtility::GetValue(scaJson, "Time", &sca.Time);
	//	KdJsonUtility::GetArray(scaJson, "Scale", &sca.vScale.x, 3);
	//	ScaleKeys.push_back(sca);
	//}

	return true;
}

// ���W�⊮
KdVector3 KdAnimationData::PerNode::PositionKey::Interpolation(const PositionKey& last, double time) const
{
	double betweenTime = last.Time - Time;
	if (betweenTime <= 0) { return vPos; } // �w�肵���L�[�̏������s��
	if (time <= Time) { return vPos; } // �擪�L�[����O�̎��Ԃ�����
	if (time >= last.Time) { return last.vPos; } // �Ō�̃L�[����̎��Ԃ�����

	double nowProgressTime = time - Time; // �擪�L�[����ǂꂭ�炢�o�߂��Ă��邩
	float nowProgressPercent = (float)(nowProgressTime / betweenTime); // �L�[�Ԃ̐i�s�

	return KdVector3::Lerp(vPos, last.vPos, nowProgressPercent);
}
// ��]�⊮
KdQuaternion KdAnimationData::PerNode::RotationKey::Interpolation(const RotationKey& last, double time) const
{
	double betweenTime = last.Time - Time;
	if (betweenTime <= 0) { return qRot; } // �w�肵���L�[�̏������s��
	if (time <= Time) { return qRot; } // �擪�L�[����O�̎��Ԃ�����
	if (time >= last.Time) { return last.qRot; } // �Ō�̃L�[����̎��Ԃ�����

	double nowProgressTime = time - Time; // �擪�L�[����ǂꂭ�炢�o�߂��Ă��邩
	float nowProgressPercent = (float)(nowProgressTime / betweenTime); // �L�[�Ԃ̐i�s�

	return KdQuaternion::Lerp(qRot, last.qRot, nowProgressPercent);
}
// �g�k�⊮
KdVector3 KdAnimationData::PerNode::ScaleKey::Interpolation(const ScaleKey& last, double time) const
{
	double betweenTime = last.Time - Time;
	if (betweenTime <= 0) { return vScale; } // �w�肵���L�[�̏������s��
	if (time <= Time) { return vScale; } // �擪�L�[����O�̎��Ԃ�����
	if (time >= last.Time) { return last.vScale; } // �Ō�̃L�[����̎��Ԃ�����

	double nowProgressTime = time - Time; // �擪�L�[����ǂꂭ�炢�o�߂��Ă��邩
	float nowProgressPercent = (float)(nowProgressTime / betweenTime); // �L�[�Ԃ̐i�s�

	auto v = KdVector3::Lerp(vScale, last.vScale, nowProgressPercent);
	return KdVector3::Lerp(vScale, last.vScale, nowProgressPercent);
}


// �w�肳�ꂽ�t���[���̑O��̃L�[��Ԃ�(�w����Tuple�Ō����I)
// �L�[�͏����ŕ���ł��邱�Ƃ�O��Ƃ��Ă��܂�
std::tuple<const KdAnimationData::PerNode::PositionKey&, const KdAnimationData::PerNode::PositionKey&>
	KdAnimationData::PerNode::GetPositionKeyByTime(double frame) const
{
	// �⊮�̕K�v���Ȃ�
	if (PositionKeys.size() == 0) { return {KdAnimationData::PerNode::PositionKey(),KdAnimationData::PerNode::PositionKey() }; }
	if (PositionKeys.size() == 1){return { PositionKeys[0], PositionKeys[0] };}
	if (frame <= 0) { return { PositionKeys[0], PositionKeys[0] }; }
	auto endKey = PositionKeys.end() - 1; // �Ō�̗v�f
	if (endKey->Time <= frame) { return { *endKey, *endKey }; }

	// �n���ꂽ���Ԃ��L�[��ʂ肷����Ƃ���܂ŌJ��Ԃ�
	auto it = PositionKeys.begin();
	while (it != PositionKeys.end())
	{
		// �Y���̃L�[�Ǝ��̃L�[�̊Ԃɂ���
		if (it->Time <= frame)
		{
			if (it + 1 == PositionKeys.end()) { return { *endKey, *endKey }; }
			if ((it + 1)->Time >= frame) { return { *it, *(it + 1) }; }
		}
		it++;
	}

	// �������邱�Ƃ͂Ȃ����A�Ō�܂Ŏw�莞�Ԃ��s�����Ȃ������ꍇ
	return { *endKey, *endKey };
}
std::tuple<const KdAnimationData::PerNode::RotationKey&, const KdAnimationData::PerNode::RotationKey&>
	KdAnimationData::PerNode::GetRotationKeyByTime(double frame) const
{
	// �⊮�̕K�v���Ȃ�
	if (RotationKeys.size() == 0) { return { KdAnimationData::PerNode::RotationKey(),KdAnimationData::PerNode::RotationKey() }; }
	if (RotationKeys.size() == 1) { return { RotationKeys[0], RotationKeys[0] }; }
	if (frame < 0) { return { RotationKeys[0], RotationKeys[0] }; }
	auto endKey = RotationKeys.end() - 1; // �Ō�̗v�f
	if (endKey->Time <= frame) { return { *endKey, *endKey }; }

	// �n���ꂽ���Ԃ��L�[��ʂ肷����Ƃ���܂ŌJ��Ԃ�
	auto it = RotationKeys.begin();
	while (it != RotationKeys.end())
	{
		// �Y���̃L�[�Ǝ��̃L�[�̊Ԃɂ���
		if (it->Time <= frame)
		{
			if(it+1 == RotationKeys.end()){return { *endKey, *endKey };}
			if( (it+1)->Time >= frame){ return { *it, *(it + 1) }; }
		}
		it++;
	}

	// �������邱�Ƃ͂Ȃ����A�Ō�܂Ŏw�莞�Ԃ��s�����Ȃ������ꍇ
	return { *endKey, *endKey };
}
std::tuple<const KdAnimationData::PerNode::ScaleKey&, const KdAnimationData::PerNode::ScaleKey&>
	KdAnimationData::PerNode::GetScaleKeyByTime(double frame) const
{
	// �⊮�̕K�v���Ȃ�
	if (ScaleKeys.size() == 0) { return { KdAnimationData::PerNode::ScaleKey(),KdAnimationData::PerNode::ScaleKey() }; }
	if (ScaleKeys.size() == 1) { return { ScaleKeys[0], ScaleKeys[0] }; }
	if (frame <= 0) { return { ScaleKeys[0], ScaleKeys[0] }; }
	auto endKey = ScaleKeys.end() - 1; // �Ō�̗v�f
	if (endKey->Time <= frame) { return { *endKey, *endKey }; }

	// �n���ꂽ���Ԃ��L�[��ʂ肷����Ƃ���܂ŌJ��Ԃ�
	auto it = ScaleKeys.begin();
	while (it != ScaleKeys.end())
	{
		// �Y���̃L�[�Ǝ��̃L�[�̊Ԃɂ���
		if (it->Time <= frame)
		{
			if (it + 1 == ScaleKeys.end()) { return { *endKey, *endKey }; }
			if ((it + 1)->Time >= frame) 
			{ 
				return { *it, *(it + 1) }; 
			}
		}
		it++;
	}

	// �������邱�Ƃ͂Ȃ����A�Ō�܂Ŏw�莞�Ԃ��s�����Ȃ������ꍇ
	return { *endKey, *endKey };
}

// Index�w��ł̃L�[�̎擾(���i�͎g��Ȃ�)
const KdAnimationData::PerNode::PositionKey& KdAnimationData::PerNode::GetPositionKey(int idx) const
{
	if (PositionKeys.size() <= idx) { return PositionKeys[PositionKeys.size() - 1]; }
	return PositionKeys[idx];
}
const KdAnimationData::PerNode::RotationKey& KdAnimationData::PerNode::GetRotationKey(int idx) const
{
	if (RotationKeys.size() <= idx) { return RotationKeys[RotationKeys.size() - 1]; }
	return RotationKeys[idx];
}
const KdAnimationData::PerNode::ScaleKey& KdAnimationData::PerNode::GetScaleKey(int idx) const
{
	if (ScaleKeys.size() <= idx) { return ScaleKeys[ScaleKeys.size() - 1]; }
	return ScaleKeys[idx];
}
