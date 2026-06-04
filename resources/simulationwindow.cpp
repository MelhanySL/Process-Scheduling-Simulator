#include "headers/simulationwindow.h"
#include "ui_simulationwindow.h"

// Constructor: Inicializa la interfaz de la simulación
SimulationWindow::SimulationWindow(Scheduler *scheduler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimulationWindow)
    , myScheduler(scheduler)
{
    ui->setupUi(this);

    QString titulo = QString("Algoritmo en ejecución: %1").arg(QString::fromStdString(myScheduler->get_algorithm_name()));
    ui->label->setText(titulo);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QScrollArea *miScroll = ui->scrollArea;
    miScroll->setWidgetResizable(true);
    miScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    miScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    if (miScroll->widget())
    {
        miScroll->widget()->setMinimumSize(1200, 1200);
    }
    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &SimulationWindow::autoStep);
    updateUI();
}

// Destructor
SimulationWindow::~SimulationWindow()
{
    delete ui;
}

// Botón Iniciar: Activa el temporizador para la simulación automática
void SimulationWindow::on_pushButton_iniciar_clicked()
{
    if (myScheduler->get_all_processes().empty())
    {
        QMessageBox::warning(this, "Sin Procesos", "No hay procesos cargados en el sistema. Regresa a la configuración para añadir procesos.");
        return;
    }

    if (!simTimer->isActive())
    {
        simTimer->start(myScheduler->get_tick_duration_ms());
    }
}

// Botón Parar: Pausa el temporizador, deteniendo el flujo de la simulación
void SimulationWindow::on_pushButton_parar_clicked()
{
    if (simTimer->isActive())
    {
        simTimer->stop();
    }
}

// Botón Siguiente: Ejecuta un único tick de simulación (paso a paso)
void SimulationWindow::on_pushButton_next_clicked()
{
    if (verificarFinSimulacion())
    {
        return;
    }

    myScheduler->step();
    updateUI();

    verificarFinSimulacion();
}

// Botón Nuevo: Limpia el planificador y cierra la ventana para reiniciar la configuración
void SimulationWindow::on_pushButton_new_clicked()
{
    simTimer->stop();
    myScheduler->clear_all();
    this->close();
}

// Refresca tablas y etiquetas con el estado actual del Scheduler
void SimulationWindow::updateUI()
{
    ui->lineEdit_reloj->setText(QString::number(myScheduler->get_global_clock()));

    Algorithm algorithm_ = myScheduler->get_active_algorithm();
    bool use_priority = (algorithm_ == Algorithm::PRIORITY_SCHEDULING || algorithm_ == Algorithm::PRIORITY_SCHEDULING_EXPULSIVE);
    ui->tableWidget_listos->setColumnHidden(3, !use_priority);

    bool use_quantum = (algorithm_ == Algorithm::RR);
    if(!use_quantum)
    {
        ui->lbl_quantum->hide();
        ui->lbl_nombre_q->hide();
        ui->lbl_quantum_2->hide();
        ui->lbl_nombre_q_2->hide();
    }
    else
    {
        ui->lbl_quantum_2->show();
        ui->lbl_nombre_q_2->show();
    }

    ui->tableWidget_listos->setRowCount(0);
    for (Process* p : myScheduler->get_ready_queue()) {
        int row = ui->tableWidget_listos->rowCount();
        ui->tableWidget_listos->insertRow(row);
        ui->tableWidget_listos->setItem(row, 0, new QTableWidgetItem(QString::number(p->get_id())));
        ui->tableWidget_listos->setItem(row, 1, new QTableWidgetItem(QString::number(p->get_arrival_time())));
        ui->tableWidget_listos->setItem(row, 2, new QTableWidgetItem(QString::number(p->get_remaining_cpu_time())));
        ui->tableWidget_listos->setItem(row, 3, new QTableWidgetItem(QString::number(p->get_priority())));

    }

    ui->tableWidget_bloqueados->setRowCount(0);
    std::queue<Process*> temp_blocked = myScheduler->get_blocked_queue();
    while (!temp_blocked.empty()) {
        Process* p = temp_blocked.front();
        int row = ui->tableWidget_bloqueados->rowCount();
        ui->tableWidget_bloqueados->insertRow(row);
        ui->tableWidget_bloqueados->setItem(row, 0, new QTableWidgetItem(QString::number(p->get_id())));
        ui->tableWidget_bloqueados->setItem(row, 1, new QTableWidgetItem(QString::number(p->get_remaining_io_time())));
        temp_blocked.pop();
    }

    ui->tableWidget_terminados->setRowCount(0);
    for (Process* p : myScheduler->get_terminated_processes()) {
        int row = ui->tableWidget_terminados->rowCount();
        ui->tableWidget_terminados->insertRow(row);
        ui->tableWidget_terminados->setItem(row, 0, new QTableWidgetItem(QString::number(p->get_id())));
        ui->tableWidget_terminados->setItem(row, 1, new QTableWidgetItem(QString::number(p->get_arrival_time())));
        ui->tableWidget_terminados->setItem(row, 2, new QTableWidgetItem(QString::number(p->get_ending_time())));
        ui->tableWidget_terminados->setItem(row, 3, new QTableWidgetItem(QString::number(p->get_waiting_time())));
        ui->tableWidget_terminados->setItem(row, 4, new QTableWidgetItem(QString::number(p->get_block_time())));
    }

    Process* running = myScheduler->get_running_process();
    if (running != nullptr) {
        ui->lbl_state->setText("EJECUTANDO");
        ui->lbl_process->setText(QString("P%1").arg(running->get_id()));
        ui->lbl_cpu_remaining->setText(QString::number(running->get_remaining_cpu_time()));
        ui->lbl_io_remaining->setText(QString::number(running->get_remaining_io_time()));

        int quantum_restante = myScheduler->get_quantum() - myScheduler->get_current_quantum_ticks();
        ui->lbl_quantum->setText(QString::number(quantum_restante));
        ui->lbl_quantum_2->setText(QString::number(myScheduler->get_quantum()));
    } else {
        ui->lbl_state->setText("CPU OCIOSA");
        ui->lbl_process->setText("-");
        ui->lbl_cpu_remaining->setText("-");
        ui->lbl_io_remaining->setText("-");
        ui->lbl_quantum->setText("-");
        ui->lbl_quantum_2->setText("-");
    }

    ui->lbl_uso_total_cpu->setText(QString("%1 %").arg(myScheduler->get_cpu_usage(), 0, 'f', 2));
}

// Automatiza el avance del reloj lógico (ejecutado por el timer)
void SimulationWindow::autoStep()
{
    if (verificarFinSimulacion())
    {
        return;
    }

    myScheduler->step();
    updateUI();
}
 // Evalúa si se alcanzó la condición de terminación y presenta el reporte de métricas
bool SimulationWindow::verificarFinSimulacion()
{
    if (myScheduler->get_terminated_processes().size() == myScheduler->get_all_processes().size() && !myScheduler->get_all_processes().empty())
    {
        simTimer->stop();
        updateUI();

        QString reporte = QString(
          "Todos los procesos han sido completados con éxito.\n\n"
          "--- ESTADÍSTICAS GENERALES ---\n"
          "• Tiempo Total de Simulación: %1 ticks\n"
          "• Uso Total de la CPU: %2%\n"
          "• Tiempo Promedio de Espera: %3 ticks\n"
          "• Tiempo Promedio en Bloqueo: %4 ticks\n"
          "• Tiempo Promedio de Retorno/Ejecución: %5 ticks\n"
          )
          .arg(myScheduler->get_global_clock())
          .arg(QString::number(myScheduler->get_cpu_usage(), 'f', 2))
          .arg(QString::number(myScheduler->get_avg_waiting_time(), 'f', 2))
          .arg(QString::number(myScheduler->get_avg_block_time(), 'f', 2))
          .arg(QString::number(myScheduler->get_avg_execution_time(), 'f', 2));

        QMessageBox::information(this, "Simulación Finalizada", reporte);
        return true;
    }
    return false;
}
