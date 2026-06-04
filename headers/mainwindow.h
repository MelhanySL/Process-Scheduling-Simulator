#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scheduler.h"
#include "configurationwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btn_configurar_nueva_clicked();

private:
    Ui::MainWindow *ui;
    ConfigurationWindow *configWindow = nullptr;
    Scheduler myScheduler;
};
#endif // MAINWINDOW_H
