#include "headers/configurationwindow.h"
#include "ui_configurationwindow.h"

#include <QFileDialog>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

// Constructor: Inicializa la interfaz de configuración
ConfigurationWindow::ConfigurationWindow(Scheduler *scheduler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConfigurationWindow),
    myScheduler(scheduler)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    myScheduler->clear_all();
    // Valores predeterminados para experiencia de usuario
    ui->spinBox_processes_number->setValue(1);
    ui->spinBox_cpu_burst->setValue(1);
    ui->spinBox_max_burst_cpu->setValue(1);
    ui->spinBox_tick_rate->setValue(1000);

    ui->comboBox_algorithm->clear();
    ui->comboBox_algorithm->addItem("First Come, First Served (FCFS)");
    ui->comboBox_algorithm->addItem("Shortest Job First (SJF)");
    ui->comboBox_algorithm->addItem("Random Selection");
    ui->comboBox_algorithm->addItem("Priority Scheduling");
    ui->comboBox_algorithm->addItem("Round Robin (RR)");
    ui->comboBox_algorithm->addItem("Shortest Remaining Time First (SRTF)");
    ui->comboBox_algorithm->addItem("Priority Scheduling (versión expulsiva)");

    on_comboBox_algorithm_currentIndexChanged(ui->comboBox_algorithm->currentIndex());

    ui->radioButton_manual->setChecked(true);
    ui->widget_random->hide();
}

// Destructor
ConfigurationWindow::~ConfigurationWindow()
{
    delete ui;
}

// Guarda un proceso creado manualmente por el usuario
void ConfigurationWindow::on_pushButton_save_process_clicked()
{
    int total_procesos = static_cast<int>(myScheduler->get_all_processes().size());
    if (!myScheduler->get_all_processes().empty() && id_contador_manual <= total_procesos)
    {
        id_contador_manual = total_procesos + 1;
    }

    int id = id_contador_manual;
    int cpu = ui->spinBox_cpu_burst->value();
    int io = ui->spinBox_io_burst->value();
    int llegada = ui->spinBox_arrival_time->value();
    int prioridad = ui->spinBox_priority->value();

    if (cpu <= 0)
    {
        QMessageBox::warning(this, "Error", "El Burst de CPU deben ser mayores a 0.");
        return;
    }

    Process nuevo_proceso(id, prioridad, llegada, cpu, io);
    myScheduler->add_process(nuevo_proceso);

    ui->spinBox_cpu_burst->setValue(1);
    ui->spinBox_io_burst->setValue(0);
    ui->spinBox_arrival_time->setValue(0);
    ui->spinBox_priority->setValue(1);

    id_contador_manual++;

    ui->lbl_id->setText(QString::number(id_contador_manual));
}

// Genera un conjunto de procesos usando el motor aleatorio del Scheduler
void ConfigurationWindow::on_pushButton_generate_processes_clicked()
{
    int cantidad = ui->spinBox_processes_number->value();
    int max_cpu = ui->spinBox_max_burst_cpu->value();
    int max_io = ui->spinBox_max_io_burst->value();
    int max_llegada = ui->spinBox_max_arrival_time->value();
    int max_prioridad = ui->spinBox_max_priority->value();

    if (cantidad <= 0 || max_cpu <= 0)
    {
        QMessageBox::warning(this, "Error", "La cantidad de procesos y el Burst de CPU deben ser mayores a 0.");
        return;
    }

    if (max_io < 0 || max_llegada < 0 || max_prioridad < 0)
    {
        QMessageBox::warning(this, "Error", "Los valores de I/O, Llegada y Prioridad no pueden ser negativos.");
        return;
    }
    myScheduler->clear_all();
    myScheduler->generate_random_processes(cantidad, max_cpu, max_io, max_prioridad, max_llegada);

    if (myScheduler->get_all_processes().size() > 0)
    {
        ui->pushButton_save_process->setEnabled(false);
        on_pushButton_start_simulation_clicked();
    }
    else
    {
        QMessageBox::warning(this, "Error", "No se pudieron generar procesos.");
    }
}

// Transiciona del estado de configuración al estado de ejecución
void ConfigurationWindow::on_pushButton_start_simulation_clicked()
{
    int algo_index = ui->comboBox_algorithm->currentIndex();
    myScheduler->set_algorithm(static_cast<Algorithm>(algo_index));

    myScheduler->set_quantum(ui->spinBox_quantum->value());

    this->hide();
    SimulationWindow *simWindow = new SimulationWindow(myScheduler, this);

    connect(simWindow, &QWidget::destroyed, this, &ConfigurationWindow::show);

    simWindow->show();
}

// Gestiona la visibilidad de los controles según el algoritmo seleccionado
void ConfigurationWindow::on_comboBox_algorithm_currentIndexChanged(int index)
{
    if (index == 4)
    {
        ui->lbl_quantum->show();
        ui->spinBox_quantum->show();
    }
    else
    {
        ui->lbl_quantum->hide();
        ui->spinBox_quantum->hide();
    }

    if (index == 3 || index == 6)
    {
        ui->lbl_priority->show();
        ui->spinBox_priority->show();

        ui->lbl_max_priority->show();
        ui->spinBox_max_priority->show();
    }
    else
    {
        ui->lbl_priority->hide();
        ui->spinBox_priority->hide();

        ui->lbl_max_priority->hide();
        ui->spinBox_max_priority->hide();
    }
}

// Alterna la interfaz para el modo de generación aleatoria de procesos
void ConfigurationWindow::on_radioButton_random_toggled(bool checked)
{
    if (checked)
    {
        myScheduler->clear_all();
        id_contador_manual = 1;
        ui->lbl_id->setText(QString::number(id_contador_manual));

        ui->widget_manual->hide();
        ui->lbl_id->hide();
        ui->pushButton_save_process->hide();
        ui->widget_random->show();
        ui->pushButton_start_simulation->hide();
        ui->pushButton_csv->hide();
        ui->pushButton_txt->hide();
    }
}

// Alterna la interfaz para el modo de creación manual de procesos
void ConfigurationWindow::on_radioButton_manual_toggled(bool checked)
{
    if (checked)
    {
        ui->pushButton_save_process->setEnabled(true);
        myScheduler->clear_all();
        id_contador_manual = 1;
        ui->lbl_id->setText(QString::number(id_contador_manual));

        ui->widget_manual->show();
        ui->lbl_id->show();
        ui->pushButton_save_process->show();
        ui->widget_random->hide();
        ui->pushButton_start_simulation->show();
        ui->pushButton_csv->hide();
        ui->pushButton_txt->hide();
    }
}

// Esta función detecta cuando la ventana de configuración vuelve a estar activa y actualiza el contador del id para el widget manual
void ConfigurationWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);

    if (e->type() == QEvent::ActivationChange && this->isActiveWindow())
    {
        if (ui->radioButton_manual->isChecked())
        {
            ui->pushButton_save_process->setEnabled(true);
            id_contador_manual = static_cast<int>(myScheduler->get_all_processes().size()) + 1;
            ui->lbl_id->setText(QString::number(id_contador_manual));
        }
    }
}

// Guarda procesos que vienen de un .csv
void ConfigurationWindow::on_pushButton_csv_clicked()
{
    QMessageBox::information
    (
        this,
        tr("Formato Requerido"),
        tr("Por favor, asegúrese de que su archivo siga exactamente este formato:\n\n"
           "# CPU_Burst, IO_Burst, Arrival_Time, Priority\n"
           "8, 2, 0, 3\n"
           "4, 0, 1, 1\n\n"
           "Nota: Si un proceso no requiere prioridad, coloque un 0 o 1 en su lugar.\n"
           "Recuerde guardar el .csv como CSV(Archivo Separado por comas) por favor antes de subir")
    );

    QString rutaArchivo = QFileDialog::getOpenFileName(
        this,
        tr("Seleccionar archivo CSV de procesos"),
        "",
        tr("Archivos CSV (*.csv);;Todos los archivos (*)")
        );

    if (rutaArchivo.isEmpty())
    {
        return;
    }

    std::ifstream archivo(rutaArchivo.toStdString());
    if (!archivo.is_open())
    {
        QMessageBox::critical(this, tr("Error"), tr("No se pudo abrir el archivo."));
        return;
    }
    myScheduler->clear_all();
    std::string linea;
    int procesos_cargados = 0;

    while (std::getline(archivo, linea))
    {
        linea.erase(std::remove(linea.begin(), linea.end(), '\r'), linea.end());
        linea.erase(std::remove(linea.begin(), linea.end(), ' '), linea.end());

        std::replace(linea.begin(), linea.end(), ';', ',');

        if (linea.empty() || linea.find('#') != std::string::npos || linea.find("CPU") != std::string::npos)
        {
            continue;
        }

        if (linea.empty() || linea[0] == '#')
        {
            continue;
        }

        std::stringstream ss(linea);
        std::string valor;

        int cpuBurst, ioBurst, arrivalTime, priority;

        try
        {
            if (std::getline(ss, valor, ',')) cpuBurst = std::stoi(valor); else continue;
            if (std::getline(ss, valor, ',')) ioBurst = std::stoi(valor); else continue;
            if (std::getline(ss, valor, ',')) arrivalTime = std::stoi(valor); else continue;

            if (std::getline(ss, valor))
            {
                valor.erase(std::remove(valor.begin(), valor.end(), ','), valor.end());

                if (!valor.empty())
                {
                    priority = std::stoi(valor);
                }
                else
                {
                    priority = 1;
                }
            }
            else
            {
                priority = 1;
            }

            procesos_cargados++;
            Process process(procesos_cargados, priority, arrivalTime, cpuBurst, ioBurst);
            myScheduler->add_process(process);
        }
        catch (const std::exception& e)
        {
            continue;
        }
    }

    archivo.close();

    QMessageBox::information(
        this,
        tr("Carga Exitosa"),
        tr("Se han importado correctamente %1 procesos.").arg(procesos_cargados)
        );
}

// Guarda procesos que vienen de un archivo .txt
void ConfigurationWindow::on_pushButton_txt_clicked()
{
    QMessageBox::information
        (
        this,
        tr("Formato Requerido"),
        tr("Por favor, asegúrese de que su archivo siga exactamente este formato:\n\n"
           "# CPU_Burst, IO_Burst, Arrival_Time, Priority\n"
           "8, 2, 0, 3\n"
           "4, 0, 1, 1\n\n"
           "Nota: Si un proceso no requiere prioridad, coloque un 0 o 1 en su lugar.")
    );

    QString rutaArchivo = QFileDialog::getOpenFileName
    (
        this,
        tr("Seleccionar archivo de procesos"),
        "",
        tr("Archivos de Texto (*txt);;Todos los archivos (*)")
    );

    if (rutaArchivo.isEmpty())
    {
        return;
    }

    std::ifstream archivo(rutaArchivo.toStdString());
    if (!archivo.is_open())
    {
        QMessageBox::critical(this, tr("Error"), tr("No se pudo abrir el archivo seleccionado."));
        return;
    }

    myScheduler->clear_all();
    std::string linea;
    int procesos_cargados = 0;

    while (std::getline(archivo, linea))
    {
        linea.erase(std::remove(linea.begin(), linea.end(), '\r'), linea.end());
        linea.erase(std::remove(linea.begin(), linea.end(), ' '), linea.end());
        std::replace(linea.begin(), linea.end(), ';', ',');

        if (linea.empty() || linea[0] == '#')
        {
            continue;
        }

        std::stringstream ss(linea);
        std::string valor;

        int cpuBurst, ioBurst, arrivalTime, priority;

        if (std::getline(ss, valor, ',')) cpuBurst = std::stoi(valor); else continue;
        if (std::getline(ss, valor, ',')) ioBurst = std::stoi(valor); else continue;
        if (std::getline(ss, valor, ',')) arrivalTime = std::stoi(valor); else continue;

        if (std::getline(ss, valor, ','))
        {
            priority = std::stoi(valor);
        } else {
            priority = 1;
        }
        procesos_cargados++;
        Process process(procesos_cargados, priority, arrivalTime, cpuBurst, ioBurst);
        myScheduler->add_process(process);
    }

    archivo.close();

    QMessageBox::information(
        this,
        tr("Carga Exitosa"),
        tr("Se han cargado correctamente %1 procesos desde el archivo.").arg(procesos_cargados)
    );
}


void ConfigurationWindow::on_radioButton_toggled(bool checked)
{
    if (checked)
    {
        ui->pushButton_save_process->setEnabled(false);

        ui->widget_manual->hide();
        ui->pushButton_save_process->hide();
        ui->widget_random->hide();
        ui->pushButton_start_simulation->show();
        ui->pushButton_csv->show();
        ui->pushButton_txt->show();
    }
}

