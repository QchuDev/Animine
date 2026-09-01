#include "classes/engine.h"
#include "classes/paths.h"
#include <filesystem>
#include <string>

int main(int argc, char* argv[]) {
    g_basePath = std::filesystem::canonical(argv[0]).parent_path().parent_path();

    int w = 800, h = 800;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--res" && i + 1 < argc) {
            std::string res = argv[++i];
            auto x = res.find('x');
            if (x != std::string::npos) {
                w = std::stoi(res.substr(0, x));
                h = std::stoi(res.substr(x + 1));
            }
        }
    }

    Engine engine;
    if (engine.init(w, h, "QchuAnims"))
        engine.run();
    return 0;
}
