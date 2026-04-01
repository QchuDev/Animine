cmake_minimum_required(VERSION 3.10)
project(QchuAnims)

set(CMAKE_CXX_STANDARD 17)

# 1. Directorios de cabeceras
include_directories(include)

# 2. Buscar GLFW
find_package(glfw3 REQUIRED)

# 3. Listar TODOS los archivos fuente automáticamente
# Esto busca en src y en src/classes
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# 4. Crear el ejecutable con la lista de fuentes
add_executable(qchu-anims ${SOURCES})

# 5. Vinculación
target_link_libraries(qchu-anims glfw opengl32)

add_custom_command(TARGET qchu-anims POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:qchu-anims>/shaders"
    COMMENT "Copiando shaders al directorio de salida..."
)