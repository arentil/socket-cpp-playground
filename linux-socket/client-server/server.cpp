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
constexpr auto BACKLOG = 10;

void sigchld_handler(int s)
{
    // waitpid() might overwrite eerno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, nullptr, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr);
    }

    return &(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr);
}

int main()
{
    int sockfd, new_fd; // listen on sock
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector address info
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv == getaddrinfo(nullptr, PORT, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this scructure

    if (p == nullptr)
    {
        std::cerr << "server: failed to bind" << std::endl;
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    std::cout << "server: waiting for connections..." << std::endl;

    while (1) // main accept() loop
    {
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, reinterpret_cast<struct sockaddr *>(&their_addr), &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&their_addr)), s, sizeof(s));
        std::cout << "server: got connection from " << s << std::endl;

        if (!fork())
        {
            close(sockfd);
            if (send(new_fd, "Hello, world!\n", 13, 0) == -1)
            {
                perror("send");
            }
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }

    return 0;
}