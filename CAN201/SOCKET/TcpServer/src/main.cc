// tcp_server.cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

bool send_all(int fd, const char* buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, MSG_NOSIGNAL);
        if (n > 0) {
            sent += (size_t)n;
            continue;
        }
        if (n < 0 && errno == EINTR) continue;
        return false;
    }
    return true;
}

void handle_client(int cfd, sockaddr_in cli) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
    std::cout << "[+] client " << ip << ":" << ntohs(cli.sin_port) << "\n";

    char buf[4096];
    for (;;) {
        ssize_t n = recv(cfd, buf, sizeof(buf), 0);
        if (n == 0) break;  // 对端正常关闭
        if (n < 0) {
            if (errno == EINTR) continue;  // 被信号打断重试
            perror("recv");
            break;
        }
        // 回显
        if (!send_all(cfd, buf, (size_t)n)) break;
    }
    close(cfd);
    std::cout << "[-] client closed\n";
}

int main(int argc, char** argv) {
    int port = (argc >= 2) ? std::atoi(argv[1]) : 9000;
    std::signal(SIGPIPE, SIG_IGN);  // 防止写已关闭套接字导致进程被杀

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("socket");
        return 1;
    }

    int yes = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0
    addr.sin_port = htons(port);

    if (bind(sfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(sfd, 128) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "[*] listening on 0.0.0.0:" << port << "\n";

    for (;;) {
        sockaddr_in cli{};
        socklen_t len = sizeof(cli);
        int cfd = accept(sfd, (sockaddr*)&cli, &len);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }
        std::thread(handle_client, cfd, cli).detach();
    }
}
