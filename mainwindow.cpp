#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "approximate.h"
#include <QErrorMessage>
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
    ui->customPlot_prob_mass_func->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);
    ui->customPlot_prob_mass_func->axisRect()->setupFullAxesBox();
    connect( ui->customPlot_prob_mass_func->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot_prob_mass_func->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->customPlot_prob_mass_func->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot_prob_mass_func->yAxis2, SLOT(setRange(QCPRange)));
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

void MainWindow::probMassFunc(QVector<float> X)
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

QString MainWindow::fileOpen(const QString type)
{
    QString str = "";
    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"),
                                                    QString(""),
                                                    type); //выбор файла
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        str = file.readAll(); //записываем файл в строку
    }
    file.close(); //закрываем файл
    return str;
}

void MainWindow::loadStringHistory(const QString string)
{
    QStringList value = string.split("\t");
    QDate date; float TemperatureOutside; bool readOk = true;
    for (unsigned int i = 0; i < (uint)value.size(); ++i) {
        switch (i) {
        case 0: {
            date = QDate::fromString(value.at(i),"dd.MM.yyyy");
            readOk &= date.isValid();
            break;
        }
        case 1: {
            bool ok = false;
            value.at(i).toFloat(&ok);
            if (readOk &= ok){
                TemperatureOutside = value.at(i).toFloat(&ok);
            }
            break;
        }
        default:
            break;
        }
    }
    if (readOk) tempData.writeTemperatureOutside(date,TemperatureOutside);
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

//void MainWindow::on_pushButton_calculate_PMF_clicked()
//{
//    QVector<float> temp_out;
//    for (unsigned int i = 0; i < (uint)data.size(); ++i) {
//        temp_out.append(data.at(i).temp_ins-data.at(i).temp_out);
//    }
//    probMassFunc(temp_out);
//    /*----------- отрисовка граффика в отдельной функции ----------*/
//    ui->customPlot_prob_mass_func->clearPlottables();
//    QCPBars *bars1 = new QCPBars(ui->customPlot_prob_mass_func->xAxis, ui->customPlot_prob_mass_func->yAxis);
//    bars1->setWidth(1);
//    //    bars1->setPen(Qt::NoPen);
//    //    bars1->setBrush(QColor(10, 140, 70, 160));
//    for (unsigned int i = 0; i < (uint)P->size(); ++i) {
//        qDebug() << P[0].at(i) << ":" << P[1].at(i);
//        bars1->addData(P[0].at(i),P[1].at(i));
//    }
//    ui->customPlot_prob_mass_func->rescaleAxes();
//    ui->customPlot_prob_mass_func->replot();
//    /*----------- draw_PMF() {...}; ----------*/

//    /*ui->customPlot_temp->clearGraphs();
//    ui->customPlot_temp->addGraph();
//    ui->customPlot_temp->xAxis->setLabel("Date");
//    ui->customPlot_temp->xAxis->setRange(QDateTime(data.at(0).date).toTime_t(), QDateTime(data.at(data.size()-1).date).toTime_t());
//    for (unsigned int i = 0; i < (uint)data.size(); ++i) {
//        if(data.at(i).read_ok) {
//            ui->customPlot_temp->graph(0)->addData(QDateTime(data.at(i).date).toTime_t(),data.at(i).temp_ins-data.at(i).temp_out);
//        }
//    }
//    ui->customPlot_temp->rescaleAxes();
//    ui->customPlot_temp->replot();*/

//    /*----------- draw_temp() {...}; ----------*/

//    ui->statusbar->showMessage(QString("mean: "+QString::number(mean(P[0],P[1]))+";\tdispersion:")+QString::number(dispersion(P[0],P[1])));
//}



void MainWindow::on_action_temperatureHistory_triggered()
{
    QString file = fileOpen(QString("TXT (*.txt);;All files (*.*)"));
    /*режем большую строку считанную из файла,
     * на маленькие строки содержащие переменные.
     * "split(10)" - разрезание строки по символу с
     * кодом 10 (это символ переноса на следующую строку)*/
    QStringList string = file.split(10);
    file.clear(); file.~QString();
    for (int i = 0; i < string.size(); ++i) {
        loadStringHistory(string.at(i));
    }
    /*---------------------разбить на 2 маленькие функции------------------------*/
    qDebug() << "№" << "[" << "ok" << "]:\t|" << "date" << "\t\t|" << "temp_out" << "\t|";
    for (std::map<QDate,temperature_data::oneDay>::iterator it = tempData.data.begin();it != tempData.data.end();it++) {
        qDebug() << it->first.toString("dd.MM.yyyy") << "]:\t|" << it->second.temperatureInside << "\t|" "\t|" << it->second.temperatureOutside << "\t|";
    }
}


/*боролся с XML-ом как мог. в итоге подбором нашел способ считвать нужные значения*/
void MainWindow::on_action_consumptionReport_triggered()
{
    QString file = fileOpen(QString("XML (*.xml);;All files (*.*)"));
    QXmlStreamReader xmlReader;
    xmlReader.addData(file);
    file.clear(); file.~QString();
    QStack<QTreeWidgetItem*> widgetStack;
    bool nameLoaded = false;
    while (!xmlReader.atEnd()) {
        if (!nameLoaded){
            if (xmlReader.name() == "Data") {
                if (xmlReader.readElementText() == "Узел учета:") {
                    xmlReader.readNext();
                    while (xmlReader.name() != "Data") xmlReader.readNext();
                    QString name = xmlReader.readElementText();
                    nameLoaded = true;
                    if (tempData.object == "") tempData.object = name;
                    else if (tempData.object != name) {
                        QMessageBox::critical(this, "Объект изменился", "Выберите файл для "+tempData.object+"и повторите попытку");
                        return;
                    }
                }
            }
        }
        foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
            if (attr.value().toString() == "DateTime") {
                QString day = xmlReader.readElementText();
                day.truncate(10);
                for (int i = 0; i < 43; ++i) {
                    xmlReader.readNext();
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                        if (attr.value().toString() == "Number") {
                            QString buff = xmlReader.readElementText();
                            if (i==41) tempData.writeThermalEnergy(QDate::fromString(day,"yyyy-MM-dd"),buff.toFloat());
                        }
                    }
                }
            }
        }
        xmlReader.readNext();
    }
    /*---------------------разбить на 2 маленькие функции------------------------*/
    qDebug() << "№" << "[" << "ok" << "]:\t|" << "date" << "\t\t|" << "temp_out" << "\t|";
    for (std::map<QDate,temperature_data::oneDay>::iterator it = tempData.data.begin();it != tempData.data.end();it++) {
        qDebug() << it->first.toString("dd.MM.yyyy") << "]:\t|" << it->second.thermalEnergy << "\t|" << it->second.temperatureInside <<"\t|" << it->second.temperatureOutside << "\t|";
    }
    qDebug() << "отчет загружен";
}

void MainWindow::on_action_clear_triggered()
{
    tempData.data.clear();
    tempData.object = "";

}

void MainWindow::on_pushButton_calculate_PMF_clicked()
{
    QVector<float> x,y;
    ui->customPlot_prob_mass_func->clearGraphs();
    ui->customPlot_prob_mass_func->addGraph();
    ui->customPlot_prob_mass_func->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot_prob_mass_func->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    for (std::map<QDate,temperature_data::oneDay>::iterator it = tempData.data.begin();it != tempData.data.end();it++) {
        if ((it->second.thermalEnergy != 0) && it->second.temperatureOutside != INFINITY) {
            ui->customPlot_prob_mass_func->graph(0)->addData(tempData.deltaTemperature(it->first),tempData.kilowattHour(it->first));
            x.append(tempData.deltaTemperature(it->first)); y.append(tempData.kilowattHour(it->first));
            qDebug() << it->first.toString("dd.MM.yyyy") << "]:\t|" << it->second.thermalEnergy << "\t|" << it->second.temperatureInside <<"\t|" << it->second.temperatureOutside << "\t|"<< tempData.thermalEnergyPerHour(it->first) << "\t|"<< tempData.power(it->first) << "\t|" << tempData.deltaTemperature(it->first) << "\t|"<< tempData.kilowattHour(it->first) << "\t|";
        }
    }
    for (int i = 0; i < x.size(); i++) {
        for (int j = 0; j < x.size(); j++) {
            if (x[j] > x[j + 1]) {
                // меняем элементы местами
                float temp = x[j];
                x[j] = x[j + 1];
                x[j + 1] = temp;
                temp = y[j];
                y[j] = y[j + 1];
                y[j + 1] = temp;
            }
        }
    }
    int polinom = ui->spinBox_polynomialDegree->value();

    x.remove(0); y.remove(0);

    std::vector<double> averageX, averageY;
    for (int i = 0; i <= polinom;i++) {
        averageX.push_back(0); averageY.push_back(0);
//        for (int j = i*x.size()/(polinom+1); j < (i+1)*x.size()/(polinom+1); j++) {
            averageX[i] = x[i*(x.size())/(polinom)];
            averageY[i] = y[i*(y.size())/(polinom)];
//        }
//        averageX[i]/=(x.size()/(polinom+1));
//        averageY[i]/=(y.size()/(polinom+1));
    }
    approximate approx(averageX,averageY);
    ui->customPlot_prob_mass_func->addGraph();
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    pen.setColor(Qt::red);
    ui->customPlot_prob_mass_func->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->customPlot_prob_mass_func->graph(1)->setPen(pen);
    for (int i = x[0]; i <= x[x.size()-1]; ++i) {
        ui->customPlot_prob_mass_func->graph(1)->addData(i, approx.calculate(i));
    }
    qDebug() << x << y;
    ui->customPlot_prob_mass_func->rescaleAxes();
    ui->customPlot_prob_mass_func->replot();
}
