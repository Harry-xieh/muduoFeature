#ifndef MUDUO_EXAMPLES_CURL_CURL_H
#define MUDUO_EXAMPLES_CURL_CURL_H

#include "muduo/base/StringPiece.h"
#include "muduo/base/noncopyable.h"
#include "muduo/net/Callbacks.h"

extern "C" {
typedef void CURLM;
typedef void CURL;
}

namespace muduo {
namespace net {
class Channel;
class EventLoop;
} // namespace net
} // namespace muduo

namespace curl {

class Curl;

class Request : public std::enable_shared_from_this<Request>, muduo::noncopyable
{
public:
    typedef std::function<void(const char*, int)> DataCallback;
    typedef std::function<void(Request*, int)>    DoneCallback;

    Request(Curl*, const char* url);
    ~Request();

    void setDataCallback(const DataCallback& cb)
    {
        dataCb_ = cb;
    }

    void setDoneCallback(const DoneCallback& cb)
    {
        doneCb_ = cb;
    }

    void setHeaderCallback(const DataCallback& cb)
    {
        headerCb_ = cb;
    }

    // void allowRedirect(int redirects);
    void headerOnly();
    void setRange(const muduo::StringArg range);

    template <typename OPT>
    int setopt(OPT opt, long p)
    {
        return curl_easy_setopt(curl_, opt, p);
    }

    template <typename OPT>
    int setopt(OPT opt, const char* p)
    {
        return curl_easy_setopt(curl_, opt, p);
    }

    template <typename OPT>
    int setopt(OPT opt, void* p)
    {
        return curl_easy_setopt(curl_, opt, p);
    }

    template <typename OPT>
    int setopt(OPT opt, size_t (*p)(char*, size_t, size_t, void*))
    {
        return curl_easy_setopt(curl_, opt, p);
    }

    const char* getEffectiveUrl();
    const char* getRedirectUrl();
    int         getResponseCode();

    // internal
    muduo::net::Channel* setChannel(int fd);
    void                 removeChannel();
    void                 done(int code);
    CURL*                getCurl()
    {
        return curl_;
    }
    muduo::net::Channel* getChannel()
    {
        return muduo::get_pointer(channel_);
    }

private:
    void          dataCallback(const char* buffer, int len);
    void          headerCallback(const char* buffer, int len);
    static size_t writeData(char* buffer, size_t size, size_t nmemb, void* userp);
    static size_t headerData(char* buffer, size_t size, size_t nmemb, void* userp);
    void          doneCallback();

    class Curl*                          owner_;
    CURL*                                curl_;
    std::shared_ptr<muduo::net::Channel> channel_;
    DataCallback                         dataCb_;
    DataCallback                         headerCb_;
    DoneCallback                         doneCb_;
};

typedef std::shared_ptr<Request> RequestPtr;

class Curl : muduo::noncopyable
{
public:
    enum Option
    {
        kCURLnossl = 0,
        kCURLssl   = 1,
    };

    explicit Curl(muduo::net::EventLoop* loop);
    ~Curl();

    RequestPtr getUrl(muduo::StringArg url);

    static void initialize(Option opt = kCURLnossl);

    // internal
    CURLM* getCurlm()
    {
        return curlm_;
    }
    muduo::net::EventLoop* getLoop()
    {
        return loop_;
    }

private:
    void onTimer();
    void onRead(int fd);
    void onWrite(int fd);
    void checkFinish();

    static int socketCallback(CURL*, int, int, void*, void*);
    static int timerCallback(CURLM*, long, void*);

    muduo::net::EventLoop* loop_;
    CURLM*                 curlm_;
    int                    runningHandles_;
    int                    prevRunningHandles_;
};

} // namespace curl

#endif // MUDUO_EXAMPLES_CURL_CURL_H
