cmake_minimum_required(VERSION 3.10)
project(QchuAnims)

set(CMAKE_CXX_STANDARD 17)

# 1. Directorios de cabeceras
include_directories(include)

# 2. Buscar GLFW
find_package(glfw3 REQUIRED)

# 3. Listar TODOS los archivos fuente automáticamente
# Esto busca en src y en src/classes
file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "src/*.c" 
    "src/classes/*.cpp" 
    "src/classes/entities/*.cpp"
)

message(STATUS "Sources found: ${SOURCES}")

# 4. Crear el ejecutable con la lista de fuentes
add_executable(qchu-anims ${SOURCES})

# 5. Vinculación
target_link_libraries(qchu-anims glfw opengl32)

# 6. Definir dónde están tus carpetas de recursos
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# 7. Copiar recursos (Versión mejorada)
# Usamos una expresión generadora para asegurar la ruta correcta del ejecutable
set(OUTPUT_DIR "$<TARGET_FILE_DIR:qchu-anims>")

add_custom_command(TARGET qchu-anims POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${OUTPUT_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "${OUTPUT_DIR}/shaders"
    COMMENT "--- Actualizando assets y shaders en el directorio de ejecucion ---"
)