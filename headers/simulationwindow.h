#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QMainWindow>
#include "scheduler.h"
//#include "configurationwindow.h"
#include <QTimer>
#include <QMessageBox>

class ConfigurationWindow;

namespace Ui {
class SimulationWindow;
}

class SimulationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulationWindow(Scheduler *scheduler, QWidget *parent = nullptr);
    ~SimulationWindow();

private slots:
    // Controles de flujo
    void on_pushButton_iniciar_clicked();
    void on_pushButton_parar_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_new_clicked();

    // Método ejecutado por el QTimer en cada intervalo de tiempo
    void autoStep();

private:
    Ui::SimulationWindow *ui;
    Scheduler *myScheduler;
    QTimer *simTimer; // Temporizador para la ejecución automática de la simulación

    void updateUI(); // Refresca las tablas y labels con el estado actual
    bool verificarFinSimulacion(); // Valida si todos los procesos han terminado
};

#endif // SIMULATIONWINDOW_H
