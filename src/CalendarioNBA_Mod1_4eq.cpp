#include <vector>
#include <string>
#include <iostream>
#include <fstream>  
#include "gurobi_c++.h"

using namespace std;

const int N = 4; // Número de equipos
const int TOTAL_JORNADAS = 12; // jornada 0 y jornada 13 ficticias
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

int main() {

    try {

        // Inicializar el entorno de Gurobi
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "nba_schedule.log");
        env.start();

        // Crear el modelo
        GRBModel model = GRBModel(env);

        // Variables binarias: x[i][j][k] donde i es el equipo local, j es el visitante y k es la jornada
        GRBVar x[N][N][TOTAL_JORNADAS];
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (i != j) {
                    for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                        x[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k));
                    }
                }
            }
        }



        // Restricción: Cada equipo solo puede jugar un partido por jornada
        for (int i = 0; i < N; ++i) {
            for (int k = 0; k < TOTAL_JORNADAS; k++) {

                // Inicializar contadores para ver la cantidad de partidos que juega cada equipo por jornada
                GRBLinExpr partidosPorJornada = 0;

                for (int j = 0; j < N; ++j) {
                    if (i != j) {
                        partidosPorJornada += x[i][j][k]; // partidos de i en casa contra j 
                        partidosPorJornada += x[j][i][k];  // partidos de i fuera de casa contra j
                    }

                }

                // Restricción para que cada equipo juegue 1 partido por jornada
                model.addConstr(partidosPorJornada == 1, "UnPartidoPorJornada_" + to_string(i) + "_Jornada_" + to_string(k));
            }
        }



        // Restricción: Partidos dentro de la división
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < EQUIPOS_POR_DIVISION - 1; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_division[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                    partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                }

                // Restricción para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra un rival de la misma división que ellos
                model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
            }
        }



        // Función objetivo vacía (solo generar el calendario)
        model.setObjective(GRBLinExpr(5), GRB_MINIMIZE);

        // Optimizar el modelo
        model.optimize();

        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            cout << "Solución óptima encontrada:" << std::endl;

            // Imprimir el calendario
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                cout << "Jornada " << k + 1 << ":" << endl;
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < N; ++j) {
                        if (i != j && x[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                            cout << equipos[i].nombre << " vs " << equipos[j].nombre << endl;
                        }
                    }
                }

                cout << "-----------------------------" << endl;
            }

        }
        else {
            cout << "No se encontró una solución óptima." << std::endl;
        }

    }
    catch (GRBException e) {
        cout << "Error: " << e.getMessage() << endl;
    }
    catch (...) {
        cout << "Error desconocido" << endl;
    }
    return 0;
}

