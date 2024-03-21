
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
const int   d_buffer_size = 424242;

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

t_client *close_client(t_client *client, int fd)
{
    t_client *start = client;
    while (client != NULL)
    {
        if (client->fd == fd)
        {
            start = client->next;
            close(client->fd);
            free(client);
            client = NULL;
            break;
        }
        else if (client->next != NULL && client->next->fd == fd)
        {
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
    t_client *tmp = client;
    
    while (tmp != NULL)
    {
        if (tmp->fd != fd)
            send(tmp->fd, buffer, strlen(buffer), 0);
        tmp = tmp->next;
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
    char *buffer = calloc(sizeof(char), d_buffer_size + 1);
    if (!buffer)
        exit_error("buffer error\n");
    char *msg_buffer;
    t_client *client = NULL;

    while (1)
    {
        ready_socket = active_socket;
        if (select(max_socket + 1, &ready_socket, NULL, NULL, NULL) < 0)
            exit_error("Problem select");
        bzero(buffer, d_buffer_size);
        for (int socket_id = 0; socket_id <= max_socket; socket_id++)
        {
            if (!FD_ISSET(socket_id, &ready_socket))
                continue;
            if (socket_id == server_socket)
            {
                int client_socket;

                if ((client_socket = accept(server_socket, NULL, NULL)) < 0)
                    exit_error("Problem client socket\n");
                FD_SET(client_socket, &active_socket);
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
                int bytes_read = recv(socket_id, buffer, 4242424242, MSG_DONTWAIT);

                printf("[%d]\n", bytes_read);
                if (bytes_read <= 0) 
                {
                    bzero(buffer, d_buffer_size);
                    sprintf(buffer, "server: client %d just left\n", find_id(client, socket_id));
                    send_message(client, buffer, socket_id);
                    client = close_client(client, socket_id);
                    FD_CLR(socket_id, &active_socket);
                } 
                else 
                {
                    msg_buffer = calloc(sizeof(char), strlen(buffer) + 1);
                    if (!msg_buffer)
                        exit_error("MSG_BUFFER error\n");
                    strcpy(msg_buffer, buffer);
                    //printf("%s | [%c]\n", msg_buffer, msg_buffer[strlen(msg_buffer) - 1]);
                    if (msg_buffer[strlen(msg_buffer) - 1] == 10)
                    {
                        char *full_msg = calloc(sizeof(char), strlen(msg_buffer) + 50);
                        if (!full_msg)
                            exit_error("full message error\n");
                        sprintf(full_msg, "client %d: %s", find_id(client, socket_id), msg_buffer);
                        send_message(client, full_msg, socket_id);
                        bzero(msg_buffer, strlen(msg_buffer));
                        bzero(full_msg, strlen(full_msg));
                        free(msg_buffer);
                        free(full_msg);
                    }
                }
            }
        }
    }
    return (0);
}
