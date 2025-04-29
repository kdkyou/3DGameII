#include "Pch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "KdMeshLoader.h"
#include "KdAnimationData.h"

// モデルデータからアニメーションデータのみを抜き出して独自ファイルに保存
void KdAnimationData::Parse(aiAnimation* src, const std::vector<KdMeshScene::NodeData>& allNodes)
{
	Name = src->mName.C_Str(); // アニメーション名

	LengthInTick = src->mDuration;	// アニメーションのフレーム数
	TicksParSecond = src->mTicksPerSecond; // 一秒間の進行フレーム数

	// 環境により指定されていない可能性があるらしいので、指定されていないかったら仮に60FPSとする
	if (TicksParSecond == 0)
	{
		TicksParSecond = 60.0f;
	}

	// アニメーションの継続時間(秒)
	LengthInSecond = LengthInTick / TicksParSecond;

	// アニメーションの割り振られている部位ごとに読み込み
	UINT numChannel = src->mNumChannels;
	for (UINT idx = 0; idx < numChannel; idx++)
	{
		aiNodeAnim* srcChannel = src->mChannels[idx];
		KdAnimationData::PerNode destNode;

		destNode.Name = srcChannel->mNodeName.C_Str(); // 対応するパーツの名前
		
		// 名前からノードのIndexを割り出しておく------------------->
		for(UINT nc = 0; nc < allNodes.size(); nc++)
		{
			if(allNodes[nc].NodeName == destNode.Name)
			{
				destNode.NodeIndex = nc;
				break;
			}
		}
		//<--------------------アニメーションコンポーネントで管理すべき

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

// アニメーションデータを一つのアセットとして出力
bool KdAnimationData::Save(const std::string& directory) const
{
	// 不正文字を全部アンダーバーに置き換える
	std::string forFile = SwapUnavailableCharacters(Name, "_");

	// セーブファイル名=アニメーション名.kdanim
	std::string filename = directory + "/" + forFile + ".kdanim";

	std::ofstream ofs(filename);
	if (!ofs) return false;

	// アニメーションデータをシリアライズ
	auto json = Serialize();

	// 文字列化
	std::string str = json.dump();

	// ファイル書き込み
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

	// アニメーション基本データ
	jsonObj["Name"] = Name;
	jsonObj["LengthInTick"] = LengthInTick;
	jsonObj["TicksParSecond"] = TicksParSecond;
	jsonObj["LengthInSecond"] = LengthInSecond;

	// アニメーションの各キーの情報
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
	// アニメーション基本データ
	KdJsonUtility::GetValue(jsonObj, "Name", &Name);
	KdJsonUtility::GetValue(jsonObj, "LengthInTick", &LengthInTick);
	KdJsonUtility::GetValue(jsonObj, "TicksParSecond", &TicksParSecond);
	KdJsonUtility::GetValue(jsonObj, "LengthInSecond", &LengthInSecond);

	// ノード毎のデータ
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

// すべての要素を補完
KdTransform KdAnimationData::Interpolation(const KdAnimationData::PerNode& node, double time)
{
	KdTransform trans;
	double frames = time * TicksParSecond; // 現在のフレーム数
	trans.SetLocalPosition(node.InterpolationPosition(frames));
	trans.SetLocalRotation(node.InterpolationRotation(frames));
	trans.SetLocalScale(node.InterpolationScale(frames));
	return trans;
}

// 時間を指定してのアニメーション補完
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
	ret["NodeIdx"] = NodeIndex; // 現状未使用

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

// 座標補完
KdVector3 KdAnimationData::PerNode::PositionKey::Interpolation(const PositionKey& last, double time) const
{
	double betweenTime = last.Time - Time;
	if (betweenTime <= 0) { return vPos; } // 指定したキーの順序が不正
	if (time <= Time) { return vPos; } // 先頭キーより手前の時間だった
	if (time >= last.Time) { return last.vPos; } // 最後のキーより後の時間だった

	double nowProgressTime = time - Time; // 先頭キーからどれくらい経過しているか
	float nowProgressPercent = (float)(nowProgressTime / betweenTime); // キー間の進行具合

	return KdVector3::Lerp(vPos, last.vPos, nowProgressPercent);
}
// 回転補完
KdQuaternion KdAnimationData::PerNode::RotationKey::Interpolation(const RotationKey& last, double time) const
{
	double betweenTime = last.Time - Time;
	if (betweenTime <= 0) { return qRot; } // 指定したキーの順序が不正
	if (time <= Time) { return qRot; } // 先頭キーより手前の時間だった
	if (time >= last.Time) { return last.qRot; } // 最後のキーより後の時間だった

	double nowProgressTime = time - Time; // 先頭キーからどれくらい経過しているか
	float nowProgressPercent = (float)(nowProgressTime / betweenTime); // キー間の進行具合

	return KdQuaternion::Lerp(qRot, last.qRot, nowProgressPercent);
}
// 拡縮補完
KdVector3 KdAnimationData::PerNode::ScaleKey::Interpolation(const ScaleKey& last, double time) const
{
	double betweenTime = last.Time - Time;
	if (betweenTime <= 0) { return vScale; } // 指定したキーの順序が不正
	if (time <= Time) { return vScale; } // 先頭キーより手前の時間だった
	if (time >= last.Time) { return last.vScale; } // 最後のキーより後の時間だった

	double nowProgressTime = time - Time; // 先頭キーからどれくらい経過しているか
	float nowProgressPercent = (float)(nowProgressTime / betweenTime); // キー間の進行具合

	auto v = KdVector3::Lerp(vScale, last.vScale, nowProgressPercent);
	return KdVector3::Lerp(vScale, last.vScale, nowProgressPercent);
}


// 指定されたフレームの前後のキーを返す(学生はTupleで検索！)
// キーは昇順で並んでいることを前提としています
std::tuple<const KdAnimationData::PerNode::PositionKey&, const KdAnimationData::PerNode::PositionKey&>
	KdAnimationData::PerNode::GetPositionKeyByTime(double frame) const
{
	// 補完の必要性なし
	if (PositionKeys.size() == 0) { return {KdAnimationData::PerNode::PositionKey(),KdAnimationData::PerNode::PositionKey() }; }
	if (PositionKeys.size() == 1){return { PositionKeys[0], PositionKeys[0] };}
	if (frame <= 0) { return { PositionKeys[0], PositionKeys[0] }; }
	auto endKey = PositionKeys.end() - 1; // 最後の要素
	if (endKey->Time <= frame) { return { *endKey, *endKey }; }

	// 渡された時間がキーを通りすぎるところまで繰り返し
	auto it = PositionKeys.begin();
	while (it != PositionKeys.end())
	{
		// 該当のキーと次のキーの間にいる
		if (it->Time <= frame)
		{
			if (it + 1 == PositionKeys.end()) { return { *endKey, *endKey }; }
			if ((it + 1)->Time >= frame) { return { *it, *(it + 1) }; }
		}
		it++;
	}

	// 多分来ることはないが、最後まで指定時間が行きつかなかった場合
	return { *endKey, *endKey };
}
std::tuple<const KdAnimationData::PerNode::RotationKey&, const KdAnimationData::PerNode::RotationKey&>
	KdAnimationData::PerNode::GetRotationKeyByTime(double frame) const
{
	// 補完の必要性なし
	if (RotationKeys.size() == 0) { return { KdAnimationData::PerNode::RotationKey(),KdAnimationData::PerNode::RotationKey() }; }
	if (RotationKeys.size() == 1) { return { RotationKeys[0], RotationKeys[0] }; }
	if (frame < 0) { return { RotationKeys[0], RotationKeys[0] }; }
	auto endKey = RotationKeys.end() - 1; // 最後の要素
	if (endKey->Time <= frame) { return { *endKey, *endKey }; }

	// 渡された時間がキーを通りすぎるところまで繰り返し
	auto it = RotationKeys.begin();
	while (it != RotationKeys.end())
	{
		// 該当のキーと次のキーの間にいる
		if (it->Time <= frame)
		{
			if(it+1 == RotationKeys.end()){return { *endKey, *endKey };}
			if( (it+1)->Time >= frame){ return { *it, *(it + 1) }; }
		}
		it++;
	}

	// 多分来ることはないが、最後まで指定時間が行きつかなかった場合
	return { *endKey, *endKey };
}
std::tuple<const KdAnimationData::PerNode::ScaleKey&, const KdAnimationData::PerNode::ScaleKey&>
	KdAnimationData::PerNode::GetScaleKeyByTime(double frame) const
{
	// 補完の必要性なし
	if (ScaleKeys.size() == 0) { return { KdAnimationData::PerNode::ScaleKey(),KdAnimationData::PerNode::ScaleKey() }; }
	if (ScaleKeys.size() == 1) { return { ScaleKeys[0], ScaleKeys[0] }; }
	if (frame <= 0) { return { ScaleKeys[0], ScaleKeys[0] }; }
	auto endKey = ScaleKeys.end() - 1; // 最後の要素
	if (endKey->Time <= frame) { return { *endKey, *endKey }; }

	// 渡された時間がキーを通りすぎるところまで繰り返し
	auto it = ScaleKeys.begin();
	while (it != ScaleKeys.end())
	{
		// 該当のキーと次のキーの間にいる
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

	// 多分来ることはないが、最後まで指定時間が行きつかなかった場合
	return { *endKey, *endKey };
}

// Index指定でのキーの取得(普段は使わない)
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
