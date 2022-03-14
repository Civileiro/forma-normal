#include "forma_normal.hpp"

#include "imgui.h"

void FormaNormal::drawWindowAndProcess() {
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration;

	// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
	// Based on your use case you may want one of the other.
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if (ImGui::Begin("Example: Fullscreen window", nullptr, flags)) {
		if (ImGui::BeginChild("Tabela Verdade", ImVec2 {400, 0}, true, ImGuiWindowFlags_NoDecoration)) {
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("Direita", ImVec2 {0, 0}, false, ImGuiWindowFlags_NoDecoration)) {
			if (ImGui::BeginChild("Input", ImVec2 {0, 300}, true, ImGuiWindowFlags_NoDecoration)) {
			}
			ImGui::EndChild();
			if (ImGui::BeginChild("Formas Normais", ImVec2 {0, 0}, true, ImGuiWindowFlags_NoDecoration)) {
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}