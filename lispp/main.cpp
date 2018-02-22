#include <iostream>
#include <sstream>
#include "lispp.h"

void NextInput(std::stringstream *ss){
    std::cout << "Lispp>> ";
        std::string old_input;
        getline(*ss, old_input);
        std::cout << old_input;
        ss->clear();
        std::string new_input;
        getline(std::cin, new_input);
        ss->str(old_input + new_input);
}

int main() {
    std::stringstream in;
    Lispp lispp(&in, &std::cout);
    std::cout << "Lispp prompt\nFor exit press Ctrl+D\n\n";
    while (std::cin){
        try {
            NextInput(&in);
            lispp.Run();
        } catch (const std::exception& exception){
            std::cout << "     >> "  << exception.what() << std::endl;
        }
    }
    return 0;
}
