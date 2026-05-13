#include "classes/engine.h"
#include "classes/paths.h"
#include <filesystem>

int main(int argc, char* argv[]) {
    g_basePath = std::filesystem::canonical(argv[0]).parent_path().parent_path();

    Engine engine;
    if(engine.init(1280, 720, "QchuAnims - Ready"))
        engine.run();
    return 0;
}
