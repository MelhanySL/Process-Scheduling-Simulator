#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <QMainWindow>
#include "scheduler.h"
#include <QMessageBox>
#include "simulationwindow.h"

class SimulationWindow;

namespace Ui {
class ConfigurationWindow;
}

class ConfigurationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConfigurationWindow(Scheduler *scheduler, QWidget *parent = nullptr);
    ~ConfigurationWindow();

private slots:
    // Eventos de los botones de la interfaz
    void on_pushButton_save_process_clicked();
    void on_pushButton_generate_processes_clicked();
    void on_pushButton_start_simulation_clicked();

    // Eventos de control de lógica de interfaz
    void on_comboBox_algorithm_currentIndexChanged(int index);
    void on_radioButton_random_toggled(bool checked);
    void on_radioButton_manual_toggled(bool checked);

private:
    Ui::ConfigurationWindow *ui;
    Scheduler *myScheduler;
    int id_contador_manual = 1;

protected:
    void changeEvent(QEvent *e) override; //Función que detecta cuando la ventana de configuración vuelve a estar activa
};

#endif // CONFIGURATIONWINDOW_H
