#include "mainwindow.h"
#include <QFile>
#include <QJsonParseError>
#include <QDebug>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(2);
    treeWidget->setHeaderLabels(QStringList() << "Key" << "Value");
    treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    loadButton = new QPushButton("Load JSON", this);
    saveButton = new QPushButton("Save JSON", this);

    layout->addWidget(treeWidget);
    layout->addWidget(loadButton);
    layout->addWidget(saveButton);
    setCentralWidget(central);

    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadJson);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveJson);
    connect(treeWidget, &QTreeWidget::itemChanged, this, &MainWindow::onItemChanged);
    connect(treeWidget, &QTreeWidget::itemClicked, this, &MainWindow::onItemClicked);

    loadJsonFromFile(defaultJsonPath);
    treeWidget->setContextMenuPolicy(Qt::DefaultContextMenu);
}

MainWindow::~MainWindow() {}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    QPoint vpPos = treeWidget->viewport()->mapFrom(this, event->pos());
    QTreeWidgetItem *item = treeWidget->itemAt(vpPos);
    if (!item) return;

    QMenu menu(this);
    menu.addAction("Add Object Entry", this, SLOT(addObjectEntry()));
    menu.addAction("Add Array Item", this, SLOT(addArrayEntry()));
    menu.addSeparator();

    menu.addAction("Add String", this, SLOT(addStringEntry()));
    menu.addAction("Add Bool", this, SLOT(addBoolEntry()));
    menu.addAction("Add Double", this, SLOT(addDoubleEntry()));
    menu.addAction("Add Null", this, SLOT(addNullEntry()));
    menu.addSeparator();

    menu.addAction("Delete Entry", this, SLOT(deleteEntry()));
    menu.exec(event->globalPos());
}

void MainWindow::loadJsonFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File does not found:" << filePath;
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File does not opened:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse Error:" << error.errorString();
        return;
    }

    currentPath = filePath;

    treeWidget->clear();
    QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget, QStringList() << "root");
    root->setExpanded(true);

    if (doc.isObject())
        traverseJson(doc.object(), root);
    else if (doc.isArray())
        traverseJson(doc.array(), root);
}

void MainWindow::loadJson() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open JSON File", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;

    loadJsonFromFile(fileName);
}

void MainWindow::addObjectEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setText(0, "newKey");
    child->setText(1, "newValue");
    child->setData(1, Qt::UserRole, "string");
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    item->setExpanded(true);
}

void MainWindow::addArrayEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    int index = item->childCount();
    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setText(0, QString("[%1]").arg(index));
    child->setText(1, "value");
    child->setData(1, Qt::UserRole, "string");
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    item->setExpanded(true);
}

void MainWindow::addStringEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setText(0, "newKey");
    child->setText(1, "value");
    child->setData(1, Qt::UserRole, "string");
    child->setData(1, Qt::UserRole + 1, "value");  // previousValue
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    item->setExpanded(true);
}

void MainWindow::addBoolEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setText(0, "newBool");
    child->setText(1, "false");
    child->setData(1, Qt::UserRole, "bool");
    child->setData(1, Qt::UserRole + 1, "false");
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    item->setExpanded(true);
}

void MainWindow::addDoubleEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setText(0, "newDouble");
    child->setText(1, "0.0");
    child->setData(1, Qt::UserRole, "double");
    child->setData(1, Qt::UserRole + 1, "0.0");
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    item->setExpanded(true);
}

void MainWindow::addNullEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setText(0, "newNull");
    child->setText(1, "null");
    child->setData(1, Qt::UserRole, "null");
    child->setData(1, Qt::UserRole + 1, "null");
    child->setFlags(child->flags() | Qt::ItemIsEditable);
    item->setExpanded(true);
}


void MainWindow::deleteEntry() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QTreeWidgetItem *parent = item->parent();
    if (parent) {
        parent->removeChild(item);
    } else {
        int index = treeWidget->indexOfTopLevelItem(item);
        if (index >= 0)
            treeWidget->takeTopLevelItem(index);
    }
    delete item;
}

void MainWindow::traverseJson(const QJsonValue &value, QTreeWidgetItem *parentItem) {
    if (value.isObject()) {
        QJsonObject obj = value.toObject();
        for (const QString &key : obj.keys()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
            item->setText(0, key);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            traverseJson(obj.value(key), item);
        }
    } else if (value.isArray()) {
        QJsonArray arr = value.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
            item->setText(0, QString("[%1]").arg(i));
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            traverseJson(arr[i], item);
        }
    } else {
        parentItem->setText(1, value.toVariant().toString());
        parentItem->setFlags(parentItem->flags() | Qt::ItemIsEditable);

        if(value.isBool())
            parentItem->setData(1, Qt::UserRole, "bool");
        else if(value.isDouble())
            parentItem->setData(1,Qt::UserRole, "double");
        else if(value.isString())
            parentItem->setData(1,Qt::UserRole, "string");
        else if(value.isNull())
            parentItem->setData(1,Qt::UserRole, "null");
    }
}

QJsonValue MainWindow::buildJson(QTreeWidgetItem *item) {
    int childCount = item->childCount();
    if (childCount == 0) {
        QString raw = item->text(1).trimmed();

        if (raw.compare("null", Qt::CaseInsensitive) == 0) {
            return QJsonValue();  // null
        } else if (raw.compare("true", Qt::CaseInsensitive) == 0) {
            return QJsonValue(true);
        } else if (raw.compare("false", Qt::CaseInsensitive) == 0) {
            return QJsonValue(false);
        } else {
            bool isInt = false;
            int intVal = raw.toInt(&isInt);
            if (isInt) return QJsonValue(intVal);

            double doubleVal;
            bool isDouble = false;
            doubleVal = raw.toDouble(&isDouble);
            if (isDouble) return QJsonValue(doubleVal);

            return QJsonValue(raw);  // string
        }
    }

    bool isArray = true;
    QJsonArray array;
    QJsonObject obj;

    for (int i = 0; i < childCount; ++i) {
        QTreeWidgetItem *child = item->child(i);
        QString key = child->text(0);

        if (!key.startsWith("["))
            isArray = false;

        if (isArray)
            array.append(buildJson(child));
        else
            obj.insert(key, buildJson(child));
    }

    return isArray ? QJsonValue(array) : QJsonValue(obj);
}

void MainWindow::onItemClicked(QTreeWidgetItem *item, int column){
    if(item && column == 1){
        item->setData(1, Qt::UserRole + 1, item->text(1));
    }
}

void MainWindow::onItemChanged(QTreeWidgetItem *item, int column){
    if (column != 1) return;

    QString rawData = item->text(1).trimmed();
    QString expectedType = item->data(1, Qt::UserRole).toString();
    QString previousValue = item->data(1, Qt::UserRole + 1).toString();

    bool valid = true;

    if(expectedType == "bool"){
        if(rawData.compare("true", Qt::CaseInsensitive) != 0 && rawData.compare("false", Qt::CaseInsensitive) != 0){
            valid = false;
            QMessageBox::warning(this, "Type Error", "You must enter a bool");
        }else{
            rawData = rawData.toLower();
        };
    }else if(expectedType == "double"){
        bool ok = false;
        rawData.toDouble(&ok);
        if(!ok){
            valid = false;
            QMessageBox::warning(this, "Type Error", "You must enter a double value");
        }
    }else if(expectedType == "null"){
        if(rawData.compare("null", Qt::CaseInsensitive) != 0){
            valid = false;
            QMessageBox::warning(this, "Type Error", "You must enter 'null' value");
        }else{
            rawData = "null";
        }
    }

    if(!valid){
        treeWidget->blockSignals(true);
        item->setText(1,previousValue);
        treeWidget->blockSignals(false);
        return;
    }

    item->setData(1, Qt::UserRole + 1, rawData);
    treeWidget->blockSignals(true);
    item->setText(1,rawData);
    treeWidget->blockSignals(false);
}

void MainWindow::saveJson() {
    if (treeWidget->topLevelItemCount() == 0 || currentPath.isEmpty()) return;

    QTreeWidgetItem *root = treeWidget->topLevelItem(0);
    QJsonValue json = buildJson(root);
    QJsonDocument doc = QJsonDocument::fromVariant(json.toVariant());


    QFile file(currentPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "File could not written:" << currentPath;
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    qDebug() << "JSON saved successfully:" << currentPath;
}
