#include "datosNBA_4eq.h"

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

vector<vector<int>> copiar_calendario(const vector<vector<int>> & nuevo) {
    return nuevo;
}

void busqueda_tabu(vector<vector<int>>& viajes, double& distancia, int max_iter, int max_tabu_size) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;
    mejor_calendario = copiar_calendario(viajes);

    set<tuple<int, int, int>> lista_tabu;

    for (int iter = 0; iter < max_iter; iter++) {
        double mejor_dist_vec = -numeric_limits<double>::max();
        tuple<int, int, int> mejor_mov;
        vector<vector<int>> mejor_vecino;

        for (int k1 = 0; k1 < TOTAL_JORNADAS; k1++) {
            for (int k2 = k1 + 1; k2 < TOTAL_JORNADAS; k2++) {
                if (lista_tabu.count({ 1,k1,k2 })) {
                    continue;
                }

                double distancia_inicial;
                double distancia_final;
                if (k2 - k1 == 1) {
                    distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);
                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1, viajes) + calculaDistancias_h2(k1, k2, k2 + 1, viajes);
                }
                else {
                    distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);
                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k1, k2 - 1, k2 + 1, viajes);
                }

                
                cambiaJornadas_h2(k1, k2, viajes);
                double diferencia = distancia_inicial - distancia_final;

                
                if (diferencia > mejor_dist_vec) {
                    mejor_dist_vec = diferencia;
                    mejor_vecino = copiar_calendario(viajes);
                    mejor_mov = { 1,k1,k2 };

                }
                cambiaJornadas_h2(k2, k1, viajes);

                

            }
        }

        
        viajes = copiar_calendario(mejor_vecino);
        
        distancia -= mejor_dist_vec;

        lista_tabu.insert(mejor_mov);
        if (lista_tabu.size() > max_tabu_size) {
            lista_tabu.erase(lista_tabu.begin());
        }


        if (distancia < distancia_mejor) {
            mejor_calendario = copiar_calendario(viajes);
            distancia_mejor = distancia;
            
        }
    }

    viajes = copiar_calendario(mejor_calendario);
    distancia = distancia_mejor;
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

    vector<int> m_iter = { 100,200,500 };
    vector<int> t_size = { 10, 50, 100 };

    double dist_mejor = numeric_limits<double>::max();
    vector<vector<int>> mejor_calend;

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distancia;

    ofstream archivo2("resultados_tabu_h2_4eq.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    

    for (int i : m_iter) {
        for (int s : t_size) {
            busqueda_tabu(viajes, distancia, i, s);
            archivo2 << "Max iter = " << i << ", Tabu size = " << s << ", Distancia -> " << distancia << endl;
            cout << "Max iter = " << i << ", Tabu size = " << s << ", Distancia -> " << distancia << endl;
            if (distancia < dist_mejor) {
                dist_mejor = distancia;
                mejor_calend = copiar_calendario(viajes);
            }
            viajes = copiar_calendario(calendario_inicial);
            distancia = dist_inicial;
        }
    }

    archivo2.close(); // Cierra el archivo

    
    imprimeCalendario(mejor_calend, "Tabu_h2_4eq.txt");


    return 0;
}

