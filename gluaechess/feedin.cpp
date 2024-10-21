#include <iostream>

#include "gl_protocol.h"
#include "os_net.h"
#include "sg_cout.h"
#include "gl_utils.h"

const char * sip = "127.0.0.1";
const char * udp = "224.0.0.1";
const gl::ushort port = 8081;

gl::NetworkLimits nl;

using vs = std::vector<string>;


void client(const string & m)
{
    os::IpAddr linkTcp(sip, port);
    os::IpAddr linkUdp(udp, port);

    gl::ProtHq http(gl::ProtHq::Client);

    os::net::TcpClient tcpClient(&http, linkTcp, nl, nullptr);
    //os::net::Multicast udpClient(linkUdp);

    tcpClient.send_msg(m);
    //udpClient.send_str("Hello udp world!");

    string msg;
    switch ( tcpClient.recv_msg(msg) )
    {
        case gl::PKT_COMPLETE:
            os::Cout() << "client got [" << msg << "]\n" << os::flush;
            break;
        case gl::PKT_INCOMPLETE:
            os::Cout() << "client got incomplete message\n" << os::flush;
            break;
        case gl::PKT_ERROR:
            os::Cout() << "client got error\n" << os::flush;
            break;
        default:
            os::Cout() << "Really bad: urgent debug needed\n" << os::flush;
    }
}

void server()
{
    gl::ProtHq http(gl::ProtHq::Server);

    os::IpAddr linkTcp(sip, port);

    os::net::TcpAcceptor tcpServer(&http, linkTcp);

    os::net::Selector select(nl);

    select.addAcceptor(&tcpServer);

    std::cerr << "Listening on " << sip << ':' << port << std::endl;

    while (1)
    {
        os::net::Socket * s = select.wait(400000);
        if ( !s ) { std::cerr << '.' << std::flush; continue; }

        string packet = s->getReceivedMessage();

        std::cerr << "server got [" << packet << "] from "
                  << s->getAddr().str() << "\n" << std::flush;
        s->send_msg(packet + ":hehe");
        delete s;
        if ( packet == "bye" ) break;
    }
}

struct Console : os::Blockable
{
    os::Semaphore * s;
    bool exitf;

    Console(os::Semaphore * s): Blockable(&exitf), s(s), exitf(false) {}
    void runOnceUnconditionally();
    os::Semaphore * getMainSemaphore() const { return s; }
};

int main(int ac, char * av[])
try
{
    os::net::NetInitialiser netini;

    if ( ac < 2 ) {}

    {
        os::Semaphore sem;
        Console con(&sem);
        os::Thread t(con);
        sem.up();
        server();
    }

    return 0;
}
catch (gl::Exception e)
{
    std::cout << "Error: " << e.str() << '\n';
}


void Console::runOnceUnconditionally()
{
    // allow next run
    getMainSemaphore()->up();

    try
    {
        string cmd;
        std::getline(std::cin, cmd);
        if ( !std::cin ) cmd = "quit";

        vs toks = gl::tokenise(cmd);

        if ( toks.empty() ) {}
        else if ( toks[0][0] == '#' ) {}

        else if ( toks[0] == "help" )
        {
            os::Cout() << "help       - print help page" << os::endl;
            os::Cout() << "ips        - print interfaces" << os::endl;
            os::Cout() << "quit or q  - shutdown" << os::endl;
        }

        else if ( toks[0] == "quit" || toks[0] == "q" )
        {
            os::Cout() << "bye" << os::endl;
            exitf = true;
            client("bye");
            return;
        }

        else if ( toks[0] == "ips" )
        {
            os::Cout() << os::net::NetInitialiser::str() << os::endl;
        }

        else
            os::Cout() << cmd << os::endl;

        ///os::Cout() << "> " << os::flush;
    }
    catch (gl::Exception e)
    {
        os::Cout() << "Fatal error (gl::Exception): " << e.str() << os::endl;
    }
    catch (const char * e) { os::Cout() << "Fatal error (char*): " << e << os::endl; }
    catch (string * e) { os::Cout() << "Fatal error (string): " << e << os::endl; }

}

