// http://www.nongnu.org/simulavr/intro.html
// https://github.com/Traumflug/simulavr/blob/traumflug/examples/simple_ex1/fred.c

// Hook SimulAVR into the runtime
#ifdef SIMULAVR

#include <simulavr.h>

/* This port correponds to the "--writetopipe=0x91,-" command line option. */
#define special_output_port (*((volatile char *)0x91))

/* This port correponds to the "--readfrompipe 0x92,-" command line option. */
#define special_input_port  (*((volatile char *)0x92))

/* Poll the specified string out the debug port. */
void debug_puts(const char *str) {
    const char *c;

    for (c = str; *c; c++) {
        special_output_port = *c;
    }
}


// conveniency wrappers
void _l(std::string str) {
    debug_puts(str.c_str());
}

void _d(int val) {
    char buffer[20];
    itoa(val, buffer, 10);
    //sprintf(buffer,"%d",val);
    _d(buffer);
}
void _d(double val) {
    char buffer[20];
    dtostrf(val, 0, 2, buffer);
    //sprintf(buffer,"%d",val);
    _d(buffer);
}
void _d(const char *str) {
    debug_puts(str);
    debug_puts("\n");
}
void _d(std::string str) {
    debug_puts(str.c_str());
    debug_puts("\n");
}


#else

// Bypass added functionality

#include <iterator>
#include <string>

// empty wrappers
void _l(std::string str) {}
void _d(int val) {}
void _d(double val) {}
void _d(const char *str) {}
void _d(std::string str) {}

#endif
