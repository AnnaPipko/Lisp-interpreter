#include "common_functions.h"

int64_t StringToInt(const std::string& str){
    return std::stoll(str);
}

std::string IntToString(int64_t value){
    return std::to_string(value);
}

bool StringToBool(const std::string& str){
    return str != "#f";
}

std::string BoolToString(bool value){
    if (value){
        return "#t";
    }
    return "#f";
}