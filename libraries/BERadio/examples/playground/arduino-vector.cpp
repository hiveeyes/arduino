// Variant 1: avr-stl
//#include <Arduino.h>
//#include <pnew.cpp>

// Variant 2: Standard C++ for Arduino (port of uClibc++)
//#include <Arduino.h>
//#include <new.cpp>
//#include <StandardCplusplus.h>

// Variant 3: Classic ArduinoSTL
#include <ArduinoSTL.h>
#include <new.cpp>

// main program
#include <iterator>
#include <vector>

int main() {
    std::vector<double> values;
    values.push_back(42.42);
    return 0;
}
