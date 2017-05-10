/*
  PrintCascade.h - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PrintCascade_h
#define PrintCascade_h

#include "Print.h"
#include <inttypes.h>

class PrintCascade
{
  public:
	PrintCascade();
    PrintCascade& print(char);
    PrintCascade& print(const char[]);
    PrintCascade& print(uint8_t);
    PrintCascade& print(int);
    PrintCascade& print(unsigned int);
    PrintCascade& print(long);
    PrintCascade& print(unsigned long);
    PrintCascade& print(long, int);
    PrintCascade& print(double);
    PrintCascade& println(void );
    PrintCascade& println(char);
    PrintCascade& println(const char[]);
    PrintCascade& println(uint8_t);
    PrintCascade& println(int);
    PrintCascade& println(unsigned int);
    PrintCascade& println(long);
    PrintCascade& println(unsigned long);
    PrintCascade& println(long, int);
    PrintCascade& println(double);
    Print* printer;
  private:
};

#endif
