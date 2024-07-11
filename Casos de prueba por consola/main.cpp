//Código main hecho para los casos de prueba
//Código por Cristóbal Artus, Diego Muñoz, Esteban Rojas y Benjamín Villablanca
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "grafos.h"

using namespace std;

//Generar el grafo a partir del archivo de entrada
GrafoViaje genGrafo(string file){
    string linea;
    char delimitador = ',';
    GrafoViaje grafo;

    ifstream archivo(file);

    

    if(archivo.is_open()){
        getline(archivo,linea);
        int viajes=0;
        int santiago=0;
        while(getline(archivo,linea)){
            
            stringstream stream(linea);
            string origen, destino, empresa, fecha, hora, duracion, precio;
            
            getline(stream, origen, delimitador);
            getline(stream, destino, delimitador);
            getline(stream, empresa, delimitador);
            getline(stream, fecha, delimitador);
            getline(stream, hora, delimitador);
            getline(stream, duracion, delimitador);
            getline(stream, precio, delimitador);

            
            if(duracion[3]!='-'){
                viajes++;
                grafo.ingresarTramo(origen,destino,fecha,hora,empresa,duracion,precio);
                if(origen=="Santiago"||destino=="Santiago"){
                    santiago++;
                }
                
            }
        }
        cout<<viajes<<endl;
        cout<<santiago<<endl;
        archivo.close();
        return grafo;
    }
    else {
        cerr << "No se pudo abrir el archivo" << endl;
        return grafo;
    }

}

//Time point a string
string time_point_to_string(const chrono::system_clock::time_point& tp) {
    // Convertir time_point a time_t
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *localtime(&time);

    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

//Segundos a string en formato hhmmss
string seconds_to_hhmmss(const chrono::seconds& duration) {
    
    auto seconds = duration.count();

    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;

    ostringstream oss;
    oss << setw(2) << setfill('0') << hours << ":"
        << setw(2) << setfill('0') << minutes << ":"
        << setw(2) << setfill('0') << seconds;
    return oss.str();
}

void printViaje(Viaje viaje){
    cout<<"Origen:"<<viaje.origen;
    cout<<" Destino:"<<viaje.destino;
    cout<<" Empresa:"<<viaje.empresa.Nombre;
    cout<<" Salida:"<<time_point_to_string(viaje.salida);
    cout<<" Duracion:"<<seconds_to_hhmmss(viaje.duracion);
    cout<<" Precio:$"<<viaje.precio<<endl;
}

int main(){
    
    GrafoViaje grafo = genGrafo("DataBase1.csv");
    
    cout<<grafo.cant_ciudades()<<endl;
    cout<<grafo.cant_aristas()<<endl;
    
    //Codigo para guardar en el archivolos casos de prueba, comentar hasta abajo en caso de querer hacer pruebas por consola
    vector<string> ciudades = grafo.lista_ciudades();

    string nombreArchivo = "resultados.csv";
    ofstream archivo;
    archivo.open(nombreArchivo.c_str(), fstream::out);

    archivo << "recorridos_precio,dijkstra_precio,tiempo_ejecucion_precio,recorrido_tiempo,dijkstra_tiempo,tiempo_ejecucion_tiempo" << endl;

    for(int i=0;i<ciudades.size();i++){
        cout<<ciudades[i]<<endl;
        for(int j=0;j<ciudades.size();j++){
            string origen=ciudades[i];
            string destino=ciudades[j];
            if(origen!=destino){
                
                //Precio
                Viaje propuesta = grafo.mejor_directo(origen,destino,1);
                if(propuesta.origen=="notfound"){
                    archivo<<-1<<',';
                } else {
                    archivo<<propuesta.precio<<',';
                }

                auto start = chrono::high_resolution_clock::now();
                pair<int,vector<Viaje>> resultado = grafo.dijkstra_price(origen,destino);
                auto end = chrono::high_resolution_clock::now();

                if(resultado.second.size()==0){
                    archivo<<-1<<',';
                } else{
                    archivo<<resultado.first<<',';
                }
                std::chrono::duration<double, std::micro> duration = end - start;
                archivo<<duration.count()<<',';

                //Tiempo
                propuesta = grafo.mejor_directo(origen,destino,2);
                if(propuesta.precio==2147483647){
                    archivo<<-1<<',';
                } else {
                    archivo<<time_point_to_string(propuesta.salida+propuesta.duracion)<<',';
                }

                start = chrono::high_resolution_clock::now();
                pair<chrono::system_clock::time_point,vector<Viaje>> resultado2 = grafo.dijkstra_time(origen,destino);
                end = chrono::high_resolution_clock::now();

                if(resultado2.second.size()==0){
                    archivo<<-1<<',';
                } else{
                    archivo<<time_point_to_string(resultado2.first)<<',';
                }

                duration = end - start;
                archivo<<duration.count()<<endl;
            }
        }
    }

    archivo.close();
    cout << "Escrito correctamente";
    //COMENTAR HASTA ACÁ PARA PRUEBAS POR CONSOLA

    //Pruebas de viajes por consola
    //Dejar comentado para casos de prueba
    
    /*
    string origen,destino;

    cout<<"Ingresa la ciudad de origen"<<endl;
    getline(cin,origen);
    cout<<"Ingresa la ciudad de destino"<<endl;
    getline(cin,destino);

    auto start = std::chrono::high_resolution_clock::now();
    pair<int,vector<Viaje>> resultado = grafo.dijkstra_price(origen,destino);
    auto end = std::chrono::high_resolution_clock::now();

    cout<<"Resultados:"<<endl;

    if(resultado.first>=0){
        for(int i=0;i<resultado.second.size();i++){
            printViaje(resultado.second[i]);
        }
    } else{
        if(resultado.first==-1){
            cout<<"ERROR: La ciudad de origen o destino no existen"<<endl;
        }
        else if(resultado.first==-2){
            cout<<"No existe un camino entre la ciudad de origen y destino"<<endl;
        }
    }
    
    cout<<"Precio global: "<<resultado.first<<endl;

    std::chrono::duration<double, std::micro> duration = end - start;
    cout<<"Tiempo de ejecucion: "<<duration.count()<<"microsegundos"<<endl;
    cout<<"Propuesta de recorridos:"<<endl;
    
    Viaje propuesta = grafo.mejor_directo(origen, destino, 1);

    printViaje(propuesta);

    cout<<"---------Por tiempo de llegada:-----------"<<endl;

    pair<chrono::system_clock::time_point,vector<Viaje>> resultados_tiempo = grafo.dijkstra_time(origen,destino);

    if(resultados_tiempo.first>=chrono::system_clock::time_point::min()){
        for(int i=0;i<resultados_tiempo.second.size();i++){
        printViaje(resultados_tiempo.second[i]);
        }
    } else{
        if(resultados_tiempo.first==chrono::system_clock::time_point::min()){
            cout<<"ERROR: La ciudad de origen o destino no existen"<<endl;
        }
        else if(resultados_tiempo.first==chrono::system_clock::time_point::min()+chrono::seconds(1)){
            cout<<"No existe un camino entre la ciudad de origen y destino"<<endl;
        }
    }

    cout<<"Propuesta de recorridos:"<<endl;
    
    propuesta = grafo.mejor_directo(origen, destino, 2);

    printViaje(propuesta);
    */
    
    

    

}