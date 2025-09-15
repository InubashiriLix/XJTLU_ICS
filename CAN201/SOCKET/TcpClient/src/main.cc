// tcp_client.cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

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

int main(int argc, char** argv) {
    const char* ip = (argc >= 2) ? argv[1] : "127.0.0.1";
    int port = (argc >= 3) ? std::atoi(argv[2]) : 9000;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &srv.sin_addr) != 1) {
        std::cerr << "bad ip\n";
        return 1;
    }

    if (connect(fd, (sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect");
        return 1;
    }
    std::cout << "[*] connected to " << ip << ":" << port << "\n";

    std::string line;
    char buf[4096];
    while (std::getline(std::cin, line)) {
        line.push_back('\n');  // 以换行作为一条消息
        if (!send_all(fd, line.data(), line.size())) break;

        // 读回显，直到读到 '\n'
        std::string resp;
        for (;;) {
            ssize_t n = recv(fd, buf, sizeof(buf), 0);
            if (n == 0) {
                std::cout << "server closed\n";
                close(fd);
                return 0;
            }
            if (n < 0) {
                if (errno == EINTR) continue;
                perror("recv");
                close(fd);
                return 1;
            }
            resp.append(buf, buf + n);
            if (!resp.empty() && resp.back() == '\n') break;
        }
        std::cout << "[echo] " << resp;
    }
    close(fd);
    return 0;
}
