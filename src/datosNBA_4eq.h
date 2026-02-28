#ifndef DATOS_NBA_4EQ_H
#define DATOS_NBA_4EQ_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <tuple>
#include <limits>
#include "gurobi_c++.h"

using namespace std;

const int N = 4;
const int TOTAL_JORNADAS = 12;
const int EQUIPOS_POR_DIVISION = 4;

struct InfoEquipo {
    int id;
    string nombre;
    string conferencia;
    string division;
    vector<int> rivales_division;
    vector<int> rivales_conf1;
    vector<int> rivales_conf2;
    vector<int> rivales_conf3;
    vector<int> rivales_interconf;
};

vector<InfoEquipo> equipos = {
    {0, "Boston Celtics", "Este", "Atlántico", {1, 2, 3}, {8, 7, 5, 10, 12}, {9, 13}, {6, 11}, {}},
    {1, "Brooklyn Nets", "Este", "Atlántico", {0, 2, 3}, {13, 6, 9, 10, 5, 11}, {8}, {7, 12}, {}},
    {2, "Philadelphia 76ers", "Este", "Atlántico", {0, 1, 3}, {6, 9, 5, 13, 12, 11}, {7, 10}, {8}, {}},
    {3, "New York Knicks", "Este", "Atlántico", {0, 1, 2}, {7, 8, 11, 13, 6}, {5, 12}, {10, 9}, {}}
};

vector<vector<double>> distanciasNBA = {
    {0, 217.9, 309.3, 213.78},
    {217.9, 0, 98.12, 5.45},
    {309.3, 98.12, 0, 96.67},
    {213.78, 5.45, 96.67, 0},
};

#endif