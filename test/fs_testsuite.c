#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fs.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define LOREM_SIZE 4096

#define test_fs_error(fmt, ...) \
	fprintf(stderr, "%s: "fmt"\n", __func__, ##__VA_ARGS__)

#define die(...)				\
do {							\
	test_fs_error(__VA_ARGS__);	\
	exit(1);					\
} while (0)

#define die_perror(msg)			\
do {							\
	perror(msg);				\
	exit(1);					\
} while (0)

char insert_buff[LOREM_SIZE];
void insert_lorem();

struct thread_arg {
	int argc;
	char **argv;
};

void thread_fs_offread(void *arg)
{
	struct thread_arg *t_arg = arg;
	char *diskname, *filename, *buf;
	int fs_fd;
	int stat, read;

	if (t_arg->argc < 2)
		die("need <diskname> <filename>");

	diskname = t_arg->argv[0];
	filename = t_arg->argv[1];

	if (fs_mount(diskname))
		die("Cannot mount diskname");

	fs_fd = fs_open(filename);
	if (fs_fd < 0) {
		fs_umount();
		die("Cannot open file");
	}

	stat = fs_stat(fs_fd);
	if (stat < 0) {
		fs_umount();
		die("Cannot stat file");
	}
	if (!stat) {
		/* Nothing to read, file is empty */
		printf("Empty file\n");
		return;
	}
	buf = malloc(stat);
	if (!buf) {
		perror("malloc");
		fs_umount();
		die("Cannot malloc");
	}

	if (fs_lseek(fs_fd, 3000) < 0)
	{
		fs_umount();
		die("Cannot seek to file");
	}

	read = fs_read(fs_fd, buf, stat);

	if (fs_close(fs_fd)) {
		fs_umount();
		die("Cannot close file");
	}

	if (fs_umount())
		die("cannot unmount diskname");

	printf("Read file '%s' (%d/%d bytes)\n", filename, read, stat);
	printf("Content of the file:\n");
	printf("%.*s", (int)stat, buf);

	free(buf);
}

void thread_fs_rewrite(void *arg)
{
	struct thread_arg *t_arg = arg;
	char *diskname, *filename, *buf;
	int fd, fs_fd;
	struct stat st;
	int written;

	if (t_arg->argc < 2)
		die("Usage: <diskname> <host filename>");

	diskname = t_arg->argv[0];
	filename = t_arg->argv[1];

	/* Open file on host computer */
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		die_perror("open");
	if (fstat(fd, &st))
		die_perror("fstat");
	if (!S_ISREG(st.st_mode))
		die("Not a regular file: %s\n", filename);

	insert_lorem();
	buf = insert_buff;

	/* Now, deal with our filesystem:
	 * - mount, create a new file, copy content of host file into this new
	 *   file, close the new file, and umount
	 */
	if (fs_mount(diskname))
		die("Cannot mount diskname");

	fs_fd = fs_open(filename);
	if (fs_fd < 0)
	{
		fs_umount();
		die("Cannot open file");
	}

	if (fs_lseek(fs_fd, 3000) < 0)
	{
		fs_umount();
		die("Cannot seek to file");
	}

	written = fs_write(fs_fd, buf, LOREM_SIZE);

	if (fs_close(fs_fd))
	{
		fs_umount();
		die("Cannot close file");
	}

	if (fs_umount())
		die("Cannot unmount diskname");

	printf("Wrote file '%s' (%d/%zu bytes)\n", filename, written,
		   st.st_size);

	munmap(buf, st.st_size);
	close(fd);
}

void thread_fs_edge(void *arg)
{
	struct thread_arg *t_arg = arg;
	char *diskname, *filename, *buf;
	int fs_fd;
	int stat, read;

	if (t_arg->argc < 2)
		die("need <diskname> <filename>");

	diskname = t_arg->argv[0];
	filename = t_arg->argv[1];

	if (fs_mount(diskname))
		die("Cannot mount diskname");

	// EDGE CASE TESTING GOES HERE

	fs_fd = fs_open(filename);
	if (fs_fd < 0)
	{
		fs_umount();
		die("Cannot open file");
	}

	// EDGE CASE TESTING GOES HERE
	

	if (fs_close(fs_fd))
	{
		fs_umount();
		die("Cannot close file");
	}

	// EDGE CASE TESTING GOES HERE

	if (fs_umount())
		die("cannot unmount diskname");

	free(buf);
}
size_t get_argv(char *argv)
{
	long int ret = strtol(argv, NULL, 0);
	if (ret == LONG_MIN || ret == LONG_MAX)
		die_perror("strtol");
	return (size_t)ret;
}

static struct {
	const char *name;
	void(*func)(void *);
} commands[] = {
	{"rewrite", thread_fs_rewrite},
	{"off_read", thread_fs_offread},
	{"check_edge_cases", thread_fs_edge}};

void usage(char *program)
{
	int i;
	fprintf(stderr, "Usage: %s <command> [<arg>]\n", program);
	fprintf(stderr, "Possible commands are:\n");
	for (i = 0; i < ARRAY_SIZE(commands); i++)
		fprintf(stderr, "\t%s\n", commands[i].name);
	exit(1);
}

int main(int argc, char **argv)
{
	int i;
	char *program;
	char *cmd;
	struct thread_arg arg;

	program = argv[0];

	if (argc == 1)
		usage(program);

	/* Skip argv[0] */
	argc--;
	argv++;

	cmd = argv[0];
	arg.argc = --argc;
	arg.argv = &argv[1];

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (!strcmp(cmd, commands[i].name)) {
			commands[i].func(&arg);
			break;
		}
	}
	if (i == ARRAY_SIZE(commands)) {
		test_fs_error("invalid command '%s'", cmd);
		usage(program);
	}

	return 0;
}
void insert_lorem() {
	char buff[LOREM_SIZE] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum. Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum. Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum. Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum. Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum. Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.Lorem Ipsum is simply dummy text of the printing and typesetting industry";

	for (size_t i = 0; i < LOREM_SIZE; i++)
	{
		insert_buff[i] = buff[i];
	}
}