#include "appFN.hpp"
#include "cambriamath.cpp"
#include "droidsans.cpp"

#include <utf8.h>

#include <iostream>
#include <string.h>
#include <string_view>

namespace ImGuiHelper {

void TextCentered(std::string_view text) {
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.data()).x;

	if(textWidth < 800) ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::TextWrapped(text.data());
}

} // namespace ImGuiHelper

int inputCallback(ImGuiInputTextCallbackData *data) {
	auto myData = reinterpret_cast<AppFN *>(data->UserData);
	myData->setCursorPos(data->CursorPos);
	data->CursorPos += myData->addCursor();
	static std::string oldText;
	static bool doProcess = false;
	if(doProcess) {
		myData->processInput();
		doProcess = false;
	}
	if (oldText != std::string {data->Buf}) {
		doProcess = true;
	}

	oldText = std::string {data->Buf};
	return 0;
}

int AppFN::addCursor() {
	if(hasToAdd) {
		hasToAdd = false;
		return amountToAdd;
	}
	return 0;
}

void loadDroid(std::unordered_map<std::string, ImFont *> &fonts, ImGuiIO &io, float size) {
	fonts["droid" + std::to_string(int(size))] = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, size);
}

void AppFN::loadFonts(ImGuiIO &io) {
	loadDroid(fonts, io, 20.f);
	loadDroid(fonts, io, 36.f);
	loadDroid(fonts, io, 54.f);
	static const ImWchar range[] = {0x20, 0x3000, 0};
	fonts["math54"] = io.Fonts->AddFontFromMemoryCompressedTTF(CambriaMath_compressed_data, CambriaMath_compressed_size, 54.f, nullptr, range);
	fonts["math36"] = io.Fonts->AddFontFromMemoryCompressedTTF(CambriaMath_compressed_data, CambriaMath_compressed_size, 36.f, nullptr, range);
}

void AppFN::drawWindowAndProcess() {
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration;

	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::PushFont(fonts["droid54"]);
	if (ImGui::Begin("Example: Fullscreen window", nullptr, flags)) {
		if (ImGui::BeginChild("Tabela Verdade", ImVec2 {-800, 0}, true, ImGuiWindowFlags_NoDecoration)) {
			if (!inputValid ) {
				ImGuiHelper::TextCentered("Input Invalido!");
			} else if(tv.getTabela().size() > 100) {
				ImGuiHelper::TextCentered("Tabela Verdade muito grande para mostrar");
			} else {
				secaoTabela();
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("Direita", ImVec2 {0, 0}, false, ImGuiWindowFlags_NoDecoration)) {
			if (ImGui::BeginChild("Input", ImVec2 {0, 300}, true, ImGuiWindowFlags_NoDecoration)) {
				secaoInput();
			}
			ImGui::EndChild();
			if (ImGui::BeginChild("Formas Normais", ImVec2 {0, 0}, true, ImGuiWindowFlags_NoDecoration)) {
				if(tv.getTabela().size() > 300) {
					ImGuiHelper::TextCentered("Muitas variaveis para computar!");
				}
				else if (inputValid) secaoFormas();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopFont();
}
void AppFN::secaoTabela() {
	ImGui::PushFont(fonts["math36"]);
	const auto tabela = tv.getTabela();
	const auto tableWidth = std::get<0>(tabela[0]).size() + 1;
	if (ImGui::BeginTable("table1", tableWidth, ImGuiTableFlags_Borders)) {
		for (const auto &[name, state] : std::get<0>(tabela[0])) {
			ImGui::TableSetupColumn(utf8::utf32to8(std::u32string {name}).data(), ImGuiTableColumnFlags_WidthFixed);
		}
		ImGui::TableSetupColumn(text.data(), ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();
		int row = 0;
		for (const auto &[mapa_vars, result] : tabela) {
			ImGui::TableNextRow();
			int column = 0;
			for (const auto [nome, state] : mapa_vars) {
				ImGui::TableSetColumnIndex(column);
				ImGui::Text(state ? "V" : "F", row, column);

				ImU32 cell_bg_color = state ? ImGui::GetColorU32(ImVec4(0.2f, 1.f, 0.3f, 0.65f)) : ImGui::GetColorU32(ImVec4(0.9f, 0.1f, 0.3f, 0.65f));
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);

				column++;
			}
			ImGui::TableSetColumnIndex(column);
			ImGui::Text(result ? "V" : "F", row, column);

			ImU32 cell_bg_color = result ? ImGui::GetColorU32(ImVec4(0.2f, 1.f, 0.3f, 0.65f)) : ImGui::GetColorU32(ImVec4(0.9f, 0.1f, 0.3f, 0.65f));
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
			row++;
		}
		ImGui::EndTable();
	}
	ImGui::PopFont();
}

void AppFN::secaoInput() {
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
			const auto tam = strlen(text.data());
			std::copy(text.begin() + cursorPos, text.begin() + tam, text.begin() + cursorPos + cu8.size());
			std::copy(cu8.begin(), cu8.end(), text.begin() + cursorPos);
			text[tam + cu8.size()] = '\0';
			ImGui::SetKeyboardFocusHere(inputHead);
			
			amountToAdd = cu8.size();
			hasToAdd = true;
		}
	}
	ImGui::PopFont();
	ImGui::EndGroup();

	ImGui::SetCursorPosX(50);
	ImGui::SetCursorPosY(200);
	ImGui::PushFont(fonts["math36"]);
	ImGui::PushItemWidth(700);
	if (ImGui::InputText("##source", text.data(), text.size(), ImGuiInputTextFlags_CallbackAlways, inputCallback, this)) {
	}
	ImGui::PopItemWidth();
	ImGui::PopFont();
}

void AppFN::secaoFormas() {
	FormaNormal fn{tv.getTabela()};

	ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 50);
	ImGui::PushFont(fonts["droid36"]);
	ImGuiHelper::TextCentered("Forma Normal Conjuntiva:");
	ImGui::PushFont(fonts["math36"]);
	ImGuiHelper::TextCentered(FormaNormal::formatClausula(fn.getFNC(), U'∨', U'∧', true));
	ImGui::PopFont();
	
	ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 50);
	ImGuiHelper::TextCentered("Forma Normal Disjuntiva:");
	ImGui::PushFont(fonts["math36"]);
	ImGuiHelper::TextCentered(FormaNormal::formatClausula(fn.getFND(), U'∧', U'∨', false));
	ImGui::PopFont();
	ImGui::PopFont();
}

void AppFN::processInput() {
	try {
		tv = TabelaVerdade{text.data()};
		inputValid = true;
	} catch (InvalidFormulaException &e) {
		inputValid = false;
		std::cout << e.what() << '\n';
	}
}

void AppFN::setCursorPos(int pos) {
	cursorPos = pos;
}