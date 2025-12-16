#include "TestApp.h"
#include "CGlobalMessage.h"
#include "TGenericInvoker.h"
#include "TFastRPC.h"

int &Add(int &a, int &b)
{
    b++;
    return b;
}

TestApp::TestApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TestAppClass())
{
    ui->setupUi(this);
	CGlobalMessage::GetKernel()->attach("TestMessage", this, &TestApp::recvMessage1);
    CGlobalMessage::GetKernel()->attach("TestMessage", this, &TestApp::recvMessage2);

    //TGenericInvoker::GetKernel()->RegisterAny(&Add);
    //TGenericInvoker::GetKernel()->RegisterAny(&TestApp::newTestApp);
    //TGenericInvoker::GetKernel()->RegisterAny(&TestApp::GetTestIndex);
    //TGenericInvoker::GetKernel()->RegisterAnyExact(&TestApp::newTestApp);
    //TGenericInvoker::GetKernel()->RegisterAnyExact(&TestApp::GetTestIndex);
    //TGenericInvoker::GetKernel()->Register("Add", &Add);
    //TGenericInvoker::GetKernel()->Register("newTestApp", &TestApp::newTestApp);
    //TGenericInvoker::GetKernel()->RegisterExact("Add", &Add);
    //TGenericInvoker::GetKernel()->RegisterExact("newTestApp", &TestApp::newTestApp);
    TFastRPC::Instance().Register<&Add>("Add");
    TFastRPC::Instance().Register<&TestApp::refTestApp>("refTestApp");
    TFastRPC::Instance().Register<&TestApp::newTestApp>("newTestApp");
}

TestApp::~TestApp()
{
    delete ui;
}

void TestApp::on_pushButton_clicked()
{
	//CGlobalMessage::GetKernel()->notify("TestMessage");
    int b = 5;
	//std::any result = TGenericInvoker::GetKernel()->Invoke("Add", 3, b);
    //TGenericInvoker::GetKernel()->InvokeAny(this, this);
    //TGenericInvoker::GetKernel()->InvokeAny(3, 5);
    //TGenericInvoker::GetKernel()->InvokeAny();
    //TGenericInvoker::GetKernel()->InvokeAnyExact<TestApp*, TestApp>(this, this);
    //TGenericInvoker::GetKernel()->InvokeAnyExact<int, void>();
    //TGenericInvoker::GetKernel()->Invoke("Add", 3, 5);
    //TGenericInvoker::GetKernel()->Invoke("newTestApp", this, this);
    //TGenericInvoker::GetKernel()->InvokeExact<int, void>("Add", 3,5);
    //TGenericInvoker::GetKernel()->InvokeExact<TestApp*, TestApp>("newTestApp", this, this);
    TFastRPC::Instance().Invoke<int&>("Add", b, b);
    TestApp& app_ref = TFastRPC::Instance().Invoke<TestApp&>("refTestApp", this, *this);
    TestApp* app_ptr = TFastRPC::Instance().Invoke<TestApp*>("newTestApp", this, this);
}

void TestApp::on_pushButton2_clicked()
{
    CGlobalMessage::GetKernel()->notify("TestMessage", QVariant("123"));
}

void TestApp::recvMessage1()
{
    CGlobalMessage::GetKernel()->detach("TestMessage", this, &TestApp::recvMessage1);

    
}

void TestApp::recvMessage2(const QVariant& var)
{
    CGlobalMessage::GetKernel()->detach("TestMessage", this, &TestApp::recvMessage2);
}
