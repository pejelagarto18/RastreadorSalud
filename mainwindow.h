#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    ~MainWindow();

private slots:
    void on_addRecordButton_clicked();
    void on_exportCSVButton_clicked();

private:
    Ui::MainWindow *ui;

    // ID del usuario actualmente logueado
    int currentUserId = -1;

    // Métodos principales
    void loadRecords();
    void updateAverages();

    // Ventana de inicio de sesión integrada
    int showLoginDialog();
};

#endif // MAINWINDOW_H
