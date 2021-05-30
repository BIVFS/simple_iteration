#include <iostream>

#include <my_calc.h>

int main()
try
{
     my_calc::Calculator calc;
     calc.Input();
     return EXIT_SUCCESS;
}
catch( const std::exception& error )
{
     std::cerr << __PRETTY_FUNCTION__ << ": " << error.what() << std::endl;
     return EXIT_FAILURE;
}
