#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    mainLayout = new QVBoxLayout(mainWidget);
    formLayout = new QFormLayout();

    saveButton = new QPushButton("Save", this);
    loadButton = new QPushButton("Load Config", this);
    addVar = new QPushButton("Add Variable", this);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(addVar);
    mainLayout->addWidget(saveButton);
    mainLayout->addWidget(loadButton);


    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveJson);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadConfigClicked);
    connect(addVar, &QPushButton::clicked, this, &MainWindow::addVariable);

    loadJson();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addVariable(){
    bool ok;
    QString varName = QInputDialog::getText(this, "Add Variable",
                                            "Variable name:",
                                            QLineEdit::Normal,
                                            "", &ok);
    if (!ok || varName.isEmpty()) return;

    QStringList types = {"Boolean", "String", "Number"};
    QString varType = QInputDialog::getItem(this, "Select Type",
                                            "Variable type:",
                                            types, 0, false, &ok);
    if (!ok) return;

    if (varType == "Boolean") {
        jsonData[varName] = false;
    }
    else if (varType == "String") {
        jsonData[varName] = "";
    }
    else if (varType == "Number") {
        jsonData[varName] = 0;
    }

    createDynamicUI();
}


void MainWindow::onLoadConfigClicked(){
    QString filename = QFileDialog::getOpenFileName(this,tr("Open config file"), QDir::currentPath(), tr("JSON Files (*.json);;All Files (*)"));

    if(!filename.isEmpty()){
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)){
            qDebug() << "Can not open file:" << file.errorString();
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument document = QJsonDocument::fromJson(data);
        jsonData = document.object();

        qDebug() << "File opened:" << jsonData;

        createDynamicUI();

    }
}

void MainWindow::loadJson(){
    QFile file("../../conf.json"); // i suppose that we are on build folder by default and i try to open conf.
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File did not opened!";
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(data);
    jsonData = document.object();

    qDebug() << "File opened:" << jsonData;

    createDynamicUI();
}

void MainWindow::createDynamicUI(){
    widgets.clear();

    while (formLayout->count() > 0) {
        QLayoutItem *item = formLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    for (auto i = jsonData.begin(); i != jsonData.end(); i++){
        QString key = i.key();
        QJsonValue value = i.value();

        QWidget *widget = nullptr;

        if(value.isBool()){
            QCheckBox *checkbox = new QCheckBox;
            checkbox->setChecked(value.toBool());
            widget = checkbox;
        }else if(value.isString()){
            QLineEdit *lineEdit = new QLineEdit;
            lineEdit->setText(value.toString());
            widget = lineEdit;
        }else if(value.isDouble()){
            QSpinBox *spinbox = new QSpinBox;
            spinbox->setRange(0,999);
            spinbox->setValue(value.toInt());
            widget = spinbox;
        }

        if(widget){
            formLayout->addRow(key + ":", widget);
            widgets[key] = widget;
            qDebug() << "Widget created:" << key;
        }
    }

    qDebug() << "Total widgets:" << widgets.size();
}

void MainWindow::saveJson(){
    for (auto it = widgets.begin(); it != widgets.end(); ++it) {
        QString key = it.key();
        QWidget *widget = it.value();

        if (QCheckBox *checkBox = qobject_cast<QCheckBox*>(widget)) {
            jsonData[key] = checkBox->isChecked();
        }
        else if (QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget)) {
            jsonData[key] = spinBox->value();
        }
        else if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget)) {
            jsonData[key] = lineEdit->text();
        }
    }
    QJsonDocument document(jsonData);

    QFile file("D:/qt-projects/conf-management-system/conf.json");

    if (file.open(QIODevice::WriteOnly)) {
        file.write(document.toJson());
        file.close();
        qDebug() << "File saved";
    } else {
        qDebug() << "File did not saved";
    }
}
