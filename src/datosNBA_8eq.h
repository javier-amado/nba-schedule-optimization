#ifndef DATOS_NBA_8EQ_H
#define DATOS_NBA_8EQ_H

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

const int N = 8; 
const int TOTAL_JORNADAS = 20; 
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
    {0, "Boston Celtics", "Este", "Atlántico", {1, 2, 3}, {8, 7, 5, 10, 12}, {9, 13}, {6, 11}, {4,5,6,7}},
    {1, "Brooklyn Nets", "Este", "Atlántico", {0, 2, 3}, {13, 6, 9, 10, 5, 11}, {8}, {7, 12}, {4,5,6,7}},
    {2, "Philadelphia 76ers", "Este", "Atlántico", {0, 1, 3}, {6, 9, 5, 13, 12, 11}, {7, 10}, {8}, {4,5,6,7}},
    {3, "New York Knicks", "Este", "Atlántico", {0, 1, 2}, {7, 8, 11, 13, 6}, {5, 12}, {10, 9}, {4,5,6,7}},
    {4,"Oklahoma City Thunder", "Oeste", "Noroeste", {5, 6, 7}, {28, 26, 29, 21, 24, 20}, {23, 25}, {27, 22} , {0, 1, 2, 3} },
    {5,"Minnesota Timberwolves", "Oeste", "Noroeste", {4, 6, 7}, {28, 27, 29, 21, 24, 22}, {26, 20}, {25, 23} , {0, 1, 2, 3} },
    {6,"Utah Jazz", "Oeste", "Noroeste", {4, 5, 7}, {26, 25, 27, 21, 22, 23}, {28, 24}, {29, 20} , {0, 1, 2, 3} },
    {7,"Denver Nuggets", "Oeste", "Noroeste", {4, 5, 6}, {25, 26, 29, 24, 23, 20}, {27, 22}, {28, 21} , {0, 1, 2, 3} }
};

vector<vector<double>> distanciasNBA = {
    {0, 217.9, 309.3, 213.78, 1689.49, 1399.98, 2365.09, 1969.76},
    {217.9, 0, 98.12, 5.45, 1467.56, 1209, 2174.12, 1778.79},
    {309.3, 98.12, 0, 96.67, 1383.31, 1175.94, 2141.06, 1726.81},
    {213.78, 5.45, 96.67, 0, 1451.06, 1208.36, 2173.39, 1777.98},
    {1689.49, 1467.56, 1383.31, 1451.06, 0, 789.32, 1188.33, 677.57},
    {1399.98, 1209, 1175.94, 1208.36, 789.32, 0, 1247.22, 914.03},
    {2365.09, 2174.12, 2141.06, 2173.39, 1188.33, 1247.22, 0, 519.27},
    {1969.76, 1778.79, 1726.81, 1777.98, 677.57, 914.03, 519.27, 0},
};

#endif