//Declaración de estructuras y clases del proyecto
//Código por Cristóbal Artus, Diego Muñoz, Esteban Rojas y Benjamín Villablanca
#include <bits/stdc++.h>
using namespace std;
#ifndef GRAFOS_H
#define GRAFOS_H

struct Empresa {
    string Nombre;
};

struct Viaje {
    string origen;
    string destino;
    chrono::system_clock::time_point salida;
    Empresa empresa;
    chrono::seconds duracion;
    int precio;

    //Operador comparador de Viajes, necesario para la cola de prioridad
    bool operator<(const Viaje& other) const {
        if(precio!=other.precio){
            return precio<other.precio;
        }
        if(salida!=other.salida){
            return salida<other.salida;
        }
        return duracion<other.duracion;
    }
};

//Objeto del grafo
class GrafoViaje {
public:
    GrafoViaje();

    void ingresarTramo(string origen, string destino, string fecha, string hora, string empresa, string duracion, string precio);

    int cant_ciudades() const;

    int cant_aristas() const;

    pair<int,vector<Viaje>> dijkstra_price(string origen, string destino, chrono::system_clock::time_point I = default_start(), chrono::hours T = chrono::hours(24));

    pair<chrono::system_clock::time_point,vector<Viaje>> dijkstra_time(string origen, string destino, chrono::system_clock::time_point I = default_start(), chrono::hours T = chrono::hours(24));

    Viaje mejor_directo(string origen, string destino, int tipo);

    vector<string> lista_ciudades() const;
private:
    //Diccionario de ciudades con su código, vector que registra las aristas y el codigo asignado a la última ciudad registrada
    map<string,int> ciudades;
    vector<map<string,vector<Viaje>>> aristas;
    int ult_codigo_ciudad;
};

#endif