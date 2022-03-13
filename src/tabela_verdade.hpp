#pragma once

#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <memory>

using tabela_t = std::vector<std::tuple<std::map<char, bool>, bool>>;

class ArvoreSintatica {
  public:
	ArvoreSintatica(std::string::iterator ini, std::string::iterator fim, std::map<char, bool> &mapaVariaveis);
	bool avaliar() const;

  private:
	char opr;
	bool *valor;
	std::unique_ptr<ArvoreSintatica> dir, esq;
};

class TabelaVerdade {
  public:
	TabelaVerdade(std::string_view formula);
	auto getTabela();

  private:
	std::string formula;
	tabela_t tabela;
	void removerEspacos();
	void criarTabela();
};

auto getTabelaFormatada(const tabela_t &tabela, std::string_view formula);