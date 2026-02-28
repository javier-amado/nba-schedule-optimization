#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "gurobi_c++.h"
using namespace std;

const int N = 8; // Número de equipos
const int TOTAL_JORNADAS_REALES = 20;
const int TOTAL_JORNADAS = TOTAL_JORNADAS_REALES + 2; // jornada 0 y jornada 13 ficticias
const int EQUIPOS_POR_DIVISION = 4;
const int NUM_EQUIPOS_CONFERENCIA = 4;

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

        // Restricción: Partidos contra los equipos de la conferencia contraria
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < NUM_EQUIPOS_CONFERENCIA; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_interconf[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += y[i][rival][k]; // partidos de i en la ciudad del rival
                    partidosFuera += y[rival][i][k];  // partidos del rival en la ciudad de i
                    model.addConstr(y[rival][i][k] <= y[i][i][k], "Local_" + to_string(i) + "_" + to_string(k));
                    model.addConstr(y[i][rival][k] <= y[rival][rival][k], "Visitante" + to_string(i) + "_" + to_string(k));
                }

                // Restricción para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra cada uno de los 15 rivales a los que se enfrentan de la otra conferencia
                model.addConstr(partidosEnCasa == 1, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 1, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
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
        //model.setObjective(GRBLinExpr(5), GRB_MINIMIZE);
        model.optimize();

        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            cout << "Solución óptima encontrada:" << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

            // Imprimir el calendario
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                cout << "Jornada " << k + 1 << ":" << endl;
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < N; ++j) {
                        if (i != j && y[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                            cout << equipos[j].nombre << " vs " << equipos[i].nombre << endl;
                        }
                    }
                }
                cout << "-----------------------------" << endl;
            }

            vector<vector<int>> viajes;
            bool encontrado;


            for (int i = 0; i < N; i++) {
                vector<int> recorrido;
                for (int k = 1; k < TOTAL_JORNADAS-1; k++) {
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

            imprimeCalendario(viajes, "Modelo_2_8eq.txt");


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
