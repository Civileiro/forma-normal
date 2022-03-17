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

#include <iostream>

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

std::string c32_to_u8(char32_t c) {
	return utf8::utf32to8(std::u32string {c});
}

auto acharForaParenteses(std::u32string::iterator ini, std::u32string::iterator fim, char32_t c) {
	int parCount = 0;
	for (auto i = ini; i != fim; i++) {
		if (*i == '(') parCount++;
		else if (*i == ')') parCount--;
		else if (*i == c && parCount == 0) return i;
	}
	return fim;
}
std::vector<mapa_vars_t> FormaNormal::getFNC() const {
	std::vector<mapa_vars_t> clausulas;
	for(const auto &[mapa_vars, result] : tabela) {
		if(result == false) {
			auto clausula = mapa_vars;
			for(auto &[var, state] : clausula) {
				state = !state;
			}
			clausulas.push_back(clausula);
		}
	}
	return clausulas;
}
std::vector<mapa_vars_t> FormaNormal::getFND() const {
	std::vector<mapa_vars_t> clausulas;
	for(const auto &[mapa_vars, result] : tabela) {
		if(result) {
			clausulas.push_back(mapa_vars);
		}
	}
	return clausulas;
}
std::string FormaNormal::formatClausula(const std::vector<mapa_vars_t> &clausulas, const char32_t inner, const char32_t outer) {
	std::basic_stringstream<char32_t> ss;
	bool f_first = true;
	for(const auto &clausula : clausulas) {
		if(!f_first) ss << outer;
		
		f_first = false;

		ss << char32_t{'('};

		bool f_first = true;
		for(const auto [var, notNot] : clausula) {
			if(!f_first) ss << inner;
			f_first = false;
			if(!notNot) ss << U'¬';
			ss << var;
		}
		ss << char32_t{')'};
	}
	char32_t c = utf8::utf8to32("∨")[0];
	return utf8::utf32to8(ss.str());
}


ArvoreSintatica::ArvoreSintatica(std::u32string::iterator ini, std::u32string::iterator fim, mapa_vars_t &mapaVariaveis) {
	std::u32string::iterator var;
	// simbolos logicos permitidos em ordem de prioridade ('v' eh o ultimo a ser processado etc)
	auto oprs = utf8::utf8to32("⟷→∨∧¬");
	// para cada simbolo
	for (auto c : oprs) {
		// tentar achar esse simbolo na formula
		var = acharForaParenteses(ini, fim, c);
		// se achar
		if (var != fim) {
			// o simbolo vira a raiz dessa arvore
			opr = *var;
			// manda processar tudo a direita como uma formula tambem
			dir = std::make_unique<ArvoreSintatica>(var + 1, fim, mapaVariaveis);
			// se o simbolo nao for '~', tudo a esquerda tambem eh processado
			if (c != U'¬') {
				// std::cout << int{c} << " nao eh igual ah " << int{symbols::sNot} << '\n';
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
	// se chegou aqui entao nenhum simbolo foi encontrado, entao eh para ser um nome de variavel ou tudo parenteses
	if (*ini == '(' && fim - ini > 2) {
		*this = ArvoreSintatica {ini + 1, fim - 1, mapaVariaveis};
		return;
	}
	opr = *ini;
	// um nome de variavel precisa ter comprimento igual a 1, e ser uma letra
	if (fim - ini != 1 || !std::isalpha(opr)) {
		std::stringstream ss;
		ss << "O texto < " << utf8::utf32to8(std::u32string {ini, fim}) << " > não é um nome de variável válido\n"
		   << "Tamanho: " << (fim - ini) << " (precisa ser 1)"
		   << "\nÈ alfanumérico? " << (std::isalpha(opr) ? "sim" : "não") << '\n';
		throw InvalidFormulaException {ss.str()};
	}
	// o valor da variavel se refere a um mapa externo de variaveis
	valor = &mapaVariaveis[opr];
}
// avaliar o resultado da formula baseado no mapa de variaveis externo
bool ArvoreSintatica::avaliar() const {

	if (opr == U'⟷') {
		return esq->avaliar() == dir->avaliar();
	}
	if (opr == U'→') {
		return !esq->avaliar() || dir->avaliar();
	}
	if (opr == U'∨') {
		return esq->avaliar() || dir->avaliar();
	}
	if (opr == U'∧') {
		return esq->avaliar() && dir->avaliar();
	}
	if (opr == U'¬') {
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

std::string TabelaVerdade::getTabelaFormatada(std::string_view formula) const {
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