#include "Server.hpp"

int main() {
    srand(time(NULL));
    Server serverInstance;
    serverInstance.run();
}