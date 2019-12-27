
#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    m_mqttClient=NULL;
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_buttonConnect_clicked()
{
    if(m_mqttClient!=NULL)
    {
        m_mqttClient->close();
        m_mqttClient=NULL;
    }
    m_mqttClient=new MQTTClient(ui->lineHostName->text().trimmed(), ui->linePort->text().trimmed().toInt(), ui->lineUserName->text(), ui->linePassword->text());
    connect(m_mqttClient, SIGNAL(log(QString)), this, SLOT(doLog(QString)));
    m_mqttClient->open();
}

void MainWidget::on_buttonClear_clicked()
{
    ui->textLog->clear();
}

void MainWidget::doLog(const QString &text)
{
    ui->textLog->append(text);
}

void MainWidget::on_buttonSub_clicked()
{
    if((m_mqttClient==NULL) || (!m_mqttClient->isOpened()))
    {
        ui->textLog->append("MQTT客户端未打开!");
        return;
    }
    m_mqttClient->sub(ui->lineSubTopic->text().trimmed());
}

void MainWidget::on_buttonPub_clicked()
{
    m_mqttClient->pub(ui->linePubTopic->text(), ui->linePubData->text());
}

void MainWidget::on_buttonClean_clicked()
{
    ui->textLog->clear();
}
