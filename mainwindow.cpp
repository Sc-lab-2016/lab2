#include "mainwindow.h"
#include "ui_supervisorio.h"
#include <QtCore/qmath.h>
#include <QToolTip>
#include <cmath>


supervisorio::supervisorio(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::supervisorio)
{
    /* +++++++++++++++++++++++++++++++++++++++++++++++++
     * BEGIN - Flags
     * +++++++++++++++++++++++++++++++++++++++++++++++++
     */

    /* -----------------------------------------------
     * END - Flags
     * -----------------------------------------------
     */

    /* +++++++++++++++++++++++++++++++++++++++++++++++++
     * BEGIN - Vincula e exibe interface
     * +++++++++++++++++++++++++++++++++++++++++++++++++
     */

    ui->setupUi(this);
    //QMainWindow::showFullScreen();

    qApp->setStyleSheet("QGroupBox { border: 0px solid gray;}");

    plotRange = 60;
    plotRange = 300;
    ui->spinBox->setValue(plotRange);
    ui->scaleValue->setValue(plotRange);

    // Setup plots
    setupPlot1(ui->customPlot);
    setupPlot2(ui->customPlot2);

    //Inicialização para o plot randômico
    timeToNextRandomNumber=0;

    /* -----------------------------------------------
     * END - Vincula e exibe interface
     * -----------------------------------------------
     */

    /* +++++++++++++++++++++++++++++++++++++++++++++++++
     * BEGIN - Thread init
     * +++++++++++++++++++++++++++++++++++++++++++++++++
     */

    cThread = new commThread(this);
    connect(cThread,SIGNAL(plotValues(double, double, double, double, double, double, double, double, double, double, double, double)),this,SLOT(onPlotValues(double, double, double, double, double, double, double, double, double, double, double, double)));

    /* -----------------------------------------------
     * END - Thread init
     * -----------------------------------------------
     */


    /* +++++++++++++++++++++++++++++++++++++++++++++++++
     * BEGIN - Valores para popular a interface grafica
     * +++++++++++++++++++++++++++++++++++++++++++++++++
     */
    //Inicializa valores
    frequencia = 20;
    amplitude = 5;
    offset = 15;
    duracaoMax = 3;
    duracaoMin = 1;
    wave = 0;
    nextWave = wave;

    //controladores
    taw[0] = 75;
    taw[1] = 75;

    cascade = false;

    //mestre
    control[0] = P;
    nextControl[0] = control[0];
    kp[0] = 1;
    ki[0] = 0;
    kd[0] = 0;
    windup[0] = false;
    conditionalIntegration[0] = false;

    //escravo
    control[1] = P;
    nextControl[1] = control[1];
    kp[1] = 2;
    ki[1] = 0.05;
    kd[1] = 0.005;
    windup[1] = false;
    conditionalIntegration[1] = false;



        /* -----------------------------------------------
     * END - Valores para popular a interface grafica
     * -----------------------------------------------
     */

    /* +++++++++++++++++++++++++++++++++++++++++++++++++
     * BEGIN - Set valores na interface grafica
     * +++++++++++++++++++++++++++++++++++++++++++++++++
     */
    //Frequencia
    // Configura sinal de controle
    int index = ui->comboBox->findText("Período (s)");
    ui->comboBox->setCurrentIndex(index);
    ui->doubleSpinBox->setValue(frequencia);
    //Amplitude
    ui->doubleSpinBox_2->setValue(amplitude);
    //Offset
    ui->doubleSpinBox_3->setValue(offset);
    //Max
    ui->doubleSpinBox_4->setValue(duracaoMax);
    //Min
    ui->doubleSpinBox_5->setValue(duracaoMin);

    // Configura wave padrao
    //ui->radioButton_11->setChecked(true);
    ui->comboBox_6->setCurrentIndex(wave);
    on_comboBox_6_currentIndexChanged(wave);//Degrau

    // Configura sinal de controle
    index = ui->comboBox_tipoControle->findText("P");
    ui->comboBox_tipoControle->setCurrentIndex(index);


    ui->doubleSpinBox_6->setValue(kp[0]);
    ui->doubleSpinBox_7->setValue(ki[0]);
    ui->doubleSpinBox_8->setValue(kd[0]);



    // Wind Up
    index = ui->comboBox_windup->findText("Sem");
    ui->comboBox_windup->setCurrentIndex(index);


    // Configura canal de escrita padrao (bomba)
    channel = 0;





    //Carrega valores da interface para cthread
    supervisorio::on_pushButton_8_clicked();//Atualiza valores (evita bug no demo)


}

supervisorio::~supervisorio()
{
    delete ui;
}

void supervisorio::setupPlot1(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "Você deve usar a versão > 4.7");
#endif

  plot1Enable[0]=true;//Blue Enabled
  plot1Enable[1]=true;//Red Enabled
  plot1Enable[2]=true;//Green Enabled
  plot1Enable[3]=true;//Orange Enabled
  plot1Enable[4]=true;//Red2 Enabled
  plot1Enable[5]=true;//Green2 Enabled
  plot1Enable[6]=true;//Orange2 Enabled


  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  customPlot->graph(0)->setAntialiasedFill(false);
  customPlot->addGraph(); // red line
  customPlot->graph(1)->setPen(QPen(Qt::red));

  customPlot->addGraph(); // blue dot
  customPlot->graph(2)->setPen(QPen(Qt::blue));
  customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph(3)->setPen(QPen(Qt::red));
  customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

  //PARTE NOVA
  customPlot->addGraph(); // green line
  customPlot->graph(4)->setPen(QPen(Qt::green));
  customPlot->addGraph(); // orange line
  customPlot->graph(5)->setPen(QPen(qRgb(255,128,0)));


  customPlot->addGraph(); // green dot
  customPlot->graph(6)->setPen(QPen(Qt::green));
  customPlot->graph(6)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(6)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // orange dot
  customPlot->graph(7)->setPen(QPen(qRgb(255,128,0)));
  customPlot->graph(7)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(7)->setScatterStyle(QCPScatterStyle::ssDisc);
  //FIM DA PARTE NOVA

  //PARTE NOVA NOVA
  customPlot->addGraph(); // blue2 line
  customPlot->graph(8)->setPen(QPen(qRgb(0,0,102)));
  customPlot->addGraph(); // green2 line
  customPlot->graph(9)->setPen(QPen(qRgb(0,102,0)));
  customPlot->addGraph(); // orange2 line
  customPlot->graph(10)->setPen(QPen(qRgb(127,64,0)));


  customPlot->addGraph(); // blue2 dot
  customPlot->graph(11)->setPen(QPen(qRgb(0,0,102)));
  customPlot->graph(11)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(11)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // green2 dot
  customPlot->graph(12)->setPen(QPen(qRgb(0,102,0)));
  customPlot->graph(12)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(12)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // orange2 dot
  customPlot->graph(13)->setPen(QPen(qRgb(127,64,0)));
  customPlot->graph(13)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(13)->setScatterStyle(QCPScatterStyle::ssDisc);
  //FIM DA PARTE NOVA NOVA

  customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setAutoTickStep(false);
  //Valores no eixo X por segundo, proporcao utilizada no exemplo 8/4=2s
  customPlot->xAxis->setTickStep(plotRange/4);
  customPlot->axisRect()->setupFullAxesBox();


  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  //Coloca o Label dos eixos
  customPlot->xAxis->setLabel("Tempo (s)");
  customPlot->yAxis->setLabel("Tensão (v)");
}

void supervisorio::setupPlot2(QCustomPlot *customPlot2)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "Você deve usar a versão > 4.7");
#endif

  plot2Enable[0]=true;//Red Enabled
  plot2Enable[1]=true;//Blue Enabled
  plot2Enable[2]=true;//Green Enabled
  plot2Enable[3]=true;//Green2 Enabled
  plot2Enable[4]=true;//Pink Enabled
  plot2Enable[5]=true;//Blue2 Enabled
  plot2Enable[6]=true;//Orange Enabled
  plot2Enable[7]=true;//Orange2 Enabled

  customPlot2->addGraph(); // red line
  customPlot2->graph(0)->setPen(QPen(Qt::red));
  customPlot2->graph(0)->setAntialiasedFill(false);
  customPlot2->addGraph(); // blue line
  customPlot2->graph(1)->setPen(QPen(Qt::blue));
  customPlot2->addGraph(); // green line
  customPlot2->graph(2)->setPen(QPen(Qt::green));
  customPlot2->addGraph(); // green2 line
  customPlot2->graph(3)->setPen(QPen(qRgb(0,102,0)));
  customPlot2->addGraph(); // Pink line
  customPlot2->graph(4)->setPen(QPen(qRgb(255,0,180)));
  customPlot2->addGraph(); // Blue2 line
  customPlot2->graph(5)->setPen(QPen(qRgb(0,0,102)));
  customPlot2->addGraph(); // Orange line
  customPlot2->graph(6)->setPen(QPen(qRgb(255,128,0)));
  customPlot2->addGraph(); // Orange2 line
  customPlot2->graph(7)->setPen(QPen(qRgb(127,64,0)));

  customPlot2->addGraph(); // red dot
  customPlot2->graph(8)->setPen(QPen(Qt::red));
  customPlot2->graph(8)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(8)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // blue dot
  customPlot2->graph(9)->setPen(QPen(Qt::blue));
  customPlot2->graph(9)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(9)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // green dot
  customPlot2->graph(10)->setPen(QPen(Qt::green));
  customPlot2->graph(10)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(10)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // green2 dot
  customPlot2->graph(11)->setPen(QPen(qRgb(0,102,0)));
  customPlot2->graph(11)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(11)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // pink dot
  customPlot2->graph(12)->setPen(QPen(qRgb(255,0,180)));
  customPlot2->graph(12)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(12)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // blue2 dot
  customPlot2->graph(13)->setPen(QPen(qRgb(0,0,102)));
  customPlot2->graph(13)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(13)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // orange dot
  customPlot2->graph(14)->setPen(QPen(qRgb(255,128,0)));
  customPlot2->graph(14)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(14)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot2->addGraph(); // orange2 dot
  customPlot2->graph(15)->setPen(QPen(qRgb(127,64,0)));
  customPlot2->graph(15)->setLineStyle(QCPGraph::lsNone);
  customPlot2->graph(15)->setScatterStyle(QCPScatterStyle::ssDisc);

  customPlot2->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot2->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot2->xAxis->setAutoTickStep(false);
  //Valores no eixo X por segundo, proporcao utilizada no exemplo 8/4=2s
  customPlot2->xAxis->setTickStep(plotRange/4);
  customPlot2->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot2->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot2->yAxis2, SLOT(setRange(QCPRange)));

  //Coloca o Label dos eixos
  customPlot2->xAxis->setLabel("Tempo (s)");
  customPlot2->yAxis->setLabel("Centímetro (cm)");

}

void supervisorio::updatePlot1(double timeStamp, double redPlot, double bluePlot, double greenPlot, double orangePlot, double blue2Plot, double green2Plot, double orange2Plot)
{
    //Red
    if(plot1Enable[0]) {
        ui->customPlot->graph(0)->addData(timeStamp, redPlot);
        ui->customPlot->graph(2)->clearData();
        ui->customPlot->graph(2)->addData(timeStamp, redPlot);
        ui->customPlot->graph(0)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(0)->rescaleValueAxis(true);
    }

    //Blue
    if(plot1Enable[1]) {
        //qDebug() << "blueDot" << bluePlot;
        ui->customPlot->graph(1)->addData(timeStamp, bluePlot);
        ui->customPlot->graph(3)->clearData();
        ui->customPlot->graph(3)->addData(timeStamp, bluePlot);
        ui->customPlot->graph(1)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(1)->rescaleValueAxis(true);
    }


    //Green
    if(plot1Enable[2]) {
        ui->customPlot->graph(4)->addData(timeStamp, greenPlot);
        ui->customPlot->graph(6)->clearData();
        ui->customPlot->graph(6)->addData(timeStamp, greenPlot);
        ui->customPlot->graph(4)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(4)->rescaleValueAxis(true);
    }

    //Orange
    if(plot1Enable[3]) {
        ui->customPlot->graph(5)->addData(timeStamp, orangePlot);
        ui->customPlot->graph(7)->clearData();
        ui->customPlot->graph(7)->addData(timeStamp, orangePlot);
        ui->customPlot->graph(5)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(5)->rescaleValueAxis(true);
    }

    //Blue2
    if(plot1Enable[4]) {
        ui->customPlot->graph(8)->addData(timeStamp, blue2Plot);
        ui->customPlot->graph(11)->clearData();
        ui->customPlot->graph(11)->addData(timeStamp, blue2Plot);
        ui->customPlot->graph(8)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(8)->rescaleValueAxis(true);
    }

    //Green2
    if(plot1Enable[5]) {
        ui->customPlot->graph(9)->addData(timeStamp, green2Plot);
        ui->customPlot->graph(12)->clearData();
        ui->customPlot->graph(12)->addData(timeStamp, green2Plot);
        ui->customPlot->graph(9)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(9)->rescaleValueAxis(true);
    }

    //Orange2
    if(plot1Enable[6]) {
        ui->customPlot->graph(10)->addData(timeStamp, orange2Plot);
        ui->customPlot->graph(13)->clearData();
        ui->customPlot->graph(13)->addData(timeStamp, orange2Plot);
        ui->customPlot->graph(10)->removeDataBefore(timeStamp-plotRange);
        ui->customPlot->graph(10)->rescaleValueAxis(true);
    }

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(timeStamp+0.25, plotRange, Qt::AlignRight);
    ui->customPlot->yAxis->setRange(10,20,Qt::AlignRight);
    ui->customPlot->replot();

}

void supervisorio::updatePlot2(double timeStamp, double redPlot, double bluePlot, double greenPlot, double green2Plot)
{
  //Red
  if(plot2Enable[0]) {
      ui->customPlot2->graph(0)->addData(timeStamp, redPlot);
      ui->customPlot2->graph(8)->clearData();
      ui->customPlot2->graph(8)->addData(timeStamp, redPlot);
      ui->customPlot2->graph(0)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(0)->rescaleValueAxis(true);
  }

  //Blue
  if(plot2Enable[1]){
      ui->customPlot2->graph(1)->addData(timeStamp, bluePlot);
      ui->customPlot2->graph(9)->clearData();
      ui->customPlot2->graph(9)->addData(timeStamp, bluePlot);
      ui->customPlot2->graph(1)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(1)->rescaleValueAxis(true);
  }

  //Green
  if(plot2Enable[2]) {
      ui->customPlot2->graph(2)->addData(timeStamp, greenPlot);
      ui->customPlot2->graph(10)->clearData();
      ui->customPlot2->graph(10)->addData(timeStamp, greenPlot);
      ui->customPlot2->graph(2)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(2)->rescaleValueAxis(true);
  }

  //Green2
  if(plot2Enable[3]) {
      ui->customPlot2->graph(3)->addData(timeStamp, green2Plot);
      ui->customPlot2->graph(11)->clearData();
      ui->customPlot2->graph(11)->addData(timeStamp, green2Plot);
      ui->customPlot2->graph(3)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(3)->rescaleValueAxis(true);
  }

  //Pink
  if(plot2Enable[4]) {
      ui->customPlot2->graph(12)->clearData();
      ui->customPlot2->graph(4)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(4)->rescaleValueAxis(true);
  }

  //Blue2
  if(plot2Enable[5]) {
      ui->customPlot2->graph(13)->clearData();
      ui->customPlot2->graph(5)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(5)->rescaleValueAxis(true);
  }

  //Orange
  if(plot2Enable[6]) {
      ui->customPlot2->graph(14)->clearData();
      ui->customPlot2->graph(6)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(6)->rescaleValueAxis(true);
  }

  //Orange2
  if(plot2Enable[7]) {
      ui->customPlot2->graph(15)->clearData();
      ui->customPlot2->graph(7)->removeDataBefore(timeStamp-plotRange);
      ui->customPlot2->graph(7)->rescaleValueAxis(true);
  }


  // make key axis range scroll with the data (at a constant range size of 8):
  ui->customPlot2->xAxis->setRange(timeStamp+0.25, plotRange, Qt::AlignRight);
  ui->customPlot2->yAxis->setRange(30,60,Qt::AlignRight);
  ui->customPlot2->replot();
  //ui->customPlot2->yAxis->setRange(0, 30, Qt::AlignCenter);

}





void supervisorio::setLayout(bool frequencia, bool amplitude, bool offset, bool duracao) {
    //Frequencia
    ui->comboBox->setEnabled(frequencia);
    ui->doubleSpinBox->setEnabled(frequencia);
    //Amplitude
    ui->label_2->setEnabled(amplitude);
    ui->doubleSpinBox_2->setEnabled(amplitude);
    //Offset
    ui->label_3->setEnabled(offset);
    ui->doubleSpinBox_3->setEnabled(offset);
    //Duracao Max Min
    ui->label_12->setEnabled(duracao);
    //Max
    ui->doubleSpinBox_4->setEnabled(duracao);
    //Min
    ui->doubleSpinBox_5->setEnabled(duracao);
}

void supervisorio::setControlParams(bool kp, bool ki, bool kd)
{
    //kp
    ui->comboBox_5->setEnabled(kp);
    ui->doubleSpinBox_6->setEnabled(kp);
    //ki
    ui->comboBox_3->setEnabled(ki);
    ui->doubleSpinBox_7->setEnabled(ki);
    //kd
    ui->comboBox_4->setEnabled(kd);
    ui->doubleSpinBox_8->setEnabled(kd);
}


void supervisorio::setTickStep(void) {
    //Valores no eixo X por segundo, proporcao utilizada no exemplo 8/4=2s
    ui->customPlot->xAxis->setTickStep(plotRange/4);
    ui->customPlot2->xAxis->setTickStep(plotRange/4);

}

//==================================================================================================
//==================================================================================================
//END PLOT
//==================================================================================================
//==================================================================================================



void supervisorio::on_comboBox_currentIndexChanged(int index)
{
    //Muda os valores quando escolhemos período em vez de frequência
    double aux = ui->doubleSpinBox->minimum();
    double aux2 = ui->doubleSpinBox->value();
    ui->doubleSpinBox->setMinimum(1/ui->doubleSpinBox->maximum());
    ui->doubleSpinBox->setMaximum(1/aux);
    ui->doubleSpinBox->setValue(1/aux2);
    index++;//Só para não dar warning

}

void supervisorio::on_comboBox_3_currentIndexChanged(int index)
{
    double aux = ui->doubleSpinBox_7->value();
    ui->doubleSpinBox_7->setValue(kp[0]/aux);
    index++;
}

void supervisorio::on_comboBox_4_currentIndexChanged(int index)
{
    double aux = ui->doubleSpinBox_8->value();

    if (index == 0){
        ui->doubleSpinBox_8->setValue(kp[0]*aux);
    }
    else {
        ui->doubleSpinBox_8->setValue(aux/kp[0]);
    }
}

// Channel

void supervisorio::on_canal0_clicked()
{
    channel = 0;
}

void supervisorio::on_canal1_clicked()
{
    channel = 1;
}

void supervisorio::on_canal2_clicked()
{
    channel = 2;
}

void supervisorio::on_canal3_clicked()
{
    channel = 3;
}

void supervisorio::on_canal4_clicked()
{
    channel = 4;
}

void supervisorio::on_canal5_clicked()
{
    channel = 5;
}

void supervisorio::on_canal6_clicked()
{
    channel = 6;
}

void supervisorio::on_canal7_clicked()
{
    channel = 7;
}

//OBS: Para adicionar gráficos vá em Widget -> Promote to -> QCustomPLot

//Enable and disable plots
void supervisorio::on_pushButton_2_clicked()
{
    if(plot1Enable[0]==true){
        ui->pushButton_2->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot1Enable[0]=false;
    }
    else{
        ui->pushButton_2->setIcon(QIcon(QString::fromUtf8(":/img/Colors/blue.png")));
        plot1Enable[0]=true;
    }
}
void supervisorio::on_pushButton_3_clicked()
{
    if(plot1Enable[1]==true){
        ui->pushButton_3->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot1Enable[1]=false;
    }
    else{
        ui->pushButton_3->setIcon(QIcon(QString::fromUtf8(":/img/Colors/red.png")));
        plot1Enable[1]=true;
    }
}
void supervisorio::on_pushButton_4_clicked()
{
    if(plot2Enable[0]==true){
        ui->pushButton_4->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot2Enable[0]=false;
    }
    else{
        ui->pushButton_4->setIcon(QIcon(QString::fromUtf8(":/img/Colors/red.png")));
        plot2Enable[0]=true;
    }
}
void supervisorio::on_pushButton_5_clicked()
{
    if(plot2Enable[1]==true){
        ui->pushButton_5->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot2Enable[1]=false;
    }
    else{
        ui->pushButton_5->setIcon(QIcon(QString::fromUtf8(":/img/Colors/blue.png")));
        plot2Enable[1]=true;
    }
}
void supervisorio::on_pushButton_6_clicked()
{
    if(plot2Enable[2]==true){
        ui->pushButton_6->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot2Enable[2]=false;
    }
    else if (ui->radioButton_10->isChecked()){//Malha está fechada
        ui->pushButton_6->setIcon(QIcon(QString::fromUtf8(":/img/Colors/green.png")));
        plot2Enable[2]=true;
    }
}
void supervisorio::on_pushButton_7_clicked()
{
    if(plot2Enable[3]==true){
        ui->pushButton_7->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot2Enable[3]=false;
    }
    else if (ui->radioButton_10->isChecked()){//Malha está fechada
        ui->pushButton_7->setIcon(QIcon(QString::fromUtf8(":/img/Colors/green2.png")));
        plot2Enable[3]=true;
    }
}





void supervisorio::on_pushButton_10_clicked()
{
    if(plot1Enable[2]==true){
        ui->pushButton_10->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
        plot1Enable[2]=false;
    }
    else if (ui->radioButton_10->isChecked()){//Malha está fechada
        ui->pushButton_10->setIcon(QIcon(QString::fromUtf8(":/img/Colors/green.png")));
        plot1Enable[2]=true;
    }
}



//Malha aberta
void supervisorio::on_radioButton_9_clicked()
{
    ui->pushButton_6->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
    plot2Enable[2]=false;
    ui->pushButton_7->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
    plot2Enable[3]=false;
    plot2Enable[2]=false;
    ui->pushButton_10->setIcon(QIcon(QString::fromUtf8(":/img/Colors/gray.png")));
    plot2Enable[3]=false;
    ui->pushButton_10->setChecked(true);



    ui->label_2->setText("Amplitude (V)");
    ui->label_3->setText("Offset (V)");


    ui->doubleSpinBox_3->setValue(0);
//    ui->doubleSpinBox_3->setMaximum(15);
//    ui->doubleSpinBox_3->setMinimum(-15);

    //Desativa a combo box dos parâmetros de controle
    ui->groupBox_10->setEnabled(false);

}

//Malha fechada
void supervisorio::on_radioButton_10_clicked()
{
    ui->pushButton_6->setIcon(QIcon(QString::fromUtf8(":/img/Colors/green.png")));
    plot2Enable[2]=true;
    ui->pushButton_7->setIcon(QIcon(QString::fromUtf8(":/img/Colors/green2.png")));
    plot2Enable[3]=true;
    plot1Enable[2]=true;

    ui->pushButton_10->setIcon(QIcon(QString::fromUtf8(":/img/Colors/green.png")));
    plot1Enable[3]=true;
    ui->pushButton_10->setChecked(false);

    plot1Enable[4]=true;

    plot1Enable[5]=true;

    plot1Enable[6]=true;

    ui->label_2->setText("Amplitude (cm)");
    ui->label_3->setText("Offset (cm)");

    //ui->doubleSpinBox_3->setValue(0);
    //ui->doubleSpinBox_3->setMaximum(30);
    //ui->doubleSpinBox_3->setMinimum(0);

    //Ativa a combo box dos parametros de controle
    ui->groupBox_10->setEnabled(true);

}



void supervisorio::on_comboBox_6_currentIndexChanged(int index)
{
    if(index==0){//Degrau
        setLayout(false, false, true, false);
        nextWave = degrau;
    }
    else if(index==1){//Senoidal
        setLayout(true, true, true, false);
        nextWave = senoidal;
    }
    else if(index==2){//Quadrada
        setLayout(true, true, true, false);
        nextWave = quadrada;
    }
    else if(index==3){//Dente de Serra
        setLayout(true, true, true, false);
        nextWave = serra;
    }
    else if(index==4){//Aleatorio
        setLayout(false, true, true, true);
        nextWave = aleatorio;
    }


}


//Atualiza valores
void supervisorio::on_pushButton_8_clicked()
{
    qDebug() << "Atualizar";



    //Get wave configurations
    frequencia = ui->doubleSpinBox->value();
    if(ui->comboBox->currentIndex()==1) frequencia=1/frequencia; //Caso tenhamos escolhido período
    amplitude = ui->doubleSpinBox_2->value();
    offset = ui->doubleSpinBox_3->value();
    duracaoMax = ui->doubleSpinBox_4->value();
    duracaoMin= ui->doubleSpinBox_5->value();

    kd[0]=kd[1];
    ki[0]=ki[1];
    kp[0] = ui->doubleSpinBox_6->value();
    ki[0] = ui->doubleSpinBox_7->value();
    kd[0] = ui->doubleSpinBox_8->value();



    //taw = ui->doubleSpinBox_9->value();
    //taw = 237.19*sqrt(kd/ki);

    qDebug() << " >>>>>>kd[0] " <<kd[0] ;
    qDebug() << " >>>>>>kd[1] " <<kd[1] ;
    qDebug() << " >>>>>>ki[0] " <<ki[0] ;
    qDebug() << " >>>>>>ki[1] " <<ki[1] ;
    taw[0] = 75/sqrt(0.005/0.05)*sqrt(kd[0]/ki[0]);
    taw[1] = 75/sqrt(0.005/0.05)*sqrt(kd[1]/ki[1]);

    //setTaw(taw);
    wave = nextWave;
    control[0] = nextControl[0];
    control[1] = nextControl[1];
    qDebug() << ">>>>>>taw[0] " <<taw[0] ;
    qDebug() << ">>>>>>taw[1] " <<taw[1] ;
    bool malha = ui->radioButton_9->isChecked();

    int windupIndex = ui->comboBox_windup->currentIndex();
    windup[0] = false;
    conditionalIntegration[0] = false;
    windup[1] = false;
    conditionalIntegration[1] = false;
    if(windupIndex == 1) {
        windup[0] = true;
        windup[1] = true;
    } else if (windupIndex == 2) {
        conditionalIntegration[0] = true;
    }


    // Se tanque 1 selecionado escreve 1 caso contrario 2 (tanque 2)

    int tank = ui->radioButton_tanque1->isChecked()?1:2;

    int castControl[2];
    castControl[0] = static_cast<int>(control[0]);
    castControl[1] = static_cast<int>(control[1]);

    cThread->setParameters(frequencia, amplitude, offset, duracaoMax, duracaoMin, wave, malha, channel, castControl, kp, ki, kd, windup, conditionalIntegration, taw, tank, cascade,lOb, kSeg);
}

void supervisorio::onPlotValues(double timeStamp, double sinalCalculadoMestre, double sinalCalculadoEscravo, double sinalSaturado, double nivelTanque1, double nivelTanque2, double setPoint, double erro, double iMestre, double iEscravo, double dMestre, double dEscravo){
    //Update plots
    supervisorio::updatePlot1(timeStamp, sinalSaturado, sinalCalculadoMestre, iMestre, dMestre, sinalCalculadoEscravo, iEscravo, dEscravo);//Esses ultimos sao os valores integrativos e derivativos
    supervisorio::updatePlot2(timeStamp, nivelTanque1, nivelTanque2, setPoint, erro);

    //Update Water Level
    ui->progressBar->setValue(nivelTanque1*100);
    ui->label_5->setText(QString::number(nivelTanque1,'g',3)+" cm");
    ui->progressBar_2->setValue(nivelTanque2*100);
    ui->label_7->setText(QString::number(nivelTanque2,'g',3)+" cm");


}

void supervisorio::on_scaleValue_valueChanged(int value)
{
    ui->spinBox->setValue(value);
    plotRange = value;
    setTickStep();
}


void supervisorio::on_connect_clicked(bool checked)
{
    //Inicia Thread de comunicação
    if (checked) {
        cThread->setNullParameters();
        cThread->setSimulationMode(false);
        int erro = cThread->start();
        if(erro) {
            ui->connect->setText("Desconectar");
            ui->connectLabel->setText("Conectado");
        } else {
            ui->connectLabel->setText("No route to host");
            ui->connect->setChecked(false);
        }
    } else {
        ui->connect->setText("Conectar");
        ui->connectLabel->setText("Desconectado");
        //Termina a thread
        cThread->terminate();
        while(!cThread->isFinished());
    }
}



void supervisorio::on_spinBox_valueChanged(int arg1)
{
    ui->scaleValue->setValue(arg1);
}


void supervisorio::on_comboBox_tipoControle_currentIndexChanged(int index)
{
    if(index==0){//P
        setControlParams(true,false,false);
        nextControl[0] = P;
    }
    else if(index==1){//PI
        setControlParams(true,true,false);
        nextControl[0] = PI;
    }
    else if(index==2){//PD
        setControlParams(true,false,true);
        nextControl[0] = PD;
    }
    else if(index==3){//PID
        setControlParams(true,true,true);
        nextControl[0] = PID;
    }
    else if(index==4){//PI-D
        setControlParams(true,true,true);
        nextControl[0] = PI_D;
    }
    else if(index==5){//Sem
        setControlParams(false,false,false);
        nextControl[0] = SEM;
    }
}






void supervisorio::on_radioButton_tanque2_clicked()
{
    // seleciona malha fechada
    ui->radioButton_10->setChecked(true);
    // desabilita malha aberta
   // ui->radioButton_9->setEnabled(false);
    // habilita cascata
    on_radioButton_10_clicked();
//    on_checkBox_9_clicked(true);
    ui->groupBox_10->setEnabled(true);
}





void supervisorio::on_button_limpar_clicked()
{//Limpar gráficos

    for (int i=0;i<14;i++){
        ui->customPlot->graph(i)->clearData();
    }
    for (int i=0;i<8;i++){
            ui->customPlot2->graph(i)->clearData();
    }
    ui->customPlot2->yAxis->setRangeUpper(0);
    ui->customPlot2->yAxis->setRangeLower(0);
    ui->customPlot->yAxis->setRangeUpper(0);
    ui->customPlot->yAxis->setRangeLower(0);

}

void supervisorio::on_pushButton_zerar_clicked()
{
    cThread->setNullParameters();
}














