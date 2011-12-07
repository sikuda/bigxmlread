//#include <QtGui>
#include <QApplication>
#include <QHeaderView>
#include <QStatusBar>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>
#include <QHBoxLayout>
#include <QTreeWidgetItem>
#include <QMenuBar>
#include "bigxmlmainwindow.h"
#include "bigxmlreader.h"

MainWindow::MainWindow()
{
    QStringList labels;
    labels << tr("Node/Attribute") << tr("Value");

    bigxmlWidget.header()->setResizeMode(QHeaderView::ResizeToContents);
    bigxmlWidget.setHeaderLabels(labels);
    setCentralWidget(&bigxmlWidget);

    createActions();
    createMenus();
    statusBar()->showMessage(tr("Ready"));

    setWindowTitle(tr("BigXmlReader"));
    resize(480, 320);
}

void MainWindow::open()
{
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open XML File"),
                                         QDir::currentPath(),
                                         tr("XML Files (*.xml)"));
    if (fileName.isEmpty())
        return;

    QXmlStreamReader xml;
    if( bigxmlWidget.openFile(fileName, xml)){
        //if (!bigxmlWidget.readBigXML(xml)) {
        if (!bigxmlWidget.readBigXMLtoLevel(xml, 2)) {
            QMessageBox::warning(this, tr("BigXmlReader"),
                                 tr("Parse error in file %1:\n\n%2")
                                 .arg(fileName)
                                 .arg(bigxmlWidget.errorXMLString(xml)));
        } else {
            setWindowTitle(fileName);
            statusBar()->showMessage(tr("File loaded"), 2000);
        }
    }
}

void MainWindow::find()
{
    QLabel* label = new QLabel(tr("Find:"));
    QLineEdit* lineEdit = new QLineEdit;
    lineEdit->setText(bigxmlWidget.strFindString);
    label->setBuddy(lineEdit);

    QPushButton* findButton = new QPushButton(tr("&Find"));
    findButton->setDefault(true);

    QHBoxLayout *Layout = new QHBoxLayout;
    Layout->addWidget(label);
    Layout->addWidget(lineEdit);
    Layout->addWidget(findButton);

    QDialog findDialog(this);
    findDialog.setMinimumSize(320, 60);
    findDialog.setLayout(Layout);
    findDialog.setWindowTitle(tr("BigXmlReader"));
    connect(findButton, SIGNAL(pressed()), &findDialog, SLOT(accept()));

    if( findDialog.exec() == QDialog::Accepted){
        QXmlStreamReader xml;
        bigxmlWidget.strFindString = lineEdit->text();
        QString strFilename = bigxmlWidget.getFilename();
        if( bigxmlWidget.openFile(strFilename, xml, false)){
            QString findString = lineEdit->text();
            if( bigxmlWidget.findDataBigXML( xml, findString, 0 )) bigxmlWidget.buildTreeBigXMLToMaxIndex();
            else QMessageBox::warning(this, tr("BigXmlReader"), tr("Can't find %1").arg(findString));
        }
        findDialog.close();
    }
}

void MainWindow::findNext()
{
    QXmlStreamReader xml;
    QTreeWidgetItem* item = bigxmlWidget.currentItem();
    if( bigxmlWidget.openFile(bigxmlWidget.strFindString, xml, false)){
        QString findString = bigxmlWidget.strFindString;
        if( bigxmlWidget.findDataBigXML( xml, findString, item )) bigxmlWidget.buildTreeBigXMLToMaxIndex();
        else QMessageBox::warning(this, tr("BigXmlReader"), tr("Can't find %1").arg(findString));
    }
}

void MainWindow::findPrevious()
{
    // TODO
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About BigXmlReader"),
                       tr("The <b>BigXmlReader</b> example demonstrates how to quick read big xml file. E-mail: sikuda@yandex.ru"));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    findAct = new QAction(tr("&Find..."), this);
    findAct->setShortcuts(QKeySequence::Find);
    connect(findAct, SIGNAL(triggered()), this, SLOT(find()));

    findActNext = new QAction(tr("F&ind next..."), this);
    findActNext->setShortcuts(QKeySequence::FindNext);
    connect(findActNext, SIGNAL(triggered()), this, SLOT(findNext()));

    //    findActPrevious = new QAction(tr("F&ind next..."), this);
    //    findActPrevious->setShortcuts(QKeySequence::FindPrevious);
    //    connect(findActPrevious, SIGNAL(triggered()), this, SLOT(findPrevious()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    //menuBar()->addSeparator();

    findMenu = menuBar()->addMenu(tr("Find"));
    findMenu->addAction(findAct);
    findMenu->addAction(findActNext);
    //findMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
