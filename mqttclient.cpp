
#include "mqttclient.h"
#include <QDebug>

#define MQTT_HOSTNAME   "39.99.179.154"
#define MQTT_PORT       1883

MQTTClient::MQTTClient(const QString &hostName, int port, const QString &userName, const QString &passwordMD5, QObject *parent) : QObject(parent)
{
    m_client=NULL;
    this->m_hostName=hostName;
    this->m_port=port;
    this->m_userName=userName;
    this->m_passwordMD5=passwordMD5;
}

MQTTClient::~MQTTClient()
{
    if(m_client!=NULL)
    {
        if(m_client->state()==QMqttClient::Connected)
        {
            m_client->disconnectFromHost();
        }
        delete m_client;
        m_client=NULL;
    }
}

bool MQTTClient::isOpened()
{
    if(m_client==NULL)
    {
        return false;
    }
    else if(m_client->state()==QMqttClient::Connected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MQTTClient::open()
{
    if(m_client!=NULL)
    {
        return;
    }
    m_client = new QMqttClient(this);
    m_client->setHostname(this->m_hostName);
    m_client->setPort(this->m_port);
    m_client->setUsername(m_userName);
    m_client->setPassword(m_passwordMD5);
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);

    connect(m_client, SIGNAL(connected()), this, SLOT(doConnected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(doDisconnected()));
    connect(m_client, SIGNAL(messageReceived(QByteArray,QMqttTopicName)), this, SLOT(doMessageReceived(QByteArray,QMqttTopicName)));

    m_client->connectToHost();
    emit log("MQTTClient 正在连接...");
}

void MQTTClient::close()
{
    if(m_client!=NULL)
    {
        if(m_client->state()==QMqttClient::Connected)
        {
            m_client->disconnectFromHost();
        }
        delete m_client;
        m_client=NULL;
    }
}

void MQTTClient::pub(const QString &topic, const QString &data)
{
    if(m_client==NULL)
    {
        emit log("连接未建立，无法发布消息！");
        return;
    }
    m_client->publish(topic, data.toUtf8());
    emit log(QString("MQTTClient 正发布设备消息，主题: %1, 消息:%2").arg(topic).arg(data));
}

void MQTTClient::sub(const QString &topic)
{
    if(m_client==NULL)
    {
        emit log("连接未建立，无法订阅消息！");
        return;
    }
    emit log(QString("MQTTClient 正订阅设备消息，主题: %1").arg(topic));
    QMqttSubscription *subscription=m_client->subscribe(topic);
    if(subscription!=NULL)
    {
        connect(subscription, SIGNAL(stateChanged(SubscriptionState)),
                this, SLOT(doQMqttSubscriptionStateChanged(SubscriptionState)));
    }
}

void MQTTClient::doConnected()
{
    emit log("MQTTClient 已建立连接！");
}

void MQTTClient::doDisconnected()
{
    emit log("MQTTClient 已断开连接！");
    this->close();
}

void MQTTClient::doMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    emit log(QString("MQTTClient 接收主题为 \"%1\" 的消息， 数据: %2").arg(topic.name()).arg(QString::fromUtf8(message)));
}

void MQTTClient::doQMqttSubscriptionStateChanged(SubscriptionState subscriptionState)
{
    QMqttSubscription *subscription=static_cast<QMqttSubscription*>(sender());
    if(subscription==NULL)
    {
        return;
    }
    if(subscriptionState==QMqttSubscription::Subscribed)
    {
        emit log(QString("订阅主题为 \"%1\" 的消息成功！").arg(subscription->topic().filter()));
    }
}
