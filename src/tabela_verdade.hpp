#pragma once

#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <memory>
#include <stdexcept>

using mapa_vars_t = std::map<char32_t, bool>;
using tabela_t = std::vector<std::tuple<mapa_vars_t, bool>>;


class InvalidFormulaException : public std::runtime_error {
  public:
	InvalidFormulaException() : runtime_error{"Formula Invalida"} {}
	InvalidFormulaException(std::string what) : runtime_error{std::move(what)} {}
};

class ArvoreSintatica {
  public:
	ArvoreSintatica(std::u32string::iterator ini, std::u32string::iterator fim, mapa_vars_t &mapaVariaveis);
	bool avaliar() const;

  private:
	char32_t opr;
	bool *valor;
	std::unique_ptr<ArvoreSintatica> dir, esq;
};

class TabelaVerdade {
  public:
	TabelaVerdade(std::string_view formula);
	tabela_t getTabela();

  private:
	std::u32string formula;
	tabela_t tabela;
	void removerEspacos();
	void criarTabela();
};

std::string getTabelaFormatada(const tabela_t &tabela, std::string_view formula);