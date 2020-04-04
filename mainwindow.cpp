#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <math.h>
#include <map>
#define TEMP_MIN -50
#define TEMP_MAX 50

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Эрго-теплозащита");
    //    connect(this, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showPointToolTip(QMouseEvent*)));

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd.MM\nyyyy");
    ui->customPlot_temp->xAxis->setTicker(dateTicker);
}

MainWindow::~MainWindow()
{
    delete ui;
}

float MainWindow::dispersion(QVector<float> X, QVector<float> P) // функция для вычисления дисперсии
{
    /* Приравниваем начальное значение дисперсии к
     * бесконечности. В случае невозможности вычисления
     * дисперсии будет возвращена бесконечность*/
    float Dispersion = INFINITY, Mean = mean(X,P);
    if (X.size() == P.size() && Mean != INFINITY) {
        unsigned int N = X.size();
        Dispersion = 0;
        for (unsigned int i = 0; i < N; ++i) {
            Dispersion+=powf(X.at(i)-Mean,2)*P.at(i);
        }
    }
    return Dispersion;
}

float MainWindow::mean(QVector<float> X, QVector<float> P) // функция для вычисления математического ожидания
{
    /* Приравниваем начальное значение математического ожидания
     * к бесконечности. В случае невозможности вычисления
     * математического ожидания будет возвращена бесконечность*/
    float Mean = INFINITY;
    if (X.size() == P.size()) {
        unsigned int N = X.size();
        Mean = 0;
        for (unsigned int i = 0; i < N; ++i) {
            Mean+=X.at(i)*P.at(i);
        }
    }
    return Mean;
}

void MainWindow::prob_mass_func(QVector<float> X)
{
    std::map<int,int> PMF; // Probability mass function - Функция вероятности (плотность вероятности)
    P[0].clear();
    P[1].clear();
    for (unsigned int i = 0; i < (uint)X.size(); ++i) {
        if (PMF.find((int)round(X[i])) == PMF.end()) {
            PMF.insert(std::make_pair((int)round(X[i]),1));
        } else {
            PMF[X[i]]++;
        }
    }
#if !defined(TEMP_MIN) || !defined(TEMP_MAX)
#undef TEMP_MIN
#undef TEMP_MAX
    short int TEMP_MIN = -50;
    short int TEMP_MAX = 50;
#endif
    for (short int i = TEMP_MIN; i < TEMP_MAX; ++i) {
        if (PMF.find(i) != PMF.end()) {
            P[0].append(i);
            P[1].append((float)PMF[i]/(float)X.size());
        }
    }
}

QString MainWindow::file_open()
{
    QString str = "";
    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"),
                                                    QString(""),
                                                    QString("TXT (*.txt);;All files (*.*)")); //выбор файла
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        str = file.readAll(); //записываем файл в строку
    }

    file.close(); //закрываем файл
    return str;
}

MainWindow::Day MainWindow::read_string(QString string)
{
    Day current_day;
    QStringList value = string.split("\t");
    for (unsigned int i = 0; i < (uint)value.size(); ++i) {
        bool ok = false;
        current_day.read_ok = true;
        switch (i) {
        case 0: {
            QDate date = QDate::fromString(value.at(i),"dd.MM.yyyy");
            if (current_day.read_ok &= date.isValid()){
                current_day.date = date;
            }
            break;
        }
        case 1: {
            value.at(i).toFloat(&ok);
            if (current_day.read_ok &= ok){
                current_day.temp_out = value.at(i).toFloat(&ok);
            }
            break;
        }
        default:
            break;
        }
    }
    return current_day;
}

void MainWindow::onMouseMoveGraph(QMouseEvent *evt)
{
    /*    int x = this->ui->customPlot_prob_mass_func->xAxis->pixelToCoord(evt->pos().x());
    int y = this->ui->customPlot_prob_mass_func->yAxis->pixelToCoord(evt->pos().y());
    qDebug()<<"pixelToCoord: "<<data.key<<data.value; //this is correct when step is greater 1 second

    if (this->ui->customPlot_prob_mass_func->selectedGraphs().count()>0)
    {
        QCPGraph* graph = this->ui->customPlot_prob_mass_func->selectedGraphs().first();
        QCPData data = graph->data()->lowerBound(x).value();

        double dbottom = graph->valueAxis()->range().lower;        //Yaxis bottom value
        double dtop = graph->valueAxis()->range().upper;           //Yaxis top value
        long ptop = graph->valueAxis()->axisRect()->top();         //graph top margin
        long pbottom = graph->valueAxis()->axisRect()->bottom();   //graph bottom position
        // result for Y axis
        double valueY = (evt->pos().y() - ptop) / (double)(pbottom - ptop)*(double)(dbottom - dtop) + dtop;

        //or shortly for X-axis
        double valueX = (evt->pos().x() - graph->keyAxis()->axisRect()->left());  //graph width in pixels
        double ratio = (double)(graph->keyAxis()->axisRect()->right() - graph->keyAxis()->axisRect()->left()) / (double)(graph->keyAxis()->range().lower - graph->keyAxis()->range().upper);    //ratio px->graph width
        //and result for X-axis
        valueX=-valueX / ratio + graph->keyAxis()->range().lower;


        qDebug()<<"calculated:"<<valueX<<valueY;
    }*/
}

float MainWindow::G(float Q, float dT)
{
    return Q/dT;
}


void MainWindow::on_pushButton_load_temp_clicked()
{
    QString file = file_open();
    /*режем большую строку считанную из файла,
     * на маленькие строки содержащие переменные.
     * "split(10)" - разрезание строки по символу с
     * кодом 10 (это символ переноса на следующую строку)*/
    data.clear();
    QStringList string = file.split(10);
    file.clear(); file.~QString();
    for (unsigned int i = 0; i < (uint)string.size(); ++i) {
        data.append(read_string(string.at(i)));
    }
    /*---------------------разбить на 2 маленькие функции------------------------*/
    qDebug() << "№" << "[" << "ok" << "]:\t|" << "date" << "\t\t|" << "temp_out" << "\t|";
    for (unsigned int i = 0; i < (uint)data.size(); ++i) {
        qDebug() << i << "[" << (uint8_t)data.at(i).read_ok << "]:\t|" << data.at(i).date.toString("dd.MM.yyyy") << "\t|" << data.at(i).temp_out << "\t|";
    }
}

void MainWindow::on_pushButton_calculate_PMF_clicked()
{
    QVector<float> temp_out;
    for (unsigned int i = 0; i < (uint)data.size(); ++i) {
        temp_out.append(data.at(i).temp_ins-data.at(i).temp_out);
    }
    prob_mass_func(temp_out);
    /*----------- отрисовка граффика в отдельной функции ----------*/
    ui->customPlot_prob_mass_func->clearPlottables();
    QCPBars *bars1 = new QCPBars(ui->customPlot_prob_mass_func->xAxis, ui->customPlot_prob_mass_func->yAxis);
    bars1->setWidth(1);
    //    bars1->setPen(Qt::NoPen);
    //    bars1->setBrush(QColor(10, 140, 70, 160));
    for (unsigned int i = 0; i < (uint)P->size(); ++i) {
        qDebug() << P[0].at(i) << ":" << P[1].at(i);
        bars1->addData(P[0].at(i),P[1].at(i));
    }
    ui->customPlot_prob_mass_func->rescaleAxes();
    ui->customPlot_prob_mass_func->replot();
    /*----------- draw_PMF() {...}; ----------*/

    ui->customPlot_temp->clearGraphs();
    ui->customPlot_temp->addGraph();
    ui->customPlot_temp->xAxis->setLabel("Date");
    ui->customPlot_temp->xAxis->setRange(QDateTime(data.at(0).date).toTime_t(), QDateTime(data.at(data.size()-1).date).toTime_t());
    for (unsigned int i = 0; i < (uint)data.size(); ++i) {
        if(data.at(i).read_ok) {
            ui->customPlot_temp->graph(0)->addData(QDateTime(data.at(i).date).toTime_t(),data.at(i).temp_ins-data.at(i).temp_out);
        }
    }
    ui->customPlot_temp->rescaleAxes();
    ui->customPlot_temp->replot();

    /*----------- draw_temp() {...}; ----------*/

    ui->statusbar->showMessage(QString("mean: "+QString::number(mean(P[0],P[1]))+";\tdispersion:")+QString::number(dispersion(P[0],P[1])));
}
