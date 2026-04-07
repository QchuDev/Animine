#include "classes/engine.h"

int main() {
    Engine engine;
    if(engine.init(1920, 1080, "QchuAnims - Ready"))
        engine.run();
    return 0;
}