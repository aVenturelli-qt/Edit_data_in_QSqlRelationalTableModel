/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtSql module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database_access_info.h"
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // connecting to the postgres database
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(db_info::HOST);
    db.setDatabaseName(db_info::DB_NAME);
    db.setUserName(db_info::USER_NAME);
    db.setPassword(db_info::PASSWORD);
    db.setPort(db_info::PORT);

    // try connection
    bool connected = db.open();
    if (!connected) qDebug() << "Db is unable to establish a connection";

    /////////////////////// debug
    ///
    /*
    db.transaction();
    QSqlQuery my_query(db);
    auto res = my_query.exec("UPDATE comments_table SET body_message = 'HELLO MY FRIENDS'::text WHERE id = 1;");
    if (res)
    {
        qDebug() << "All right";
        db.commit();
    } else {
        qDebug() << "problems:" << my_query.lastError().text();
    }*/

    // define the model and passing the connection
    m_model = new QSqlRelationalTableModel(this, db);
    m_model->setTable("comments_table");
    m_model->setRelation(1, QSqlRelation("vin_table", "id", "vin"));
    m_model->setRelation(2, QSqlRelation("users_table", "id", "mail"));
    m_model->setJoinMode(QSqlRelationalTableModel::InnerJoin);

    m_model->select();

    // set the model to the view
    ui->display_tv->setModel(m_model);
    ui->display_tv->setItemDelegate(new QSqlRelationalDelegate(ui->display_tv));

    // check for editing strategy
    qDebug() << "Edit strategy: " << m_model->editStrategy();       // 1 = on_row_change, 2 = manual_submit
    //m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    /*
    // connect lineedit "ReturnPressed" to model->setFilter()
    connect(ui->search_le, &QLineEdit::returnPressed, this, [this](){
        // get the content of QLineEdit
        auto txt = this->ui->search_le->text();

        // set filter whereas txt is empty or not
        auto filter = (!txt.isEmpty())? QString("vin = '%1'").arg(txt) : "";
        this->m_model->setFilter(filter);
        this->m_model->select();

        auto err = m_model->lastError();
        printLastError("after submitAll");      // debug()
    } );*/

    connect(ui->commit_btn, &QPushButton::clicked, this, &MainWindow::commitChanges);
}


MainWindow::~MainWindow()
{
    delete m_model;
    delete ui;
}

bool MainWindow::commitChanges()
{
    auto idx = m_model->index(0, 5);        // body message
    qDebug() << "the Data() returns:" << m_model->data(idx);        // prints TEST MESSAGE

    bool result = m_model->setData(idx, "Modification works correctly");   // returns true, but do not update the view
    qDebug() << "the setData() returns:" << result;
    m_model->select();

    /*
    m_model->submitAll();
    printLastError("after setData");      // debug()
    */

    return result;
}

void MainWindow::printLastError(std::string label)
{
    auto err = m_model->lastError();
    qDebug() << "[" << label << "]" << "last error is\t" << err.text();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
    m_model->submitAll();
    printLastError("after setData");      // debug()
}
