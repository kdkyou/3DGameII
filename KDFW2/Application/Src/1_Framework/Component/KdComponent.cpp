#include "KdComponent.h"

#include "KdFramework.h"

void KdComponent::Editor_ImGui()
{
	// �L��/����
	ImGui::Checkbox("Enable", &m_enable);
}
