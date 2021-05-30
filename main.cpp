#include <iostream>

#include <my_calc.h>

// 7x1+10.5x2-3x3+4x4=15 // должна быть такая исходня строка, но она ломает код, поэтому
//введена эквивалентная преобразованная
// 6.75x1+7x2+16x3=-4.1825
// 8x1-13x2+x3=8.33
// 10x1+2.5x2-4x3=20
// 2x1+0.1x3-7x4=-13.23

int main()
try
{
     my_calc::Calculator calc;
     calc.Input();
     calc.PrepareData();
     calc.Calc();
     return EXIT_SUCCESS;
}
catch( const std::exception& error )
{
     std::cerr << __PRETTY_FUNCTION__ << ": " << error.what() << std::endl;
     return EXIT_FAILURE;
}
