// SimulAVR debug statement shortcuts
// http://www.nongnu.org/simulavr/intro.html
#ifndef simulavr_h
#define simulavr_h

#include <string>
void _l(std::string str);
void _d(int val);
void _d(double val);
void _d(const char *str);
void _d(std::string str);

#endif
