#ifndef BUSCAS_H
#define BUSCAS_H

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


#include "KTNS.h"
#include "delta_avaliacao.h"
#define MAXBIT 40
extern std::vector < std::bitset <MAXBIT> > bitMatrix;
extern std::vector<unsigned>tProcessamento;
extern const int tabu_size;
extern std::size_t tabu[2][500];
extern int idx_tabu;



std::chrono::high_resolution_clock::time_point tb1;
std::chrono::high_resolution_clock::time_point tb2;
std::chrono::duration<double> time_span_b;
double IBSMakespan, EFBMakespan, ONBMakespan;
using namespace std;


template <typename T>
void hash_combine(std::size_t& seed, const T& value) {
    std::hash<T> hasher;
    seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Gera um hash para um vector de int
std::size_t hash_vector(const std::vector<int>& vec) {
    std::size_t seed = 0; // Valor inicial do hash
    for (const auto& value : vec) {
        hash_combine(seed, value);
    }
    return seed;
}


void db_print(vector<int> s){
  for (int i=0;i<s.size();++i)
    cout << s[i] << " ";
  cout <<"\n";
}

bool isTabu(std::vector<int> s, int p){
  std::size_t hash_value = hash_vector(s);
  for (int i=0;i<idx_tabu;i++)
    if (tabu[p][i]==hash_value)
      return true;
  return false;
}

void rotacao(std::vector<int> &solution, long &trocas){
	int aux;
	long tempTrocas;
	std::vector<int>solutionTemp;
	solutionTemp.clear();
	solutionTemp = solution;
	//solution = f_solution;
	// tempTrocas = trocas;
	
	for (int i =0; i<n; ++i){
		aux = solutionTemp[0];
		solutionTemp.erase(solutionTemp.begin());
		solutionTemp.push_back(aux);
		tempTrocas = KTNS(solutionTemp);
		if (tempTrocas < trocas){
			trocas = tempTrocas;
			solution.clear();
			solution = solutionTemp;
		}
	}
}

void inverte(std::vector<int> &solution, int a, int b){
  int aux;
  if (a>b){
    aux = a;
    a = b;
    b = aux;
  }
  for (int i=a;i<=b;++i){
    aux = solution[i];
    solution[i] = solution[b];
    solution[b] = aux;
    --b;
  }
}

void two_optFull(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas, bestTrocas;
	std::vector<int>solutionTemp;
  std::vector<int>bestNeighbor;
  vector<int> order; // Ordem em que os vértices serão acessados.
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  bool feito = false;
  solutionTemp.clear();
  solutionTemp = solution;
  bestNeighbor.clear();
  bestNeighbor = solution;
  bestTrocas = 99999999999999;

  for (unsigned i=0;i<solutionTemp.size();++i)
    order.push_back(i);
  
  shuffle(order.begin(),order.end(),std::default_random_engine(seed));

  for (int i=0;i<order.size()-1;++i){
    for (int j = i+1;j<order.size();++j){
      inverte(solutionTemp,order[i],order[j]);
      // db_print(solutionTemp);
      if (!isTabu(solutionTemp,0)){
        tempTrocas = KTNS(solutionTemp);
        if (tempTrocas < bestTrocas) { // E não for TABU
          bestTrocas = tempTrocas;
          bestNeighbor.clear();
          bestNeighbor = solutionTemp;
        } 
      } 
        
      solutionTemp.clear();
      solutionTemp = solution;
    }
  }

  solution = bestNeighbor;
  trocas = bestTrocas;
  // db_print(bestNeighbor);
  
}

void two_optBI(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  bool feito = false;
  int a,b;
  solutionTemp.clear();
  solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i){
    order.push_back(i);
  }

  random_shuffle(order.begin(),order.end());
  
  for (int i=0;i<order.size()-1;++i){
    for (int j = i+1;j<order.size();++j){
      inverte(solutionTemp,order[i],order[j]);
      tempTrocas = KTNS(solutionTemp);
      if (tempTrocas < trocas){
        trocas = tempTrocas;
        a = order[i];
        b = order[j];
        feito = true;
      } 
      solutionTemp.clear();
      solutionTemp = solution;
    }
  }
  if (feito){
    inverte(solution,a,b);
  }
}


void two_swap(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas, bestTrocas;
	std::vector<int>solutionTemp;
  std::vector<int>bestNeighbor;
  vector<int> order,order2; // Ordem em que os vértices serão acessados.
  int job1, job2, tamanho;
  bool feito = false;
  solutionTemp.clear();
  solutionTemp = solution;
  bestNeighbor.clear();
  bestNeighbor = solution;
  bestTrocas = 99999999999999;
  for (unsigned i=0;i<solutionTemp.size();++i){
    order.push_back(i);
    order2.push_back(i);
  }
  random_shuffle(order.begin(),order.end());
  random_shuffle(order2.begin(),order2.end());

  for (int i=0;i<solutionTemp.size();++i){
    job1 = solutionTemp[order[i]];
    for (int j=0;j<solutionTemp.size();++j){
      job2 = solutionTemp[order2[j]];
      if (job1 == job2)
        continue;
      solutionTemp[order[i]] = job2;
      solutionTemp[order2[j]] = job1;
      if (!isTabu(solutionTemp,1)){
        tempTrocas = KTNS(solutionTemp);
        if (tempTrocas<bestTrocas){
          // feito = true;
          bestTrocas = tempTrocas;
          bestNeighbor.clear();
          bestNeighbor = solutionTemp;
        }
      }
      solutionTemp[order[i]] = job1;
      solutionTemp[order2[j]] = job2;
    }
  }

}


void swap_blocks(std::vector<int> &solution, long &trocas){

  vector<int> orderA, orderB, solutionTemp; // Ordem em que os vértices serão acessados.
  int pivo1, pivo2;
  bool jaFoi = false;
  long trocasTemp = trocas;
  for (unsigned i=0;i<solution.size();++i){
    orderA.push_back(i);
    orderB.push_back(i);
  }

  random_shuffle(orderA.begin(),orderA.end());
  random_shuffle(orderB.begin(),orderB.end());

  solutionTemp.clear();
  solutionTemp = solution;

  for (int i=0;i<orderA.size();++i){
    if ((orderA[i] == 0) || (orderA[i]==orderA.size()-1))
      continue;
    pivo1 = orderA[i];
    for (int j=0;j<orderB.size();++j){
      int aux;
      if ((orderB[j]>0) && (orderB[j]<orderB.size()-1) &&  ((orderB[j]<pivo1-2) || (orderB[j]>pivo1+2))){
        pivo2 = orderB[j];
        
        aux = solutionTemp[pivo1];
        solutionTemp[pivo1] = solutionTemp[pivo2];
        solutionTemp[pivo2] = aux;
        aux = solutionTemp[pivo1-1];
        solutionTemp[pivo1-1] = solutionTemp[pivo2-1];
        solutionTemp[pivo2-1] = aux;
        aux = solutionTemp[pivo1+1];
        solutionTemp[pivo1+1] = solutionTemp[pivo2+1];
        solutionTemp[pivo2+1] = aux;
        
        trocasTemp =  KTNS(solutionTemp);
        if (trocasTemp < trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = solutionTemp;
          jaFoi = true;
          break;
        } else {
          // Desfaz a troca;
          aux = solutionTemp[pivo1];
          solutionTemp[pivo1] = solutionTemp[pivo2];
          solutionTemp[pivo2] = aux;
          aux = solutionTemp[pivo1-1];
          solutionTemp[pivo1-1] = solutionTemp[pivo2-1];
          solutionTemp[pivo2-1] = aux;
          aux = solutionTemp[pivo1+1];
          solutionTemp[pivo1+1] = solutionTemp[pivo2+1];
          solutionTemp[pivo2+1] = aux;
        }
      }else
        continue;
    }
    if (jaFoi)
      break;
  }
}

void movimento1(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j)
    temp.push_back(solution[j]);
  
  // Pega do ponto 2 até ponto 1 +1
  for (int j=p2;j>p1;--j)
    temp.push_back(solution[j]);
  
  // Pega do ponto 3 até o ponto 2 +1
  for (int j=p3;j>p2;--j)
    temp.push_back(solution[j]);
  
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j)
    temp.push_back(solution[j]);
  
}

void movimento2(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para Ponto 2+1 até Ponto 3
  for (int j=p2+1;j<=p3;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 1+1 até o ponto 2
  for (int j=p1+1;j<=p2;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento3(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para Ponto 3, inverte até Ponto 2 +1
  for (int j=p3;j>p2;--j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 1+1 até Ponto 2
  for (int j=p1+1;j<=p2;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento4(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para Ponto 3, inverte até Ponto 1 +1
  for (int j=p3;j>p1;--j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 3 + 1 até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento5(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 1 segue para último, inverte até Ponto 3 + 1
  for (int j=solution.size()-1;j>p3;--j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 2 + 1 até o Ponto 3
  for (int j=p2+1;j<=p3;++j){
    temp.push_back(solution[j]);
  }
  // Pega do ponto 2, inverte até Ponto 1-1
  for (int j=p2;j>p1;--j){
    temp.push_back(solution[j]);
  }
}

void movimento6(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 2, inverte até Ponto 2+1
  for (int j=p2;j>=p1+1;--j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 2+1 até o final
  for (int j=p2+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}

void movimento7(std::vector<int> solution, std::vector<int> &temp, int p1, int p2, int p3){
  temp.clear();
  // Em ordem até o ponto 1
  for (int j=0;j<=p1;++j){
    temp.push_back(solution[j]);
  }
  // Ponto 2+1, segue até Ponto 3
  for (int j=p2+1;j<=p3;++j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 2, inverte até P1+1
  for (int j=p2;j>p1;--j){
    temp.push_back(solution[j]);
  }
  // Pega do Ponto 3+1, segue até o final
  for (int j=p3+1;j<solution.size();++j){
    temp.push_back(solution[j]);
  }
}
void three_opt(std::vector<int> &solution, long &trocas){
  std::vector<int> temp;
  long trocasTemp = 0;
  bool melhorou = false;

  for (int i=0;i<solution.size();++i){
    for (int j=i+1;j<solution.size();++j){
      for (int k=j+1;k<solution.size();++k){
        movimento1(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }
        movimento2(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento3(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento4(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento5(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento6(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  

        movimento7(solution, temp, i,j,k);
        trocasTemp = KTNS(temp); 
        if (trocasTemp<trocas){
          trocas = trocasTemp;
          solution.clear();
          solution = temp;
          melhorou = true;
          break;
        }  
      }
      if (melhorou) break;
    }    
    if (melhorou) break;
  }  

}
void three_optFull(std::vector<int> &solution, long &trocas){
  std::vector<int> temp;
  std::vector<int>bestNeighbor;
  long trocasTemp = 0;
  bool melhorou = true;
  long bestTrocas = 99999999999999;
  bestNeighbor.clear();
  bestNeighbor = solution;

  for (int i=0;i<solution.size();++i){
    for (int j=i+1;j<solution.size();++j){
      for (int k=j+1;k<solution.size();++k){
        movimento1(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }
        
        movimento2(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }

        movimento3(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }  

        movimento4(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }  

        movimento5(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }

        movimento6(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }

        movimento7(solution, temp, i,j,k); 
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
          }
        }
      }
    }    
  }  
  solution = bestNeighbor;
  trocas = bestTrocas;

}



void three_optFull_restart(std::vector<int> &solution, long &trocas){
  std::vector<int> temp;
  std::vector<int>bestNeighbor;
  long trocasTemp = 0;
  bool melhorou = true;
  long bestTrocas = 99999999999999;
  bestNeighbor.clear();
  bestNeighbor = solution;

  for (int i=0;i<solution.size();++i){
    for (int j=i+1;j<solution.size();++j){
      for (int k=j+1;k<solution.size();++k){
        movimento1(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }
        
        movimento2(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }

        movimento3(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }  

        movimento4(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }  

        movimento5(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }

        movimento6(solution, temp, i,j,k);
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }

        movimento7(solution, temp, i,j,k); 
        if (!isTabu(temp,0)){
          trocasTemp = KTNS(temp); 
          if (trocasTemp<bestTrocas){
            bestTrocas = trocasTemp;
            bestNeighbor.clear();
            bestNeighbor = temp;
            solution = bestNeighbor;
            i=0;
            j=0;
            break;
          }
        }
      }
      if (j==0) break;
    }    
  }  
  solution = bestNeighbor;
  trocas = bestTrocas;

}


void double_bridge(std::vector<int> &solucao, long &trocas){
  vector<int> order;
  vector<int> points,temp;
  long tempT;
  long melhorPerturbacaoTrocas = 9999999999;
  vector<int> melhorPerturbacaoSolucao;
  for (unsigned i = 0; i<solucao.size(); ++i)               
    order.push_back(i);
  random_shuffle(order.begin(),order.end());

  for (unsigned i =0; i< solucao.size()-3; ++i){
    points.clear();
    points.push_back(order[i]);
    points.push_back(order[i+1]);
    points.push_back(order[i+2]);
    sort(points.begin(),points.end());
    temp.clear();
    for (int i=points[2]+1;i<solucao.size();++i)
      temp.push_back(solucao[i]);
    
    for (int i=points[1]+1;i<=points[2];++i)
      temp.push_back(solucao[i]);
   
    for (int i=points[0]+1;i<=points[1];++i)
      temp.push_back(solucao[i]);
    
    for (int i=0;i<=points[0];++i)
      temp.push_back(solucao[i]);

    tempT = KTNS(temp);
    if (tempT < melhorPerturbacaoTrocas){
      solucao.clear();
      solucao = temp;
      trocas = tempT;
      melhorPerturbacaoTrocas = tempT;
    }
  }
}

// void double_bridgeU(std::vector<int> &solucao, long &trocas){
//   vector<int> order;
//   vector<int> points,temp;
//   long tempT;
//   for (unsigned i = 0; i<solucao.size(); ++i)               
//     order.push_back(i);
//   random_shuffle(order.begin(),order.end());

//   for (unsigned i =0; i< solucao.size()-3; ++i){
//     points.clear();
//     points.push_back(order[i]);
//     points.push_back(order[i+1]);
//     points.push_back(order[i+2]);
//     sort(points.begin(),points.end());
//     temp.clear();
//     for (int i=points[2]+1;i<solucao.size();++i)
//       temp.push_back(solucao[i]);
    
//     for (int i=points[1]+1;i<=points[2];++i)
//       temp.push_back(solucao[i]);
   
//     for (int i=points[0]+1;i<=points[1];++i)
//       temp.push_back(solucao[i]);
    
//     for (int i=0;i<=points[0];++i)
//       temp.push_back(solucao[i]);

//     // Sem nenhum critério de aceitação
//     tempT = KTNS(temp);
//     // if (tempT<=aceite){
//       solucao.clear();
//       solucao = temp;
//       trocas = tempT;
//       break;
//     // }
//   }
// }


// void double_bridgeU(std::vector<int> &solucao, long &trocas){
//   vector<int> order;
//   vector<int> points,temp;
//   long tempT;
//   for (unsigned i = 0; i<solucao.size(); ++i)               
//     order.push_back(i);
//   random_shuffle(order.begin(),order.end());

//   for (unsigned i =0; i< solucao.size()-4; ++i){
//     points.clear();
//     points.push_back(order[i]);
//     points.push_back(order[i+1]);
//     points.push_back(order[i+2]);
//     points.push_back(order[i+3]);

//     sort(points.begin(),points.end());
//     temp.clear();
//     for (int i=0;i<=points[0];i++)
//       temp.push_back(solucao[i]);

//     for (int i=points[3]+1;i<solucao.size();++i)
//       temp.push_back(solucao[i]);
    
//     for (int i=points[2]+1;i<=points[3];++i)
//       temp.push_back(solucao[i]);
   
//     for (int i=points[1]+1;i<=points[2];++i)
//       temp.push_back(solucao[i]);
    
//     for (int i=points[0]+1;i<=points[1];i++)
//       temp.push_back(solucao[i]);
    

//     // Sem nenhum critério de aceitação
//     tempT = KTNS(temp);
//     // if (tempT<=aceite){
//       solucao.clear();
//       solucao = temp;
//       trocas = tempT;
//       break;
//     // }
//   }
// }



void double_bridgeU(std::vector<int> &solucao, long &trocas){
  vector<int> order;
  vector<int> points,temp;
  long tempT;
  for (unsigned i = 0; i<solucao.size(); ++i)               
    order.push_back(i);
  random_shuffle(order.begin(),order.end());

  for (unsigned i =0; i< solucao.size()-4; ++i){
    points.clear();
    points.push_back(order[i]);
    points.push_back(order[i+1]);
    points.push_back(order[i+2]);
    points.push_back(order[i+3]);

    sort(points.begin(),points.end());
    temp.clear();
    for (int i=0;i<=points[0];i++)
      temp.push_back(solucao[i]);

    for (int i=points[2]+1;i<=points[3];++i)
      temp.push_back(solucao[i]);
    
    for (int i=points[1]+1;i<=points[2];++i)
      temp.push_back(solucao[i]);
   
    for (int i=points[0]+1;i<=points[1];++i)
      temp.push_back(solucao[i]);
    
    for (int i=points[3]+1;i<solucao.size();i++)
      temp.push_back(solucao[i]);
    

    // Sem nenhum critério de aceitação
    tempT = KTNS(temp);
    // if (tempT<=aceite){
      solucao.clear();
      solucao = temp;
      trocas = tempT;
      break;
    // }
  }
}

void relocation(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  int job, tamanho;
  int destino = -1;
	solutionTemp.clear();
	solutionTemp = solution;
  for (unsigned i=0;i<solutionTemp.size();++i)
    order.push_back(i);

  // random_shuffle(order.begin(),order.end());

  tamanho = solutionTemp.size();
  
  for (int i=0;i<tamanho;++i){
    job = solutionTemp[order[i]];
    solutionTemp.erase(solutionTemp.begin()+order[i]);
    destino = -1;
    for (int j=0;j<tamanho;++j){
      solutionTemp.insert(solutionTemp.begin()+j,job);
      auxTrocas = KTNS(solutionTemp);
      if (auxTrocas < tempTrocas){
        tempTrocas = auxTrocas;
        destino = j;
        //break;
      }
      solutionTemp.erase(solutionTemp.begin()+j);
    }
    // volta o job para o local original
    solutionTemp.insert(solutionTemp.begin()+order[i],job);
    
    // Se achou um melhor lugar para este job
    if (destino!=-1){
      solutionTemp.erase(solutionTemp.begin()+order[i]);
      solutionTemp.insert(solutionTemp.begin()+destino,job);
      solution.clear();
      solution = solutionTemp;
      trocas = tempTrocas;
      i = 0;
      // break;
    }
    
  }
}
vector<int> usados;
bool jaFoi(int job){
  for(int i=0;i<usados.size();++i){
    if(usados[i]==job) {
     
      return true;

    } 
  }
  return false;
}

void US(std::vector<int> &solution, long &trocas) {
  long tempTrocas, auxTrocas;
  std::vector<int>solutionTemp;
  int job,p;
  tempTrocas = trocas;
  auxTrocas = trocas;
  solutionTemp = solution;
  usados.clear();

  for (int k=0;k<solution.size();++k){
    for (int i=0; i<solution.size();++i){
      job = solutionTemp[i];

      if (jaFoi(job) != true){
        solutionTemp.erase(solutionTemp.begin()+i);
        tempTrocas = KTNS(solutionTemp);
        if (tempTrocas <= auxTrocas){
          auxTrocas = tempTrocas;
          p = i;
        }
        // Volta o job para o lugar
        solutionTemp.insert(solutionTemp.begin()+i,job);
      }
    }

    job = solutionTemp[p];
    solutionTemp.erase(solutionTemp.begin()+p);
    usados.push_back(job);
   
    for (int i=0;i<=solutionTemp.size();++i){
      if (i==p) continue;
      solutionTemp.insert(solutionTemp.begin()+i, job);
      
      tempTrocas = KTNS(solutionTemp);
      if (tempTrocas < trocas){
        trocas = tempTrocas;
        solution.clear();
        solution = solutionTemp;
      }
      solutionTemp.erase(solutionTemp.begin()+i);
    }
    solutionTemp.insert(solutionTemp.begin()+p,job);
    auxTrocas = trocas;
  }
}

void or_opt_moviment(vector<int> &s, int p1, int p2, int p3){
  vector<int> temp;
  // Segue até o P1
  for (int i=0;i<=p1;++i)
    temp.push_back(s[i]);
  // Ponto P2+1 até P3
  for (int i=p2+1;i<=p3;++i)
    temp.push_back(s[i]);
  // Ponto P1+1 até P2
  for (int i=p1+1;i<=p2;++i)
    temp.push_back(s[i]);
  // Ponto P3+1 até o final
  for (int i=p3+1;i<s.size();++i)
    temp.push_back(s[i]);
  s.clear();
  s = temp;
}

void or_opt(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  vector<int>pontos;
  bool melhorou =false;
  int p1,p2,p3;

  for (unsigned i=0;i<solution.size();++i){
    order.push_back(i);
  }
  random_shuffle(order.begin(),order.end());

  

   for (int i=0;i<solution.size();++i){
    melhorou = false;
    for (int j=i+1;j<solution.size();++j){
      for (int k=j+1;k<solution.size();k++){
        pontos.push_back(order[i]);
        pontos.push_back(order[j]);
        pontos.push_back(order[k]);
        sort(pontos.begin(),pontos.end());
        // cout << pontos[0] << " " << pontos[1] << " " << pontos[2] << endl;
        movimento2(solution, solutionTemp, pontos[0],pontos[1],pontos[2]);

        tempTrocas = KTNS(solutionTemp);
        if (tempTrocas < trocas){
          trocas = tempTrocas;
          solution.clear();
          solution = solutionTemp;
          melhorou = true;
          i=0;
          break;
        }
        pontos.clear();
      }
      if (melhorou)
        break;
    }
    if (melhorou)
        break;
  }
}

void perturbacao_or_opt(std::vector<int> &solution, long &trocas){
  long tempTrocas, auxTrocas;
	std::vector<int>solutionTemp;
  vector<int> order; // Ordem em que os vértices serão acessados.
  vector<int>pontos;
  int p1,p2,p3;

  for (unsigned i=0;i<solution.size();++i){
    order.push_back(i);
  }
  random_shuffle(order.begin(),order.end());

  pontos.push_back(order[0]);
  pontos.push_back(order[1]);
  pontos.push_back(order[2]);
  sort(pontos.begin(),pontos.end());
  // cout << pontos[0] << " " << pontos[1] << " " << pontos[2] << endl;
  movimento2(solution, solutionTemp, pontos[0],pontos[1],pontos[2]);

  trocas = KTNS(solutionTemp);
  solution.clear();
  solution = solutionTemp;
  
  pontos.clear();

}

void pertubacao_swap_blocks(std::vector<int> &solution, long &trocas, int tam){

  vector<int> orderA, orderB, solutionTemp; // Ordem em que os vértices serão acessados.
  int pivo1, pivo2;
  bool jaFoi = false;
  long trocasTemp = trocas;
  for (unsigned i=0;i<solution.size();++i){
    orderA.push_back(i);
    orderB.push_back(i);
  }

  random_shuffle(orderA.begin(),orderA.end());
  random_shuffle(orderB.begin(),orderB.end());

  solutionTemp.clear();
  solutionTemp = solution;
  bool ok = false;
  int idx = 0;
  do {
    pivo1 = orderA[idx];
    if (pivo1 <= (solution.size()-(tam*2)))
      ok = true;
    else
      ok = false;
    ++idx;
  } while (!ok);

  ok = false;
  do {
    pivo2 = orderA[idx];
    if (pivo2<(pivo1+tam))
      ok = false;
    else{
      if (pivo2 <= (solution.size()-tam))
        ok = true;
      else
        ok = false;
    }
    ++idx;
  } while (!ok);
  int aux;
  for (int i=0;i<tam;++i){
    aux = solution[pivo1+i];
    solution[pivo1+i] = solution[pivo2+i];
    solution[pivo2+i] = aux;
  }
  trocas = KTNS(solution);
}
void makeABreak(std::vector<int> &solucao, long &trocas){
  extern unsigned n;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::uniform_int_distribution<int> distribution(1,n-1);
  vector<int>temp;
  int a;
  a = distribution(generator);

  for (int i=a;i<solucao.size();++i)
    temp.push_back(solucao[i]);
  for (int i=0;i<a;++i)
    temp.push_back(solucao[i]);
  solucao.clear();
  solucao = temp;
  trocas = KTNS(solucao);
}



void US_Swap (std::vector<int> &solution, long &trocas) {
  long tempTrocas, auxTrocas;
  std::vector<int>solutionTemp;
  std::vector<int>order;
  vector<long>diminuicao;
  int job,p;
  tempTrocas = trocas;
  auxTrocas = trocas;
  solutionTemp = solution;
  usados.clear();

  for (int i=0;i<solution.size();++i){
    job = solutionTemp[i];
    solutionTemp.erase(solutionTemp.begin()+i);
    auxTrocas = KTNS(solutionTemp);
    diminuicao.push_back(trocas - auxTrocas);
    solutionTemp.insert(solutionTemp.begin()+i, job);
  }
  int maior = -1;
  for (int j=0;j<diminuicao.size();++j){
    maior = -1;
    for (int i=0;i<diminuicao.size();++i){
      if (diminuicao[i]>=maior){
        maior = diminuicao[i];
        p = i;
      }
    }
    diminuicao[p]=-2;
    order.push_back(p);
  }
  solutionTemp.clear();
  solutionTemp = solution;  
  for (int i=0;i<solutionTemp.size();++i){
    for (int j=0;j<solutionTemp.size();++j){
      if (j == order[i]) 
        continue;
      job = solutionTemp[order[i]];
      solutionTemp[order[i]] = solutionTemp[j];
      solutionTemp[j] = job;
      auxTrocas = KTNS(solutionTemp);
      if (auxTrocas<trocas){
        trocas = auxTrocas;
        solution.clear();
        solution = solutionTemp;
      } else {
        job = solutionTemp[order[i]];
        solutionTemp[order[i]] = solutionTemp[j];
        solutionTemp[j] = job;
      }
    }
  }
}

void luck_way(std::vector<int> &solution, long &trocas){
  extern unsigned n;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  srand(seed);
  std::vector<int> temp,slice;
  long trocasTemp;
  int A,B;
  
  std::vector<int> points;
  for (int i=0;i<solution.size();++i)
    points.push_back(i);
  random_shuffle(points.begin(),points.end());

  for (int i=0;i<points.size();i+=2){
    if (points[i]<points[i+1]){
      A = points[i];
      B = points[i+1];
    } else{
      A = points[i+1];
      B = points[i];
    }
    // Shuffle at max 20% of n
    if ((B - A) <= (n*0.2)){
      slice.clear();
      for (int i=A;i<=B;++i)
        slice.push_back(solution[i]);
      random_shuffle(slice.begin(),slice.end());
      
      temp.clear();
      for (int i=0;i<A;++i)
        temp.push_back(solution[i]);
      for (int i=0;i<slice.size();++i)
        temp.push_back(slice[i]);
      for (int i=B+1;i<solution.size();++i)
        temp.push_back(solution[i]);

      trocasTemp = KTNS(temp);
      if (trocasTemp<trocas){
        solution.clear();
        solution = temp;
        trocas = trocasTemp;
      }
    }
  }
}

void luck_step(std::vector<int> &solution, long &trocas){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  srand(seed);
  std::vector<int> temp,slice;
  long trocasTemp;
  int A,B;
  
  std::vector<int> pointsA,pointsB;
  for (int i=0;i<solution.size();++i){
    pointsA.push_back(i);
    pointsB.push_back(i);
  }
  random_shuffle(pointsA.begin(),pointsA.end());
  random_shuffle(pointsB.begin(),pointsB.end());  

  for (int j=0;j<pointsA.size();++j){
    for (int i=0;i<pointsB.size();i++){
      if (pointsA[j]==pointsB[i])
        continue;

      if (pointsA[j]<pointsB[i]){
        A = pointsA[j];
        B = pointsB[i];
      } else{
        A = pointsB[i];
        B = pointsA[j];
      }
      // cout << "A: " << A << " B: " << B << endl;
      slice.clear();
      for (int k=A;k<=B;++k)
        slice.push_back(solution[k]);
      random_shuffle(slice.begin(),slice.end());
      
      temp.clear();
      for (int k=0;k<A;++i)
        temp.push_back(solution[k]);

      for (int k=0;k<slice.size();++k)
        temp.push_back(slice[k]);

      for (int k=B+1;k<solution.size();++k)
        temp.push_back(solution[k]);

      trocasTemp = KTNS(temp);
      if (trocasTemp<trocas){
        cout << "A: " << A << " B: " << B << endl;
        for (int h=A;h<=B;++h)
          cout << solution[h] << " ";
        cout << endl;

        for(int h=0;h<slice.size();++h)
          cout << slice[h] << " ";
        cout << "\n";
        
        solution.clear();
        solution = temp;
        trocas = trocasTemp;
      }
    }
  }
}
void switch_pair(std::vector<int> &solution, long &trocas){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  srand(seed);
  std::vector<int> temp,p;
  int aux;
  long trocasTemp;
  p.clear();
  for (unsigned i=1;i<solution.size();++i)
    p.push_back(i);
  random_shuffle(p.begin(),p.end());

  temp.clear();
  temp = solution;
  trocasTemp = trocas;
  for (int i=0;i<p.size();++i){
    aux = temp[p[i]-1];
    temp[p[i]-1] = temp[p[i]];
    temp[p[i]] = aux;
    trocasTemp = KTNS(temp);
    if (trocasTemp<trocas){
      solution.clear();
      solution=temp;
      trocas = trocasTemp;
    } else {
      aux = temp[p[i]-1];
      temp[p[i]-1] = temp[p[i]];
      temp[p[i]] = aux;
    }
  }
}

#endif
