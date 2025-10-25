//
// Created by Carol on 1/10/2025.
//

#include "GeocodificadorAPI.h"
#include "GeocodificadorAPI.h"
#include <iostream>
#include <thread>
#include <chrono>

GeocodificadorAPI::GeocodificadorAPI() : regionPorDefecto("AR") {
    std::cout << "Geocodificador API inicializado\n";
}

void GeocodificadorAPI::configurarAPIKey(const std::string& apiKey) {
    this->apiKey = apiKey;
}

void GeocodificadorAPI::configurarRegion(const std::string& pais) {
    this->regionPorDefecto = pais;
}

std::pair<double, double> GeocodificadorAPI::obtenerCoordenadas(const std::string& direccion) {
    std::cout << "Geocodificando: " << direccion << "..." << std::endl;

    // Intentar primero con Nominatim (GRATIS)
    auto resultado = usarNominatim(direccion);
    if (coordenadasValidas(resultado.first, resultado.second)) {
        std::cout << "✓ Coordenadas encontradas: " << resultado.first << ", " << resultado.second << std::endl;
        return resultado;
    }

    // Si falla, intentar con Google Maps (requiere API key)
    if (!apiKey.empty()) {
        resultado = usarGoogleMaps(direccion);
        if (coordenadasValidas(resultado.first, resultado.second)) {
            std::cout << "✓ Coordenadas encontradas (Google): " << resultado.first << ", " << resultado.second << std::endl;
            return resultado;
        }
    }

    // Como último recurso, PositionStack
    resultado = usarPositionStack(direccion);
    if (coordenadasValidas(resultado.first, resultado.second)) {
        std::cout << "✓ Coordenadas encontradas (PositionStack): " << resultado.first << ", " << resultado.second << std::endl;
        return resultado;
    }

    logError("No se pudieron obtener coordenadas para: " + direccion);
    return {0.0, 0.0};
}

std::pair<double, double> GeocodificadorAPI::usarNominatim(const std::string& direccion) {
    try {
        // Construir URL para Nominatim (OpenStreetMap - GRATIS)
        std::string url = "https://nominatim.openstreetmap.org/search?format=json&limit=1&q=" +
                          httpClient.urlEncode(direccion);

        // Agregar país para mejorar resultados
        if (!regionPorDefecto.empty()) {
            url += "&countrycodes=" + regionPorDefecto;
        }

        std::cout << "Consultando Nominatim..." << std::endl;

        // Realizar request
        std::string respuesta = httpClient.get(url);
        if (respuesta.empty()) {
            return {0.0, 0.0};
        }

        // Parsear JSON
        json jsonRespuesta = json::parse(respuesta);
        return parsearNominatim(jsonRespuesta);

    } catch (const std::exception& e) {
        logError("Error en Nominatim: " + std::string(e.what()));
        return {0.0, 0.0};
    }
}

std::pair<double, double> GeocodificadorAPI::usarGoogleMaps(const std::string& direccion) {
    if (apiKey.empty()) {
        logError("API Key de Google Maps no configurada");
        return {0.0, 0.0};
    }

    try {
        // Construir URL para Google Maps Geocoding API
        std::string url = "https://maps.googleapis.com/maps/api/geocode/json?address=" +
                          httpClient.urlEncode(direccion) +
                          "&key=" + apiKey;

        // Agregar región
        if (!regionPorDefecto.empty()) {
            url += "&region=" + regionPorDefecto;
        }

        std::cout << "Consultando Google Maps..." << std::endl;

        std::string respuesta = httpClient.get(url);
        if (respuesta.empty()) {
            return {0.0, 0.0};
        }

        json jsonRespuesta = json::parse(respuesta);
        return parsearGoogleMaps(jsonRespuesta);

    } catch (const std::exception& e) {
        logError("Error en Google Maps: " + std::string(e.what()));
        return {0.0, 0.0};
    }
}

std::pair<double, double> GeocodificadorAPI::usarPositionStack(const std::string& direccion) {
    try {
        // PositionStack - alternativa gratuita limitada
        std::string url = "https://api.positionstack.com/v1/forward?access_key=afc545760e5a981467674e483fa40a78&query=" +
                          httpClient.urlEncode(direccion); // aca es donde puse mi appi key

        std::cout << "Consultando PositionStack..." << std::endl;

        std::string respuesta = httpClient.get(url);
        if (respuesta.empty()) {
            return {0.0, 0.0};
        }

        json jsonRespuesta = json::parse(respuesta);
        return parsearPositionStack(jsonRespuesta);

    } catch (const std::exception& e) {
        logError("Error en PositionStack: " + std::string(e.what()));
        return {0.0, 0.0};
    }
}

std::pair<double, double> GeocodificadorAPI::parsearNominatim(const json& respuesta) {
    if (respuesta.is_array() && !respuesta.empty()) {
        auto primer_resultado = respuesta[0];

        if (primer_resultado.contains("lat") && primer_resultado.contains("lon")) {
            double lat = std::stod(primer_resultado["lat"].get<std::string>());
            double lon = std::stod(primer_resultado["lon"].get<std::string>());

            std::cout << "Dirección encontrada: " << primer_resultado.value("display_name", "Sin nombre") << std::endl;
            return {lat, lon};
        }
    }
    return {0.0, 0.0};
}

std::pair<double, double> GeocodificadorAPI::parsearGoogleMaps(const json& respuesta) {
    if (respuesta.contains("status") && respuesta["status"] == "OK" &&
        respuesta.contains("results") && !respuesta["results"].empty()) {

        auto primer_resultado = respuesta["results"][0];

        if (primer_resultado.contains("geometry") &&
            primer_resultado["geometry"].contains("location")) {

            auto location = primer_resultado["geometry"]["location"];
            double lat = location["lat"];
            double lon = location["lng"];

            std::cout << "Dirección encontrada: " << primer_resultado.value("formatted_address", "Sin nombre") << std::endl;
            return {lat, lon};
        }
    }
    return {0.0, 0.0};
}

std::pair<double, double> GeocodificadorAPI::parsearPositionStack(const json& respuesta) {
    if (respuesta.contains("data") && !respuesta["data"].empty()) {
        auto primer_resultado = respuesta["data"][0];

        if (primer_resultado.contains("latitude") && primer_resultado.contains("longitude")) {
            double lat = primer_resultado["latitude"];
            double lon = primer_resultado["longitude"];

            return {lat, lon};
        }
    }
    return {0.0, 0.0};
}

bool GeocodificadorAPI::coordenadasValidas(double lat, double lon) {
    return (lat != 0.0 || lon != 0.0) &&
           lat >= -90.0 && lat <= 90.0 &&
           lon >= -180.0 && lon <= 180.0;
}

void GeocodificadorAPI::logError(const std::string& mensaje) {
    std::cerr << "❌ " << mensaje << std::endl;
}