/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjulliat <mjulliat@student.42.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 10:18:36 by mjulliat          #+#    #+#             */
/*   Updated: 2023/02/15 12:23:23 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"

int	main(void)
{
	char	*str;
	int		fd;

	fd = open("text.txt", O_RDONLY);
	str = get_next_line(fd);
	printf("BUFFER_SIZE -> [%d]\n", BUFFER_SIZE);
	while (str != NULL)
	{
		printf("%s", str);
		str = get_next_line(fd);
	}
	close(fd);
	return (0);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*str;

	if (BUFFER_SIZE <= 0 | fd < 0) // check error of fd and BUFFER_SIZE
		return (NULL);
	str = ft_read_fd(str, fd, BUFFER_SIZE); // read to find \n or EOF
	if (!str) // if nothing in str return NULL
		return (NULL);
	line = ft_line(str); // get the line with the \n
	str = ft_trimstr(str);
	return (line);
}

char	*ft_read_fd(char *str, int fd, int buff_s)
{
	char	*buff;
	int		byte_read;

	byte_read = 1;
	if (!str) // creat the static if it doesn't exist
		str = ft_calloc(1, 1);
	buff = ft_calloc(sizeof(char) , buff_s + 1); // creat the str that read
	while (byte_read > 0)
	{
		byte_read = read(fd, buff, buff_s); // number of character read
		if (byte_read < 0)
		{
			free(str);
			free(buff);
			return (NULL);
		}
		buff[byte_read] = '\0';
		str = ft_free_and_join(str, buff); // combine reed + all previous read
		if (ft_strchr(str, '\n') == 0) // search the \n to quit
			break ;
	}
	free(buff);	// free the str that read
	if (str[0] == '\0')	// if nothing read free and return NULL
	{
		free(str);
		return (NULL);
	}
	return (str);
}

char	*ft_line(char *str)
{
	size_t	i;
	size_t	line_len;
	char	*line;

	i = 0;
	while (str[i] != '\n' && str[i] != '\0') // find the len to the \n or the \0
		i++;
	if (str[i] == '\n') // if it's the \n add 1 to the len
		line_len = i + 1;
	else
		line_len = i;
	line = ft_calloc(line_len + 1, sizeof(char));
	if (!line)
		return (NULL);
	i = 0;
	while (i < line_len) // copy the str in line to the \n or the \0
	{
		line[i] = str[i];
		i++;
	}
	return (line);
}

char	*ft_trimstr(char *str)
{
	char	*str_trim;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (str[i] != '\n' && str[i] != '\0')
		i++;
	if (str[i] == '\n')
		i += 1;
	str_trim = ft_calloc(ft_strlen(str) - i + 1, sizeof(char));
	if (!str_trim)
		return (NULL);
	while (str[i] != '\0')
	{
		str_trim[j] = str[i];
		i++;
		j++;
	}
	free(str);
	return (str_trim);
}

//		##### UTILS #####

char	*ft_free_and_join(char *str, char *buff)
{
	char	*tmp;
	
	tmp = ft_strjoin(str, buff); // need a temp to free the old str
	free(str);	
	return (tmp);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	str = ft_calloc(ft_strlen(s1) + ft_strlen(s2) + 1, sizeof(char));
	if (!str)
		return (NULL);
	while (s1[j] != '\0')
	{
		str[i] = s1[j];
		j++;
		i++;
	}
	j = 0;
	while (s2[j] != '\0')
	{
		str[i] = s2[j];
		j++;
		i++;
	}
	return (str);
}

int	ft_strchr(char *str, char c)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == c)
			return (0);
		i++;
	}
	return (1);
}

size_t	ft_strlen(char *str)
{
	size_t	size;
	
	size = 0;
	while (str[size] != '\0')
		size++;
	return (size);
}

char	*ft_calloc(size_t size, size_t nbyte)
{
	size_t	i;
	char	*str;

	i = 0;
	str = malloc(size * nbyte);
	if (!str)
		return (NULL);
	while (i < size * nbyte)
	{
		str[i] = '\0';
		i++;
	}
	return (str);
}
