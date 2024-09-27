#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//	Color Codes

#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[93m"
#define CYAN "\x1b[96m"
#define MAGENTA "\x1b[95m"
#define BGREEN "\x1b[92m"
#define ORANGE "\x1b[38;5;208m"
#define RED "\x1b[31m"
#define PURPLE "\x1b[35m"
#define WHITESPACES " \t\f\v\n\r"

//	Buffer size

#define BUFFER_SIZE 2048

//	Debug switch

# ifndef DEBUG
#  define DEBUG 0
# endif

//	bzero because it's useful

void	ft_bzero(void *s, unsigned long len);

#endif // WEBSERV_HPP
