#include "datosNBA.h"


// ----------------------Funciones Heurística 2---------------------------------
double calculaDistancias_h2(int k, int kantes, int kdespues, const vector<vector<int>>& viajes) {
    double total_dist = 0;
    for (int i = 0; i < N; i++) {
        if (kantes >= 0 && kdespues <= TOTAL_JORNADAS - 1) {
            total_dist += distanciasNBA[viajes[i][kantes]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][kdespues]];
        }
        else if (kantes < 0) {
            total_dist += distanciasNBA[viajes[i][k]][viajes[i][kdespues]] + distanciasNBA[viajes[i][k]][i];
        }
        else if (kdespues > TOTAL_JORNADAS - 1) {
            total_dist += distanciasNBA[viajes[i][kantes]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
        }
    }

    return total_dist;
}

void cambiaJornadas_h2(int k1, int k2, vector<vector<int>>& viajes) {
    for (int i = 0; i < N; i++) {
        int aux = viajes[i][k1];
        viajes[i][k1] = viajes[i][k2];
        viajes[i][k2] = aux;
    }
}

int calculaRival(const vector<vector<int>> viajes,int jornada, int rival) {
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
    for (int k = 0; k < TOTAL_JORNADAS; k++) {
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
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
            distancia_eq += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
        }
        dist_eq.push_back(distancia_eq);
    }

    for (int i = 0; i < N; i++) {
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
        dist_eq[i] += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
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


    double disteste = 0;
    double distoeste = 0;

    for (int i = 0; i < N; i++) {
        if (i < 15) {
            disteste += dist_eq[i];
        }
        else {
            distoeste += dist_eq[i];
        }
    }

    archivo << "Media de millas recorridas Conferencia Este: " << disteste / 15 << " Millas Totales: " << disteste << endl;
    archivo << "Media de millas recorridas Conferencia Oeste: " << distoeste / 15 << " Millas Totales: " << distoeste << endl;


    archivo.close();
}

bool comprueba_balance_lyv(const vector<vector<int>>& viajes) {

    for (int i = 0; i < N; i++) {
        int local = 0;
        int visitante = 0;
        for (int k = 0; k < viajes[i].size(); k++) {
            if (viajes[i][k] != -1) {
                if (viajes[i][k] == i) {
                    local++;
                }
                else {
                    visitante++;
                }
            }

            if (abs(visitante - local) > 10) {
                return false;
            }
        }
    }


    return true;
}


int main() {

    ifstream archivo("calendario_balanceado_lyv.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    vector<vector<int>> viajes;
    for (int i = 0; i < N; i++) {
        vector<int> recorrido;
        int n;
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            archivo >> n;
            recorrido.push_back(n);
        }
        viajes.push_back(recorrido);
    }

    archivo.close(); // Cierra el archivo

    //-----------------Distancia que genera el modelo--------------------------

    double distancia = 0;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
        }
    }
    for (int i = 0; i < N; i++) {
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
    }

    cout << "Distancia inicial: " << distancia << endl;

    int j1 = 0;
    double diferencia = 0;
    double distancia_inicial = 0;
    double distancia_final = 0;
    while (j1 < TOTAL_JORNADAS) {
        int j2 = j1 + 1;
        while (j2 < TOTAL_JORNADAS) {
            distancia_inicial = calculaDistancias_h2(j1, j1 - 1, j1 + 1, viajes) + calculaDistancias_h2(j2, j2 - 1, j2 + 1, viajes);
            if (j2 - j1 == 1) {
                distancia_final = calculaDistancias_h2(j2, j1 - 1, j1, viajes) + calculaDistancias_h2(j1, j2, j2 + 1, viajes);
            }
            else {
                distancia_final = calculaDistancias_h2(j2, j1 - 1, j1 + 1, viajes) + calculaDistancias_h2(j1, j2 - 1, j2 + 1, viajes);
            }

            cambiaJornadas_h2(j1, j2, viajes);
            if (distancia_final < distancia_inicial && comprueba_balance_lyv(viajes)) {
                diferencia = distancia_inicial - distancia_final;

                distancia -= diferencia;

                j1 = -1;
                break;
            }
            else {
                cambiaJornadas_h2(j2, j1, viajes);
            }
            j2++;
        }
        j1++;
    }

    cout << "Distancia final: " << distancia << endl;

    imprimeCalendario(viajes, "Heuristica_2.txt");
   
    return 0;
}

