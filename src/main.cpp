#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

#define DEFAULT_PORT 9522

void main_loop();

void log(string log_str)
{
    cout<<log_str<<endl;
}

void summon_daemon()
{
    pid_t pid;
    pid_t sid;

    pid = fork();
    if (pid < 0)
    {
        exit(-1);
    }

    if (pid > 0)
    {
        cout<<"Created subprocess."<<endl;
        exit(0);
    }

    umask(0);

    sid = setsid();
    if (sid < 0)
    {
        log("sid failed.");
        exit(-1);
    }

    if (chdir("/") < 0)
    {
        log("Couldn't change working directory");
        exit(-1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void main_loop(short port)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    uint8_t buffer[2048];

    if (sock < 0)
    {
        log("cannot create socket");
        exit(-1);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        log("bind failed (port in use?");
        close(sock);
        exit(-1);
    }

    while(1)
    {
        struct sockaddr_storage from_addr;
        socklen_t from_len = sizeof(from_addr);
        int byte_count;
        byte_count = recvfrom(sock, buffer, 2048, 0,
                                (struct sockaddr*)&from_addr, &from_len);
        byte_count = sendto(sock, buffer, byte_count, 0,
                (struct sockaddr*)&from_addr, from_len);
    }

    close(sock);
}

int main(int argc, char* argv[])
{
    int rval;
    bool daemonize = false;
    char *pstr = NULL;
    short port;
    
    while ((rval = getopt(argc, argv, "dp:")) != -1)
    {
        switch (rval)
        {
            case 'd':
                daemonize = true;
                break;
            case 'p':
                pstr = optarg;
                break;
            case '?':
                if (optopt == 'p')
                    cerr<<"Option -p requires a port number"<<endl;
                else
                    cerr<<"Unknown option: "<<(char)optopt<<endl;
                return 1;
            default:
                abort();
        }
    }

    if (pstr != NULL)
    {
        port = atoi(pstr);
    }
    else
    {
        port = DEFAULT_PORT;
    }

    cout<<"Port: "<<port<<endl;
    cout<<"Daemonize? "<<daemonize<<endl;

    if (daemonize)
    {
        summon_daemon();
    }

    main_loop(port);

    return 0;
}

