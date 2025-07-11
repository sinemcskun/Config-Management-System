#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QTreeWidget *treeWidget;
    QPushButton *loadButton;
    QPushButton *saveButton;

    QString defaultJsonPath = "../../conf.json";
    QString currentPath = "";

    void traverseJson(const QJsonValue &value, QTreeWidgetItem *parentItem);
    QJsonValue buildJson(QTreeWidgetItem *item);
    void loadJsonFromFile(const QString &filePath);

private slots:
    void loadJson();
    void saveJson();
    void addObjectEntry();
    void addArrayEntry();
    void addStringEntry();
    void addBoolEntry();
    void addDoubleEntry();
    void addNullEntry();
    void deleteEntry();
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onItemClicked(QTreeWidgetItem *item, int column);
};

#endif // MAINWINDOW_H
