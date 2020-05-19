#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "temperature_data.h"
#include <QXmlStreamReader>
#include <QStack>

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
//    void on_pushButton_load_temp_clicked();

//    void on_pushButton_calculate_PMF_clicked();

    void on_action_temperatureHistory_triggered();

    void on_action_consumptionReport_triggered();

    void on_action_clear_triggered();

    void on_pushButton_calculate_PMF_clicked();

private:
    Ui::MainWindow *ui;

    temperature_data tempData;

    /*математическое ожидание*/
    float mean(QVector<float> X, QVector<float> P);

    /*дисперсия*/
    float dispersion(QVector<float> X, QVector<float> P);

    /*плотность вероятностей*/
    void probMassFunc(QVector<float> X);

    QVector<float> P[2]; //глобальные переменные, в которых хранится информация

    QString fileOpen(const QString type);

    void onMouseMoveGraph(QMouseEvent* evt);

    void loadStringHistory(const QString string);
};
#endif // MAINWINDOW_H
