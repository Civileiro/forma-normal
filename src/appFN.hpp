#pragma once

#include <imgui.h>
#include <unordered_map>
#include <string>
#include <array>

#include "tabela_verdade.hpp"


class AppFN {
  public:
	AppFN() = default;
	void drawWindowAndProcess();
	void loadFonts(ImGuiIO &io);
	void processInput();
	void setCursorPos(int pos);
	int addCursor();
  private:
	std::unordered_map<std::string, ImFont*> fonts;
	std::array<char, 1024> text{};
	bool inputValid = false;
	TabelaVerdade tv{};
	int cursorPos;
	int amountToAdd;
	int hasToAdd = false;

	void secaoTabela();
	void secaoInput();
	void secaoFormas();
};