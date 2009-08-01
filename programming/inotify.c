#include <stdio.h>
#include <sys/types.h>
#include <sys/inotify.h>

void get_event (int fd, const char *target);

int main (int argc, char **argv)
{
	int fd;
	int wd;
	char *target = "/tmp";

	fd = inotify_init ();
	wd = inotify_add_watch (fd, target, IN_MODIFY | IN_CREATE | IN_DELETE);

	while (1)
	{
		get_event (fd, target);
	}
	return 0;
}

void get_event (int fd, const char *target)
{
	ssize_t len, i = 0;
	char buffer[1024];
	char action[1024];

	len = read (fd, buffer, 1024);

	while (i < len)
	{
		struct inotify_event *event = (struct inotify_event *) &buffer[i];
		strcpy (action, "nada");

		if (event->mask & IN_MODIFY) strcpy (action, "modified");
		if (event->mask & IN_DELETE) strcpy (action, "deleted");
		if (event->mask & IN_CREATE) strcpy (action, "created");
		printf ("file was %s name: %s, mask: %d\n", action, event->name, event->mask);
		i += sizeof (struct inotify_event) + event->len;
	}
}
