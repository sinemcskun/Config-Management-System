#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadJson();
    void saveJson();
    void createDynamicUI();

private:
    Ui::MainWindow *ui;
    QJsonObject jsonData;
    QWidget *mainWidget;
    QFormLayout *formLayout;
    QVBoxLayout *mainLayout;
    QPushButton *saveButton;

    QMap<QString, QWidget*> widgets;
};
#endif // MAINWINDOW_H
