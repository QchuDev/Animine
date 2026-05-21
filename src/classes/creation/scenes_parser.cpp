#include "classes/creation/scenes_parser.h"
#include "classes/scenes/scene.h"
#include "classes/render/renderer.h"
#include "classes/entities/line.h"
#include "classes/entities/quad.h"
#include "classes/entities/curve.h"
#include "classes/entities/group.h"
#include "classes/animations/animation.h"
#include "classes/animations/easing_type.h"
#include "classes/animations/track.h"
#include "classes/animations/keyframe.h"

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

        std::string strokeName;
        unsigned int strokeTex = renderer->getDefaultStrokeTexture();
        if (ss >> strokeName)
            strokeTex = renderer->getOrCreateTexture("strokes/" + strokeName + ".png");

        scene->addEntity(id, new Line(glm::vec3(x1,y1,z1), glm::vec3(x2,y2,z2), glm::vec3(r,g,b), renderer->getStrokeShader(), strokeTex));
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

        float tMin = 0.0f, tMax = 6.2832f;
        std::string strokeName;
        unsigned int strokeTex = renderer->getDefaultStrokeTexture();

        // Try reading optional t_min t_max, then optional stroke name
        std::string token;
        if (ss >> token) {
            try {
                tMin = std::stof(token);
                if (ss >> tMax) {
                    // Got range, try stroke name
                    if (ss >> strokeName)
                        strokeTex = renderer->getOrCreateTexture("strokes/" + strokeName + ".png");
                }
            } catch (...) {
                // Not a float — it's the stroke name
                strokeName = token;
                strokeTex = renderer->getOrCreateTexture("strokes/" + strokeName + ".png");
            }
        }

        auto ctx = std::make_shared<ExprContext>(xt, yt, zt);
        auto fn = [ctx](float t) -> glm::vec3 {
            ctx->t_val = t;
            return glm::vec3(
                ctx->exprX ? (float)te_eval(ctx->exprX) : 0.0f,
                ctx->exprY ? (float)te_eval(ctx->exprY) : 0.0f,
                ctx->exprZ ? (float)te_eval(ctx->exprZ) : 0.0f
            );
        };

        scene->addEntity(id, new Curve(fn, glm::vec3(r,g,b), renderer->getStrokeShader(), strokeTex, tMin, tMax));
        return true;
    }

    std::cerr << "Unknown entity type: " << typeStr << "\n";
    return false;
}

// ── Animation parsing ─────────────────────────────────────────────────────────

static EasingType parseEasing(const std::string& s) {
    if (s == "ease_in")     return EasingType::EASE_IN;
    if (s == "ease_out")    return EasingType::EASE_OUT;
    if (s == "ease_in_out") return EasingType::EASE_IN_OUT;
    return EasingType::LINEAR;
}

static InterpolationMode parseInterp(const std::string& s) {
    if (s == "smooth") return InterpolationMode::SMOOTH;
    if (s == "path")   return InterpolationMode::PATH;
    return InterpolationMode::LINEAR;
}

// animate <entity_id> <property> <easing> <interp> <args> <duration>
// For linear/smooth: args = waypoints (x y z)...
// For path: args = x_expr y_expr z_expr
bool ScenesParser::createAnimation(std::stringstream& ss, float startTime, Scene* scene) {
    std::string entityId, propStr, easingStr, interpStr;
    if (!(ss >> entityId >> propStr >> easingStr >> interpStr)) return false;

    Track track;
    track.entity_id     = entityId;
    track.easing        = parseEasing(easingStr);
    track.interpolation = parseInterp(interpStr);

    if      (propStr == "position") track.property = TransformProp::POSITION;
    else if (propStr == "rotation") track.property = TransformProp::ROTATION;
    else if (propStr == "scale")    track.property = TransformProp::SCALE;
    else if (propStr == "color")    track.property = TransformProp::COLOR;
    else { std::cerr << "animate: unknown property '" << propStr << "'\n"; return false; }

    float duration = 0.0f;

    if (track.interpolation == InterpolationMode::PATH) {
        // Read 3 expressions + duration
        std::string xExpr, yExpr, zExpr;
        if (!(ss >> xExpr >> yExpr >> zExpr >> duration)) {
            std::cerr << "animate path: expected <x_expr> <y_expr> <z_expr> <duration>\n";
            return false;
        }
        track.pathT = new double(0.0);
        te_variable vars[] = { {"t", track.pathT} };
        int err = 0;
        track.pathExprX = te_compile(xExpr.c_str(), vars, 1, &err);
        if (err) std::cerr << "animate path: bad x expr '" << xExpr << "'\n";
        err = 0;
        track.pathExprY = te_compile(yExpr.c_str(), vars, 1, &err);
        if (err) std::cerr << "animate path: bad y expr '" << yExpr << "'\n";
        err = 0;
        track.pathExprZ = te_compile(zExpr.c_str(), vars, 1, &err);
        if (err) std::cerr << "animate path: bad z expr '" << zExpr << "'\n";
    } else {
        // Read waypoints + duration
        std::vector<float> nums;
        float v;
        while (ss >> v) nums.push_back(v);

        if (nums.size() < 4 || (nums.size() - 1) % 3 != 0) {
            std::cerr << "animate: bad format for '" << entityId << "'\n";
            return false;
        }

        duration = nums.back();
        int wpCount = (int)(nums.size() - 1) / 3;
        for (int i = 0; i < wpCount; ++i) {
            int base = i * 3;
            track.keyframes.push_back({ 0.0f, glm::vec3(nums[base], nums[base+1], nums[base+2]) });
        }
    }

    auto* anim = new Animation();
    anim->startTime = startTime;
    anim->duration  = duration;
    anim->tracks.push_back(std::move(track));
    scene->addAnimation(anim);
    return true;
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

    float timeOffset = 0.0f;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "animate") {
            if (!createAnimation(ss, timeOffset, scene.get()))
                std::cerr << "Failed to parse animate line: " << line << "\n";
        } else if (token == "wait") {
            float secs = 0.0f;
            ss >> secs;
            timeOffset += secs;
        } else if (token == "background") {
            std::string texName;
            if (ss >> texName)
                scene->backgroundTexture = texName;
        } else if (token == "set") {
            std::string entityId, propStr;
            float x, y, z;
            if (!(ss >> entityId >> propStr >> x >> y >> z)) {
                std::cerr << "set: bad format: " << line << "\n";
                continue;
            }
            auto* cmd = new InstantSet();
            cmd->startTime = timeOffset;
            cmd->entity_id = entityId;
            cmd->value = glm::vec3(x, y, z);
            if      (propStr == "position") cmd->property = TransformProp::POSITION;
            else if (propStr == "rotation") cmd->property = TransformProp::ROTATION;
            else if (propStr == "scale")    cmd->property = TransformProp::SCALE;
            else if (propStr == "color")    cmd->property = TransformProp::COLOR;
            else { std::cerr << "set: unknown property '" << propStr << "'\n"; delete cmd; continue; }
            scene->addAnimation(cmd);
        } else if (token == "group") {
            std::string groupId;
            if (!(ss >> groupId)) { std::cerr << "group: missing id\n"; continue; }
            auto* group = new Group();
            std::string childId;
            while (ss >> childId) {
                group->childIds.push_back(childId);
                IEntity* child = scene->getEntity(childId);
                if (child) child->parentId = groupId;
                else std::cerr << "group: child '" << childId << "' not found\n";
            }
            scene->addEntity(groupId, group);
        } else {
            // entity line — rewind and delegate
            parseLine(line, scene.get());
        }
    }

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
