#pragma once

#include <array>
#include <imgui.h>
#include <string>
#include <unordered_map>

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
	std::unordered_map<std::string, ImFont *> fonts;
	std::array<char, 1024> text {};
	bool inputValid = false;
	TabelaVerdade tv {};
	FormaNormal fn {};
	int cursorPos;
	int amountToAdd;
	int hasToAdd = false;
	bool tooBig = false;

	void secaoTabela();
	void secaoInput();
	void secaoFormas();
};