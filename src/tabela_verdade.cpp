#include "tabela_verdade.hpp"

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <utf8.h>


// codigo para uma funcao assistente no codigo, nao tem relacao com o projeto
namespace stackoverflow {
template <typename T>
struct reversion_wrapper {
	T &iterable;
};

template <typename T>
auto begin(reversion_wrapper<T> w) {
	return std::rbegin(w.iterable);
}

template <typename T>
auto end(reversion_wrapper<T> w) {
	return std::rend(w.iterable);
}

template <typename T>
reversion_wrapper<T> reverse(T &&iterable) {
	return {iterable};
}
} // namespace stackoverflow

bool cp_c32_const(char32_t c, std::string_view constant) {
	return c == utf8::utf8to32(constant)[0];
}

ArvoreSintatica::ArvoreSintatica(std::u32string::iterator ini, std::u32string::iterator fim, mapa_vars_t &mapaVariaveis) {
	std::u32string::iterator var;
	// simbolos logicos permitidos em ordem de prioridade ('v' eh o ultimo a ser processado etc)
	auto oprs = std::u32string{utf8::utf8to32("⟷→∨∧¬")};
	// para cada simbolo
	for (auto c : oprs) {
		// tentar achar esse simbolo na formula
		var = std::find(ini, fim, c);
		// se achar
		if (var != fim) {
			// o simbolo vira a raiz dessa arvore
			opr = *var;
			// manda processar tudo a direita como uma formula tambem
			dir = std::make_unique<ArvoreSintatica>(var + 1, fim, mapaVariaveis);
			// se o simbolo nao for '~', tudo a esquerda tambem eh processado
			if (!cp_c32_const(c, "¬")) {
				esq = std::make_unique<ArvoreSintatica>(ini, var, mapaVariaveis);
			}
			// so o simbolo for '~', entao a formula precisa ter tamanho 2, e o '~' precisa estar no comeco dela
			else if (var != ini) {
				throw InvalidFormulaException {};
			}
			// retornar depois de achar um simbolo
			return;
		}
	}
	// se chegou aqui entao nenhum simbolo foi encontrado, entao eh para ser um nome de variavel
	opr = *ini;
	// um nome de variavel precisa ter comprimento igual a 1, e ser uma letra
	if (fim - ini != 1 || !std::isalpha(opr)) {
		std::stringstream ss;
		ss << "O texto < " << utf8::utf32to8(std::u32string {ini, fim}) << " > não é um nome de variável válido\n"
		   << "Tamanho: " << (fim - ini) << " (precisa ser 1)" << "\nÈ alfanumérico? " << (std::isalpha(opr) ? "sim" : "não") << '\n';
		throw InvalidFormulaException {ss.str()};
	}
	// o valor da variavel se refere a um mapa externo de variaveis
	valor = &mapaVariaveis[opr];
}
// avaliar o resultado da formula baseado no mapa de variaveis externo
bool ArvoreSintatica::avaliar() const {
	if (cp_c32_const(opr, "⟷")) {
		return esq->avaliar() == dir->avaliar();
	}
	if (cp_c32_const(opr, "→")) {
		return !esq->avaliar() || dir->avaliar();
	}
	if (cp_c32_const(opr, "∨")) {
		return esq->avaliar() || dir->avaliar();
	}
	if (cp_c32_const(opr, "∧")) {
		return esq->avaliar() && dir->avaliar();
	}
	if (cp_c32_const(opr, "¬")) {
		return !dir->avaliar();
	}
	// caso nao seja uma operacao logica, eh uma variavel, nesse caso o seu valor eh retornado
	return *valor;
}

TabelaVerdade::TabelaVerdade(std::string_view formula) : formula {utf8::utf8to32(formula)} {
	removerEspacos(); // remover espacos da formula
	criarTabela();	  // processar formula
}
tabela_t TabelaVerdade::getTabela() {
	return tabela;
}

void TabelaVerdade::removerEspacos() {
	formula.erase(std::remove(formula.begin(), formula.end(), ' '), formula.end());
}
void TabelaVerdade::criarTabela() {
	// mapa para guardar todas as variaveis encontradas
	mapa_vars_t mapaVariaveis;
	// criar arvore sintatica que representa a formula
	const auto arvore = std::make_unique<ArvoreSintatica>(formula.begin(), formula.end(), mapaVariaveis);
	/*
	// checar que o numero de variaveis estah dentro das especificacoes
	if (mapaVariaveis.size() < 1 || 3 < mapaVariaveis.size()) {
		throw std::runtime_error {"numero de variaveis invalida"};
	}
	*/
	// o tamanho da tabela eh iqual a 2 elevado ao numero de variaveis
	const auto tamanho = std::pow(2, mapaVariaveis.size());
	// passar por todas as combinacoes de verdadeiro e falso das variaveis para formar a tabela verdade
	for (uint64_t i {0}; i < tamanho; i++) {
		int j {0};

		for (auto &[nome, valor] : stackoverflow::reverse(mapaVariaveis)) {
			valor = ((i & (1ull << j)) == 0);
			j++;
		}

		// adicionar essa combinacao de variaveis e resultado em uma linha da tabela
		tabela.push_back({mapaVariaveis, arvore->avaliar()});
	}

}

std::string getTabelaFormatada(const tabela_t &tabela, std::string_view formula) {
	const auto [variaves, res] = tabela[0];

	std::stringstream resultado;
	std::stringstream separador;
	for (auto &[nome, valor] : variaves) {
		separador << "+---";
	}
	separador << "+-" << std::string(formula.length(), '-') << "-+";
	resultado << separador.str() << '\n';
	for (auto &[nome, valor] : variaves) {
		resultado << "| " << char(nome) << " ";
	}
	resultado << "| " << formula << " |\n";
	resultado << separador.str() << '\n';

	for (auto &[tabVar, tabResult] : tabela) {
		for (auto &[nome, valor] : tabVar) {
			resultado << "| " << (valor ? 'V' : 'F') << ' ';
		}
		const auto espacos = std::string(formula.length() / 2, ' ');
		resultado << "|" << espacos << (tabResult ? 'V' : 'F') << espacos << (formula.length() % 2 ? "  |\n" : " |\n");
	}
	resultado << separador.str() << '\n';

	return resultado.str();
}
