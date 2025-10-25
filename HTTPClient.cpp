//
// Created by Carol on 1/10/2025.
//
#include "HTTPClient.h"
#include <iostream>

// Inicializar Winsock en Windows
#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

HTTPClient::HTTPClient() {
#ifdef _WIN32
    // Inicializar Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        std::cout << "✓ HTTPClient inicializado correctamente" << std::endl;
    } else {
        std::cerr << "❌ Error al inicializar HTTPClient" << std::endl;
    }
}

HTTPClient::~HTTPClient() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

#ifdef _WIN32
    // Limpiar Winsock
    WSACleanup();
#endif
}

std::string HTTPClient::get(const std::string& url) {
    std::string response;

    if (curl) {
        // Configurar URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // ⚠️ SOLO PARA PRUEBAS (MSYS2): desactiva verificación SSL
        // Esto evita el error "Problem with the SSL CA cert"
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Configurar callback para escribir respuesta
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Seguir redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Timeout de 10 segundos
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        // User Agent
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "SanatorioApp/1.0");

        // Realizar request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Error en request HTTP: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        // Verificar código de respuesta HTTP
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (response_code != 200) {
            std::cerr << "Error HTTP: " << response_code << std::endl;
            return "";
        }
    }

    return response;
}

std::string HTTPClient::urlEncode(const std::string& text) {
    if (curl) {
        char* encoded = curl_easy_escape(curl, text.c_str(), text.length());
        if (encoded) {
            std::string result(encoded);
            curl_free(encoded);
            return result;
        }
    }
    return text;
}

// Callback para recibir datos de la respuesta HTTP
size_t HTTPClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    size_t totalSize = size * nmemb;
    data->append((char*)contents, totalSize);
    return totalSize;
}
