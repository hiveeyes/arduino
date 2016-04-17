// http://www.nongnu.org/simulavr/intro.html
// https://github.com/Traumflug/simulavr/blob/traumflug/examples/simple_ex1/fred.c

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

void _d(const char *str) {
    debug_puts(str);
}
