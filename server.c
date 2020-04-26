// Mocanu-Prejma Vladimir-Georgian
// 325CB

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helpers.h"

void usage(char *file)
{
    fprintf(stderr, "Usage: %s server_port\n", file);
    exit(0);
}

typedef struct
{
    char topic[50];
    char **id_client;
    int last_client;
} msg;

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}


int main(int argc, char *argv[])
{
    int sock_udp, sockfd, newsockfd, portno;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i, ret;
    char *token, *topic_temporar;


    char *buff = (char *) malloc(BUFLEN * sizeof(char));
    char topic_udp[50], tip_data[2], continut[1500];
    socklen_t clilen;

    fd_set read_fds;    // multimea de citire folosita in select()
    fd_set tmp_fds;     // multime folosita temporar
    int fdmax;          // valoare maxima fd din multimea read_fds

    //verificam daca respecta numarul de argumente
    if (argc < 2)
    {
        usage(argv[0]);
    }

    // se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    //socket TCP + informatii
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    portno = atoi(argv[1]);
    DIE(portno == 0, "atoi");

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //bind si listen TCP
    ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
    DIE(ret < 0, "bind");

    ret = listen(sockfd, MAX_CLIENTS);
    DIE(ret < 0, "listen");

    //Socket UDP
    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(sock_udp < 0, "socket1");

    //bind UDP
    ret = bind(sock_udp, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));

    //fdmax
    fdmax = max(sockfd, sock_udp);

    // se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
    FD_SET(sockfd, &read_fds);
    FD_SET(sock_udp, &read_fds);

    //initializam vectorul pentru ID_Client
    char **id_client1 = malloc(10 * sizeof(char));

    for (int i = 0; i < 14; ++i)
    {
        id_client1[i] = malloc(50 * sizeof(char));
    }


    //initializam un vector de structura msg
    //contine un vectir de id_client
    //astfel, pentru fiecare topic avem un vector de id_client
    msg **topic1 = malloc(10 * sizeof(msg));

    for (int i = 0; i < 10; ++i)
    {
        topic1[i] = malloc(50 * sizeof(char));
        topic1[i]->id_client = malloc(10 * sizeof(msg));

        for (int j = 1; j <= 10; ++j)
        {
            topic1[i]->id_client[j] = malloc(50 * sizeof(char));
        }
    }

    //initializam cu 0
    int *id_ramas = malloc(10 * sizeof(int));
    for (int i = 0; i < 10; ++i)
    {
        topic1[i]->last_client = 0;
        id_ramas[i] = -1;
    }

    //merge pana oprim programul
    while (1)
    {
        tmp_fds = read_fds;

        //apelam functia select
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        //parcurgem toti socketii
        for (i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &tmp_fds))
            {
                if (i == sockfd)
                {
                    // ocerere de conexiune, pe care o acceptam

                    clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd,
                                       (struct sockaddr *) &cli_addr,
                                       &clilen);
                    DIE(newsockfd < 0, "accept");

                    // se adauga noul socket la multimea descriptorilor
                    FD_SET(newsockfd, &read_fds);

                    if (newsockfd > fdmax)
                    {
                        fdmax = newsockfd;
                    }

                    memset(buffer, 0, BUFLEN);
                    n = recv(newsockfd, buffer, sizeof(buffer), 0);
                    DIE(n < 0, "recv");

                    //retinem id_client
                    strcpy(id_client1[newsockfd], buffer);

                    printf("New client %s connected from %s:%d\n",
                           id_client1[newsockfd],
                           inet_ntoa(cli_addr.sin_addr),
                           ntohs(cli_addr.sin_port));

                }
                else if(i == sock_udp)//daca vine de pe o conexiune udp
                {

                    unsigned int len;
                    len = sizeof(cli_addr);
                    bzero(buffer, sizeof(buffer));
                    //primim mesajul udp
                    n = recvfrom(sock_udp, buffer, BUFLEN, 0,
                                 (struct sockaddr *)&cli_addr, &len);

                    //copiem continutul mesajului de la clientul udp
                    memcpy(topic_udp, buffer, 50);
                    memcpy(tip_data, buffer + 51, 1);
                    memcpy(continut, buffer + 53, 1500);

                    sprintf(buff, "%s:%d - %s - %s - %s",
                            inet_ntoa(cli_addr.sin_addr),
                            ntohs(cli_addr.sin_port),
                            topic_udp,
                            tip_data, continut);
                    //parcurgem topicurile si trimitem informatia
                    //la clientii potriviti
                    for (int z = 0; z < 10; z++)
                    {
                        if(strcmp(topic1[z]->topic, topic_udp) == 0)
                        {

                            for (int x = 1; x <= 10; ++x)
                            {
                                if(strcmp(topic1[z]->id_client[x],
                                          id_client1[x]) == 0)
                                {
                                    send(x, buff, strlen(buff), 0);
                                }

                            }
                        }
                    }
                }
                else
                {
                    // s-au primit date pe unul din socketii de client
                    memset(buffer, 0, BUFLEN);

                    n = recv(i, buffer, sizeof(buffer), 0);
                    DIE(n < 0, "recv");

                    if (n == 0 )
                    {
                        // conexiunea s-a inchis
                        printf("Client %s disconnected\n",
                               id_client1[i]);
                        close(i);

                        // se scoate socketul inchis
                        FD_CLR(i, &read_fds);
                    }
                    else
                    {
                        token = strtok(buffer, " ");

                        //vedem daca comanda este subscribe
                        //sau unsubscribe

                        if(strcmp(token, "subscribe") == 0)
                        {
                            //daca comanda este subscribe
                            //verificam daca exista topicul
                            //altfel, ii facem

                            topic_temporar = strtok(NULL, " ");

                            int contor = 0;
                            for (int j = 0; j < 10; ++j)
                            {
                                if(strcmp(topic1[j]->topic,
                                          topic_temporar) == 0)
                                {
                                    //lnia depaseste 80 de caractere

                                    //nu am putut sa o scurtez
                                    strcpy(topic1[j]->id_client[topic1[j]->last_client + 1],
                                           id_client1[i]);

                                    topic1[j]->last_client++ ;

                                    strcpy(buff, "subscribed");
                                    send(i, buff, strlen(buff), 0);
                                    contor = 0;
                                }
                            }


                            if(contor == 0)
                            {
                                strcpy(topic1[0]->topic, topic_temporar);
                                strcpy(topic1[0]->id_client[topic1[0]->last_client + 1],
                                       id_client1[i]);

                                topic1[0]->last_client++ ;
                                strcpy(buff, "subscribed");
                                send(i, buff, strlen(buff), 0);
                                contor = 0;
                            }

                        }
                        else
                        {
                            //pentru comanda unsubscribe
                            //cautam clientul si il scoatem din topicul
                            //corespunzator
                            topic_temporar = strtok(NULL, "\n");

                            for (int j = 0; j < 10; ++j)
                            {
                                if(strcmp(topic1[j]->topic,
                                          topic_temporar) == 0)
                                {
                                    for (int k = 1; k <= 10; k++)
                                    {
                                        if(strcmp(topic1[j]->id_client[k],
                                                  id_client1[i]) == 0)
                                        {
                                            free(topic1[j]->id_client[k]);

                                            strcpy(buff, "unsubscribed");
                                            send(i, buff, strlen(buff), 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    close(sockfd);
    close(sock_udp);
    return 0;
}
