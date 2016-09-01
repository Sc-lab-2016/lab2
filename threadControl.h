#ifndef COMMTHREAD_H
#define COMMTHREAD_H

#include <QThread>
#include "quanser.h"
#include <complex>
#include <QtCore>
#include <cmath>
#include <armadillo>

using std::complex;
using arma::mat;

struct Controlador {
  double p, i, d;
  double sinalCalculado, sinalSaturado;
  double setPoint, erro ,lastSinalCalculado;
  double kp, ki, kd, taw, lastI, lastD, diferencaSaida;
  bool windup, conditionalIntegration;
};

class commThread : public QThread
{
    Q_OBJECT
public:
    explicit commThread(QObject *parent =0);
    enum Control { P, PI, PD, PID, PI_D, SEM };
    void run();
    void setParameters(double frequencia, double amplitude, double offset , double duracaoMax, double duracaoMin, int wave, bool malha, int channel, int *control, double *kp, double *ki, double *kd, bool *windup, bool *conditionalIntegration, double *taw, int tank, bool cascade, double *lOb,double *kSeg);
    void setNullParameters(void);
    void setSimulationMode(bool on);
    void disconnect(void);
    void terminate(void);
    void calculoDeControle(Controlador*, double,double,double);
    int start();
    //Observador

private:
    Controlador contMestre, contEscravo;
    typedef QThread super;
    double frequencia;
    double amplitude;
    double offset;
    double duracaoMax;
    double duracaoMin;
    double sinalDaOndaGerada, sinalSaturadoDaOndaGerada, lastTimeStamp, timeToNextRandomNumber, lastSinalCalculado;
    //variavel aux para anti-windup
    double lastLoopTimeStamp;
    int wave;
    Control control[2], lastControl[2];
    double period;
    bool malha, cascade;
    bool simulationMode;
    bool levelSimulationFinished;
    bool connected;
    //flag para ligar o observador
    bool observer, follower;
    int channel, tank;
    int simulationNivelTanque1;
    double waveTime, waveTimeStamp;
    Quanser* q;


    double v;

    // Travas
    double lockSignal(double sinalCalculado, double nivelTanque1, double nivelTanque2);


signals:
    void plotValues(double timeStamp, double sinalCalculadoMestre, double sinalCalculadoEscravo,
                    double sinalSaturado, double nivelTanque1, double nivelTanque2, double setPoint,
                    double erro, double iMestre, double iEscravo, double dMestre, double dEscravo);

};
#endif // COMMTHREAD_H
