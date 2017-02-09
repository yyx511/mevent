#include <stdio.h>

#include "../http_server.h"
#include "../util.h"
#include "../http_client.h"
#include "../lock_guard.h"

using namespace mevent;
using namespace mevent::util;

class HelloWorld {
public:
    void Index(Connection *conn) {
        conn->Resp()->SetHeader("Content-Type", "text/html");
        conn->Resp()->WriteString("hello world!");
    }
};

int main() {
    HelloWorld hello;
    
    HTTPServer *server = new HTTPServer();
    server->SetHandler("/", std::bind(&::HelloWorld::Index, &hello, std::placeholders::_1));

    server->SetWorkerThreads(4);
    server->SetIdleTimeout(60);
    server->SetMaxWorkerConnections(8192);
    
    server->ListenAndServe("0.0.0.0", 80);
    
    return 0;
}
