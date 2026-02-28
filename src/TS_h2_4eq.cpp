#include "datosNBA_4eq.h"


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
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS - 1]];
        dist_eq[i] += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS - 1]];
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

vector<vector<int>> copiar_calendario(const vector<vector<int>> &nuevo) {
    return nuevo;
}

void temple_simulado(vector<vector<int>>& viajes, double& distancia, double t_inicial, double t_minima, int M, double alpha) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;

    mejor_calendario = copiar_calendario(viajes);

    double distancia_nueva = 0;
    double distancia_inicial;
    double distancia_final;
    while (t_inicial > t_minima) {
        int M_act = M;
        if (t_inicial < 60 && t_inicial > 25) {
            M_act = M / 2;
        }
        else if (t_inicial < 25) {
            M_act = 1;
        }
        for (int i = 0; i < M_act; i++) {
            int k1 = rand() % TOTAL_JORNADAS;
            int k2 = rand() % TOTAL_JORNADAS;
            while (k1 >= k2) {
                k1 = rand() % TOTAL_JORNADAS;
                k2 = rand() % TOTAL_JORNADAS;
            }

            distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);

            if (k1 < k2) {
                if (k2 - k1 == 1) {

                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1, viajes) + calculaDistancias_h2(k1, k2, k2 + 1, viajes);
                }
                else {

                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k1, k2 - 1, k2 + 1, viajes);
                }
            }

            cambiaJornadas_h2(k1, k2, viajes);
            distancia_nueva = distancia_final - distancia_inicial;
            distancia += distancia_nueva;
            double dist = distancia;

        }

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


                if (distancia_final < distancia_inicial) {
                    diferencia = distancia_inicial - distancia_final;

                    distancia -= diferencia;
                    cambiaJornadas_h2(j1, j2, viajes);
                    j1 = -1;
                    break;
                }
                j2++;
            }
            j1++;
        }


        if (distancia < distancia_mejor) {
            distancia_mejor = distancia;
            mejor_calendario = copiar_calendario(viajes);
            
        }


        t_inicial *= alpha;

    }

    distancia = distancia_mejor;
    viajes = copiar_calendario(mejor_calendario);
}




int main() {

    ifstream archivo("calendario_4eq.txt"); // Abre el archivo en modo lectura

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
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS-1]];
    }

    cout << "Distancia inicial dada por el modelo: " << distancia << endl;

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distancia;

    vector<double> t = { 50, 100 };
    //double t_inicial = 100;
    double t_minimo = 0.01;
    vector<int> Ms = { 2, 3, 4, 5, 6, 8, 10 };
    vector<double> alphas = { 0.90, 0.95, 0.99 };

    ofstream archivo2("resultados_ts_h2_4eq.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    double dist_mejor = numeric_limits<double>::max();
    vector<vector<int>> mejor_calend;

    for (double t_inicial : t) {
        for (int M : Ms) {
            for (double alpha : alphas) {
                temple_simulado(viajes, distancia, t_inicial, t_minimo, M, alpha);
                archivo2 << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distancia << endl;
                cout << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distancia << endl;
                if (distancia < dist_mejor) {
                    dist_mejor = distancia;
                    mejor_calend = copiar_calendario(viajes);
                }
                viajes = copiar_calendario(calendario_inicial);
                distancia = dist_inicial;
            }
        }
    }

    archivo2.close(); // Cierra el archivo

    imprimeCalendario(mejor_calend, "TS_h2_4eq.txt");


    return 0;
}

