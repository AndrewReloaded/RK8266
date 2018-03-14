#ifndef LIBESP_TCPSOCKET_H
#define LIBESP_TCPSOCKET_H


#include <c_types.h>
#include <os_type.h>
#include <ip_addr.h>


class TCPSocket
{
public:
    // ������� ������ �����
    TCPSocket();
    
    // ������� ������
    TCPSocket(int port);
    
    // ������� ������
    TCPSocket(const char *host, int port);
    
    // ������� ����� �� ����������
    TCPSocket(TCPSocket *parent, struct espconn *c);
    
    // ����� �����
    virtual ~TCPSocket();
    
    
    // ������������ � �����
    void connectToHost(const char *host, int port);
    
    // ��������� ������
    void listen(int port);
    
    
    virtual TCPSocket* clientConnected(struct espconn *c);
    
    virtual void connected();
    virtual void dataReceived(const uint8_t *data, int len);
    virtual void dataSent();
    virtual void poll();
    virtual void error(int err);
    virtual void disconnected();
    
    
    bool send(const uint8_t *data, int len);
    bool sendAndFree(const uint8_t *data, int len);
    void enableSendQueue(int len);
    int sendQueueFree();
    void hold();
    void unhold();
    void setPoll(int ms);
    void close();
    void setTimeout(int secs);
    const uint8_t* localIP();
    const uint8_t* remoteIP();


    
//private:
    TCPSocket *__parent;
    bool __is_server;
    struct espconn *__conn;
    os_timer_t __tmr;
    ip_addr_t __ip;
    int __port;
    bool __delete_espconn;
    struct sendq *__sendq;
    int __sendq_len, __sendq_used;
    
    void dataSent_Q();
};


#endif
