#include <dirent.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <algorithm>

#include <iostream>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>

#include "KTNS.h"
#include "Buscas.h"
#include "heuristics.hpp"

#define MAXBIT 40
#define maxsize 4501
typedef uint8_t u8;

std::vector < std::bitset <MAXBIT> > bitMatrix(202);


std::vector<std::vector<int> > matrixFerramentas;
unsigned n = 0; // tarefas
int m = 0; // máquinas
unsigned t = 0; // ferramentas
int c = 0; // tool magazine
unsigned tempoTroca = 0; // tempo de troca entre duas ferramentas
unsigned contadorUniversal = 0;
unsigned processamento;
std::vector<unsigned>tProcessamento; // tempo de processamento das tarefas

const int TAM = 4500;

int distancia[maxsize][maxsize];	

int s; // Variável para retorno de programas externos

int retiradas[200]; // Maior do que t, 200 é o suficiente para todas as instancias atualmente existentes

std::vector<int>solution; // Melhor solução conhecida
std::vector<int>corrente; // Solução corrente
std::vector<int>iSolution; // Solução inicial
long vSolution; // Avaliação da melhor solução conhecida
long vCorrente; // Avaliação da solução corrente
long vISolution; // Avaliação da solução inicial
const int tabu_size = 500;
std::size_t tabu[2][tabu_size]; // no buscas está como constante
int idx_tabu = 0;

long tot_tabu = 0;

std::ofstream fileR;
std::ifstream file;
std::ofstream fileSolution;





int main(int argc, char* argv[]) {

	using namespace std::chrono;
	using namespace std;
	using ST = std::vector<int>;
	std::srand ( unsigned ( std::time(0) ) );
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
  	std::uniform_real_distribution<double> distribution(0,1);

	int idx_inicial = 0;

	// const unsigned n = 100;		// size of chromosomes
	tProcessamento.clear();
	matrixFerramentas.clear();
	// cin >> m;
	cin >> n;
	cin >> t;
	cin >> c;

	int total_iteracoes = ceil(3*n);

	matrixFerramentas.reserve(t); // Reserve space for the outer vector

	for (unsigned j = 0; j < t; j++){
	    std::vector<int> tmpF;
	    tmpF.reserve(n); // Reserve space for the inner vector
	    for (unsigned i = 0; i < n; ++i){
	        int tooli = 0;
	        cin >> tooli;
	        tmpF.push_back(tooli);
	    }
	    matrixFerramentas.push_back(tmpF);
	}
	
	for(int i=0; i<n; ++i)
		solution.push_back(i);
	
	vISolution = 9999999999999;
	for(int i=0;i<10;++i){
		shuffle(solution.begin(),solution.end(), std::default_random_engine(seed));
		vSolution = KTNS(solution);
		if (vSolution<vISolution){
			vISolution = vSolution;
			iSolution = solution;
		}
	}
	// Mais próximo
	for (int j=0;j<10;++j){
		for (int i=0;i<10;i++){
			solution = maisProximo(n);
			vSolution = KTNS(solution);
			if (vSolution<vISolution){
				vISolution = vSolution;
				iSolution = solution;
				idx_inicial = 1;
			}
		}
	}


	// Sub Rotas
	for (int j=0;j<10;++j){
		for (int i=0;i<10;i++){
			solution = sub(n);
			vSolution = KTNS(solution);
			if (vSolution<vISolution){
				vISolution = vSolution;
				iSolution = solution;
				idx_inicial = 2;
			}
		}
	}
	solution = iSolution;
	vSolution = vISolution;
	vCorrente = vSolution;
	corrente = solution;
	// cout << vISolution;
	// two_swap(iSolution,vISolution);
	// cout << " voltou " << vISolution << endl;
	// cout << vISolution;
	// swap_blocks(iSolution,vISolution);
	// cout << " voltou " << vISolution << endl;

	// cout << vISolution;
	// relocation(iSolution,vISolution);
	// cout << " voltou " << vISolution << endl;

	// cout << vISolution;
	// switch_pair(iSolution,vISolution);
	// cout << " voltou " << vISolution << endl;
	// solution = iSolution;
	// vSolution = vISolution;
	// vCorrente = vSolution;
	// corrente = solution;
		

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	high_resolution_clock::time_point t2;
	duration<double> time_span;


	tabu[0][idx_tabu] = hash_vector(solution);
	tabu[1][idx_tabu] = hash_vector(solution);
	idx_tabu++;
	
	for (int iteracoes=0;iteracoes<total_iteracoes;++iteracoes){
		three_optFull_restart(corrente,vCorrente);
		// Achou solução melhor
		if (vCorrente<vSolution){
			vSolution = vCorrente;
			solution = corrente;
			iteracoes = 0;
		}

		if (idx_tabu == tabu_size){
			idx_tabu = 0;
		}
		tabu[0][idx_tabu] = hash_vector(corrente);
		idx_tabu++;
		tot_tabu++;
		if (iteracoes==n){
			cout << "Perturbei. Estava em " << vCorrente << endl;
			double_bridge(corrente,vCorrente);
			cout << "Ficou em " << vCorrente << endl;
			tabu[0][idx_tabu] = hash_vector(corrente);
			idx_tabu++;
			tot_tabu++;
		}
	}



	t2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double> >(t2 - t1);

	fileSolution.open(argv[1]);
	// Corrigir cabecalho da solucao
	fileSolution << n << " " << t << " " <<  c << " " << vISolution << " " << vSolution << " " << time_span.count() << endl; 
	fileSolution.close();
	
	// cout << "Inicial: " << vISolution << " Final: " << vSolution << " Temp de execução: " << time_span.count() << " Solução inicial: " << idx_inicial << endl;
	// cout << "Soluções Tabu " << tot_tabu << endl;
	// cout << "Solução:";
	// db_print(solution);

	return 0;
}
