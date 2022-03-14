#pragma once

#include <imgui.h>
#include <unordered_map>
#include <string>
#include <array>


class FormaNormal {
  public:
	FormaNormal() = default;
	void drawWindowAndProcess();
	void loadFonts(ImGuiIO &io);
  private:
	std::unordered_map<std::string, ImFont*> fonts;
	std::array<char, 1024> text{};
	bool inputValid = false;
	std::string asd;

	void secaoTabela();
	void secaoInput();
	void secaoFormas();
	void processInput();
};