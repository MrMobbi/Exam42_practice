/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjulliat <mjulliat@student.42.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 10:38:18 by mjulliat          #+#    #+#             */
/*   Updated: 2023/02/06 13:09:25 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GNL_H
# define GNL_H

# include <stdlib.h>
# include <fcntl.h>
# include <stdio.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

char	*get_next_line(int fd);
char	*ft_read_fd(char *str, int fd, int buff_s);
char	*ft_line(char *str);
char	*ft_trimstr(char *str);

//		##### Utils #####
size_t	ft_strlen(char *str);
char	*ft_strjoin(char *s1, char *s2);
char	*ft_free_and_join(char *str, char *buff);
char	*ft_calloc(size_t size, size_t nbyte);
int		ft_strchr(char *str, char c);

#endif
