#include <iostream>
#include <fstream>          // For reading the .txt file
#include <sstream>          // For the parsing of text

#define exprtk_disable_string_capabilities  // Si no vas a procesar texto dentro de las fórmulas
#define exprtk_disable_rtl_io_capabilities  // Desactiva funciones de impresión/consola en las fórmulas
#define exprtk_disable_break_repeat_loop_capabilities
#include <external/exprtk.hpp>

#include <memory>

// Managers
#include "classes/scenes/scene.h"
#include "classes/render/renderer.h"

// Entities
#include "classes/entities/line.h"
#include "classes/entities/quad.h"
#include "classes/entities/curve.h"


// Estructura para agrupar lo que ExprTk necesita para evaluar
struct ExprContext {
    float t_val;
    exprtk::symbol_table<float> symbol_table;
    exprtk::expression<float> exprX, exprY, exprZ;

    // Constructor que compila las 3 fórmulas
    ExprContext(std::string x_str, std::string y_str, std::string z_str) {
        symbol_table.add_variable("t", t_val);
        symbol_table.add_constants();
        
        exprX.register_symbol_table(symbol_table);
        exprY.register_symbol_table(symbol_table);
        exprZ.register_symbol_table(symbol_table);

        exprtk::parser<float> parser;
        parser.compile(x_str, exprX);
        parser.compile(y_str, exprY);
        parser.compile(z_str, exprZ);
    }
};

enum class EntityType {
    LINE,
    CURVE,
    QUAD,
    UNKNOWN
};

EntityType getEntityType(const std::string& type) {
    if (type == "line") return EntityType::LINE;
    if (type == "curve") return EntityType::CURVE;
    if (type == "quad") return EntityType::QUAD;
    return EntityType::UNKNOWN;
}

bool Scene::loadScene(std::string path) {
    // Creates an input file stream and open the file 
    std::ifstream myFile(path);
    
    // Check if the file was opened successfullu
    if (!myFile.is_open()) {
        std::cerr << "Error: Could not find the specified file" << std::endl;
        return false;
    }
    
    // we read and pring (for now...) the content of the file
    std::string line;
    std::cerr << "Reading file ..." << std::endl;
    while (std::getline(myFile, line))
    {
        parseLine(line);        
    }
    
    // Close the file
    myFile.close();
    return true;
}

void Scene::parseLine(const std::string& line) {
    // We ignore comments -> lines starting with '#'
    if (line.empty() || line[0] == '#') { return; }
    std::cout << "Processing --> " + line << std::endl; // temp.
    
    std::stringstream ss(line);
    std::string typeStr;
    ss >> typeStr;     // Extracts the firt word --> the object type (example: line)
    
    if (!entityCreation(typeStr, ss)) {
        std::cout << "Error: couldn't create entity " << std::endl;
    }
        
}

bool Scene::entityCreation(std::string& typeStr, std::stringstream& ss) {
    EntityType type = getEntityType(typeStr); 
    switch (type) {
        
        case EntityType::LINE: {
            std::string id;
            float x1, y1, z1, x2, y2, z2, r, g, b;
            
            if (ss >> id >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b) {
                if (entities.find(id) == entities.end()) {
                    entities[id] = new Line(glm::vec3(x1,y1,z1), glm::vec3(x2,y2,z2), glm::vec3(r,g,b), renderer->getGizmoShader());
                    std::cout << "Line created" << std::endl;
                    return true;
                } else {
                    std::cout << "Error: duplicated id at .txt" << std::endl;

                }
            }
            
            break;
        }
        
        case EntityType::QUAD: {
            std::string id, texName;
            
            if (ss >> id >> texName) {
                // Make a temporal list of parameters 
                std::vector<float> params;
                float temp;
                while(ss >> temp) {
                    params.push_back(temp);
                }

                unsigned int texID = renderer->getOrCreateTexture(texName);
                
                if (entities.find(id) == entities.end()) {
                    float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
                                        
                    if (params.size() == 12) {
                        x1 = params[0]; y1 = params[1]; z1 = params[2];    
                        x2 = params[3]; y2 = params[4]; z2 = params[5];    
                        x3 = params[6]; y3 = params[7]; z3 = params[8];    
                        x4 = params[9]; y4 = params[10]; z4 = params[11];    
                    } else if (params.size() == 2) {
                        x1 = -0.5f*params[0]; y1 = -0.5f*params[1]; z1 = 0.0f;    
                        x2 = 0.5f*params[0]; y2 = -0.5f*params[1]; z2 = 0.0f;    
                        x3 = 0.5f*params[0]; y3 = 0.5f*params[1]; z3 = 0.0f;    
                        x4 = -0.5f*params[0]; y4 = 0.5f*params[1]; z4 = 0.0f;    
                    } else {
                        x1 = -0.5; y1 = -0.5; z1 = 0.0f;    
                        x2 = 0.5; y2 = -0.5; z2 = 0.0f;    
                        x3 = 0.5; y3 = 0.5; z3 = 0.0f;    
                        x4 = -0.5; y4 = 0.5; z4 = 0.0f;
                    }
                    
                    // Finally we create the quad with the adjusted params 
                    entities[id] = new Quad(
                        glm::vec3(x1,y1,z1), 
                        glm::vec3(x2,y2,z2), 
                        glm::vec3(x3,y3,z3), 
                        glm::vec3(x4,y4,z4),
                        texID, 
                        renderer->getTextureShader());
                    
                    std::cout << "Quad created" << std::endl;
                    
                    return true;
                } else {
                    std::cout << "Error: duplicated id at .txt" << std::endl;
                }
                
            } else {
                std::cout << "Error: check id and texture name" << std::endl;
            }
            
            break;
        }
        
        case EntityType::CURVE: {
            std::string id, xt, yt, zt;
            float r, g, b;
            
            if (ss >> id >> xt >> yt >> zt >> r >> g >> b) {

                if (entities.find(id) == entities.end()) {
                    
                    // Creamos el contexto en el Heap y lo envolvemos en un shared_ptr
                    auto ctx = std::make_shared<ExprContext>(xt, yt, zt);
                    auto paramCurve =  [ctx](float t) {
                        ctx->t_val = t; // Actualizamos la 't' vinculada a ExprTk
                        return glm::vec3(
                            ctx->exprX.value(),
                            ctx->exprY.value(),
                            ctx->exprZ.value()
                        );
                    };
                    
                    // Using ExprTk
                    entities[id] = new Curve(paramCurve, glm::vec3(r,g,b), renderer->getGizmoShader());
                    std::cout << "Curve created" << std::endl;
                    return true;
                } else {
                    std::cout << "Error: duplicated id at .txt" << std::endl;
                }

            }
            
            break;
        }
        
        default: {
            std::cerr << "Unknown entity type: " << typeStr << std::endl;
        }
    }
    
    return false;

}

const std::map<std::string, IEntity*>& Scene::getAllEntities() const {
    return entities;
}

Scene::~Scene() {
    for (auto const& [id, entity] : entities) {
        delete entity;
    }
    entities.clear();
}