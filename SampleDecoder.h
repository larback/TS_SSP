/*
 * SampleDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#ifndef SAMPLEDECODER_H
#define SAMPLEDECODER_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <cstdio>
#include "KTNS.h"
#include "Buscas.h"
#define maxsize 4501
extern bool vai;
extern unsigned n;
extern int distancia[maxsize][maxsize];
int noise_distance[maxsize][maxsize];
extern int s; // retorno de programas externos;
extern std::vector<int>solution; 
extern unsigned generation;

std::ofstream fileLKH;
std::ifstream fileSolutionLKH;
std::ifstream fileTeste;



class SampleDecoder {
public:
	SampleDecoder()  { }
	// SampleDecoder(std::vector<unsigned> _tProcessamento): tProcessamento(_tProcessamento) { }
	~SampleDecoder() { }

	double decode(std::vector< double >& chromosome) const{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_real_distribution<double> distribution(0,1);
		
		double random_part;

		if ((generation == 1) && (vai)){
			// cout << "Primeira geração sem ruído\n";
			vai = false;
			for (int i=0;i<chromosome.size();++i) chromosome[i]=0;
		}
		int k = 0;
		for (int i=0;i<=n;i++){
			for(int j=i+1;j<=n;j++){
				// noise_distance[i][j] = round(distancia[i][j]*chromosome[k]);
				
				noise_distance[i][j] = round(chromosome[k]) + distancia[i][j];
				if (noise_distance[i][j]<0) noise_distance[i][j] = 0;

				noise_distance[j][i] = noise_distance[i][j];
				k++;
			}
		}
		// diagonal principal;
		for (int i=0;i<=n;++i) noise_distance[i][i] = 0;

		// if (vai){
		// 	for (size_t i=0;i<=n;i++){
		// 		for (size_t j=0;j<=n;j++){
		// 			cout << noise_distance[i][j] << ' ';
		// 		}
		// 		cout << endl;
		// 	}
		// 	vai = false;
		// }

		// Make LKH parameter and config file
		string nome_par = "lunner.par";
		string problem_file= "in.tsp";
		string tour_file = "out.tsp";
		do {
			random_part = distribution(generator);
			fileTeste.open(nome_par+to_string(random_part));
		} while (fileTeste.is_open());
		fileTeste.close();

		nome_par+=to_string(random_part);
		problem_file+=to_string(random_part);
		tour_file+=to_string(random_part);

		fileLKH.open(nome_par);
		fileLKH << "PROBLEM_FILE = " << problem_file << endl;
		fileLKH << "MOVE_TYPE = 5 " << endl;
		fileLKH << "PATCHING_C = 3 " << endl;
		fileLKH << "PATCHING_A = 2 " << endl;
		fileLKH << "TOUR_FILE = " << tour_file << endl;
		fileLKH << "RUNS = 1 " << endl;
		fileLKH << "trace_level = 0 " << endl;
		fileLKH.close();
		

		fileLKH.open(problem_file);
		fileLKH << "NAME : ." << endl;
		fileLKH << "TYPE : ATSP " << endl;
		fileLKH << "COMMENT : -- " << endl;
		fileLKH << "DIMENSION : " << n + 1 << endl;
		// fileLKH << "FIXED_EDGES_SECTION " << endl;
		// fileLKH << "-1 " << endl;
		fileLKH << "EDGE_WEIGHT_TYPE : EXPLICIT " << endl;
		fileLKH << "EDGE_WEIGHT_FORMAT : FULL_MATRIX " << endl;
		fileLKH << "EDGE_WEIGHT_SECTION " << endl;
		for(size_t i = 0; i <= n; ++i) {
			for(size_t j = 0; j <= n; ++j) {
				fileLKH << noise_distance[i][j] << " ";
			}
			fileLKH << endl;
		}
		fileLKH.close();

		// Run LKH
		// std::string cmd_modelo = "./LKH-2.0.9/LKH runner.par >saidaLKH.txt";
		std::string cmd_modelo = "./LKH-2.0.9/LKH " + nome_par + " >saidaLKH.txt";
		const char * cm = cmd_modelo.c_str();
		s = system(cm);

		// Read the LKH output file
		solution.clear();
		fileSolutionLKH.open(tour_file.c_str(), std::ifstream::in);
		string linha;
		while(fileSolutionLKH){
			fileSolutionLKH >> linha;
			if (linha == "TOUR_SECTION")
				break;
		}
		int iTour;
		fileSolutionLKH >> iTour; // Ignora a coluna adicional
		for (int i=0;i<n;++i){
			fileSolutionLKH >>iTour;
			solution.push_back((iTour -2));
		}
		fileSolutionLKH.close();

		remove(tour_file.c_str());
		remove(nome_par.c_str());
		remove(problem_file.c_str());

		long trocas = KTNS(solution);
		// two_optFull(solution, trocas);
		return trocas;

		// std::sort(ranking.begin(), ranking.end());
		// return 100;
	}

private:
	std::vector<unsigned> tProcessamento;
};

#endif
