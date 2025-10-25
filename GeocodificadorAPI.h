//
// Created by Carol on 1/10/2025.
//
#ifndef GEOCODIFICADORAPI_H
#define GEOCODIFICADORAPI_H

#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include "HTTPClient.h"

using json = nlohmann::json;

class GeocodificadorAPI {
public:
    GeocodificadorAPI();

    // Obtener coordenadas de cualquier dirección
    std::pair<double, double> obtenerCoordenadas(const std::string& direccion);

    // Configurar API key para servicios premium
    void configurarAPIKey(const std::string& apiKey);

    // Configurar región por defecto
    void configurarRegion(const std::string& pais = "AR");

private:
    HTTPClient httpClient;
    std::string apiKey;
    std::string regionPorDefecto;

    // Diferentes servicios de geocodificación
    std::pair<double, double> usarNominatim(const std::string& direccion);
    std::pair<double, double> usarGoogleMaps(const std::string& direccion);
    std::pair<double, double> usarPositionStack(const std::string& direccion);

    // Parsear respuestas JSON
    std::pair<double, double> parsearNominatim(const json& respuesta);
    std::pair<double, double> parsearGoogleMaps(const json& respuesta);
    std::pair<double, double> parsearPositionStack(const json& respuesta);

    // Validar coordenadas
    bool coordenadasValidas(double lat, double lon);

    // Log de errores
    void logError(const std::string& mensaje);
};

#endif
