#pragma once

#include <imgui.h>
#include <unordered_map>
#include <string>
#include <array>

#include "tabela_verdade.hpp"


class FormaNormal {
  public:
	FormaNormal() = default;
	void drawWindowAndProcess();
	void loadFonts(ImGuiIO &io);
	void processInput();
	void setCursorPos(int pos);
  private:
	std::unordered_map<std::string, ImFont*> fonts;
	std::array<char, 1024> text{};
	bool inputValid = false;
	tabela_t tabela;
	int cursorPos;

	void secaoTabela();
	void secaoInput();
	void secaoFormas();
};