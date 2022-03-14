#include "forma_normal.hpp"
#include "cambriamath.cpp"
#include "droidsans.cpp"
#include "tabela_verdade.hpp"

#include <utf8.h>

#include <string.h>
#include <string_view>
#include <iostream>

namespace ImGuiHelper {

void TextCentered(std::string_view text) {
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.data()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.data());
}

} // namespace ImGuiHelper

void loadDroid(std::unordered_map<std::string, ImFont *> &fonts, ImGuiIO &io, float size) {
	fonts["droid" + std::to_string(int(size))] = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, size);
}

void FormaNormal::loadFonts(ImGuiIO &io) {
	loadDroid(fonts, io, 20.f);
	loadDroid(fonts, io, 36.f);
	loadDroid(fonts, io, 54.f);
	static const ImWchar range[] = {0x20, 0x3000, 0};
	fonts["math54"] = io.Fonts->AddFontFromMemoryCompressedTTF(CambriaMath_compressed_data, CambriaMath_compressed_size, 54.f, nullptr, range);
	fonts["math36"] = io.Fonts->AddFontFromMemoryCompressedTTF(CambriaMath_compressed_data, CambriaMath_compressed_size, 36.f, nullptr, range);
}

void FormaNormal::drawWindowAndProcess() {
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration;

	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::PushFont(fonts["droid54"]);
	if (ImGui::Begin("Example: Fullscreen window", nullptr, flags)) {
		if (ImGui::BeginChild("Tabela Verdade", ImVec2 {-800, 0}, true, ImGuiWindowFlags_NoDecoration)) {
			if(inputValid)
				secaoTabela();
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("Direita", ImVec2 {0, 0}, false, ImGuiWindowFlags_NoDecoration)) {
			if (ImGui::BeginChild("Input", ImVec2 {0, 300}, true, ImGuiWindowFlags_NoDecoration)) {
				secaoInput();
			}
			ImGui::EndChild();
			if (ImGui::BeginChild("Formas Normais", ImVec2 {0, 0}, true, ImGuiWindowFlags_NoDecoration)) {
				if(inputValid)
					secaoFormas();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopFont();
}
void FormaNormal::secaoTabela() {
	ImGui::PushFont(fonts["math36"]);
	ImGui::Text(asd.data());
	ImGui::PopFont();
} 

void FormaNormal::secaoInput() {
	ImGui::SetCursorPosY(25);
	ImGuiHelper::TextCentered("Digite a formula a ser calculada");

	std::u32string buttonChars = utf8::utf8to32("∨∧¬→⟷");
	ImGui::SetCursorPosX((800 - (70 * buttonChars.size())) * 0.5f);
	ImGui::SetCursorPosY(110);

	ImGui::BeginGroup();
	ImGui::PushFont(fonts["math54"]);
	const auto buttonSize = ImVec2 {64, 64};
	bool first = true;
	int inputHead = buttonChars.size();
	for (auto c : buttonChars) {
		if (!first) {
			ImGui::SameLine();
		}
		first = false;
		inputHead--;
		auto cu8 = utf8::utf32to8(std::u32string {c});
		if (ImGui::Button(cu8.data(), buttonSize)) {
			strcat(text.data(), cu8.data());
			ImGui::SetKeyboardFocusHere(inputHead);
		}
	}
	ImGui::PopFont();
	ImGui::EndGroup();

	ImGui::SetCursorPosX(50);
	ImGui::SetCursorPosY(200);
	ImGui::PushFont(fonts["math36"]);
	ImGui::PushItemWidth(700);
	if(ImGui::InputText("##source", text.data(), text.size())) {
		try {
			processInput();
			inputValid = true;
		} catch(InvalidFormulaException &e) {
			inputValid = false;
			std::cout << e.what() << '\n';
		}
	}
	ImGui::PopItemWidth();
	ImGui::PopFont();
}

void FormaNormal::secaoFormas() {}

void FormaNormal::processInput() {
	TabelaVerdade tv{text.data()};
	asd = getTabelaFormatada(tv.getTabela(), text.data());
}