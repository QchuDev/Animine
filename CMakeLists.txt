cmake_minimum_required(VERSION 3.10)
project(QchuAnims)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Directorios de cabeceras
include_directories(include)

# 2. Buscar dependencias
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# 3. Listar TODOS los archivos fuente automáticamente
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.c"
    "src/classes/*.cpp"
    "src/classes/entities/*.cpp"
    "src/classes/scenes/*.cpp"
    "src/classes/render/*.cpp"
    "src/classes/animations/*.cpp"
    "src/classes/creation/*.cpp"
    "src/classes/creation/builders/*.cpp"
)

message(STATUS "Sources found: ${SOURCES}")

# 4. Crear el ejecutable
add_executable(qchu-anims ${SOURCES})

# 5. Vinculación multiplataforma
target_link_libraries(qchu-anims PRIVATE glfw OpenGL::GL)
target_include_directories(qchu-anims PRIVATE external)

# En Linux algunos sistemas necesitan enlazar libdl explícitamente
if(UNIX AND NOT APPLE)
    target_link_libraries(qchu-anims PRIVATE ${CMAKE_DL_LIBS})
endif()

# 6. Directorios de recursos
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# 7. Copiar recursos tras compilar
set(OUTPUT_DIR "$<TARGET_FILE_DIR:qchu-anims>")

add_custom_command(TARGET qchu-anims POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "${OUTPUT_DIR}/shaders"
    COMMENT "--- Actualizando assets y shaders en el directorio de ejecucion ---"
)
