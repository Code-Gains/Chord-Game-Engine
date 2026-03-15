#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "EcsDebugger.h"

EcsDebugger::EcsDebugger()
{
}

EcsDebugger::~EcsDebugger()
{
}

void EcsDebugger::Draw()
{
	if (!_enabled)
		return;

    ImGui::Begin("ECS Debugger");
    ImGui::Text("Entity Count: %d", 0); //_ecs->GetEntityCount());
    ImGui::Text("Archetype Count: %d", 0); //_ecs->GetArchetypeCount());

	// if (ImGui::BeginTable("ArchetypeTable", 1, ImGuiTableFlags_Borders))
	// {
	// 	auto& signatureToArchetype = _ecs->GetSignatureToArchetype();

	// 	for (auto& signatureIt : signatureToArchetype)
	// 	{
	// 		ImGui::TableNextRow();
	// 		ImGui::TableSetColumnIndex(0);

	// 		auto signature = signatureIt.second->GetSignature();

	// 		auto fullBitsetStr = signature.to_string();
	// 		int firstOnePos = fullBitsetStr.find('1');
	// 		auto bitsetStr = fullBitsetStr.substr(firstOnePos);
	// 		//ImGui::Text("%s", bitsetStr.c_str());
	// 		if (ImGui::TreeNode(bitsetStr.c_str()))
	// 		{
	// 			auto& typeToComponentVector = signatureIt.second.get()->GetTypeToComponentVector();
	// 			for (auto& typeIt : typeToComponentVector)
	// 			{
	// 				ImGui::Text("Type: %d Count: %d", typeIt.first, typeIt.second.get()->GetComponentCount());
	// 			}
	// 			ImGui::TreePop();
	// 		}
	// 	}
	// 	ImGui::EndTable();
	// }
	ImGui::End();
}

void EcsDebugger::Toggle()
{
	_enabled = !_enabled;
}