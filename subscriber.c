// Mocanu-Prejma Vladimir-Georgian
// 325CB

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"

void usage(char *file)
{
    fprintf(stderr, "Usage: %s id_client server_address server_port\n", file);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, n, ret;
    struct sockaddr_in serv_addr;
    char buffer[BUFLEN], buffer_copy[BUFLEN];
    char *ptr;
    char *topic_subscribe, *topic_unsubscribe, *SF;

    if (argc < 4)
    {
        usage(argv[0]);
    }

    fd_set read_fds;    // multimea de citire folosita in select()
    fd_set tmp_fds;     // multime folosita temporar
    int fdmax;

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    //creeare socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    //adaugam informatiile
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));

    ret = inet_aton(argv[2], &serv_addr.sin_addr);
    DIE(ret == 0, "inet_aton");
    //ne conectam la retea
    ret = connect(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr));
    DIE(ret < 0, "connect");

    FD_SET(0, &read_fds);
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

    //trimitem id_client catre server
    n = send(sockfd, argv[1], strlen(argv[1]), 0);
    DIE(n < 0, "send");

    //rulam pana oprim programul
    while (1)
    {
        tmp_fds = read_fds;
        //apelam functia select
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        if(FD_ISSET(0, &tmp_fds))
        {

            // se citeste de la tastatura
            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN - 1, stdin);
            strcpy(buffer_copy, buffer);

            //impartim mesajul dupa spatiu
            ptr = strtok(buffer_copy, " ");

            //vericam daca comanda este exit, subscribe, unsubscribe
            //altfel, comanda gresita (optional)
            //si trimitem mesajul corespunzator catre server
            if (strncmp(ptr, "exit", 4) == 0)
            {
                break;
            }
            else if(strcmp(ptr, "subscribe") == 0)
            {
                topic_subscribe = strtok(NULL, " ");

                if(topic_subscribe == NULL)
                {
                    //printf("Comanda gresita subscribe\n");
                }
                else
                {

                    SF = strtok(NULL, " ");
                    if(SF == NULL)
                    {
                        // printf("Comanda gresita SF\n");
                    }
                    else
                    {
                        if(strtok(NULL, " ") == NULL)
                        {
                            n = send(sockfd, buffer, strlen(buffer), 0);
                            DIE(n < 0, "send");
                        }
                        else
                        {
                            // printf("Comanda gresita subscribe\n");
                        }

                    }
                }
            }
            else if(strcmp(ptr, "unsubscribe") == 0)
            {
                topic_unsubscribe = strtok(NULL, " ");

                if(topic_unsubscribe == NULL)
                {
                    //printf("Comanda gresita unsubscribe\n");
                }
                else
                {
                    if(strtok(NULL, " ") == NULL)
                    {
                        n = send(sockfd, buffer, strlen(buffer), 0);
                        DIE(n < 0, "send");
                    }
                    else
                    {
                        //printf("Comanda gresita unsubscribe\n");
                    }

                }

            }
            else
            {
                // printf("Comanda gresita\n");
            }

        }

        //verificam daca clientul s-a abonat sau nu la topic
        //vedem ce mesaj am primit de la server si afisam corespunzator
        if(FD_ISSET(sockfd, &tmp_fds))
        {
            memset(buffer, 0, BUFLEN);
            n = recv(sockfd, buffer, sizeof(buffer), 0);

            if(n == 0)
            {
                break;
            }

            if(strcmp(buffer, "subscribed") == 0)
            {
                printf("subscribed %s\n", topic_subscribe);
            }
            else if(strcmp(buffer, "unsubscribed") == 0)
            {
                printf("unsubscribed %s\n", topic_unsubscribe);
            }
            else
            {
                printf("%s\n", buffer);
            }

        }
    }

    close(sockfd);
    return 0;
}
