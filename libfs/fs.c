#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

//********************************
// * MACRO DEFINITIONS
//********************************
// https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// MACRO for printing error messages. Disabled when 0 and enabled when 1.
// ! THIS MUST BE DISABLED BEFORE SUBMISSION <--- ENABLED CURRENTLY
#define PRINT_OUT 1 // * MODES: 0 - DISABLE, 1 - ENABLE
#define print_out(fmt, ...)                                            \
	do                                                                 \
	{                                                                  \
		if (PRINT_OUT)                                                 \
			fprintf(stderr, "%s: " fmt "\n", __func__, ##__VA_ARGS__); \
	} while (0)

#define FAT_EOC 0xFFFF

//********************************
// * GLOBAL ARRAYS AND STRUCTURES
//********************************
/**
 * @brief The Superblock data structure definition
 */
typedef struct __attribute__((__packed__)) Superblock {
	uint8_t sig[8];
	uint16_t total_num_blocks;
	uint16_t root_dir_block_index;
	uint16_t data_block_start_index;
	uint16_t total_num_data_blocks;
	uint8_t num_block_fat;
	uint8_t padding[4079];
} Superblock;
/**
 * @brief  The root directory table NODE data structure definition
 * @note   An empty entry is defined by the first character of the entry’s 
 * 			filename being equal to the NULL character.
 */
typedef struct __attribute__((__packed__)) DirectoryTableNode
{
	uint8_t filename[16];
	uint32_t file_size;
	uint16_t first_data_block_index;
	uint8_t padding[10];
} DirectoryTableNode;
/**
 * @brief  File Allocation Table (FAT), initialized during `fs_mount()`.
 * @note   First element in the array is always FAT_EOC. Size of FAT is
 * 			is 2 x # of data blocks. 
 */
uint16_t *FAT;
/**
 * @brief Root directory table consisting of FS_FILE_MAX_COUNT entries.
 */
DirectoryTableNode RootDirectory[FS_FILE_MAX_COUNT];

//********************************
// * GLOBAL VARIABLES
//********************************
Superblock superblock; // * Superblock instance
DirectoryTableNode dir_table_node; // * DirectoryTableNode instance

//********************************
// * IMPLEMENTATION
//********************************

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
	if(block_disk_open(diskname)){
		print_out("disk cannot be opened.");
		return -1;
	}

	if (block_read(sizeof(Superblock), &superblock))
	{
		print_out("Unable to read superblock.");
		return -1;
	}
	print_out("%d", superblock.sig[0]);
	return 0;
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
	return 0;
}

int fs_info(void)
{
	/* TODO: Phase 1 */
	return 0;
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
	return 0;
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
	return 0;
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
	return 0;
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	return 0;
}

