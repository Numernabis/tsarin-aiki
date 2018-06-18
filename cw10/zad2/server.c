/*
  author: Ludi
  file:   server.c
  start:  18.06.2018
  end:    []
  lines:  228
*/
#include "commmon.h"

int inet, local, epolld;
char* socket_path;
client clients[MAX_CLIENTS];
pthread_mutex_t mtx;
pthread_t event_thread, ping_thread;

/* -------------------------------------------------------------------------- */
void handle_signal(int signum) {
    printf(RED"\nReceived SIG=%d -- closing\n"RST, signum);
    exit(EXIT_SUCCESS);
}

void clean_up(void) {
    // send message to clients (inform about closing)
    pthread_mutex_destroy(&mtx);
    close(epolld); close(inet); close(local);
    remove(socket_path);
}
/* -------------------------------------------------------------------------- */
void init_inet_socket(unsigned const short port_num) {
    inet = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(inet, (struct sockaddr*) &(addr), sizeof(addr));
}

void init_local_socket(const char* socket_path) {
    local = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    bind(local, (struct sockaddr*) &(addr), sizeof(addr));
}

void init_epoll() {
    epolld = epoll_create1(0);
    struct epoll_event ee;

    ee.events = EPOLLIN | EPOLLET;
    ee.data.fd = inet;
    epoll_ctl(epolld, EPOLL_CTL_ADD, inet, &ee);

    ee.events = EPOLLIN | EPOLLET;
    ee.data.fd = local;
    epoll_ctl(epolld, EPOLL_CTL_ADD, local, &ee);
}
/* -------------------------------------------------------------------------- */
void close_connection(struct epoll_event ee) {
    pthread_mutex_lock(&mtx);
    shutdown(ee.data.fd, SHUT_RDWR);
    close(ee.data.fd);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].pings > 0 && ee.data.fd == clients[i].fd) {
            clients[i].pings = -1;
            for (int j = 0; j < MAX_NAME; j++)
                clients[i].name[j] = 0;
        }
    }
    pthread_mutex_unlock(&mtx);
}

void handle_response(struct epoll_event ee) {
    msg m;
    struct sockaddr* addr = malloc(sizeof(struct sockaddr));
    socklen_t addrs =  sizeof(struct sockaddr);
    ssize_t bytes_read = recvfrom(ee.data.fd, &m, sizeof(m), 0, addr, &addrs);
    if (bytes_read == 0) {
        printf(YEL"ending connection with client %d\n"RST, ee.data.fd);
        fflush(stdout);
        close_connection(ee);
        return;
    }

    switch (m.type) {
        case REPLY:
            printf(GRE"expr %d, client %s, answer: %d\n"RST, m.mid, m.name, m.expr.arg1);
            fflush(stdout);
            return;
        case PINGU:
            pthread_mutex_lock(&mtx);
            clients[m.mid].pings = 0;
            pthread_mutex_unlock(&mtx);
            return;
        case LOGIN:
            pthread_mutex_lock(&mtx);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].pings >= 0 && strcmp(m.name, clients[i].name) == 0) {
                    pthread_mutex_unlock(&mtx);
                    sendto(ee.data.fd, &m, sizeof(m), 0, &clients[i].addr, clients[i].addrs);
                    close_connection(ee);
                    return;
                }
            }
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].pings == -1) {
                    printf(GRE"starting connection with client %d - %s.\n"RST, ee.data.fd, m.name);
                    fflush(stdout);
                    strcpy(clients[i].name, m.name);
                    clients[i].addr = *addr;
                    clients[i].addrs = addrs;
                    clients[i].fd = ee.data.fd;
                    clients[i].pings = 0;
                    pthread_mutex_unlock(&mtx);
                    return;
                }
            }
        default:
            break;
    }
}
/* -------------------------------------------------------------------------- */
void* event(void* args) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].pings = -1;
    struct epoll_event ee[MAX_EVENTS];

    while (1) {
        int counter = epoll_wait(epolld, ee, MAX_EVENTS, -1);
        for (int i = 0; i < counter; i++) {
            handle_response(ee[i]);
        }
    }
}

void* ping(void* args) {
    while (1) {
        msg m;
        m.type = PINGU;
        pthread_mutex_lock(&mtx);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].pings == 0) {
                clients[i].pings = 1;
                m.mid = i;
                sendto(clients[i].fd, &m, sizeof(m), 0, &clients[i].addr, clients[i].addrs);
            }
        }
        pthread_mutex_unlock(&mtx);
        sleep(5);
        pthread_mutex_lock(&mtx);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].pings == 1) {
                clients[i].pings = -1;
                for (int j = 0 ; j < MAX_NAME; j++)
                    clients[i].name[j] = 0;
            }
        }
        pthread_mutex_unlock(&mtx);
        sleep(10);
    }
}
/* -------------------------------------------------------------------------- */
int main(int argc, char** argv) {
    if (argc != 3) {
        printf(YEL"Invalid program call. Proper arguments as follows:\n");
        printf("./main  port_num  socket_path\n"RST);
        return 2;
    }
    signal(SIGINT, handle_signal);
    atexit(clean_up);

    unsigned short port_num = (unsigned short) strtoul(argv[1], NULL, 0);
    socket_path = argv[2];

    init_inet_socket(port_num);
    init_local_socket(socket_path);
    init_epoll();
    pthread_mutex_init(&mtx, NULL);
    pthread_create(&event_thread, NULL, event, NULL);
    pthread_create(&ping_thread, NULL, ping, NULL);
    /* ---------------------------------------------------------------------- */
    int z = 0;
    while (1) {
        char c, s;
        int arg1, arg2;
        printf("%d> ", z);

        int res = scanf("%d %c %d", &arg1, &s, &arg2);
        if (res != 3) {
            while((c = getchar()) != '\n' && c != EOF);
            printf(YEL"give three arguments! \n"RST);
            continue;
        }
        msg m;
        m.expr.arg1 = arg1;
        m.expr.arg2 = arg2;
        m.mid = z++;
        m.type = TASK;

        switch (s) {
            case '+': m.expr.type = SUM; break;
            case '-': m.expr.type = SUB; break;
            case '*': m.expr.type = MUL; break;
            case '/': m.expr.type = DIV; break;
        }
        int done = 0;
        while (!done) {
            pthread_mutex_lock(&mtx);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].pings >= 0) {
                    sendto(clients[i].fd, &m, sizeof(m), 0, &clients[i].addr, clients[i].addrs);
                    sleep(2);
                    done = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&mtx);
        }
    }
    /* ---------------------------------------------------------------------- */
    pthread_cancel(event_thread);
    pthread_cancel(ping_thread);
    return 0;
}
/* -------------------------------------------------------------------------- */
