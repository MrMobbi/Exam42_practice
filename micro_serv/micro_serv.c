
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Stucture of the chain list client 
typedef struct s_client {
    int fd;
    int id;
    struct s_client *next;
}   t_client;

// all the define variable needed
int         d_id_client = 0;
const int   d_max_client = 128;
const int   d_buffer_size = 200000;

// Function for handle error
void exit_error(char *str)
{
    write(2, str, strlen(str));
    exit(1);
}

// function client chain list
t_client *new_client(int fd)
{
    t_client    *new;
    new = malloc(sizeof(t_client));
    if (!new)
        exit_error("Malloc client error\n");
    new->fd = fd;
    new->id = d_id_client;
    d_id_client++;
    new->next = NULL;
    return (new);
}

void add_client(t_client *client, int fd)
{
    t_client    *tmp = client;

    while (tmp != NULL)
    {
       if (tmp->next == NULL)
       {
            tmp->next = new_client(fd);
            break;
       }
       tmp = tmp->next;
    }
}

t_client *close_client(t_client *client, int id)
{
    t_client *start = client;
    while (client != NULL)
    {
        if (client->id == id)
        {
            printf("CLIENT1 :[%p] | [%d]\n", client, client->id);
            start = client->next;
            close(client->fd);
            free(client);
            client = NULL;
            break;
        }
        else if (client->next != NULL && client->next->id == id)
        {
            printf("CLIENT2 :[%p] | [%d]\n", client, client->id);
            t_client *tmp = client->next;
            client->next = client->next->next;
            close(tmp->fd);
            free(tmp);
            tmp = NULL;
            break ;
        }
        else
            client = client->next;
    }
    printf("START :[%p] | [%d]\n", start, start->id);
    return (start);
}

// function to find the id of tu curent client in the server
int find_id(t_client *client, int fd)
{
    while (client != NULL)
    {
        if (client->fd  == fd)
            return (client->id);
        client = client->next;
    }
    return (0);
}

// function that send the message to all other client
void send_message(t_client *client, char *buffer, int fd)
{
    while (client != NULL)
    {
        if (client->fd != fd)
            send(client->fd, buffer, strlen(buffer), 0);
        client = client->next;
    }
}

int main(int ac, char **av)
{
    //checking number of arguments
    if (ac != 2)
        exit_error("Wrong number of arguments\n");

    // initialazing server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //127.0.0.1
    server_address.sin_port = htons(atoi(av[1]));

    //binding socket to the server for read the future event
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        exit_error("Problem Socket\n");
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        exit_error("Problem Binding\n");
    if (listen(server_socket, d_max_client) < 0)
        exit_error("Problem Listening\n");

    fd_set  active_socket, ready_socket;
    FD_ZERO(&active_socket);
    FD_SET(server_socket, &active_socket);
    int max_socket = server_socket;
    char buffer[d_buffer_size];
    char msg_buffer[d_buffer_size];
    t_client *client = NULL;

    while (1)
    {
        ready_socket = active_socket;
        if (select(max_socket + 1, &ready_socket, NULL, NULL, NULL) < 0)
            exit_error("Problem select");
        for (int socket_id = 0; socket_id <= max_socket; socket_id++)
        {
            if (!FD_ISSET(socket_id, &ready_socket))
                continue;
            bzero(buffer, d_buffer_size);
            if (socket_id == server_socket)
            {
                int client_socket;

                if ((client_socket = accept(server_socket, NULL, NULL)) < 0)
                    exit_error("Problem client socket\n");
                max_socket = (client_socket > max_socket) ? client_socket : max_socket;
                if (client == NULL)
                    client = new_client(client_socket);
                else
                    add_client(client, client_socket);
                sprintf(buffer, "server: client %d just arrived\n", find_id(client, client_socket));
                send_message(client, buffer, client_socket);
            }
            else
            { 
                int bytes_read = recv(socket_id, buffer, sizeof(buffer) - 1, 0);

                if (bytes_read <= 0) 
                {
                    bzero(msg_buffer, d_buffer_size);
                    sprintf(msg_buffer, "server: client %d just left\n", find_id(client, socket_id));
                    send_message(client, msg_buffer, socket_id);
                    close_client(client, socket_id);
                    FD_CLR(socket_id, &active_socket);
                } 
                else 
                {
                    bzero(msg_buffer, d_buffer_size);
                    sprintf(msg_buffer, "client %d: %s\n", find_id(client, socket_id), buffer);
                    send_message(client, msg_buffer, socket_id);
                }
            }
        }
    }

    printf("open\n");
    return (0);
}
