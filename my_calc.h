#ifndef _MY_CALC_H_
#define _MY_CALC_H_

#include <vector>   // std::vector
#include <string>
#include <cstddef>  // size_t

namespace my_calc
{

class Calculator
{
public:
     Calculator();

     ~Calculator();

     bool Input();

     void FindAllConst( const std::string& equation );

     void ShowEquation( const size_t& n );

     void ShowEquation( const std::vector<std::pair<size_t, float> >& equation );

     void ShowEquations();

     void ShowSystem();

     void PrepareSystem();

     void TransformSystem();
     void Transformation();

     void DivisionSystem();

     void Calc();


     void Fix();
     bool FindFor( size_t n, int& num );

     bool CheckAccuracy();

     //TODO fix dups
     void AddNullRoots( const size_t& n );
     void AddNullRoots();

     float& OldRoot( const size_t& n ) { return roots_.at( n ).first; }
     float& NewRoot( const size_t& n ) { return roots_.at( n ).second; }

private:
     std::vector<std::vector<std::pair<size_t, float> > > consts_;

     std::vector<std::vector<std::pair<size_t, float> >* > system_;

     std::vector<std::pair<float, float> > roots_;

     float epsilon_;
};

} // namespace my_calc

#endif // _MY_CALC_H_
