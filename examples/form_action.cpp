#include <stdio.h>

#include "../http_server.h"
#include "../util.h"
#include "../http_client.h"
#include "../lock_guard.h"

using namespace mevent;
using namespace mevent::util;

const char *index_html =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n"
"    \"http://www.w3.org/TR/html4/strict.dtd\">\n"
"<html>\n"
"<head>\n"
"    <meta http-equiv=\"Content-type\" content=\"text/html charset=utf-8\" />\n"
"    <title>mevent - form test</title>\n"
"</head>\n"
"<body>\n"
"<form action=\"form_action\" method=\"get\">\n"
"  <p>First name: <input type=\"text\" name=\"fname\" /></p>\n"
"  <p>Last name: <input type=\"text\" name=\"lname\" /></p>\n"
"  <input type=\"submit\" value=\"Submit(GET)\" />\n"
"</form>\n"
"<hr />\n"
"<form action=\"form_action\" method=\"post\">\n"
"  <p>First name: <input type=\"text\" name=\"fname\" /></p>\n"
"  <p>Last name: <input type=\"text\" name=\"lname\" /></p>\n"
"  <input type=\"submit\" value=\"Submit(POST)\" />\n"
"</form>\n"
"</body>\n"
"</html>\n";

class FormAction {
public:
    void Index(Connection *conn) {
        conn->Resp()->SetHeader("Content-Type", "text/html");
        conn->Resp()->WriteString(index_html);
    }
    
    void Action(Connection *conn) {
        Request *req = conn->Req();
        Response *resp = conn->Resp();
        conn->Resp()->SetHeader("Content-Type", "text/plain");
        std::string str;
        if (req->method() == RequestMethod::GET) {
            req->ParseQueryString();
            str = "GET:";
            str += req->QueryString() + "\n";
            str += "First name:" + req->QueryStringValue("fname") + "\n";
            str += "Last name:" + req->QueryStringValue("lname") + "\n";
        } else if (req->method() == RequestMethod::POST) {
            req->ParsePostForm();
            str = "POST:";
            str += req->Body() + "\n";
            str += "First name:" + req->PostFormValue("fname") + "\n";
            str += "Last name:" + req->PostFormValue("lname") + "\n";
        }
        
        resp->WriteString(str);
    }
};

int main() {
    FormAction action;
    
    HTTPServer *server = new HTTPServer();
    server->SetHandler("/", std::bind(&::FormAction::Index, &action, std::placeholders::_1));
    server->SetHandler("/form_action", std::bind(&::FormAction::Action, &action, std::placeholders::_1));
    
    server->SetWorkerThreads(4);
    server->SetIdleTimeout(60);
    server->SetMaxWorkerConnections(8192);
    
    server->ListenAndServe("0.0.0.0", 80);
    
    return 0;
}