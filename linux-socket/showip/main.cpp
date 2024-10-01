#include <iostream>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
  struct addrinfo hints, *res, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  if (argc != 2)
  {
    std::cerr << "usage: showip hostname" << std::endl;
    return 1;
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC; // AF_INET for force IPv4 or AF_INET6 for force IPv6
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(argv[1], nullptr, &hints, &res)) != 0)
  {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    return 2;
  }

  std::cout << "IP address for " << argv[1] << std::endl;

  for (p = res; p != nullptr; p = p->ai_next)
  {
    void *addr;
    std::string ipver;

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET) // IPv4
    {
      struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    }
    else
    {
      struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6 *>(p->ai_addr);
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
    std::cout << "   " << ipver << ": " << ipstr << std::endl;
  }

  return 0;
}