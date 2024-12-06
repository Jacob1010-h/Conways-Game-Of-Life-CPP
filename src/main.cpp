#include "Window.h"

int main(int argc, char *args[]) {

    Window game;
    game.gameLoop();

    return 0;
    // std::string glider =
    //     "#Life 1.05\n"
    //     "#D This is a glider.\n"
    //     "#P -1 -1\n"
    //     ".*.\n"
    //     "..*\n"
    //     "***\n";
    //
    // Pattern p;
    // p.toPattern(glider);  // Load the pattern
    // p.printMetadata();    // Print metadata (title, description, rule, position)
    // p.printPattern();     // Print the pattern
    // p.printSize();        // Print the size of the pattern
}
