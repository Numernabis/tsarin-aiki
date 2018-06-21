/*
  author: Ludi
  file:   client.c
  start:  12.06.2018
  end:    21.06.2018
  lines:  123
*/
#include "commmon.h"

char* client_name;
char* mode;
char* server_address; //IPv4
unsigned short port_num;
char* socket_path;
int socketd;

/* -------------------------------------------------------------------------- */
void handle_signal(int signum) {
    printf(RED"\nReceived SIG=%d -- closing\n"RST, signum);
    exit(EXIT_SUCCESS);
}

void clean_up(void) {
    shutdown(socketd, SHUT_RDWR);
    close(socketd);
}
/* -------------------------------------------------------------------------- */
int init_inet_socket() {
    int inet = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    inet_pton(AF_INET, server_address, &addr.sin_addr);

    connect(inet, (struct sockaddr*) &(addr), sizeof(addr));
    return inet;
}

int init_local_socket() {
    int local = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    connect(local, (struct sockaddr*) &(addr), sizeof(addr));
    return local;
}
/* -------------------------------------------------------------------------- */
void calc(expr e, int id) {
    int ans;
    switch (e.type) {
        case SUM: ans = e.arg1 + e.arg2; break;
        case SUB: ans = e.arg1 - e.arg2; break;
        case MUL: ans = e.arg1 * e.arg2; break;
        case DIV: ans = e.arg1 / e.arg2; break;
    }
    msg m;
    strcpy(m.name, client_name);
    m.mid = id;
    m.type = REPLY;
    m.expr.arg1 = ans;
    write(socketd, &m, sizeof(m));
}
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc < 4) {
        printf(YEL"Invalid program call. Proper arguments as follows:\n");
        printf("./main  client_name  mode  address  (port)\n"RST);
        return 2;
    }
    signal(SIGINT, handle_signal);
    atexit(clean_up);

    client_name = argv[1];
    mode = argv[2];

    if (strcmp(mode, "local") == 0) {
        socket_path = argv[3];
        socketd = init_local_socket();
    } else if (strcmp(mode, "inet") == 0) {
        server_address = argv[3];
        port_num = (unsigned short) strtoul(argv[4], NULL, 0);
        socketd = init_inet_socket();
    } else {
        printf(YEL"Invalid mode. Available: local, inet\n"RST);
        return 2;
    }

    /* login client */
    msg m;
    strcpy(m.name, client_name);
    m.type = LOGIN;
    write(socketd, &m, sizeof(m));

    /* ---------------------------------------------------------------------- */
    while (1) {
        msg m;
        ssize_t bytes_read = recv(socketd, &m, sizeof(m), MSG_WAITALL);
        if (bytes_read == 0) {
            printf(RED"probably server caput\n"RST);
            fflush(stdout);
            exit(0);
        }
        switch (m.type) {
            case PINGU:
                printf(GRE"pongu!\n"RST);
                fflush(stdout);
                write(socketd, &m, sizeof(m));
                break;
            case TASK:
                printf(YEL"task received\n"RST);
                fflush(stdout);
                calc(m.expr, m.mid);
                break;
            case LOGIN:
                printf(RED"client_name is occupied!\n"RST);
                exit(1);
        }
    }
    /* ---------------------------------------------------------------------- */
    return 0;
}
/* -------------------------------------------------------------------------- */
