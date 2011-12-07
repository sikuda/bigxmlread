#ifndef MAINWINDOWBIGXMLREADER_H
#define MAINWINDOWBIGXMLREADER_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include "bigxmlreader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    void open();
    void find();
    void findNext();
    void findPrevious();
    void about();

private:
    void createActions();
    void createMenus();

    BigXmlReader bigxmlWidget;

    QMenu *fileMenu;
    QMenu *findMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QAction *findAct;
    QAction *findActNext;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
