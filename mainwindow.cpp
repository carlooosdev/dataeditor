#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonConnect_clicked()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Error", db.lastError().text());
    } else {
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS students (name TEXT, id INT);");
        QMessageBox::information(this, "Success", "Connection established!");
    }
}

void MainWindow::on_buttonDisconnect_clicked()
{
    db.close();
    QMessageBox::information(this, "Success", "Disconnected!");
}

void MainWindow::on_buttonLoadData_clicked()
{
    QSqlQuery query;
    query.exec("SELECT * FROM students;");
    model->setQuery(query);
}

void MainWindow::on_buttonAddData_clicked()
{
    QString name = ui->lineEditName->text();
    QString id = ui->lineEditId->text();

    if (!name.isEmpty() && !id.isEmpty()) {
        QSqlQuery query;
        query.prepare("INSERT INTO students (name, id) VALUES (?, ?);");
        query.addBindValue(name);
        query.addBindValue(id.toInt());
        if (!query.exec()) {
            QMessageBox::critical(this, "Error", query.lastError().text());
        } else {
            ui->lineEditName->clear();
            ui->lineEditId->clear();
            QMessageBox::information(this, "Success", "Record added!");
        }
    }
}

void MainWindow::on_buttonDeleteData_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();

    foreach (QModelIndex index, selectedIndexes) {
        int row = index.row();
        QString id = model->index(row, 1).data().toString();
        QSqlQuery query;
        query.prepare("DELETE FROM students WHERE id = ?;");
        query.addBindValue(id.toInt());
        if (!query.exec()) {
            QMessageBox::critical(this, "Error", query.lastError().text());
        }
    }

    on_buttonLoadData_clicked();
}
