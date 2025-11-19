#include <iostream>
#include "core/app.hpp"

int main() {
    do {
        App::affiche();
    } while (App::choice());

    return 0;
}

