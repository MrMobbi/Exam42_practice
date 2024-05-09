#include <string.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

fd_set wfds, rfds, fds;
int maxfd = 0, client_id = 0;
int clientIds[424242];
char *messages[424242];
char writebuf[65], readbuf[1025];

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len +strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

int extractMsg(char **buf, char **msg)
{
	char *newbuf;

	*msg = 0;
	if (*buf == 0)
		return (0);
	for (int i = 0; (*buf)[i]; i++)
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf+i+1);
			*msg = *buf;
			(*msg)[i+1] = 0;
			*buf = newbuf;
			return (1);
		}
	}
	return (0);
}

void error_message(char *msg)
{
	if (msg)
		write(2, msg, strlen(msg));
	else
		write(2, "Fatal error", 11);
	write(2, "\n", 1);
	exit(1);
}

void server_send(int sender, char *msg)
{
	for (int fd = 0; fd <= maxfd; fd++)
	{
		if (FD_ISSET(fd, &wfds) && fd != sender)
			if (send(fd, msg, strlen(msg), 0) == -1)
				error_message(NULL);
	}
}

void server_dispatch(int fd)
{
	char	*data;
	while (extractMsg(&(messages[fd]), &data))
	{
		sprintf(writebuf, "client %d: ", clientIds[fd]);
		server_send(fd, writebuf);
		server_send(fd, data);
		free(data);
		data = NULL;
	}
}

int main(int ac, char **av)
{
	if (ac != 2)
		error_message("Wrong number of arguments\n");
	FD_ZERO(&fds);

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	maxfd = socketfd;
	if (socketfd < 0)
		error_message(NULL);
	FD_SET(socketfd, &fds);

	struct sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	server_address.sin_port = htons(atoi(av[1]));

	if (bind(socketfd, (const struct sockaddr *)&server_address, sizeof(server_address)) ||  listen(socketfd, 100) == -1)
		error_message(NULL);


	while (1)
	{
		rfds = wfds = fds;
		if (select(maxfd + 1, &rfds, &wfds, NULL, NULL) < 0)
			error_message(NULL);

		for (int fd = 0; fd <= maxfd; fd++)
		{
			if (!FD_ISSET(fd, &rfds))
				continue;
			if (fd == socketfd)
			{
				//new connection
				socklen_t addr_len = sizeof(server_address);
				int client = accept(socketfd, (struct sockaddr *)&server_address, &addr_len);
				if (client == -1)
					continue;
				maxfd = client > maxfd ? client : maxfd;
				clientIds[client] = client_id ++;
				messages[client] = NULL;
				FD_SET(client, &fds);
				sprintf(writebuf, "server: client %d just arrived\n", clientIds[client]);
				server_send(client, writebuf);
				break;
			}
			else 
			{
				int bytes_read = recv(fd, readbuf, 1024, 0);
				if ( bytes_read <= 0)
				{
					sprintf(writebuf, "server : client %d just left\n", clientIds[fd]);
					server_send(fd, writebuf);
					free(messages[fd]);
					messages[fd] = NULL;
					FD_CLR(fd, &fds);
					close(fd);
					break;
				}
				readbuf[bytes_read] = '\0';
				messages[fd] = str_join(messages[fd], readbuf);
				server_dispatch(fd);
			}
		}
	}
	return (0);
}
