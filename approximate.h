#ifndef APPROXIMATE_H
#define APPROXIMATE_H
#include "vector"

class approximate
{
public:
    //коэффициенты аппроксимирующего полинома
    std::vector<double> coefficient{0};

    //инициализация с подсчетом коэффициентов
    approximate(const std::vector<double> x, const std::vector<double> y);

    //аппроксимирующая функция
    float calculate(double x);

private:

    //рассчет коэфициентов
    bool calculateCoefficients(const std::vector<double> x, const std::vector<double> y);

    double Determinant(double **mas, int m);

    void GetMatr(double **mas, double **p, int i, int j, int m);

};

#endif // APPROXIMATE_H
