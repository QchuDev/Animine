#include "classes/creation/scenes_parser.h"
#include "classes/scenes/scene.h"
#include "classes/render/renderer.h"
#include "classes/entities/line.h"
#include "classes/entities/quad.h"
#include "classes/entities/curve.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>

extern "C" {
#include "external/tinyexpr.h"
}

namespace fs = std::filesystem;

// ── tinyexpr helper ───────────────────────────────────────────────────────────

struct ExprContext {
    double t_val = 0.0;
    te_expr* exprX = nullptr;
    te_expr* exprY = nullptr;
    te_expr* exprZ = nullptr;

    ExprContext(const std::string& x, const std::string& y, const std::string& z) {
        te_variable vars[] = { {"t", &t_val} };
        int err = 0;
        exprX = te_compile(x.c_str(), vars, 1, &err);
        if (err) std::cerr << "tinyexpr: bad x expression '" << x << "' (col " << err << ")\n";
        exprY = te_compile(y.c_str(), vars, 1, &err);
        if (err) std::cerr << "tinyexpr: bad y expression '" << y << "' (col " << err << ")\n";
        exprZ = te_compile(z.c_str(), vars, 1, &err);
        if (err) std::cerr << "tinyexpr: bad z expression '" << z << "' (col " << err << ")\n";
    }

    ~ExprContext() {
        te_free(exprX);
        te_free(exprY);
        te_free(exprZ);
    }
};

// ── Entity creation ───────────────────────────────────────────────────────────

bool ScenesParser::createEntity(const std::string& typeStr, std::stringstream& ss, Scene* scene) {

    if (typeStr == "line") {
        std::string id;
        float x1, y1, z1, x2, y2, z2, r, g, b;
        if (!(ss >> id >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b)) return false;
        scene->addEntity(id, new Line(glm::vec3(x1,y1,z1), glm::vec3(x2,y2,z2), glm::vec3(r,g,b), renderer->getGizmoShader()));
        return true;
    }

    if (typeStr == "quad") {
        std::string id, texName;
        if (!(ss >> id >> texName)) return false;

        std::vector<float> params;
        float tmp;
        while (ss >> tmp) params.push_back(tmp);

        unsigned int texID = renderer->getOrCreateTexture(texName);

        glm::vec3 v1, v2, v3, v4;
        if (params.size() == 12) {
            v1 = {params[0],  params[1],  params[2]};
            v2 = {params[3],  params[4],  params[5]};
            v3 = {params[6],  params[7],  params[8]};
            v4 = {params[9],  params[10], params[11]};
        } else if (params.size() == 2) {
            float hw = params[0] * 0.5f, hh = params[1] * 0.5f;
            v1 = {-hw,-hh,0}; v2 = {hw,-hh,0};
            v3 = { hw, hh,0}; v4 = {-hw, hh,0};
        } else {
            v1 = {-0.5f,-0.5f,0}; v2 = {0.5f,-0.5f,0};
            v3 = { 0.5f, 0.5f,0}; v4 = {-0.5f, 0.5f,0};
        }

        scene->addEntity(id, new Quad(v1, v2, v3, v4, texID, renderer->getTextureShader()));
        return true;
    }

    if (typeStr == "curve") {
        std::string id, xt, yt, zt;
        float r, g, b;
        if (!(ss >> id >> xt >> yt >> zt >> r >> g >> b)) return false;

        auto ctx = std::make_shared<ExprContext>(xt, yt, zt);
        auto fn = [ctx](float t) -> glm::vec3 {
            ctx->t_val = t;
            return glm::vec3(
                ctx->exprX ? (float)te_eval(ctx->exprX) : 0.0f,
                ctx->exprY ? (float)te_eval(ctx->exprY) : 0.0f,
                ctx->exprZ ? (float)te_eval(ctx->exprZ) : 0.0f
            );
        };

        scene->addEntity(id, new Curve(fn, glm::vec3(r,g,b), renderer->getGizmoShader()));
        return true;
    }

    std::cerr << "Unknown entity type: " << typeStr << "\n";
    return false;
}

// ── Line / file parsing ───────────────────────────────────────────────────────

void ScenesParser::parseLine(const std::string& line, Scene* scene) {
    if (line.empty() || line[0] == '#') return;

    std::stringstream ss(line);
    std::string typeStr;
    ss >> typeStr;

    if (!createEntity(typeStr, ss, scene))
        std::cerr << "Failed to create entity from line: " << line << "\n";
}

std::unique_ptr<Scene> ScenesParser::parseFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "SceneParser: cannot open " << file_path << "\n";
        return nullptr;
    }

    std::string sceneId = fs::path(file_path).stem().string();
    auto scene = std::make_unique<Scene>(sceneId);

    std::string line;
    while (std::getline(file, line))
        parseLine(line, scene.get());

    return scene;
}

std::vector<std::unique_ptr<Scene>> ScenesParser::extractScenes(const std::string& folder_path) {
    std::vector<std::unique_ptr<Scene>> scenes;

    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".txt") {
            auto scene = parseFile(entry.path().string());
            if (scene) scenes.push_back(std::move(scene));
        }
    }

    return scenes;
}
