#include "headers/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QLabel, QTableWidget, QHeaderView, QComboBox, QSpinBox, QLineEdit, QRadioButton, QGroupBox { color: #000000; }");
    MainWindow w;
    w.show();
    return a.exec();
}
