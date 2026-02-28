#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "gurobi_c++.h"
using namespace std;

const int N = 4; // Número de equipos
const int TOTAL_JORNADAS_REALES = 12;
const int TOTAL_JORNADAS = TOTAL_JORNADAS_REALES + 2; // jornada 0 y jornada 13 ficticias
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

int calculaRival(const vector<vector<int>> viajes, int jornada, int rival) {
    bool encontrado = false;
    int ind = -1;
    for (int i = 0; i < N && !encontrado; i++) {
        if (viajes[i][jornada] == rival && i != rival) {
            ind = i;
            encontrado = true;
        }
    }

    return ind;
}

void imprimeCalendario(const vector<vector<int>> viajes, string n_archivo) {
    ofstream archivo(n_archivo);

    if (!archivo) {
        cerr << "Error al abrir el archivo" << std::endl;
    }
    for (int k = 0; k < TOTAL_JORNADAS_REALES; k++) {
        archivo << "Jornada " << k + 1 << ":" << endl;
        for (int i = 0; i < N; i++) {
            if (viajes[i][k] == i) {
                int rival = calculaRival(viajes, k, i);
                archivo << equipos[i].nombre << " vs " << equipos[rival].nombre << endl;
            }
        }
        archivo << "----------------------" << endl;
    }

    double distancia = 0;
    vector<double> dist_eq;
    for (int i = 0; i < N; i++) {
        double distancia_eq = 0;
        for (int k = 0; k < TOTAL_JORNADAS_REALES - 1; k++) {
            distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
            distancia_eq += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
        }
        dist_eq.push_back(distancia_eq);
    }

    for (int i = 0; i < N; i++) {
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS_REALES - 1]];
        dist_eq[i] += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS_REALES - 1]];
    }

    archivo << "Distancia final: " << distancia << endl;

    double maximo = -numeric_limits<double>::max();
    double minimo = numeric_limits<double>::max();
    int idxmin = -1;
    int idxmax = -1;

    for (int i = 0; i < N; i++) {
        archivo << equipos[i].nombre << ": " << dist_eq[i] << " millas recorridas" << endl;
        if (dist_eq[i] > maximo) {
            maximo = dist_eq[i];
            idxmax = i;
        }
        if (dist_eq[i] < minimo) {
            minimo = dist_eq[i];
            idxmin = i;
        }
    }

    archivo << "Equipo con menor distancia recorrida: " << equipos[idxmin].nombre << " con " << dist_eq[idxmin] << " millas" << endl;
    archivo << "Equipo con mayor distancia recorrida: " << equipos[idxmax].nombre << " con " << dist_eq[idxmax] << " millas" << endl;

    archivo.close();
}


int main() {
    try {
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "nba_schedule.log");
        env.start();

        GRBModel model = GRBModel(env);


        // Variables y[i][j][k]: Indica si el equipo i está en la ciudad j durante la jornada k
        GRBVar y[N][N][TOTAL_JORNADAS];
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    y[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k));
                }
            }
        }


        // Variables z[i][j][j'][k]: Indica si el equipo i viaja de j a j' entre jornadas k-1 y k
        GRBVar z[N][N][N][TOTAL_JORNADAS - 1];
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int j2 = 0; j2 < N; ++j2) {
                    for (int k = 1; k < TOTAL_JORNADAS; ++k) {
                        z[i][j][j2][k - 1] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z_" + to_string(i) + "_" + to_string(j) + "_" + to_string(j2) + "_Jornada_" + to_string(k));
                    }
                }
            }
        }

        // Todos en casa en jornada 0 y jornada final
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                model.addConstr(y[i][j][0] == (i == j ? 1 : 0));
                model.addConstr(y[i][j][TOTAL_JORNADAS - 1] == (i == j ? 1 : 0));
            }
        }
            

        // Restricción: Cada equipo está en una ciudad por jornada
        for (int i = 0; i < N; ++i) {
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                GRBLinExpr sum_y = 0;
                for (int j = 0; j < N; ++j) {
                    sum_y += y[i][j][k];
                }
                model.addConstr(sum_y == 1, "UnicaCiudad_" + to_string(i) + "_Jornada_" + to_string(k));
            }
        }

        // Restricción: No más de un equipo visitante en la misma ciudad por jornada
        for (int j = 0; j < N; ++j) {  // Ciudad j
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {  // Jornada k
                GRBLinExpr visitantesEnCiudad = 0;
                for (int i = 0; i < N; ++i) {  // Equipos i
                    if (i != j) {
                        visitantesEnCiudad += y[i][j][k];  // Cuenta cuántos equipos están en j en k
                    }
                }
                model.addConstr(visitantesEnCiudad <= 1, "MaxUnVisitante_" + to_string(j) + "_Jornada_" + to_string(k));
            }
        }

        // Relación entre y y z: Si viajan entre ciudades
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int j2 = 0; j2 < N; ++j2) {
                    for (int k = 1; k < TOTAL_JORNADAS; ++k) {
                        model.addConstr(z[i][j][j2][k - 1] <= y[i][j][k - 1], "RestriccionViaje1_" + to_string(i) + "_" + to_string(j) + "_" + to_string(j2) + "_Jornada_" + to_string(k));
                        model.addConstr(z[i][j][j2][k - 1] <= y[i][j2][k], "RestriccionViaje2_" + to_string(i) + "_" + to_string(j) + "_" + to_string(j2) + "_Jornada_" + to_string(k));
                        model.addConstr(z[i][j][j2][k - 1] >= y[i][j][k - 1] + y[i][j2][k] - 1, "RestriccionViaje3_" + to_string(i) + "_" + to_string(j) + "_" + to_string(j2) + "_Jornada_" + to_string(k));
                    }
                }
            }
        }
            
            
                    

        // Partidos dentro de la división: 2 en casa y 2 fuera
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < EQUIPOS_POR_DIVISION - 1; ++j) {
                int rival = equipos[i].rivales_division[j];
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosFuera += y[i][rival][k];
                    partidosEnCasa += y[rival][i][k];
                    model.addConstr(y[rival][i][k] <= y[i][i][k]);
                    model.addConstr(y[i][rival][k] <= y[rival][rival][k]);
                }
                model.addConstr(partidosEnCasa == 2);
                model.addConstr(partidosFuera == 2);
            }
        }

        // Función objetivo: Minimizar distancias
        GRBLinExpr distanciaTotal = 0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int j2 = 0; j2 < N; ++j2) {
                    for (int k = 0; k < TOTAL_JORNADAS - 1; ++k) {
                        distanciaTotal += z[i][j][j2][k] * distanciasNBA[j][j2];
                    }
                }
            }
        }

        model.setObjective(distanciaTotal, GRB_MINIMIZE);
        model.optimize();

        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            
            vector<vector<int>> viajes;
            bool encontrado;


            for (int i = 0; i < N; i++) {
                vector<int> recorrido;
                for (int k = 1; k < TOTAL_JORNADAS - 1; k++) {
                    encontrado = false;
                    for (int j = 0; j < N && !encontrado; j++) {
                        if (y[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                            recorrido.push_back(j);
                            encontrado = true;
                        }
                    }
                }

                viajes.push_back(recorrido);

            }

            imprimeCalendario(viajes, "Modelo_2_4eq.txt");


        }
        else {
            cout << "No se encontró una solución óptima." << std::endl;
        }

    }
    catch (GRBException e) {
        cout << "Error de Gurobi: " << e.getMessage() << endl;
    }
    catch (...) {
        cout << "Otro error" << endl;
    }

    return 0;
}
