#ifndef MUDUO_EXAMPLES_HUB_PUBSUB_H
#define MUDUO_EXAMPLES_HUB_PUBSUB_H

#include "muduo/net/TcpClient.h"

namespace pubsub {
using muduo::string;

// FIXME: dtor is not thread safe
class PubSubClient : muduo::noncopyable
{
public:
    typedef std::function<void(PubSubClient*)>                                                ConnectionCallback;
    typedef std::function<void(const string& topic, const string& content, muduo::Timestamp)> SubscribeCallback;

    PubSubClient(muduo::net::EventLoop* loop, const muduo::net::InetAddress& hubAddr, const string& name);
    void start();
    void stop();
    bool connected() const;

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }

    bool subscribe(const string& topic, const SubscribeCallback& cb);
    void unsubscribe(const string& topic);
    bool publish(const string& topic, const string& content);

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receiveTime);
    bool send(const string& message);

    muduo::net::TcpClient        client_;
    muduo::net::TcpConnectionPtr conn_;
    ConnectionCallback           connectionCallback_;
    SubscribeCallback            subscribeCallback_;
};
} // namespace pubsub

#endif // MUDUO_EXAMPLES_HUB_PUBSUB_H
