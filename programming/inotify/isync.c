#include <stdio.h>
#include <errno.h>
#include <sys/inotify.h>
#include <linux/types.h>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

void event_handler (struct inotify_watch *watch, __u32 wd, __u32 mask,
		    __u32 cookie, const char *name, struct inode *inode)
{
	char buf[BUF_LEN];
	int len, i = 0;

	len = read (fd, buf, BUF_LEN);
	if (len < 0) {
		if (errno == EINTR)
			perror ("need to reissue system call");
		else
			perror ("read");
	} else if (!len)
		perror ("BUF_LEN too small?");

	while (i < len) {
		struct inotify_event *event;

		event = (struct inotify_event *) &buf[i];

		printf ("wd=%d mask=%u cookie=%u len=%u\n",
			event->wd, event->mask,
			event->cookie, event->len);

		if (event->len)
			printf ("name=%s\n", event->name);

		i += EVENT_SIZE + event->len;
	}
}

int main (int argc, char **argv)
{
	int fd;
	int wd;

	fd = inotify_init ();
	if (fd < 0)
		perror ("inotify_init");

	wd = inotify_add_watch (fd, "/home/wfelipe/Desktop",
		IN_MODIFY | IN_CREATE | IN_DELETE);

	if (wd < 0)
		perror ("inotify_add_watch");

	while (1) {
		read_events (fd);
		printf ("read_events\n");
	}

	if (inotify_rm_watch (fd, wd))
		perror ("inotify_rm_watch");


	return 0;
}
