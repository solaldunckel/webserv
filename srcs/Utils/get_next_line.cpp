#include "get_next_line.hpp"

void	*ft_calloc(size_t count, size_t size)
{
	void	*mem;

	if (!(mem = malloc(count * size)))
		return (NULL);
	memset(mem, 0, count * size);
	return (mem);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;

	if (!(str = (char*)ft_calloc(strlen(s1) + strlen(s2)
		+ 1, sizeof(char))))
		return (NULL);
	strcpy(str, s1);
	strcat(str, s2);
	return (str);
}

size_t		ft_strlen_c(char *s, char c)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*str;
	int		i;

	i = 0;
	if (!s)
		return (NULL);
	if (start > strlen(s))
		return (NULL);
	if (!(str = (char*)ft_calloc(len + 1, sizeof(char))))
		return (NULL);
	while (s[start] && len)
	{
		str[i] = s[start];
		i++;
		start++;
		len--;
	}
	str[i] = '\0';
	return (str);
}

int		ft_is_in_stri(char c, char *str)
{
	int		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

static int		handle_line(char *s[], int fd)
{
	char	*tmp;

	if (ft_is_in_stri('\n', s[fd]) >= 0)
	{
		tmp = s[fd];
		s[fd] = ft_substr(s[fd], ft_is_in_stri('\n', s[fd]) + 1, strlen(s[fd]));
		free(tmp);
	}
	else if (strlen(s[fd]) > 0) {
    free(s[fd]);
    s[fd] = NULL;
  }
	else
	{
    free(s[fd]);
    s[fd] = NULL;
		return (0);
	}
	return (1);
}

int				get_next_line(int fd, char **line)
{
	static char		*s[10240];
	char			buf[BUFFER_SIZE + 1];
	int				ret;
	char			*tmp;

	if (fd < 0 || !line || BUFFER_SIZE < 1 || read(fd, buf, 0) < 0)
		return (-1);
	if (!s[fd] && !(s[fd] = (char*)ft_calloc(1, sizeof(char *))))
		return (-1);
	while ((ft_is_in_stri('\n', s[fd])) < 0 && (ret = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		buf[ret] = '\0';
		tmp = s[fd];
		s[fd] = ft_strjoin(s[fd], buf);
		free(tmp);
	}
	if (s[fd])
		*line = ft_substr(s[fd], 0, ft_strlen_c(s[fd], '\n'));
	if (!handle_line(s, fd))
		return (0);
	return (1);
}