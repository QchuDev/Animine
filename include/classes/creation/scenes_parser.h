#ifndef SCENES_PARSER_H
#define SCENES_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <sstream>

class Renderer;
class Scene;

class ScenesParser {
public:
    ScenesParser(Renderer* r) : renderer(r) {}

    // Scans folder_path and builds one Scene per .txt file found
    std::vector<std::unique_ptr<Scene>> extractScenes(const std::string& folder_path);

private:
    Renderer* renderer;

    std::unique_ptr<Scene> parseFile(const std::string& file_path);
    void parseLine(const std::string& line, Scene* scene);
    bool createEntity(const std::string& typeStr, std::stringstream& ss, Scene* scene);
};

#endif
