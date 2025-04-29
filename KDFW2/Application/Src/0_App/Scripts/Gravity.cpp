#include"KdFramework.h"
#include"Gravity.h"

void Gravity::Start()
{

}

void Gravity::Update()
{
	if (IsEnable() == false) { return; }

	//入力検知
	auto& keyboard = KdInput::GetInstance().GetKeyboardState();

	//１秒間の量→1フレームの量
	float deltaTime = KdTime::GetInstance().GetDeltaTime();

	//コンポーネントの持ち主のトランスフォームと座標の取得
	const auto& transform = GetGameObject()->GetTransform();
	auto pos = transform->GetPosition();

//	float vMoveOnes = 0.0f;
	KdVector3 vMoveOnes = { 0,0,0 };

	bool isJump = false;

	//radiobuttonのデータによる条件分岐
	if (m_isSelect==Select::Key) {
		if (keyboard.Space) {
			isJump = true;
		}
	}
	else {
		//インターバル減少
		--m_interval;

		if (m_interval < Select::None)
		{
			isJump = true;
			m_interval = m_intervalVal;
		}
	}

	if (isJump)
	{
		//落下値をジャンプ力に変更
		m_fall = -m_jumpPow;
	}
	
	vMoveOnes.y -= m_fall;
	m_fall += m_gravity * deltaTime;

	pos += vMoveOnes;

	//更新結果をTransformに教える
	transform->SetLocalPosition(pos);
}

void Gravity::LateUpdate()
{
}

void Gravity::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//キー入力か自動か
	ImGui::Text(u8"ジャンプタイミング");
	ImGui::RadioButton(u8"なし", &m_isSelect,Select::None);
	ImGui::RadioButton(u8"スペース", &m_isSelect,Select::Key);
	ImGui::RadioButton(u8"自動", &m_isSelect,Select::Auto);

	//ジャンプ力調整
	ImGui::DragFloat(u8"ジャンプ力",&m_jumpPow,0.001f,0.0f,100.0f	);

	//重力調整
	ImGui::DragFloat(
		u8"重力",
		&m_gravity,
		0.001f,
		0.0f,
		100.0f
	);

	//自動時のインターバル
	ImGui::DragFloat(u8"インターバル値", &m_intervalVal, 0.001f, 0.0f, 100.0f);

	ImGui::LabelText(std::to_string(m_interval).c_str(), u8"現在時間");
}

void Gravity::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);
}

void Gravity::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);
}
