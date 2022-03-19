#include "tabela_verdade.hpp"

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
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
std::vector<mapa_vars_t> FormaNormal::getFNCunop() const {
	std::vector<mapa_vars_t> clausulas;
	for (const auto &[mapa_vars, result] : tabela) {
		if (result == false) {
			auto clausula = mapa_vars;
			for (auto &[var, state] : clausula) {
				state = !state;
			}
			clausulas.push_back(clausula);
		}
	}
	// simplifyFormula(clausulas);
	return clausulas;
}
std::vector<mapa_vars_t> FormaNormal::getFNDunop() const {
	std::vector<mapa_vars_t> clausulas;
	for (const auto &[mapa_vars, result] : tabela) {
		if (result) {
			clausulas.push_back(mapa_vars);
		}
	}
	// simplifyFormula(clausulas);
	return clausulas;
}
/*
template <typename Map>
bool key_compare(const Map &lhs, const Map &rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](auto a, auto b) {
			   return a.first == b.first;
		   });
}
template <typename Map>
char32_t keyprime(const Map &lhs, const Map &rhs) {
	if (lhs.size() >= rhs.size()) return {};
	int diffs {};
	char32_t differ = 0;
	for (const auto [var, state] : lhs) {
		if (rhs.find(var) == rhs.end()) return {};
		if (rhs.find(var)->second != state) {
			diffs++;
			differ = var;
		}
		if (diffs > 1) return {};
	}
	return differ;
}
template <typename Map>
bool keysubset(const Map &lhs, const Map &rhs) {
	if (lhs.size() > rhs.size()) return false;
	for (const auto [var, state] : lhs) {
		const auto itFound = rhs.find(var);
		if (itFound == rhs.end()) return false;
		if (itFound->second != state) return false;
	}
	return true;
}

void FormaNormal::simplifyFormula(std::vector<mapa_vars_t> &clausulas) {
	if (clausulas.size() == 0 || clausulas.size() == 1) return;
	for (bool change = true; change;) {
		change = false;
		for (int i = 0; i < clausulas.size() - 1; i++) {
			for (int j = i + 1; j < clausulas.size(); j++) {
				auto c1i = i, c2i = j;
				if (clausulas[i].size() > clausulas[j].size()) {
					std::swap(c1i, c2i);
				}
				auto &c1 = clausulas[c1i];
				auto &c2 = clausulas[c2i];
				// test if theres only 1 negated var diff
				if (key_compare(c1, c2)) {
					int diffs = 0;
					char32_t differ;
					for (const auto &[var, state] : c1) {
						if (state != c2[var]) {
							diffs++;
							if (diffs > 1) break;
							differ = var;
						}
					}
					if (diffs == 1) {
						c1.erase(differ);
						clausulas.erase(clausulas.begin() + c2i);
						change = true;
						goto exit_loops;
					}
				}
				if (const auto differ = keyprime(c1, c2); differ != char32_t {}) {
					c2.erase(differ);
					change = true;
					goto exit_loops;
				}

				// if (c1.size() == 1) {
				// 	const auto var = c1.begin()->first;
				// 	if (c2.find(var) != c2.end() && c2.find(var)->second == c1.begin()->second) {
				// 		// std::cout << formatClausula({c1, c2}, ',', ':') << '\n';
				// 		clausulas.erase(clausulas.begin() + c2i);
				// 		change = true;
				// 		goto exit_loops;
				// 	}
				// }
				if (keysubset(c1, c2)) {
					clausulas.erase(clausulas.begin() + c2i);
					change = true;
					goto exit_loops;
				}
				if (c1 == c2) {
					clausulas.erase(clausulas.begin() + c2i);
					change = true;
					goto exit_loops;
				}
			}
		}
	exit_loops:;
	}
	if (clausulas[0].size() == 0) clausulas[0][0] = true;
	std::sort(clausulas.begin(), clausulas.end());
}*/
template <typename Map>
bool keysubset(const Map &lhs, const Map &rhs) {
	if (lhs.size() > rhs.size()) return false;
	for (const auto [var, state] : lhs) {
		const auto itFound = rhs.find(var);
		if (itFound == rhs.end()) return false;
		if (itFound->second != state) return false;
	}
	return true;
}
struct TabelaLine {
	uint64_t bitState;
	bool hasCircle;
};
struct TabelaBitState {
	mapa_vars_t vars;
	uint64_t toBitState(const mapa_vars_t &mapa) const {
		int i = 0;
		uint64_t result = 0;
		for (const auto [var, state] : mapa) {
			result |= ((1ull << std::distance(vars.begin(), vars.find(var))) << (state ? 0 : 32));
		}
		return result;
	}
	uint64_t toBitState(const std::vector<std::pair<char32_t, bool>> &mapa) const {
		int i = 0;
		uint64_t result = 0;
		for (const auto [var, state] : mapa) {
			result |= ((1ull << std::distance(vars.begin(), vars.find(var))) << (state ? 0 : 32));
		}
		return result;
	}
	mapa_vars_t fromBitState(uint64_t bits) const {
		uint64_t i = 1;
		mapa_vars_t mapa;
		for (const auto var : vars) {
			if (bits & i) mapa[var.first] = true;
			i <<= 1;
		}
		i = 1;
		i <<= 32;
		for (const auto var : vars) {
			if (bits & i) mapa[var.first] = false;
			i <<= 1;
		}
		return mapa;
	}
	static bool bitStateIncludes(const uint64_t checker, const uint64_t checked) {
		const auto res = (checker & checked) == checker;
		// std::cout << "checking " << checker << " with " << checked << "  " << res << '\n';
		return (checker & checked) == checker;
	}
};
std::vector<mapa_vars_t> FormaNormal::getKarnaugh(bool isFNC) const {
	mapa_vars_t m;
	m[0] = true;
	const auto tautologia = m;
	m[0] = false;
	const auto contradicao = m;

	std::vector<std::pair<char32_t, bool>> vars;
	std::vector<TabelaLine> results;
	const TabelaBitState tbs {tabela[0].first};
	for (const auto [var, state] : tabela[0].first) {
		vars.push_back({var, false});
	}
	const auto tabSize = std::pow(2, vars.size());
	for (const auto &[mapa, result] : tabela) {
		if (result ^ isFNC) results.push_back({tbs.toBitState(mapa), false});
	}
	/*
	std::cout << "eh FNC? " << isFNC << '\n';
	for (const auto line : results) {
		std::cout << "b: " << line.bitState << '\n';
	}*/
	// std::cout << "tam results: " << results.size() << '\n';
	// std::cout << "tam vars: " << vars.size() << '\n';
	// std::cout << "tam tam: " << tabSize << '\n';

	if (results.size() == 0 && isFNC || results.size() == tabSize && !isFNC) return {tautologia};
	if (results.size() == 0 && !isFNC || results.size() == tabSize && isFNC) return {contradicao};

	std::vector<std::vector<std::pair<char32_t, bool>>> allVarCombinations;
	allVarCombinations.reserve(tabSize);
	// std::cout << "tam allvars: " << allVarCombinations.size() << '\n';
	bool first = true;
	for (uint64_t i {0}; i < tabSize; i++) {
		if (first) {
			first = false;
			continue;
		}
		int j {0};
		std::vector<std::pair<char32_t, bool>> varComb;
		varComb.reserve(vars.size());
		for (const auto [nome, valor] : vars) {
			if ((i & (1ull << j)) != 0) {
				// std::cout << "i j: " << i << "  " << j << '\n';
				varComb.push_back(vars[j]);
			}
			j++;
		}
		allVarCombinations.push_back(std::move(varComb));
	}
	// std::cout << "tam allvars: " << allVarCombinations.size() << '\n';
	std::sort(allVarCombinations.begin(), allVarCombinations.end(), [](auto lhs, auto rhs) {
		return rhs.size() > lhs.size();
	});
	std::vector<uint64_t> winnerBitStates;
	for (auto &comb : allVarCombinations) {
		const auto tamanho = std::pow(2, comb.size());
		std::vector<uint64_t> allBitStates;
		allBitStates.reserve(tamanho);
		for (uint64_t i {0}; i < tamanho; i++) {
			int j {0};
			for (auto &[nome, valor] : comb) {
				valor = ((i & (1ull << j)) == 0);
				j++;
			}
			const auto bitState = tbs.toBitState(comb);
			/*bool found = false;
			for (const auto winner : winnerBitStates) {
				if (TabelaBitState::bitStateIncludes(winner, bitState)) {
					found = true;
					break;
				}
			} 
			if (!found)*/ {
				allBitStates.push_back(bitState);
				// std::cout << "a bitstate found: " << bitState << '\n';
			}
		}
		// std::cout << "all bits size: " << allBitStates.size() << '\n';
		for (const auto bitState : allBitStates) {
			bool hasUncircled = false;

			int count = 0;
			for (const auto line : results) {
				if (TabelaBitState::bitStateIncludes(bitState, line.bitState)) {
					hasUncircled |= !line.hasCircle;
					count++;
				} else {
					//success = false;
					// break;
				}
			}
			// std::cout << "tabsize: " << tabSize << "\ntamanho: " << tamanho << "\ncount: " << count << '\n';

			if (hasUncircled && count == tabSize / tamanho) {
				// std::cout << "winner: " << bitState << '\n';
				winnerBitStates.push_back(bitState);
				for (auto &line : results) {
					if (TabelaBitState::bitStateIncludes(bitState, line.bitState)) {
						// std::cout << "cleaning\n";
						line.hasCircle = true;
					}
				}
			}
		}
	}
	std::vector<mapa_vars_t> result;
	result.reserve(winnerBitStates.size());
	for (const auto bitState : winnerBitStates) {
		auto clausula = tbs.fromBitState(bitState);
		for (auto &[var, state] : clausula) state = state ^ isFNC;
		result.push_back(clausula);
	}
	std::sort(result.begin(), result.end());
	// std::cout << "result size: " << result.size() << '\n';
	// std::cout << "winners size: " << winnerBitStates.size() << '\n';
	return result;
}
std::string FormaNormal::formatClausula(const std::vector<mapa_vars_t> &clausulas, const char32_t inner, const char32_t outer) {
	if (clausulas.size() == 0) return "error";
	if (const auto homoFound = clausulas[0].find(0); homoFound != clausulas[0].end()) return homoFound->second ? "Tautologia" : "Contradição";
	std::basic_stringstream<char32_t> ss;
	bool f_first = true;
	for (const auto &clausula : clausulas) {
		if (!f_first) ss << outer;

		f_first = false;

		if (clausula.size() > 1) ss << char32_t {'('};

		bool f_first = true;
		for (const auto [var, notNot] : clausula) {
			if (!f_first) ss << inner;
			f_first = false;
			if (!notNot) ss << U'¬';
			ss << var;
		}
		if (clausula.size() > 1) ss << char32_t {')'};
	}
	auto result = ss.str();
	if (clausulas.size() == 1 && clausulas[0].size() > 1) result = result.substr(1, result.size() - 2);
	return utf8::utf32to8(result);
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