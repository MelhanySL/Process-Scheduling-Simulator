#include "headers/configurationwindow.h"
#include "ui_configurationwindow.h"

// Constructor: Inicializa la interfaz de configuración
ConfigurationWindow::ConfigurationWindow(Scheduler *scheduler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConfigurationWindow),
    myScheduler(scheduler)
{
    ui->setupUi(this);
    this->setFixedSize(1250, 900);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QScrollArea *miScroll = ui->scrollArea;
    miScroll->setWidgetResizable(true);
    miScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    miScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QScrollArea *miScroll2 = ui->scrollArea_2;
    miScroll2->setWidgetResizable(true);
    miScroll2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    miScroll2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

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

    ui->spinBox_cpu_burst->setValue(0);
    ui->spinBox_io_burst->setValue(0);
    ui->spinBox_arrival_time->setValue(0);
    ui->spinBox_priority->setValue(0);

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
        ui->widget_manual->hide();
        ui->lbl_id->hide();
        ui->pushButton_save_process->hide();
        ui->widget_random->show();
        ui->pushButton_start_simulation->hide();
    }
}

// Alterna la interfaz para el modo de creación manual de procesos
void ConfigurationWindow::on_radioButton_manual_toggled(bool checked)
{
    if (checked)
    {
        ui->widget_manual->show();
        ui->lbl_id->show();
        ui->pushButton_save_process->show();
        ui->widget_random->hide();
        ui->pushButton_start_simulation->show();
    }
}

// Esta función detecta cuando la ventana de configuración vuelve a estar activa y actualiza el contador del id para el widget manual
void ConfigurationWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);

    if (e->type() == QEvent::ActivationChange && this->isActiveWindow())
    {
        if (myScheduler->get_all_processes().empty())
        {
            this->id_contador_manual = 1;
            ui->lbl_id->setText(QString::number(this->id_contador_manual));
        }
    }
}
