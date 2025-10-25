#include "GeocodificadorAPI.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

// Clase Sanatorio simplificada para el ejemplo
class Sanatorio {
private:
    string nombre;
    string direccion;
    double latitud;
    double longitud;

public:
    Sanatorio(const string& nombre, const string& direccion, double lat, double lon)
            : nombre(nombre), direccion(direccion), latitud(lat), longitud(lon) {}

    string getNombre() const { return nombre; }
    string getDireccion() const { return direccion; }
    double getLatitud() const { return latitud; }
    double getLongitud() const { return longitud; }

    // Calcular distancia usando fórmula de Haversine
    double calcularDistancia(double latDestino, double lonDestino) const {
        const double R = 6371.0; // Radio de la Tierra en km

        double dLat = toRadians(latDestino - latitud);
        double dLon = toRadians(lonDestino - longitud);

        double a = sin(dLat/2) * sin(dLat/2) +
                   cos(toRadians(latitud)) * cos(toRadians(latDestino)) *
                   sin(dLon/2) * sin(dLon/2);

        double c = 2 * atan2(sqrt(a), sqrt(1-a));

        return R * c;
    }

private:
    double toRadians(double grados) const {
        return grados * M_PI / 180.0;
    }
};

class EmpresaSanatorio {
private:
    vector<Sanatorio*> sanatorios;
    GeocodificadorAPI geocodificador;

public:
    EmpresaSanatorio() {
        inicializarSanatorios();
        geocodificador.configurarRegion("AR");
    }

    void inicializarSanatorios() {
        sanatorios.push_back(new Sanatorio(
                "Sanatorio Güemes",
                "Av. Córdoba 2351, Buenos Aires",
                -34.5985, -58.3960
        ));

        sanatorios.push_back(new Sanatorio(
                "Hospital Italiano",
                "Juan Domingo Perón 4190, Buenos Aires",
                -34.6289, -58.4069
        ));

        sanatorios.push_back(new Sanatorio(
                "Sanatorio de la Trinidad Palermo",
                "Güemes 3821, Buenos Aires",
                -34.5799, -58.4069
        ));

        sanatorios.push_back(new Sanatorio(
                "Clínica Santa Isabel",
                "Av. Scalabrini Ortiz 3355, Buenos Aires",
                -34.5799, -58.4200
        ));

        cout << "✓ " << sanatorios.size() << " sanatorios cargados" << endl;
    }

    Sanatorio* recomendarSanatorioMasCercano() {
        cout << "\n=== BÚSQUEDA DE SANATORIO MÁS CERCANO ===" << endl;

        string direccionUsuario;
        cout << "Ingrese su dirección completa: ";
        getline(cin, direccionUsuario);

        if (direccionUsuario.empty()) {
            cout << "❌ Dirección vacía" << endl;
            return nullptr;
        }

        auto coordenadasUsuario = geocodificador.obtenerCoordenadas(direccionUsuario);

        if (coordenadasUsuario.first == 0.0 && coordenadasUsuario.second == 0.0) {
            cout << "❌ No se pudo geocodificar la dirección" << endl;
            return nullptr;
        }

        return encontrarSanatorioMasCercano(coordenadasUsuario.first, coordenadasUsuario.second);
    }

    Sanatorio* encontrarSanatorioMasCercano(double latUsuario, double lonUsuario) {
        if (sanatorios.empty()) {
            cout << "❌ No hay sanatorios disponibles" << endl;
            return nullptr;
        }

        Sanatorio* masCercano = nullptr;
        double menorDistancia = numeric_limits<double>::max();

        cout << "\n--- CALCULANDO DISTANCIAS ---" << endl;

        for (Sanatorio* sanatorio : sanatorios) {
            double distancia = sanatorio->calcularDistancia(latUsuario, lonUsuario);

            cout << "• " << sanatorio->getNombre()
                 << ": " << fixed << setprecision(2)
                 << distancia << " km" << endl;

            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                masCercano = sanatorio;
            }
        }

        if (masCercano) {
            cout << "\n🏥 SANATORIO MÁS CERCANO:" << endl;
            cout << "   Nombre: " << masCercano->getNombre() << endl;
            cout << "   Dirección: " << masCercano->getDireccion() << endl;
            cout << "   Distancia: " << fixed << setprecision(2)
                 << menorDistancia << " km" << endl;
        }

        return masCercano;
    }

    ~EmpresaSanatorio() {
        for (Sanatorio* sanatorio : sanatorios) {
            delete sanatorio;
        }
    }
};

int main() {
    cout << "🏥 SISTEMA DE SANATORIOS" << endl;
    cout << "========================\n" << endl;

    EmpresaSanatorio empresa;

    while (true) {
        cout << "\n=== MENÚ ===" << endl;
        cout << "1. Encontrar sanatorio más cercano" << endl;
        cout << "2. Salir" << endl;
        cout << "Opción: ";

        int opcion;
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
            case 1:
                empresa.recomendarSanatorioMasCercano();
                break;
            case 2:
                cout << "¡Hasta luego!" << endl;
                return 0;
            default:
                cout << "Opción inválida" << endl;
        }
    }

    return 0;
}