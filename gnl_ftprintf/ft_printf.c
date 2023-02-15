

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

int	ft_put_base_ten(int nb)
{
	size_t	len = 0;
	char	base[50] = "0123456789";
	char	nbr[50];
	int		i = 0;

	if (nb == -2147483648)
	{
		write(1, "-2147483648", 11);
		return (11);
	}
	if (nb < 0)
	{
		nb *= -1;
		len++;
		write(1, "-", 1);
	}
	while (nb >= 10)
	{
		nbr[i] = base[nb % 10];
		nb /= 10;
		len++;
		i++;
	}
	nbr[i] = base[nb % 10];
	len++;
	while (i != -1)
	{
		write(1, &nbr[i], 1);
		i--;
	}
	return (len);
}

int	ft_put_base_sixteen(unsigned int nb)
{
	size_t	len = 0;
	char	base[50] = "0123456789abcdef";
	char	nbr[50];
	int		i = 0;

	/*if (nb == -2147483648)
	{
		write(1, "-2147483648", 11);
		return (11);
	}*/
	if (nb < 0)
	{
		nb *= -1;
		len++;
		write(1, "-", 1);
	}
	while (nb >= 16)
	{
		nbr[i] = base[nb % 16];
		nb /= 16;
		len++;
		i++;
	}
	nbr[i] = base[nb % 16];
	len++;
	while (i != -1)
	{
		write(1, &nbr[i], 1);
		i--;
	}
	return (len);
}

int	ft_put_str(char *str)
{
	size_t	len = 0;
	size_t	i = 0;

	while (str[i] != '\0')
	{
		write(1, &str[i], 1);
		len++;
		i++;
	}
	return (len);
}

int	ft_printf(const char *str, ...)
{
	size_t	len = 0;
	size_t	i = 0;
	va_list	arg;

	va_start(arg, str);
	while (str[i] != '\0')
	{
		if (str[i] == '%')
		{
			if (str[i + 1] == 'd')
			{
				len += ft_put_base_ten(va_arg(arg, int));
				i++;
			}
			else if (str[i + 1] == 'x')
			{
				len += ft_put_base_sixteen(va_arg(arg, unsigned int));
				i++;
			}
			else if (str[i + 1] == 's')
			{
				len += ft_put_str(va_arg(arg, char *));
				i++;
			}
			else
			{
				write(1, &str[i], 1);
				len++;
			}
		}
		else
		{
			write(1, &str[i], 1);
			len++;
		}
		i++;
	}
	return (len);
}

int	main(void)
{
	char	*test = "Hello World!";
	int		chiffre = -2;
	int		my;
	int		not_my;

	my = ft_printf("string -> [%s] | base_10 -> [%d] | base_16 -> [%x]\n"\
			,test, chiffre, chiffre);
	not_my = printf("string -> [%s] | base_10 -> [%d] | base_16 -> [%x]\n"\
			, test, chiffre, chiffre);
	printf("ft_printf = [%d]\n", my);
	printf("printf = [%d]\n", not_my);
	return (0);
}
