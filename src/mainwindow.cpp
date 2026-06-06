#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

//Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btn_configurar_nueva, &QPushButton::clicked, this, &MainWindow::on_btn_configurar_nueva_clicked);
}

//Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

// Maneja la navegación hacia la ventana de configuración
void MainWindow::on_btn_configurar_nueva_clicked()
{
    this->hide();

    if (configWindow == nullptr)
    {
        configWindow = new ConfigurationWindow(&myScheduler, this);
        connect(configWindow, &QWidget::destroyed, this, [this]()
        {
            this->configWindow = nullptr;
            this->show();
        });
    }

    configWindow->show();
}

