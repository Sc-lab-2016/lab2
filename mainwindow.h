#ifndef SUPERVISORIO_H
#define SUPERVISORIO_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include "threadControl.h"

using std::complex;
namespace Ui {
class supervisorio;
}

class supervisorio : public QMainWindow
{
    Q_OBJECT

public:
    explicit supervisorio(QWidget *parent = 0);
    ~supervisorio();
    enum Wave { degrau, senoidal, quadrada, serra, aleatorio };
    enum Control { P, PI, PD, PID, PI_D, SEM };
    void setupPlot1(QCustomPlot *customPlot);
    void setupPlot2(QCustomPlot *customPlo2);
    void updatePlot1(double timeStamp, double redPlot, double bluePlot, double greenPlot, double orangePlot, double blue2Plot, double green2Plot, double orange2Plot);
    void updatePlot2(double timeStamp, double redPlot, double bluePlot, double greenPlot, double green2Plot, double pinkPlot, double blue2Plot, double orangePlot, double orange2Plot);
    double lockSignal(double sinalCalculado, double nivelTanque1);
    commThread *cThread;

public slots:
     void onPlotValues(double timeStamp, double sinalCalculadoMestre, double sinalCalculadoEscravo, double sinalSaturado, double nivelTanque1, double nivelTanque2, double setPoint, double erro, double iMestre, double iEscravo, double dMestre, double dEscravo, double nivelTanque1Est, double nivelTanque2Est, double erroEst1, double erroEst2);

private slots:
  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_radioButton_9_clicked();

  void on_radioButton_10_clicked();

  void on_pushButton_8_clicked();

  void on_scaleValue_valueChanged(int value);

  void on_connect_clicked(bool checked);

  void on_spinBox_valueChanged(int arg1);

  void on_canal0_clicked();

  void on_canal1_clicked();

  void on_canal2_clicked();

  void on_canal3_clicked();

  void on_canal4_clicked();

  void on_canal5_clicked();

  void on_canal6_clicked();

  void on_canal7_clicked();

  void on_comboBox_6_currentIndexChanged(int index);

  void on_comboBox_tipoControle_currentIndexChanged(int index);

  void on_comboBox_3_currentIndexChanged(int index);

  void on_comboBox_4_currentIndexChanged(int index);

  void on_pushButton_10_clicked();

  void on_radioButton_tanque1_clicked();

  void on_radioButton_tanque2_clicked();

  void on_button_limpar_clicked();

  void on_pushButton_zerar_clicked();
private:
    Ui::supervisorio *ui;
    QString demoName;
    QTimer dataTimer;
    bool plot1Enable[7], plot2Enable[8];
    double lastTimeStamp;
    double sinalCalculado[2];
    double timeToNextRandomNumber;

    double frequencia;
    double amplitude;
    double offset;
    double duracaoMax;
    double duracaoMin;
    double kp[2], ki[2], kd[2], taw[2];
    bool windup[2], conditionalIntegration[2];
    bool cascade;

    int nextWave;
    int wave;

    Control nextControl[2];
    Control control[2];

    complex<double> polesOb[2];
    double lOb[2];

    complex<double> polesSeg[3];
    double kSeg[3];

    int plotRange;

    int channel;

    void setLayout(bool frequencia, bool amplitude, bool offset, bool duracao);
    void setControlParams(bool kp, bool ki, bool kd);
    void setControlParamsSlave(bool kp, bool ki, bool kd);

    void setTickStep();
    void getPolesOb();
    void calcLOb();
    void getLOb();
    void setLOb(int lNum);
    void setPolesOb(int poleNum);
    bool isInstableOb();
    double moduleOfPole(complex<double> pole);
    void on_poles_valueChange();
    void on_l_valueChange();
    void calcPoles();
    // flags para evitarem loop infinito de atualizacao dos valores
        //observador
    bool lObHasChanged;
    bool poleObHasChanged;
    void replicaPolo(int numPolo);
    void setCascadeGraph(bool checked);
        //seguidor
    bool kSegHasChanged;
    bool poleSegHasChanged;
    void replicaPoloSeg(int numPolo);
    void getPolesSeg();
    void setPolesSeg(int poleNum);
    void getKSeg();
    void setKSeg(int num);
    void calcPolesSeg();
    void calcKSeg();
    bool isInstableSeg();
    void on_k_valueChange();
    void on_poles_seg_valueChange();
};

#endif // SUPERVISORIO_H



