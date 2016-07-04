/* -*- coding: utf-8 -*-
 *
 * (c) 2013 Matt Bolt, High5 Games <mbolt35@gmail.com>
 *     Generic variadic argument reading.
 *     ArgsExample.cpp: https://gist.github.com/mbolt35/4e60da5aaec94dcd39ca
 *
 * (c) 2015 Andreas Motl <andreas.motl@elmyra.de>
 *     Use generic vector as container for buffering and returning items.
 *
 * Read a variadic argument list into a vector.
 *
 */
#include <stdarg.h>
#include <vector>

template<class T>
extern std::vector<T> varargs(int count, T value, ...) {

    // initialize reading of variadic arguments
    va_list args;
    va_start(args, value);

    // buffer for collecting variadic arguments
    std::vector<T> values;

    // store first argument into buffer
    values.push_back(value);

    // read items of type T from argument list
    // start at index = 1
    for (int i = 0; i < count-1; ++i) {

        // pop item from argument list
        T item = va_arg(args, T);

        // store into buffer
        values.push_back(item);
    }

    // clean up from reading arguments
    va_end(args);

    return values;
}
