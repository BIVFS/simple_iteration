#ifndef _MY_CALC_H_
#define _MY_CALC_H_

#include <vector>
#include <string>
#include <cstddef>

namespace my_calc
{

class Calculator
{
public:
     Calculator();

     ~Calculator();

     /// @brief     Ввод данных
     bool Input();

     /// @brief     Подготовка данных перед вычислением
     void PrepareData();

     /// @brief     Поиск всех констант из строки с уравнением
     void FindAllConst( const std::string& equation );

     /// @brief     Печать уравнения по номеру из массива констант
     /// @param[in] Номер уравнения в массиве констант
     void ShowEquation( const size_t& n );

     /// @brief     Печать уравнения по ссылке
     /// @param[in] Ссылка на уравнение
     void ShowEquation( const std::vector<std::pair<size_t, float> >& equation );

     /// @brief     Печать всех уравнений системы (печатает из массива констант)
     void ShowEquations();

     /// @brief     Печать системы уравнений
     void ShowSystem();

     /// @brief     Подготовка системы уравнений из массива констант
     void PrepareSystem();

     /// @brief     Преобразование системы для решения методом простых итераций
     void TransformSystem();

     /// @brief     Попытка привести уравнение к условию главной диагонали
     ///
     /// param[in]  equation уравнение
     /// param[in]  n номер уравнения в системе
     bool TryFixOnePass( std::vector<std::pair<size_t, float> > equation, size_t i );

     //TODO
     /// @brief     Грязная замена уравнения, которое привести не удается, на эквивалентное
     /// @param[in] n номер уравнения
     void DutySwap( const size_t n );

     /// @brief     Деление всех уравнений системы так, чтобы при корнях главной диагонали коэффициенты были = 1.
     ///            Иначе говоря приведение системы к виду: x1 = a2x2 + a3x3...; x2 = a1x1 + a3x3...; x3 = ... и т.д.
     ///            И проверка условия сходимости
     void DivisionSystem();

     /// @brief     Вычисление корней
     void Calc();

     /// @brief     Поиск уравнения в системе для преобразования уравнения n, коэффициенты которого не соответствуют
     ///            условию преобладания коэффициентов главной диагонали
     /// @param[in] n номер уравнения, для которого необходимо преобразование
     /// @param[in] num номер уравнения, подходящего для выполнения преобразования
     ///
     /// @return    Если необходимо словить уравнения - true, иначе - false
     /// @throw     Если не удалось найти подходящее уравнение бросит std::exception
     bool FindFor( size_t n, int& num );

     /// @brief     Проверка точности, достигнутой при вычислениях
     /// @return    Если заданная точность достигнута - true, иначе - false
     bool CheckAccuracy();

     /// @brief     Сортировка констант в соответствии с их номером и вставка 0-х констант, если они опущены в уравнении
     //TODO fix dups
     /// @param[in] n номер уравнения
     void SortAndAddNullRoots( const size_t& n );

     /// @brief     Вставка нулевых констант, если они опущены в уравнении
     void AddNullRoots();

     /// @brief     Получение ссылки на корень, найденный на предыдущей итерации
     /// @param[in] n Номер корня
     /// @return    Ссылка на корень
     float& OldRoot( const size_t& n ) { return roots_.at( n ).second.first; }

     /// @brief     Получение ссылки на корень, найденный на последней итерации
     /// @param[in] n Номер корня
     /// @return    Ссылка на корень
     float& NewRoot( const size_t& n ) { return roots_.at( n ).second.second; }

private:
     // Массив констант
     // Константы с соответствующими номерами корней, структура следующая:
     // Это двумерный массив, в котором построчно рахмещены ЭЛЕМЕНТЫ для каждого уравнения
     // Каждый элемент представляет собой пару значений (номер константы, константа)
     // После сортировки ЭЛЕМЕНТЫ упорядочены по возрастанию, а на 0-й ячейке записан свободный член, его номер 0, так
     // как корня при нем нет
     // Пример массива для системы уравнений:
     //
     //  | a11x1 + a12x2 + a13x3 = C1;
     // <  a21x1 + a22x2 + a23x3 = C2;
     //  | a31x1 + a32x2 + a33x3 = C3;
     //
     // <0, C1> <1, a11> <2, a12> <3, a13>
     // <0, C2> <1, a21> <2, a22> <3, a23>
     // <0, C3> <1, a31> <2, a32> <3, a33>
     std::vector<std::vector<std::pair<size_t, float> > > consts_;

     // Система уравнений
     // ВАЖНО не путать consts_ и system_, они похожи, но разница в том, что система уравнений отсортирована по принципу
     // диагонального преобладания, то есть при расчетах нужно работать именно с system_
     // Ещё одно отличие system_ от consts_, system_ - массив указателей на consts_, чтобы избежать избыточного
     // копирования, поэтому пока работает с system_ нельзя что-то менять в consts_
     std::vector<std::vector<std::pair<size_t, float> >* > system_;

     // Корни
     // Содержит номер корня и пары значений (корень с предыдущей итерации, корень с последней итерации)
     std::vector<std::pair<size_t, std::pair<float, float> > > roots_;

     // Точность
     float epsilon_;
};

} // namespace my_calc

#endif // _MY_CALC_H_
