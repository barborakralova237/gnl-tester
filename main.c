#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>

static void	test_single_fd(const char *path, const char *label)
{
	int		fd;
	char	*line;
	int		count;

	printf("\n--- %s (%s) ---\n", label, path);
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		printf("could not open file\n");
		return ;
	}
	count = 0;
	while ((line = get_next_line(fd)) != NULL)
	{
		count++;
		printf("[%d] len=%d :: %.60s%s\n", count, (int)ft_strlen(line),
			line, ft_strlen(line) > 60 ? "...(truncated for display)" : "");
		free(line);
	}
	printf("(NULL returned, total lines read: %d)\n", count);
	close(fd);
}

static void	test_two_fds_interleaved(const char *path1, const char *path2)
{
	int		fd1;
	int		fd2;
	char	*l1;
	char	*l2;
	int		round;

	printf("\n--- Interleaved read: two fds at once ---\n");
	fd1 = open(path1, O_RDONLY);
	fd2 = open(path2, O_RDONLY);
	if (fd1 < 0 || fd2 < 0)
	{
		printf("could not open one of the files\n");
		return ;
	}
	round = 0;
	while (round < 3)
	{
		l1 = get_next_line(fd1);
		l2 = get_next_line(fd2);
		printf("round %d | fd1: %s", round,
			l1 ? l1 : "(NULL)\n");
		printf("round %d | fd2: %s", round,
			l2 ? l2 : "(NULL)\n");
		if (l1)
			free(l1);
		if (l2)
			free(l2);
		round++;
	}
	close(fd1);
	close(fd2);
}

static void	test_invalid_fd(void)
{
	char	*line;

	printf("\n--- Invalid fd (-1) ---\n");
	line = get_next_line(-1);
	printf("result: %s\n", line ? "NOT NULL (bug!)" : "NULL (correct)");
	if (line)
		free(line);
}

static void	test_stdin(void)
{
	char	*line;
	int		count;

	printf("\n--- stdin (type a few lines, Ctrl+D to end) ---\n");
	count = 0;
	while ((line = get_next_line(0)) != NULL)
	{
		count++;
		printf("[stdin %d] %s", count, line);
		free(line);
	}
	printf("(stdin done)\n");
}

int	main(int argc, char **argv)
{
	printf("=== get_next_line test suite | BUFFER_SIZE=%d ===\n", BUFFER_SIZE);

	test_single_fd("normal.txt", "Normal file, no trailing newline on last line");
	test_single_fd("empty.txt", "Empty file");
	test_single_fd("only_newline.txt", "File with only a newline");
	test_single_fd("no_trailing_nl.txt", "Single line, no newline at all");
	test_single_fd("blank_lines.txt", "File with blank lines in the middle");
	test_single_fd("long_line.txt", "One very long line (5000+ chars)");
	test_single_fd("many_lines.txt", "1000 lines");

	test_two_fds_interleaved("normal.txt", "many_lines.txt");
	test_invalid_fd();

	if (argc > 1 && argv[1][0] == 's')
		test_stdin();

	printf("\n=== done ===\n");
	return (0);
}
