#ifndef HEURISTICS_H
#define HEURISTICS_H
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string.h>
#include <bitset>
#include <random>
#include <ctime>
#include <ratio>
#include <string>
#include <set>
#include <stack>
#include "KTNS.h"
#define MAX_INT 2147483647

vector<int> escolhe_maisProximo(int inicial, int tarefas) {
    vector<int> perm;
    vector<int> permAux;
    set<int> resto; // todas as tarefas que ainda não foram pra solução
    
    int diff = MAX_INT;
    int diffAux;
    int tam = 2;

    perm.assign(tarefas, -1); 
    perm[0] = inicial; // coloca a inicial no início da solução
    
    // coloca todas as tarefas no resto ( nenhuma foi escolhida ainda )
    for(int i = 0; i < tarefas; i++)
        resto.insert(i);

    // tira a tarefa inicial das tarefa que ainda vão entrar
    resto.erase(inicial);
    
    // enquanto existis tarefas a serem alocadas
    while(resto.size() > 0) {
        diff = MAX_INT;
        permAux.assign(tam, -1);

        /*
            caso não exista nenhuma tarefa na permutação auxiliar, coloca a tarefa inicial nela
        */
        if (tam == 2) 
            permAux[0] = inicial;
        else {
            // copia a melhor solução parcial antiga 
            for(int i = 0; i < tam; i++) {
                permAux[i] = perm[i];
            }
        }
        int saiu = -1;
        // pilha de candidatos de melhor tarefa para entrar, caso existam tarefas com o mesmo valor de troca
        stack<int> candidatos;
        for(set<int>::iterator it = resto.begin(); it != resto.end(); it++) {
            int aux = permAux[tam-1];
            permAux[tam-1] = *it;
            diffAux = KTNS(permAux);

            if(diffAux < diff) {
                diff = diffAux;
                while (candidatos.size() > 0)
                    candidatos.pop();
                candidatos.push(*it);
            } else if (diffAux == diff) {
                candidatos.push(*it);    
            } else {
                permAux[tam-1] = aux;
            }
        }
        
        // resolver empates aleatoriamente     
        random_device rd;
        uniform_int_distribution<int> dist(0, candidatos.size()-1);
        int p = dist(rd); 
        
        for(int i = 0; i < p; i++) {
            candidatos.pop();
        }
        
        // pega o candidato escolhido
        saiu = candidatos.top();

        // coloca ele no final da solução parcial
        permAux[tam-1] = saiu;
        perm = permAux; 
        resto.erase(saiu); // remove ele das tarefas que ainda vão entrar 
        tam++;
    }
    
    return perm;

}
vector<int>maisProximo(int n){
    vector<int> final;
    final.assign(n, -1);
    int diff = MAX_INT;
    int diffAux;
    vector<int> permAux;
    for(int i = 0; i < n; i++) {
        permAux = escolhe_maisProximo(i,n);
        diffAux = KTNS(permAux);
        if (diffAux < diff) {
            diff = diffAux;
            final = permAux;
        }
    }
    return final;
}


vector<int> escolhe(int inicial, int n) {
    vector<int> perm;
    vector<int> permAuxFinal;
    vector<int> permAuxInicio;
    set<int> resto;
    
    int diff = MAX_INT;
    int diffFinal;
    int diffInicio;
    int tam = 2;

    bool inicio;

    perm.assign(n, -1);    
    perm[0] = inicial;

    for(int i = 0; i < n; i++)
        resto.insert(i);

    resto.erase(inicial);
    
    while(resto.size() > 0) {
        diff = MAX_INT;
        permAuxFinal.assign(tam, -1);
        permAuxInicio.assign(tam, -1);

        if (tam == 2) 
            permAuxFinal[0] = inicial;
        else {
            for(int i = 0; i < tam; i++) {
                permAuxFinal[i] = perm[i];
            }
        }
        int saiu = -1;
        stack<int> candidatos;
        for(set<int>::iterator it = resto.begin(); it != resto.end(); it++) {
            permAuxFinal[tam-1] = *it;

            for(int i = tam-1; i > 0; i--) {
                permAuxInicio[i] = permAuxFinal[i-1];
            }
            permAuxInicio[0] = *it;

            diffFinal = KTNS(permAuxFinal);

            diffInicio = KTNS(permAuxInicio);

            if(diffFinal < diff && diffFinal < diffInicio) {
                while(candidatos.size() > 0)
                    candidatos.pop();
                candidatos.push(*it);

                diff = diffFinal;
                inicio = false;
            } else if (diffInicio < diff && diffInicio < diffFinal) {
                while(candidatos.size() > 0)
                    candidatos.pop();
                candidatos.push(*it);

                diff = diffInicio;
                inicio = true;
            } else if (diffInicio == diffFinal && diffFinal < diff ) {
                random_device r;
                uniform_int_distribution<int> d(0, 1);
                int aleatorio = d(r);
                if (aleatorio == 0) {
                    inicio = true;
                    diff = diffInicio;
                } else {
                    inicio = false;
                    diff = diffFinal;
                }    
                candidatos.push(*it);
            } 


        }
        
        // resolver empates aleatoriamente     
        random_device rd;
        uniform_int_distribution<int> dist(0, candidatos.size()-1);
        int p = dist(rd); 

        for(int i = 0; i < p; i++) {
            candidatos.pop();
        }
        
        saiu = candidatos.top();
        candidatos.pop();

        if (inicio) {
            for(int i = tam-1; i > 0; i--)
                permAuxFinal[i] = permAuxFinal[i-1];
            permAuxFinal[0] = saiu;
        } else {
            permAuxFinal[tam-1] = saiu;
        }

        perm = permAuxFinal;
        resto.erase(saiu);
        tam++;
    }
    return perm;
}
vector<int>sub(int n){
    vector<int> final;
    final.assign(n, -1);

    int diff = MAX_INT;
    int diffAux;
    vector<int> permAux;
    for(int i = 0; i < n; i++) {
        permAux = escolhe(i,n);
        diffAux = KTNS(permAux);

        if (diffAux < diff) {
            diff = diffAux;
            final = permAux;
        }
    }
    return final;
}

#endif