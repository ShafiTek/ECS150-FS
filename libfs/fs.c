#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

//https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// MACRO for printing error messages. Disabled when 0 and enabled when 1.
// ! THIS IS DISABLED BEFORE SUBMISSION
#define PRINT_ERROR 1 // * MODES: 0 - DISABLE, 1 - ENABLE
#define print_error(...)                                               \
	do                                                                 \
	{                                                                  \
		if (PRINT_ERROR)                                               \
			fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__); \
	} while (0)

/* TODO: Phase 1 */

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
}

int fs_info(void)
{
	/* TODO: Phase 1 */
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

