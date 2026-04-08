#include "classes/engine.h"

int main() {
    Engine engine;
    if(engine.init(1280, 720, "QchuAnims - Ready"))
        engine.run();
    return 0;
}