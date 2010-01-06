#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	char *addr, c;
	int fd;
	int i, page_size;
	struct stat stat;

	fd = open ("/etc/passwd", O_RDONLY);
	fstat (fd, &stat);

	page_size = sysconf (_SC_PAGE_SIZE);
	page_size = ((stat.st_size % page_size) + 1) * page_size;
	/* the size of the mmap is rounded to an offset
	 * of the page size
	 */
	addr = (char *) mmap (NULL, page_size,
			PROT_READ, MAP_SHARED, fd, 0);
	close (fd);
	for (i = 0; i < stat.st_size; i++) {
		c = *addr;
		putchar (c);
		addr++;
		if (c == EOF)
			break;
	}
	return 0;
}
