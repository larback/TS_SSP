# Targets:
all:
		g++ -O3 -x c++-header -march=native -std=c++11 -o "KTNS" KTNS.h
	 	g++ -O3 -x c++-header -march=native -std=c++11 -o "delta_avaliacao" delta_avaliacao.h
		g++ -O3 -x c++-header -march=native -std=c++11 -o "Buscas" Buscas.h
		g++ -O3 -march=native -std=c++11 -o "heuristics" heuristics.hpp
		g++ -O3 -march=native -std=c++11 -o "samplecode" samplecode.cpp
		g++ -O3 -march=native -std=c++11 -o "run" run.cpp
