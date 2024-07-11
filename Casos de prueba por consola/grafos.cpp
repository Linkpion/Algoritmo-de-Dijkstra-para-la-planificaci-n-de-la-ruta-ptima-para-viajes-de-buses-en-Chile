//Código principal de la clase de GrafoViajes y funciones asociadas a este
//Código por Cristóbal Artus, Diego Muñoz, Esteban Rojas y Benjamín Villablanca

#include <bits/stdc++.h>
#include "grafos.h"
using namespace std;

//Función de time_point a string, implementado para hacer debug
string time2_point_to_string(const chrono::system_clock::time_point& tp) {
    // Convertir time_point a time_t
    time_t time = std::chrono::system_clock::to_time_t(tp);

    // Convertir time_t a tm
    tm tm = *localtime(&time);

    // Formatear tm a cadena de texto
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

//Lista a partir de un string con un delimitador
vector<int> split(const string& str, char delimiter) {
    vector<int> tokens;
    string token;
    istringstream tokenStream(str);

    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(stoi(token));
    }

    return tokens;
}

//Devuelve el mínimo valor en tiempo que está entre la hora de llegada I y el plazo T
Viaje minimo_tiempo(vector<Viaje> lista,chrono::system_clock::time_point llegada, chrono::system_clock::time_point plazo){
    bool exist=false;
    Viaje minimo;
    minimo.salida = chrono::system_clock::time_point::max() - chrono::hours(1);
    minimo.duracion = chrono::hours(1);

    for(int i=0;i<lista.size();i++){
        if(lista[i].salida>plazo){
            break;
        }
        if(lista[i].salida >= llegada && lista[i].salida + lista[i].duracion<minimo.salida+minimo.duracion){
            minimo=lista[i];
        }
    }
    
    return minimo;
}

//Devuelve el mínimo valor en precio que está entre la hora de llegada I y el plazo T
Viaje minimo(vector<Viaje> lista,chrono::system_clock::time_point llegada, chrono::system_clock::time_point plazo){
    bool exist=false;
    Viaje minimo;
    minimo.precio=INT_MAX;

    for(int i=0;i<lista.size();i++){
        bool test = lista[i].salida>=llegada;
        if(lista[i].salida>plazo){
            break;
        }
        if(lista[i].salida >= llegada && lista[i].precio<minimo.precio){
            minimo=lista[i];
        }
    }
    
    return minimo;
}

//Funcion que devuelve el tiempo de inicio por defecto para las pruebas (23 de Junio del 2024 a las 23:59:59)
chrono::system_clock::time_point default_start(){
    tm fecha1 = {};
    fecha1.tm_year = 2024 - 1900;
    fecha1.tm_mon = 6 - 1;
    fecha1.tm_mday = 23;
    fecha1.tm_hour = 23;
    fecha1.tm_min = 59;
    fecha1.tm_sec = 59;

    return chrono::system_clock::from_time_t(mktime(&fecha1));
}


//Constructor de grafo, inicia ult_cod_ciudad en 0
GrafoViaje::GrafoViaje() : ult_codigo_ciudad(0) {}

//Se ingresan los parámetros como strings para ingresarlos al viaje
void GrafoViaje::ingresarTramo(string origen, string destino, string fecha, string hora, string empresa, string duracion, string precio) {
    
    //Añadir ciudad a map de ciudades si no esta
    if (ciudades.find(origen)==ciudades.end()){
        ciudades[origen] = ult_codigo_ciudad;
        ult_codigo_ciudad ++;
        aristas.resize(ult_codigo_ciudad);
    }
    if(ciudades.find(destino)==ciudades.end()){
        ciudades[destino] = ult_codigo_ciudad;
        ult_codigo_ciudad++;
        aristas.resize(ult_codigo_ciudad);
    }

    //Transformación de fecha y hora a tipo chrono::system_clock::time_point
    vector<int> dia_c = split(fecha,'-');
    vector<int> hora_c = split(hora,':');

    
    tm fecha1 = {};
    fecha1.tm_year = dia_c[2] - 1900;
    fecha1.tm_mon = dia_c[1] - 1;
    fecha1.tm_mday = dia_c[0];
    fecha1.tm_hour = hora_c[0];
    fecha1.tm_min = hora_c[1];
    fecha1.tm_sec = 0;

    time_t tiempo_salida = mktime(&fecha1);
    if (tiempo_salida == -1) {
        cerr << "Error: mktime fallo al convertir la fecha y hora" << endl;
        return;
    }
    chrono::system_clock::time_point salida = chrono::system_clock::from_time_t(tiempo_salida);


    //Creacion de estructura de empresa
    Empresa empresa_viaje;
    empresa_viaje.Nombre = empresa;

    vector<int> duracion_c = split(duracion, ':');

    //Objeto de viaje
    Viaje viaje;
    viaje.origen = origen;
    viaje.destino = destino;
    viaje.empresa = empresa_viaje;
    viaje.duracion = chrono::hours(duracion_c[0]) + chrono::minutes(duracion_c[1]);
    viaje.precio = stoi(precio);
    viaje.salida = salida;
    
    //Añadido al grafo
    aristas[ciudades[origen]][destino].push_back(viaje);
}

//Regresa la cantidad de ciudades actuales del grafo
int GrafoViaje::cant_ciudades() const {
    return ciudades.size();
}

//Regresa la cantidad de aristas del grafo
int GrafoViaje::cant_aristas() const {
    int num_cities = aristas.size();
    int aristas_num=0;
    for(int i=0;i<num_cities;i++){
        aristas_num = aristas_num + aristas[i].size();
    }
    return aristas_num;
}

//Dijkstra de reducción de timepo
pair<int,vector<Viaje>> GrafoViaje::dijkstra_price(string origen, string destino, chrono::system_clock::time_point I = default_start(), chrono::hours T = chrono::hours(24)) {
    if(ciudades.find(origen)!=ciudades.end() && ciudades.find(destino)!=ciudades.end()){
        //Ciudades recorridas y cola de prioridad
        vector<bool> visitado(this->cant_ciudades(),false);
        priority_queue<pair<int, vector<Viaje>>, vector<pair<int, vector<Viaje>>>, greater<pair<int, vector<Viaje>>>> cola;

        //Condiciones iniciales de cola con el origen
        map<string,vector<Viaje>> viajes = aristas[ciudades[origen]];
        for (auto it = viajes.begin(); it != viajes.end(); it++){
            chrono::system_clock::time_point defecto = I;
            chrono::system_clock::time_point plazo = defecto + T;
            Viaje minimo_viaje = minimo(it->second,defecto,plazo);

            if(minimo_viaje.precio < INT_MAX){
                vector<Viaje> tramo;
                tramo.push_back(minimo_viaje);
                cola.push(make_pair(minimo_viaje.precio,tramo));
            }
        }

        //Vector actual
        pair<int,vector<Viaje>> top;
        string actual;

        //Visita de cada nodo 
        while(!cola.empty()){
            //Primeros valores de la cola de prioridad
            top = cola.top();cola.pop();
            actual = top.second.back().destino;

            //Ciudad de destino encontrada
            if(actual == destino){
                return top;
            }
            
            int cod_ciudad = ciudades[actual];
            //Si la ciudad no se ha visitado
            if(!visitado[cod_ciudad]){

                //Se rescata la lista de viajes de cada ciudad posible de destino
                visitado[cod_ciudad] = true;
                map<string,vector<Viaje>> viajes = aristas[cod_ciudad];

                //Recorrido por ciudad de destino
                for (auto it = viajes.begin(); it != viajes.end(); it++){
                    vector<Viaje> lista= it->second;
                    Viaje minimo_precio;
                    auto llegada = top.second.back().salida + top.second.back().duracion;
                    auto plazo = llegada + T;

                    minimo_precio = minimo(lista,llegada,plazo);
                    
                    if(minimo_precio.precio < INT_MAX){
                        vector<Viaje> recorrido(top.second);
                        recorrido.push_back(minimo_precio);
                        cola.push(make_pair(top.first+minimo_precio.precio,recorrido));
                    }   
                }  
            }  
        };
        //No existe un camino posible
        return make_pair(-2,vector<Viaje>{});

    }
    //La ciudad de origen o de destino no existe
    return make_pair(-1,vector<Viaje>{});
}

//Versión que minimiza la hora de llegada
pair<chrono::system_clock::time_point,vector<Viaje>> GrafoViaje::dijkstra_time(string origen, string destino, chrono::system_clock::time_point I = default_start(), chrono::hours T = chrono::hours(24)){

        if(ciudades.find(origen)!=ciudades.end() && ciudades.find(destino)!=ciudades.end()){

        //Ciudades recorridas y cola de prioridad
        vector<bool> visitado(this->cant_ciudades(),false);
        priority_queue<pair<chrono::system_clock::time_point, vector<Viaje>>, vector<pair<chrono::system_clock::time_point, vector<Viaje>>>, greater<pair<chrono::system_clock::time_point, vector<Viaje>>>> cola;

        //Condiciones iniciales de cola con el origen
        map<string,vector<Viaje>> viajes = aristas[ciudades[origen]];
        for (auto it = viajes.begin(); it != viajes.end(); it++){
            //cout<<it->first<<endl;
            chrono::system_clock::time_point defecto = I;
            chrono::system_clock::time_point plazo = defecto + T;
            Viaje minimo_viaje = minimo_tiempo(it->second,defecto,plazo);

            if(minimo_viaje.salida < chrono::system_clock::time_point::max()-chrono::hours(1)){
                //cout<<minimo_viaje.destino<<endl;
                vector<Viaje> tramo;
                tramo.push_back(minimo_viaje);
                cola.push(make_pair(minimo_viaje.salida+minimo_viaje.duracion,tramo));
            }
        }

        //Vector actual
        pair<chrono::system_clock::time_point,vector<Viaje>> top;
        string actual;

        while(!cola.empty()){
            top = cola.top();cola.pop();
            actual = top.second.back().destino;

            //Ciudad de destino encontrada
            if(actual == destino){
                return top;
            }
            
            int cod_ciudad = ciudades[actual];
            //Si la ciudad no se ha visitado
            if(!visitado[cod_ciudad]){

                //Se rescata la lista de viajes de cada ciudad posible de destino
                visitado[cod_ciudad] = true;
                map<string,vector<Viaje>> viajes = aristas[cod_ciudad];

                //Recorre todos los nodos que conectan
                for (auto it = viajes.begin(); it != viajes.end(); it++){
                    vector<Viaje> lista= it->second;
                    Viaje minimo_precio;
                    auto llegada = top.second.back().salida + top.second.back().duracion;
                    auto plazo = llegada + T;

                    minimo_precio = minimo(lista,llegada,plazo);
                    
                    if(minimo_precio.precio < INT_MAX){
                        vector<Viaje> recorrido(top.second);
                        recorrido.push_back(minimo_precio);
                        cola.push(make_pair(minimo_precio.salida+minimo_precio.duracion,recorrido));
                    }   
                }  
            }  
        };
        //No existe un camino posible
        return make_pair(chrono::system_clock::time_point::min()+chrono::seconds(1),vector<Viaje>{});

    }
    //La ciudad de origen o de destino no existe
    return make_pair(chrono::system_clock::time_point::min(),vector<Viaje>{});
}

//Regresa la propuesta de viaje directo en las condiciones del paper, ya sea origen o destino
Viaje GrafoViaje::mejor_directo(string origen, string destino, int tipo){
    if(ciudades.find(origen)!=ciudades.end() && ciudades.find(destino)!=ciudades.end()){
        int cod_origen = ciudades[origen];
        vector<Viaje> posibilidades = aristas[cod_origen][destino];

        if(tipo==1){
            Viaje escogido = minimo(posibilidades,default_start(),default_start()+chrono::hours(24));
            if(escogido.precio < INT_MAX){
                return escogido;
            }

            Viaje notfound;
            notfound.origen = "notfound";
            return notfound;

        }
        else if(tipo==2){
            Viaje escogido = minimo_tiempo(posibilidades,default_start(),default_start()+chrono::hours(24));
            if(escogido.salida < chrono::system_clock::time_point::max()-chrono::hours(1)){
                return escogido;
            }
            
            Viaje notfound;
            notfound.origen = "notfound";
            return notfound;
        }

    }
    Viaje notfound;
    notfound.origen = "notfound";
    return notfound;
}

//Lista de ciudades
vector<string> GrafoViaje::lista_ciudades() const{
    vector<string> res;
    for(auto it = ciudades.begin();it!=ciudades.end();it++){
        res.push_back(it->first);
    }
    return res;
}