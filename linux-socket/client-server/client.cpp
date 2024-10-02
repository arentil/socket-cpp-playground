#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cstring>
#include <csignal>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

constexpr auto PORT = "3490";
constexpr auto MAX_DATA_SIZE = 100;

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr);
    }

    return &(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAX_DATA_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        std::cerr << "usage: client hostname" << std::endl;
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: connect");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == nullptr)
    {
        std::cerr << "client: failed to connect" << std::endl;
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr(reinterpret_cast<struct sockaddr *>(p->ai_addr)), s, sizeof(s));
    std::cout << "client: connecting to " << s << std::endl;

    freeaddrinfo(servinfo);

    if ((numbytes = recv(sockfd, buf, MAX_DATA_SIZE - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    std::cout << "client: received " << buf;

    close(sockfd);

    return 0;
}