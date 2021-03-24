#include <iostream>
inline int getIntStringBase(std::string literal) {
    int base = 10;
    if (literal.substr(0, 1) == "0") {
        if (literal.substr(0, 2) == "0x" || literal.substr(0, 2) == "0X")
            base = 16;
        else
            base = 8;
    } else if (literal.substr(0, 1) == "-") {
        if (literal.substr(1, 2) == "0x" || literal.substr(1, 2) == "0X")
            base = 16;
        else if (literal.substr(1, 1) == "0")
            base = 8;
    }
    return base;
}