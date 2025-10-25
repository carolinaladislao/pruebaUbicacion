cmake_minimum_required(VERSION 3.20)
project(pruebaUbicacion)

# Configurar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurar vcpkg toolchain (IMPORTANTE)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

# Encontrar las librerías instaladas con vcpkg
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Archivos fuente del proyecto
set(SOURCES
        main.cpp
        HTTPClient.cpp
        HTTPClient.h
        GeocodificadorAPI.cpp
        GeocodificadorAPI.h
)

# Crear el ejecutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Enlazar las librerías
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)

# Configuraciones adicionales para Windows
if(WIN32)
    # Enlazar librerías del sistema Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wldap32 crypt32)

    # Copiar DLLs necesarias al directorio de salida
    if(TARGET CURL::libcurl)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:CURL::libcurl>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()
endif()

# Configurar directorio de salida
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Opciones de compilación
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)