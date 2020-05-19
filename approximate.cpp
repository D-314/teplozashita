#include "approximate.h"
#include "math.h"

approximate::approximate(const std::vector<double> x, const std::vector<double> y) {
    if ((x.size()==y.size()) && !x.empty()) {
        coefficient.clear();
        calculateCoefficients(x, y);
    }
}

float approximate::calculate(double x)
{
    double y = 0;
    if (!coefficient.empty()) {
        for (int i=0; i < (int)coefficient.size(); i++) {
            y+=coefficient[i]*pow(x,i);
        }
    }
    return y;
}

// Основная функция
bool approximate::calculateCoefficients(const std::vector<double> x, const std::vector<double> y)
{
    int m = x.size();
    double **mas;
    double *d;
    d = new double [m+1];

    mas = new double*[m];
    for (int k=0; k<= m; k++) {
        for (int i = 0; i<m; i++) {
            mas[i] = new double[m];
            for (int j = 0; j<m; j++) {
                if (j<m-1) mas[i][j] = pow(x[i],m-j-1);
                else mas[i][j] = 1;
            }
        }
        if (k>0) {
            for (int j = 0; j<m; j++) mas[j][k-1] = y[j];
        }
        d[k] = Determinant(mas, m);
        if (d[0] == 0) return false;
    }
    for (int k=m; k > 0; k--) coefficient.push_back(d[k]/d[0]);
    delete mas; delete d;
    return true;
}

// Рекурсивное вычисление определителя
double approximate::Determinant(double **mas, int m)
{
    int k = 1, n= m - 1;
    double **p, d = 0;
    p = new double*[m];
    for (int i = 0; i<m; i++)
        p[i] = new double[m];
    if (m<1) return 0;
    if (m == 1) {
        d = mas[0][0];
        return(d);
    }
    if (m == 2) {
        d = mas[0][0] * mas[1][1] - (mas[1][0] * mas[0][1]);
        return(d);
    }
    if (m>2) {
        for (int i = 0; i<m; i++) {
            GetMatr(mas, p, i, 0, m);
            d = d + k * mas[i][0] * Determinant(p, n);
            k = -k;
        }
    }
    return(d);
}

// Получение матрицы без i-й строки и j-го столбца
void approximate::GetMatr(double **mas, double **p, int i, int j, int m)
{
    int di = 0;
    for (int ki = 0; ki<m - 1; ki++) { // проверка индекса строки
        if (ki == i) di = 1;
        int dj = 0;
        for (int kj = 0; kj<m - 1; kj++) { // проверка индекса столбца
            if (kj == j) dj = 1;
            p[ki][kj] = mas[ki + di][kj + dj];
        }
    }
}
