#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_load_temp_clicked();

    void on_pushButton_calculate_PMF_clicked();

private:
    Ui::MainWindow *ui;

    float G(float Q, float dT);

    /*математическое ожидание*/
    float mean(QVector<float> X, QVector<float> P);

    /*дисперсия*/
    float dispersion(QVector<float> X, QVector<float> P);

    /*плотность вероятностей*/
    void prob_mass_func(QVector<float> X);

    struct Day { //данные за 1 день
        QDate date; //число
        float q = 0;
        float temp_out = 0; //гисметео
        float temp_ins = 24; //температура в помещении
        bool read_ok = false; //если параметры считаны успешно?
    };

    QVector<Day> data; //массив со всеми днями

    QVector<float> P[2]; //глобальные переменные, в которых хранится информация

    QString file_open();

    Day read_string (QString string);

    void onMouseMoveGraph(QMouseEvent* evt);
};
#endif // MAINWINDOW_H
