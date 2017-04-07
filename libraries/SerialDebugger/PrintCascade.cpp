/*
 PrintCascade.cpp - Base class that provides print() and println()
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
 
 Modified 23 November 2006 by David A. Mellis
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "wiring.h"

#include "PrintCascade.h"

// Public Methods //////////////////////////////////////////////////////////////
PrintCascade::PrintCascade(){
  printer=0;
}

PrintCascade& PrintCascade::print(uint8_t b)
{
  printer->write(b);
  return *this;
}

PrintCascade& PrintCascade::print(char c)
{
  printer->print(c);
  return *this;
}

PrintCascade& PrintCascade::print(const char c[])
{
  printer->print(c);
	
  return *this;
}

PrintCascade& PrintCascade::print(int n)
{
  printer->print(n);
  return *this;
}

PrintCascade& PrintCascade::print(unsigned int n)
{
  printer->print(n);
  return *this;
}

PrintCascade& PrintCascade::print(long n)
{
  printer->print(n);
  return *this;
}

PrintCascade& PrintCascade::print(unsigned long n)
{
  printer->print(n);
  return *this;
}

PrintCascade& PrintCascade::print(long n, int base)
{
  printer->print(n,base);
	
  return *this;
}

PrintCascade& PrintCascade::print(double n)
{
  printer->print(n);
  return *this;
}

PrintCascade& PrintCascade::println(void)
{
  printer->println();  
  return *this;
}

PrintCascade& PrintCascade::println(char c)
{
  printer->println(c);  
  return *this;
}

PrintCascade& PrintCascade::println(const char c[])
{
  printer->println(c);
  return *this;
}

PrintCascade& PrintCascade::println(uint8_t b)
{
  printer->println(b);
  return *this;
}

PrintCascade& PrintCascade::println(int n)
{
  printer->println(n);
  return *this;
}

PrintCascade& PrintCascade::println(unsigned int n)
{
  printer->println(n);
  return *this;
}

PrintCascade& PrintCascade::println(long n)
{
  printer->println(n);  
  return *this;
}

PrintCascade& PrintCascade::println(unsigned long n)
{
  printer->println(n);  
  return *this;
}

PrintCascade& PrintCascade::println(long n, int base)
{
  printer->println(n,base);
  return *this;
}

PrintCascade& PrintCascade::println(double n)
{
  printer->println(n);
  return *this;
}
