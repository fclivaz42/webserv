Allowed functions

Ctrl+K V Open Markdown preview to the side

| Fonction         | Bibliothèque      | Prototype                                             | Description                                                                                         |
|------------------|-------------------|-------------------------------------------------------|-----------------------------------------------------------------------------------------------------|
| **execve**       | `<unistd.h>`      | `int execve(const char *pathname, char *const argv[], char *const envp[]);` | Remplace le processus courant par un nouveau processus.                                              |
| **dup**          | `<unistd.h>`      | `int dup(int oldfd);`                                  | Duplique un descripteur de fichier existant.                                                         |
| **pipe**         | `<unistd.h>`      | `int pipe(int pipefd[2]);`                             | Crée un canal de communication unidirectionnel (tube).                                               |
| **strerror**     | `<string.h>`      | `char *strerror(int errnum);`                          | Retourne une chaîne de caractères décrivant l'erreur spécifiée par errnum.                           |
| **gai_strerror** | `<netdb.h>`       | `const char *gai_strerror(int errcode);`               | Retourne une chaîne de caractères décrivant l'erreur d'une fonction getaddrinfo.                     |
| **errno**        | `<errno.h>`       | `int errno;`                                           | Variable globale indiquant le dernier code d'erreur produit par une fonction système.                |
| **fork**         | `<unistd.h>`      | `pid_t fork(void);`                                    | Crée un nouveau processus en dupliquant le processus courant.                                        |
| **socketpair**   | `<sys/socket.h>`  | `int socketpair(int domain, int type, int protocol, int sv[2]);` | Crée une paire de sockets connectées.                                                                |
| **htons**        | `<arpa/inet.h>`   | `uint16_t htons(uint16_t hostshort);`                  | Convertit l'ordre des octets d'un entier court (16 bits) de l'hôte à l'ordre réseau.                 |
| **htonl**        | `<arpa/inet.h>`   | `uint32_t htonl(uint32_t hostlong);`                   | Convertit l'ordre des octets d'un entier long (32 bits) de l'hôte à l'ordre réseau.                  |
| **ntohs**        | `<arpa/inet.h>`   | `uint16_t ntohs(uint16_t netshort);`                   | Convertit l'ordre des octets d'un entier court (16 bits) du réseau à l'ordre hôte.                   |
| **ntohl**        | `<arpa/inet.h>`   | `uint32_t ntohl(uint32_t netlong);`                    | Convertit l'ordre des octets d'un entier long (32 bits) du réseau à l'ordre hôte.                    |
| **select**       | `<sys/select.h>`  | `int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);` | Surveille plusieurs descripteurs de fichiers pour voir s'ils sont prêts pour E/S.                    |
| **poll**         | `<poll.h>`        | `int poll(struct pollfd *fds, nfds_t nfds, int timeout);` | Surveille plusieurs descripteurs de fichiers pour voir s'ils sont prêts pour E/S.                    |
| **epoll_create** | `<sys/epoll.h>`   | `int epoll_create(int size);`                          | Crée un descripteur de fichier pour un nouvel instance d'epoll.                                      |
| **epoll_ctl**    | `<sys/epoll.h>`   | `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);` | Contrôle une interface epoll.                                                                        |
| **epoll_wait**   | `<sys/epoll.h>`   | `int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);` | Attend des événements sur un descripteur epoll.                                                      |
| **kqueue**       | `<sys/event.h>`   | `int kqueue(void);`                                    | Crée un nouveau descripteur d'événements de type kqueue.                                             |
| **kevent**       | `<sys/event.h>`   | `int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);` | Registre des événements avec un kqueue et attend qu'ils se produisent.                               |
| **socket**       | `<sys/socket.h>`  | `int socket(int domain, int type, int protocol);`      | Crée un point de communication (socket).                                                             |
| **accept**       | `<sys/socket.h>`  | `int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);` | Accepte une connexion sur une socket.                                                                |
| **listen**       | `<sys/socket.h>`  | `int listen(int sockfd, int backlog);`                 | Marque la socket comme passible de recevoir des connexions entrantes.                                |
| **send**         | `<sys/socket.h>`  | `ssize_t send(int sockfd, const void *buf, size_t len, int flags);` | Envoie un message sur une socket.                                                                    |
| **recv**         | `<sys/socket.h>`  | `ssize_t recv(int sockfd, void *buf, size_t len, int flags);` | Reçoit un message depuis une socket.                                                                 |
| **chdir**        | `<unistd.h>`      | `int chdir(const char *path);`                         | Change le répertoire de travail courant.                                                             |
| **bind**         | `<sys/socket.h>`  | `int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);` | Associe une adresse à une socket.                                                                    |
| **connect**      | `<sys/socket.h>`  | `int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);` | Établit une connexion sur une socket.                                                                |
| **getaddrinfo**  | `<netdb.h>`       | `int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);` | Résout un nom de domaine ou une adresse IP en adresses de socket.                                     |
| **freeaddrinfo** | `<netdb.h>`       | `void freeaddrinfo(struct addrinfo *res);`             | Libère la mémoire allouée par getaddrinfo.                                                           |
| **setsockopt**   | `<sys/socket.h>`  | `int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);` | Configure des options pour la socket.                                                                |
| **getsockname**  | `<sys/socket.h>`  | `int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);` | Retourne l'adresse actuelle d'une socket.                                                            |
| **getprotobyname** | `<netdb.h>`    | `struct protoent *getprotobyname(const char *name);`   | Renvoie une structure protoent pour un protocole réseau spécifié par son nom.                        |
| **fcntl**        | `<fcntl.h>`       | `int fcntl(int fd, int cmd, ...);`                     | Effectue des opérations sur un descripteur de fichier.                                               |
| **close**        | `<unistd.h>`      | `int close(int fd);`                                   | Ferme un descripteur de fichier.                                                                     |
| **read**         | `<unistd.h>`      | `ssize_t read(int fd, void *buf, size_t count);`       | Lit des données depuis un descripteur de fichier.                                                    |
| **write**        | `<unistd.h>`      | `ssize_t write(int fd, const void *buf, size_t count);`| Écrit des données sur un descripteur de fichier.                                                     |
| **waitpid**      | `<sys/wait.h>`    | `pid_t waitpid(pid_t pid, int *wstatus, int options);` | Attend la fin d'un processus fils.                                                                   |
| **kill**         | `<signal.h>`      | `int kill(pid_t pid, int sig);`                        | Envoie un signal à un processus ou un groupe de processus.                                           |
| **signal**       | `<signal.h>`      | `sighandler_t signal(int signum, sighandler_t handler);` | Définit une fonction de gestion des signaux pour un signal spécifié.                                 |
| **access**       | `<unistd.h>`      | `int access(const char *pathname, int mode);`          | Vérifie l'existence d'un fichier et les permissions d'accès.                                         |
| **stat**         | `<sys/stat.h>`    | `int stat(const char *pathname, struct stat *statbuf);` | Récupère les informations sur un fichier.                                                            |
| **opendir**      | `<dirent.h>`      | `DIR *opendir(const char *name);`                      | Ouvre un répertoire pour la lecture.                                                                 |
| **readdir**      | `<dirent.h>`      | `struct dirent *readdir(DIR *dirp);`                   | Lit une entrée de répertoire.                                                                        |
| **closedir**     | `<dirent.h>`      | `int closedir(DIR *dirp);`                             | Ferme un descripteur de répertoire.                                                                  |

