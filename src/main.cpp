#include "classes/engine.h"
#include "classes/paths.h"
#include <filesystem>

int main(int argc, char* argv[]) {
    g_basePath = std::filesystem::canonical(argv[0]).parent_path().parent_path();

    Engine engine;
    if(engine.init(1920, 1080, "QchuAnims - Ready"))
        engine.run();
    return 0;
}
