#include <my_calc.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <climits>
#include <cmath>

#include <thread>
#include <chrono>

namespace
{

// trim from start (in place)
void ltrim( std::string& s )
{
     s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( unsigned char ch ) { return !std::isspace( ch ); } ) );
}

// trim from end (in place)
void rtrim( std::string& s )
{
     s.erase( std::find_if( s.rbegin(), s.rend(), []( unsigned char ch ) { return !std::isspace( ch ); } ).base(), s.end() );
}

// trim from both ends (in place)
void trim( std::string& s )
{
     ltrim(s);
     rtrim(s);
}

void trimAllSpace( std::string& str )
{
     for( int i = 0; i < static_cast<int>( str.size() ); ++i )
     {
          if( std::isspace( str[i] ) )
          {
               str.erase( i, 1 );
               if( 0 >= i )
               {
                    --i;
               }
          }
     }
}

void StringtoLower( std::string& str )
{
     std::ostringstream oss;
     oss << std::nouppercase << str;
     str = oss.str();
}

float CalcSum( const std::vector<std::pair<size_t, float> >& equation, size_t n )
{
     float summ = 0;
     for( size_t j = 1; j < equation.size(); ++j )
     {
          // n + 1, т.к. константы при корнях записаны с 1 элемента, а не с 0
          if( j != n + 1 )
          {
               summ += std::fabs( equation[j].second );
          }
     }
     return summ;
}

std::vector<std::pair<size_t, float> > AdditionEquations( std::vector<std::pair<size_t, float> >& equation1,
     std::vector<std::pair<size_t, float> >& equation2 )
{
     std::vector<std::pair<size_t, float> > result( equation1 );
     for( size_t j = 0; j < result.size(); ++j )
     {
          result[j].second += equation2[j].second;
     }
     return result;
}

std::vector<std::pair<size_t, float> > SubtractionEquations( std::vector<std::pair<size_t, float> >& equation1,
     std::vector<std::pair<size_t, float> >& equation2 )
{
     std::vector<std::pair<size_t, float> > result( equation1 );
     for( size_t j = 0; j < result.size(); ++j )
     {
          result[j].second -= equation2[j].second;
     }
     return result;
}

} // anonymous namespace

namespace my_calc
{

Calculator::Calculator()
: epsilon_( std::pow( 10, -4 ) )
{
}

Calculator::~Calculator()
{
}

bool Calculator::Input()
{
     //TODO тут отладка, чтобы не вводить руками, нужно закоментировать
     std::stringstream ss;
     ss
          << "10x1+2.5x2-4x3=20" << std::endl
          << "8x1-13x2+x3=8.33" << std::endl
          << "7x1+10.5x2-3x3+4x4=15" << std::endl // "6.75x1+7x2+16x3=-4.1825" на всякий случай оставил преобразованную строку
          << "2x1+0.1x3-7x4=-13.23" << std::endl
          ;
     std::string equation;
     std::cout << "Enter the first equation in the format: A1X1 + A2X2 + A3X3 + ... + AnXn = C" << std::endl;
     while( true )
     {
          //TODO
          //getline( std::cin, equation );
          getline( ss, equation );    // Отладка
          if( equation.empty() )
          {
               break;
          }

          trimAllSpace( equation );
          StringtoLower( equation );
          FindAllConst( equation );
          std::cout << "Enter the next equation or press <Enter> to end" << std::endl;
     }

     return true;
}

void Calculator::PrepareData()
{
     AddNullRoots();
     PrepareSystem();

     std::cout << "Source system" << std::endl;
     ShowSystem();

     TransformSystem();

     //std::cout << std::endl;
     //ShowSystem();

     DivisionSystem();
     std::cout << std::endl;
     std::cout << "Normalized system" << std::endl;
     ShowSystem();
     std::cout << std::endl;
}

void Calculator::ShowEquation( const size_t& n )
{
     for( size_t i = 1; i < consts_[n].size(); ++i )
     {
          if( 1 != i && consts_[n][i].second >= 0 )
          {
               std::cout << "+";
          }
          if( 1 != consts_[n][i].second && -1 != consts_[n][i].second )
          {
               std::cout << consts_[n][i].second;
          }
          if( -1 == consts_[n][i].second )
          {
               std::cout << "-";
          }
          std::cout << 'x' << consts_[n][i].first;
     }
     std::cout << "=" << consts_[n][0].second << std::endl;
}

void Calculator::ShowEquation( const std::vector<std::pair<size_t, float> >& equation )
{
     for( size_t i = 1; i < equation.size(); ++i )
     {
          if( 1 != i && equation[i].second >= 0 )
          {
               std::cout << "+";
          }
          if( 1 != equation[i].second && -1 != equation[i].second )
          {
               std::cout << equation[i].second;
          }
          if( -1 == equation[i].second )
          {
               std::cout << "-";
          }
          std::cout << 'x' << equation[i].first;
     }
     std::cout << "=" << equation[0].second << std::endl;
}

void Calculator::ShowEquations()
{
     for( size_t i = 0; i < consts_.size(); ++i )
     {
          ShowEquation( i );
     }
}

void Calculator::ShowSystem()
{
     for( size_t i = 0; i < system_.size(); ++i )
     {
          ShowEquation( *system_[i] );
     }
}

void Calculator::FindAllConst( const std::string& equation )
{
     if( equation.empty() )
     {
          return;
     }
     consts_.emplace_back( std::vector<std::pair<size_t, float> >() );
     size_t count = 0;
     size_t posBegin = 0;
     size_t posEnd = 0;
     bool negativeVal = false;
     bool nextNegative = false;
     while( true )
     {
          if( nextNegative )
          {
               negativeVal = true;
               nextNegative = false;
          }
          const size_t posPlus = equation.find( '+' , posBegin );
          const size_t posMinus = equation.find( '-', posBegin );
          if( posMinus == posBegin )
          {
               nextNegative = true;
               ++posBegin;
               continue;
          }
          if( posPlus == std::string::npos && posMinus == std::string::npos )
          {
               size_t posEqual = equation.find( '=', posBegin );
               if( std::string::npos == posEqual )
               {
                    break;
               }
               if( posEqual + 1 > equation.length() - 1 )
               {
                    std::cerr << "Invalid syntax of equation" << std::endl;
               }
               else
               {
                    std::string buffer = equation.substr( posEqual + 1, equation.length() - 1 );
                    float value = 0;
                    if( 1 != sscanf( buffer.c_str(), "%f", &value ) )
                    {
                         std::cerr << "Invalid syntax of equation" << std::endl;
                    }
                    consts_.back().insert( consts_.back().begin(), std::make_pair( 0, value ) );
               }
               posEnd = posEqual;
          }
          else if( std::string::npos == posPlus )
          {
               posEnd = posMinus;
               nextNegative = true;
          }
          else if( std::string::npos == posMinus )
          {
               posEnd = posPlus;
          }
          else
          {
               if( posPlus < posMinus )
               {
                    posEnd = posPlus;
               }
               else
               {
                    posEnd = posMinus;
                    nextNegative = true;
               }
          }
          ++count;
          std::string constAndRoot = equation.substr( posBegin, posEnd - posBegin );
          float constVar = 0;
          int rootNum = 0;
          if( 2 != sscanf( constAndRoot.c_str(), "%fx%d", &constVar, &rootNum ) )
          {
               if( 1 != sscanf( constAndRoot.c_str(), "x%d", &rootNum ) )
               {
                    std::cerr << "Invalid syntax in one of roots" << std::endl;
               }
               else
               {
                    constVar = 1;
               }
          }

          consts_.back().emplace_back( std::make_pair( rootNum, ( negativeVal ) ? ( -1 ) * constVar : constVar ) );
          negativeVal = false;
          posBegin = posEnd + 1;
     }

     SortAndAddNullRoots( consts_.size() - 1 );
}

void Calculator::SortAndAddNullRoots( const size_t& n )
{
     for( size_t i = 1; i < consts_[n].size(); ++i )
     {
          if( i != consts_[n][i].first )
          {
               bool wasFind = false;
               for( size_t j = 1; j < consts_[n].size(); ++j )
               {
                    if( i == consts_[n][j].first )
                    {
                         consts_[n].insert( consts_[n].begin() + i, consts_[n][j] );
                         consts_[n].erase( ( j > i ) ? consts_[n].begin() + j + 1 : consts_[n].begin() + j );
                         wasFind = true;
                         break;
                    }
               }
               if( !wasFind )
               {
                    consts_[n].insert( consts_[n].begin() + i, std::make_pair( i, 0 ) );
               }
          }
     }
}

void Calculator::AddNullRoots()
{
     if( 2 > consts_.size() )
     {
          return;
     }

     size_t maxDegree = 0;
     for( size_t i = 0; i < consts_.size(); ++i )
     {
          maxDegree = std::max( consts_[i].size(), consts_[i - 1].size() );
     }
     for( auto& i : consts_ )
     {
          for( size_t j = 1; j < maxDegree; ++j )
          {
               if( i.size() - 1 < j )
               {
                    i.push_back( std::make_pair( j, 0 ) );
               }
               else if( i[j].first != j )
               {
                    i.insert( i.begin() + j, std::make_pair( j, 0 ) );
               }
          }
     }
}

void Calculator::PrepareSystem()
{
     // Проверяем consts_[0] потому что матрица квадратная, но по хорошему надо проверять каждый
     // Потому что тут необходимо идти по столбцам
     for( size_t j = 1; j < consts_[0].size(); ++j )
     {
          float maxConst = 0;
          size_t index = 0;
          for( size_t i = 0; i < consts_.size(); ++i )
          {
               if( std::fabs( maxConst ) < std::fabs( consts_[i][j].second ) )
               {
                    if( system_.end() == std::find( system_.begin(), system_.end(), &consts_[i] ) )
                    {
                         maxConst = std::fabs( consts_[i][j].second );
                         index = i;
                    }
               }
          }
          system_.emplace_back( &consts_[index] );
     }
}

void Calculator::TransformSystem()
{
     for( size_t i = 0; i < system_.size(); ++i )
     {
          float summ = CalcSum( *system_[i], i );
          if( std::fabs( system_[i]->at( i + 1 ).second ) >= summ )
          {
               continue;
          }
          if( !TryFixOnePass( *system_[i], i ) )
          {
               DutySwap( i );
          }
     }

     for( size_t i = 0; i < system_.size(); ++i )
     {
          float summ = CalcSum( *system_[i], i );
          if( std::fabs( system_[i]->at( i + 1 ).second ) < summ )
          {
               // Не удалось привести систему уравнений к нормальному виду, разрешив ее относительно диагональных неизвестных
               throw std::logic_error( "Transform system failed" );
          }
     }
}

bool Calculator::TryFixOnePass( std::vector<std::pair<size_t, float> > equation, size_t i )
{
     float maxVar = 0;
     size_t index = 0;
     for( size_t j = 1; j < equation.size(); ++j )
     {
          if( i + 1 == j )
          {
               continue;
          }
          if( std::fabs( equation[j].second ) > std::fabs( maxVar ) )
          {
               index = j;
               maxVar = equation[j].second;
          }
     }
     double aimVar = equation[index].second;
     if( 0 == index )
     {
          // Не нашли наибольший элемент по модулю, не считая элемент главной диагонали
          throw std::logic_error( "Can not find max element" );
     }

     for( size_t j = 0; j < equation.size(); ++j )
     {
          if( i + 1 == index )
          {
               continue;
          }
          equation[j].second = equation[j].second
               - system_[index - 1]->at( j ).second * aimVar / system_[index - 1]->at( index ).second;
     }

     // Проверка условия диагоналя для текущего уравнения
     float summ = CalcSum( equation, i );
     if( std::fabs( equation[i + 1].second ) >= summ )
     {
          system_[i]->swap( equation );
          return true;
     }
     else
     {
          return false;
     }
}

void Calculator::DutySwap( const size_t n )
{
     std::vector<std::vector<std::pair<size_t, float> > > system( system_.size() );
     for( size_t i = 0; i < system.size(); ++i )
     {
          system[i] = *system_[i];
     }
     for( size_t k = 0; k < system_.size(); ++k )
     {
          for( size_t i = 0; i < system.size(); ++i )
          {
               double aimVar = system[i][k + 1].second;
               if( 0 == system[i][k + 1].second )
               {
                    continue;
               }
               for( size_t j = 0; j < system[i].size(); ++j )
               {
                    if( i == k )
                    {
                         continue;
                    }
                    system[i][j].second = system[i][j].second - system[k][j].second * aimVar / system[k][k + 1].second;
               }
          }
     }
     system_[n]->swap( system[n] );
}

bool Calculator::FindFor( size_t n, int& num )
{
     for( size_t i = 0; i < system_.size(); ++i )
     {
          if( i == n )
          {
               continue;
          }
          float oldSumm = CalcSum( *system_[n], n );
          std::vector<std::pair<size_t, float> > val = AdditionEquations( *system_[n], *system_[i] );
          float newSumm = CalcSum( val, n );
          if( ( std::fabs( system_[n]->at( n + 1 ).second ) - oldSumm ) < ( std::fabs( val[n + 1].second ) - newSumm ) )
          {
               num = i;
               return true;
          }
          SubtractionEquations( *system_[n], *system_[i] ).swap( val );
          newSumm = CalcSum( val, n );
          if( ( std::fabs( system_[n]->at( n + 1 ).second ) - oldSumm ) < ( std::fabs( val[n + 1].second ) - newSumm ) )
          {
               num = -i;
               return false;
          }
     }
     throw;
}

void Calculator::DivisionSystem()
{
     for( size_t i = 0; i < system_.size(); ++i )
     {
          for( size_t j = 0; j < system_.size(); ++j )
          {
               if( i + 1 == j )
               {
                    continue;
               }
               else if( 0 != system_[i]->at( j ).second )
               {
                    system_[i]->at( j ).second = ( system_[i]->at( j ).second / system_[i]->at( i + 1 ).second );
               }
               if( 0 != j )
               {
                    system_[i]->at( j ).second *= -1;
               }
          }
          system_[i]->at( i + 1 ).second = 1;
     }

     roots_.resize( system_.size(), std::make_pair( 0, std::make_pair( 0, 0 ) ) );
     for( size_t i = 0; i < roots_.size(); ++i )
     {
          roots_[i].first = i + 1;
          NewRoot( i ) = system_[i]->at( 0 ).second;
     }

     for( size_t i = 0; i < system_.size(); ++i )
     {
          float summ = CalcSum( *system_[i], i );
          if( 1 < summ )
          {
               // Условие сходимости ( сумма коэффициентов при переменных, не принадлежащей главной диагонали <= 1 ) не выполняется
               throw std::logic_error( "Checking the convergence condition failed" );
          }
     }
}

void Calculator::Calc()
{
     size_t n = 0;
     do
     {
          ++n;
          for( size_t i = 0; i < system_.size(); ++i )
          {
               OldRoot( i ) = NewRoot( i );
               NewRoot( i ) = system_[i]->at( 0 ).second;
               for( size_t j = 1; j < system_[i]->size(); ++j )
               {
                    if( i + 1 != j )
                    {
                         NewRoot( i ) += system_[i]->at( j ).second * OldRoot( j - 1 );
                    }
               }
          }
     } while( !CheckAccuracy() );

     std::vector<std::pair< size_t, std::pair<float, float> > > tmpRoots = roots_;
     std::vector<std::pair<size_t, float> > outRoots;
     std::pair<size_t, float> tmp;
     while( !tmpRoots.empty() )
     {
          tmp = std::make_pair( 0, 0 );
          size_t n = 0;
          for( size_t i = 0; i < tmpRoots.size(); ++i )
          {
               if( NewRoot( i ) > tmp.second )
               {
                    tmp.second = tmpRoots[i].second.second;
                    tmp.first = tmpRoots[i].first;
                    n = i;
               }
          }
          outRoots.emplace_back( tmp );
          tmpRoots.erase( tmpRoots.begin() + n );
     }
     std::cout << "Number of iterations: " << n << std::endl;

     for( int i = outRoots.size() - 1; i >= 0; --i )
     {
          std::cout << "x" << outRoots[i].first << " = " << outRoots[i].second << std::endl;
     }
}

bool Calculator::CheckAccuracy()
{
     for( size_t i = 0; i < roots_.size(); ++i )
     {
          if( std::fabs( OldRoot( i ) - NewRoot( i ) ) >= epsilon_ )
          {
               return false;
          }
     }
     return true;
}

} // namespace my_calc
