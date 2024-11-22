#include "app.hpp"

int main() {
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "エラーが発生しました: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}
