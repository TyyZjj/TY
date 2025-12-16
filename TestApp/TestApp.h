#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TestApp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TestAppClass; };
QT_END_NAMESPACE

class TestApp : public QMainWindow
{
    Q_OBJECT

public:
    TestApp(QWidget *parent = nullptr);
    ~TestApp();
    TestApp* newTestApp(TestApp* p) 
    { 
        return p; 
    };
    TestApp& refTestApp(TestApp &app) 
    { 
        return app; 
    };
    static int GetTestIndex() { return 1; }

public Q_SLOTS:
    void on_pushButton_clicked();
	void on_pushButton2_clicked();
    void recvMessage1();
    void recvMessage2(const QVariant& var);

private:
    Ui::TestAppClass *ui;
};
