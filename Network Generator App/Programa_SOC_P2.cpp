#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <math.h>
#include <iomanip>
#include <random>
#include "hashmap_eda.h"

//Separados para los archivos .csv
const char SEPARADOR = ';';

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<long double> dist(0.0, 1.0);

/*
Funcion para escribir los datos de los maps en los archivos .csv

@param dicNodos: map con los nodos de la red
@param dicAristas: map con las aristas de la red
@param nodos: archivo .csv donde escribir los nodos de la red
@param aristas: archivo .csv donde escribir las aristas de la red
*/
void escribirCSVs(unordered_map<int, int> dicNodos, unordered_map<int, std::vector<int>> dicAristas, std::ofstream &nodos, std::ofstream &aristas){
	unordered_map<int, std::vector<int>>::const_iterator itDicAristas = dicAristas.cbegin();
	unordered_map<int, int>::const_iterator itDicNodos = dicNodos.cbegin();

    nodos << "Id" << SEPARADOR << "Label" << SEPARADOR << "\n";
    aristas << "Source" << SEPARADOR << "Target" << SEPARADOR << "Type" << SEPARADOR << "\n";
	while (itDicAristas != dicAristas.cend()){
		std::vector<int> vAristas = itDicAristas->valor;
		for (unsigned int i = 0; i < vAristas.size(); i++){
			std::cout << itDicAristas->clave << " " << vAristas[i] << std::endl;
			aristas << itDicAristas->clave << SEPARADOR << vAristas[i] << SEPARADOR << "Undirected" << SEPARADOR << "\n";
		}
		++itDicAristas;
	}

	while (itDicNodos != dicNodos.cend()){
		nodos << itDicNodos->clave << SEPARADOR << itDicNodos->valor << SEPARADOR << "\n";
		++itDicNodos;
	}
}

//  +---------------------+
//  |     ERDÖS-RENYI     |
//  +---------------------+

// Algoritmo de generacion de la red:
// 1. Crear N nodos aislados
// 2. Seleccionar un par de nodos y generar un número aleatorio entre 0 y 1.
//      Si es menor o igual que p entonces añadimos un enlace entre ellos.
//      En otro caso, los dejamos desconectados.
// 3. Repetir el paso 2 para los N(N−1)/ 2 pares de nodos de la red.

/*
Funcion menu para decidir que tipo de red de Erdos-Renyi se desea generar

@param nodos: numero de nodos que queremos en nuestra red
@return p: probabilidad de que los nodos de nuestra red se unan entre ellos
*/
double probabilidadRedER(int nodos, std::string &tipoER){
	long double p;
	int tipoRed;

	do{
		std::cout << "Selecciona el tipo de red aleatoria: " << std::endl;
		std::cout << "  1. Subcrítica" << std::endl;
		std::cout << "  2. Crítica" << std::endl;
		std::cout << "  3. Supercritica" << std::endl;
		std::cout << "  4. Conectada" << std::endl;
		std::cout << "  5. Introducir probabilidad manualmente" << std::endl;
		std::cout << "  Opcion: ";
		std::cin >> tipoRed;
	} while (tipoRed < 1 || tipoRed > 5);

	switch (tipoRed){

	case 1: //Red Subcritica
		tipoER = "Subcritica";
        p = 1.0 / ((double)nodos + (double)nodos/2);
		break;

	case 2: //Red Critica
		p = 1 / (double)nodos;
		tipoER = "Critica";
		break;

	case 3: //Red Supercritica
		p = 1 / ((double)nodos - (double)nodos/2);
		tipoER = "Supercritica";
		break;

	case 4: //Red Conectada
		p = (log(nodos) / ((double)nodos - (double)nodos/2));
		tipoER = "Conectada";
		break;

	default: //Introduccion manual
		std::cout << "Probabilidad = ";
		std::cin >> p;
		tipoER = "Manual";
	}

	if (tipoRed < 5)
		std::cout << "Probabilidad = " << p << std::endl;

	return p;
}

/*
Generador de la red Erdos-Renyi
*/
void generarER(){
	int nodos;
	long double aleatoriop, p;
	unordered_map<int, std::vector<int>> dicAristas;
	unordered_map<int, int> dicNodos;

	do{
		std::cout << "Introduce el numero de nodos: ";
		std::cin >> nodos;
	} while (nodos <= 0);

	std::string tipoER;
	p = probabilidadRedER(nodos, tipoER);

	std::string stringAristas = "aristas_ER_" + std::to_string(nodos) + "_" + tipoER +".csv";
	std::ofstream archivoAristas(stringAristas);
	std::string stringNodos = "nodos_ER_" + std::to_string(nodos) + "_" + tipoER + ".csv";
	std::ofstream archivoNodos(stringNodos);
	std::vector<int> vNodos(nodos);

	for (int i = 1; i <= nodos; i++){
		vNodos[i - 1] = i;
		dicNodos.insert({ i, i });
	}

	for (int i = 1; i < nodos; i++){
		std::vector<int> v;
		for (int j = i + 1; j <= nodos; j++){
            aleatoriop = dist(mt);
			if (aleatoriop <= p)
				v.push_back(j);
		}
		dicAristas.insert({ i, v });
	}

	escribirCSVs(dicNodos, dicAristas, archivoNodos, archivoAristas);
}

//  +-------------------------+
//  |     BARABASI-ALBERT     |
//  +-------------------------+

/*
Funcion para crear la red inicial a la cual añadiremos los nodos del modelo Barabasi-Albert

@param dicNodos: unordered_map con todos los nodos de la red
@param dicAristas: unordered_map con todas las aristas de la red
@param vGrados: vector donde almacenamos los grados de cada nodo de la red
@param gradoGrafo: grado total del grafo
@param nodostotales: numero total de nodos de la red
*/
void creargrafocompleto(unordered_map<int, int> &dicNodos, unordered_map<int, std::vector<int>> &dicAristas, std::vector<int> &vGrados, double &gradoGrafo, int &nodostotales, int initNodes){
	int contador = 0;
	for (int i = 1; i <= initNodes; i++) {
		dicNodos.insert({ i, i });
		nodostotales++;
	}
	for (int i = 1; i <= initNodes; i++){
		std::vector<int> v;
		for (int j = i + 1; j <= initNodes; j++){
			v.push_back(j);
			contador++;
			vGrados[i]++; vGrados[j]++;
		}
		dicAristas.insert({ i, v });
	}
	gradoGrafo = contador * 2;
}

/*
Funcion para comprobar que ya existe un enlace entre un par de nodos

@param dicAristas: unordered_map que contiene todas las aristas de la red
@param clave: es el nodo que estamos mirando
@param valor: es el conjunto de aristas unidas al vector clave
@return encontrado: true en caso de ya estar unido, false en caso de no estarlo
*/
bool estaUnido(unordered_map<int, std::vector<int>> dicAristas, int clave, int valor){
	bool encontrado = false;
	if (dicAristas.contains(clave)){
		std::vector<int> v = dicAristas.find(clave)->valor;
		unsigned int i = 0;
		while (!encontrado && i < v.size()){
			if (v[i] == valor)
				encontrado = true;
			i++;
		}
	}
	return encontrado;
}

/*
Generador de la red Barabasi-Albert
*/
void generarBA(){
	int initNodes = 0; //numero de nodos iniciales de la red
	int m = 0;  //numero de enlaces de cada nodo
	int t = 0;  //numero de nodos a unir
    long double aleatoriop;
	unordered_map<int, std::vector<int>> dicAristas;
	unordered_map<int, int> dicNodos;

	std::vector<int> vGrados;

	do{
		std::cout << "Nodos nuevos: ";
		std::cin >> t;
		if (t <= 0)
			std::cout << "ERROR: el numero de nuevos nodos debe ser mayor que 0" << std::endl;
	} while (t <= 0);

	do{
		std::cout << "Numero de enlaces de los nuevos nodos: ";
		std::cin >> m;
		if (m <= 0)
			std::cout << "ERROR: el numero de enlaces debe ser mayor que 0 y menor que los nodos iniciales" << std::endl;
	} while (m <= 0);

	initNodes = m + 1;
    t = t - initNodes;

	std::string stringAristas = "aristas_BA_" +std::to_string(t) + "_" + std::to_string(m) + ".csv";
	std::ofstream aristasB(stringAristas);
	std::string stringNodos = "nodos_BA_" +std::to_string(t) + "_" + std::to_string(m) + ".csv";
	std::ofstream nodosB(stringNodos);

	double gradoGrafo = 0;
	int nodostotales = 0;

	for (int i = 0; i < initNodes + t + 1; i++)
		vGrados.push_back(0);

	creargrafocompleto(dicNodos, dicAristas, vGrados, gradoGrafo, nodostotales, initNodes);

	for (int i = initNodes + 1; i <= initNodes + t; i++){
		int maux = m;
		while (maux > 0){
			int j = 1;
			while (j <= nodostotales && maux > 0){
				double gradoNodo = vGrados[j];
				double probabilidad = gradoNodo / gradoGrafo; //grados del nodo al que nos queremos unir / grados del grafo
                do{
                    aleatoriop = dist(mt);
                } while (aleatoriop==0 || aleatoriop==1);


				if (!estaUnido(dicAristas, j, i) && aleatoriop < probabilidad){
					std::cout << "i: " << i << " j: " << j << " aleatorio: " << aleatoriop << " probabilidad: " << probabilidad << '\n';
					std::vector<int> v;
					if (dicAristas.contains(i))
						v = dicAristas.find(i)->valor;
					v.push_back(j);
					dicAristas.insert({ i, v });

					vGrados[j]++; vGrados[i]++;
					maux--;
				}
				j++;
			}
		}
		dicNodos.insert({ i, i });
		int mul = m * 2;
		gradoGrafo += mul;
		nodostotales++;
	}
	escribirCSVs(dicNodos, dicAristas, nodosB, aristasB);
}

/*
Funcion que actua de menu para elegir que tipo de red queremos generar

@return opcion: opcion seleccionada en el menu
*/
int menu(){
	int opcion = 0;
	while (opcion != 1 && opcion != 2){
		std::cout << "Seleccione el tipo de red que desea crear" << std::endl;
		std::cout << "	1. Red Aleatoria (ErdösRenyi)." << std::endl;
		std::cout << "	2. Red de Barabasi-Albert" << std::endl;
		std::cout << "	Opcion: ";
		std::cin >> opcion;
	}
	return opcion;
}

int main(){
    //Random seed init
	//srand((unsigned int)time(0));

	int opcion = menu();

	if (opcion == 1) generarER();
	else if (opcion == 2) generarBA();

	system("pause");
	return 0;
}
