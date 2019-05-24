#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

//*************************************
// * MACRO DEFINITIONS
//*************************************
// https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// MACRO for printing error messages. Disabled when 0 and enabled when 1.
// ! THIS MUST BE DISABLED BEFORE SUBMISSION
#define PRINT_OUT 1 // * MODES: 0 - DISABLE, 1 - ENABLE
#define print_out(fmt, ...)                                            \
	do                                                                 \
	{                                                                  \
		if (PRINT_OUT)                                                 \
			fprintf(stderr, "%s: " fmt "", __func__, ##__VA_ARGS__); \
	} while (0)

#define FAT_EOC 0xFFFF
#define MALLOC_FAIL NULL

//*************************************
// * GLOBAL ARRAYS AND STRUCTURES
//*************************************
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
 * 			is 2 x # of data blocks = # of FAT blocks. 
 */
static uint16_t *FAT;
/**
 * @brief Root directory table consisting of FS_FILE_MAX_COUNT entries.
 */
static DirectoryTableNode RootDirectory[FS_FILE_MAX_COUNT];

//*************************************
// * GLOBAL VARIABLES
//*************************************
static Superblock superblock; // * Superblock instance
uint16_t fat_size; // * size of FAT

//*************************************
// ! DEBUG FUNCTIONS
// ! will be DISABLED on final release
//*************************************
/**
 * @brief  prints some of the contents of disk
 * @note   pcd() - PrintContentsOfDisk()
 * @param  fat_print_amt: amount of FAT rows to print
 * @param  root_dir_amt: amount of file data to print
 * @retval None
 */
void pcd(int fat_print_amt, int root_dir_amt){
	print_out("-----DISK DATA BEGIN-----\n");
	print_out("\n");
	print_out("---SUPER BLOCK---\n");
	print_out("Signature: %s\n", (char *)superblock.sig);
	print_out("Total amount of blocks of virtual disk: %d\n", 
				superblock.total_num_blocks);
	print_out("Root directory block index: %d\n", 
				superblock.root_dir_block_index);
	print_out("Data block start index: %d\n", 
				superblock.data_block_start_index);
	print_out("Amount of data blocks: %d\n", superblock.total_num_data_blocks);
	print_out("Number of blocks for FAT: %d\n", superblock.num_block_fat);
	print_out("\n");
		print_out("---FAT TABLE: first %d items---\n", fat_print_amt);
	for (size_t i = 0; i < fat_print_amt; i++)
	{
		print_out("Index %ld: %d\n", i, *(FAT + i));
	}
	print_out("\n");
	print_out("---ROOT DIR: first %d items---\n", root_dir_amt);
	for (size_t i = 0; i < root_dir_amt; i++)
	{
		print_out("Filename [%ld]: %s\n", i, (char *) RootDirectory[i].filename);
		print_out("Filesize [%ld]: %d\n", i, RootDirectory[i].file_size);
		print_out("Index of first data block [%ld]: %d\n", i, 
					RootDirectory[i].first_data_block_index);
		print_out("\n");
	}
	print_out("-----DISK DATA END-----\n");
}

//*************************************
// * HELPER FUNCTIONS
//*************************************
/**
 * @brief  iterate through the root directory array and increment the counter
 * 			`count` if array element is not NULL.
 * @note   
 * @retval returns the total number of elements in the root directory array
 */
int count_root_dir_nodes(void){
	int count = 0;
	for (size_t i = 0; i < FS_FILE_MAX_COUNT; i++)
	{
		if (RootDirectory[i].filename[0] != '\0')
		{
			count++;
		}
	}
	return FS_FILE_MAX_COUNT-count;
}
/**
 * @brief  iterate through all fat table entries and increment the counter
 * 			`count` if array element is not 0.
 * @note   
 * @retval returns the total number of non-zero elements in the FAT.
 */
int count_fat_entries(void)
{
	uint16_t entries = superblock.total_num_data_blocks;
	int count = 0;
	for (size_t i = 0; i < entries; i++)
	{
		if (FAT[i] != 0)
		{
			count++;
		}
	}
	return superblock.total_num_data_blocks-count;
}
//*************************************
// * IMPLEMENTATION
//*************************************

int fs_mount(const char *diskname)
{
	memset(&superblock, 0, BLOCK_SIZE);
	char *signature = "ECS150FS";

	if(block_disk_open(diskname)){
		print_out("disk cannot be opened.\n");
		return -1;
	}

	if (block_read(0, &superblock))
	{
		print_out("unable to read superblock from disk.\n");
		return -1;
	}

	for (size_t i = 0; i < strlen(signature); i++)
	{
		if(signature[i] != (char) superblock.sig[i]){
			print_out("invalid signature.\n");
			return -1;
		}
	}
	if (superblock.total_num_blocks != block_disk_count())
	{
		print_out("total number of blocks do not match.\n");
		return -1;
	}
	
	//* allocate File Allocation Table and copy its contents from disk
	fat_size = superblock.num_block_fat * BLOCK_SIZE;
	FAT = (uint16_t *)malloc(fat_size);
	if (FAT == MALLOC_FAIL)
	{
		print_out("unable to allocate memory for FAT.\n");
	}
	memset(FAT, 0, fat_size);
	for (size_t i = 0; i < superblock.num_block_fat; i++)
	{
		if (block_read(i + 1, FAT + (i * BLOCK_SIZE / 2))) {
			print_out("unable to copy contents of the FAT from disk.\n");
			return -1;
		}
	}
	FAT[0] = FAT_EOC;

	//* copy the root directory from disk
	memset(RootDirectory, 0, BLOCK_SIZE);
	if (block_read(superblock.root_dir_block_index, RootDirectory))
	{
		print_out("unable to copy contents of the root directory from disk.\n");
		return -1;
	}

	// print out superblock, FAT, and root dir block
	//pcd(10, 3);

	return 0;
}

int fs_umount(void)
{
	if (block_disk_count() < 0)
	{
		print_out("no virtual disk was open.\n");
		return -1;
	}
	// copy FAT blocks to disk
	for (size_t i = 0; i < superblock.num_block_fat; i++)
	{
		if (block_write(i + 1, FAT + (i * BLOCK_SIZE / 2)))
		{
			print_out("unable to copy contents of FAT to disk.\n");
			return -1;
		}
	}
	// copy root directory blocks to disk
	if (block_write(superblock.root_dir_block_index, 
									(const void *) RootDirectory))
	{
		print_out("unable to copy contents of the root directory to disk.\n");
		return -1;
	}
	// copy superblock to disk
	if (block_write(0, &superblock))
	{
		print_out("unable to write superblock to disk.\n");
		return -1;
	}
	if (block_disk_close())
	{
		print_out("unable to close disk file.\n");
		return -1;
	}

	free(FAT);
	return 0;
}

int fs_info(void)
{
	fprintf(stdout, "FS Info:\n");
	fprintf(stdout, "total_blk_count=%d\n", superblock.total_num_blocks);
	fprintf(stdout, "fat_blk_count=%d\n", superblock.num_block_fat);
	fprintf(stdout, "rdir_blk=%d\n", superblock.root_dir_block_index);
	fprintf(stdout, "data_blk=%d\n", superblock.data_block_start_index);
	fprintf(stdout, "data_blk_count=%d\n", superblock.total_num_data_blocks);
	fprintf(stdout, "fat_free_ratio=%d/%d\n", count_fat_entries(), 
											superblock.total_num_data_blocks);
	fprintf(stdout, "rdir_free_ratio=%d/%d\n", count_root_dir_nodes(), 
											FS_FILE_MAX_COUNT);
	return 0;
}

int fs_create(const char *filename)
{
	int filename_len = strlen(filename);
	if (filename_len < 1 || filename_len > 16)
	{
		print_out("invalid filename.\n");
		return -1;
	}

	// search for an empty entry in the root directory
	int index_of_empty_entry = -1;
	for (size_t i = 0; i < FS_FILE_MAX_COUNT; i++)
	{
		if (RootDirectory[i].filename[0] == '\0')
		{
			index_of_empty_entry = i;
			break;
		}
	}
	if (index_of_empty_entry < 0)
	{
		print_out("no free entry available.\n");
		return -1;
	}
	
	// reset the struct, empty old information
	memset(&RootDirectory[index_of_empty_entry], 0, sizeof(DirectoryTableNode));

	// copy filename
	int i = 0;
	while (i < filename_len)
	{
		RootDirectory[index_of_empty_entry].filename[i] = filename[i];
		i++;
	}

	// // set initial filesize
	// RootDirectory[index_of_empty_entry].file_size = 0;

	// // find the first free block in the fat table
	// uint16_t fat_entries = superblock.total_num_data_blocks;
	// int fat_index = 0;
	// for (size_t i = 1; i < fat_entries; i++)
	// {
	// 	if(FAT[i] == 0){
	// 		fat_index = i;
	// 		break;
	// 	}
	// }
	// // update FAT entry to FAT_EOC
	// FAT[fat_index] = FAT_EOC;
	// // set the first data block index in the file's metadata
	// RootDirectory[index_of_empty_entry].first_data_block_index = fat_index;

	RootDirectory[index_of_empty_entry].first_data_block_index = FAT_EOC;

	return 0;
}

int fs_delete(const char *filename)
{
	int filename_len = strlen(filename);
	if (filename_len < 1)
	{
		print_out("invalid filename.\n");
		return -1;
	}
	
	int index_of_entry = -1;

	for (size_t i = 0; i < FS_FILE_MAX_COUNT; i++)
	{
		if (!strcmp((char *) RootDirectory[i].filename, filename))
		{
			index_of_entry = i;
			break;
		}
	}
	if (index_of_entry < 0)
	{
		print_out("no entry found.\n");
		return -1;
	}

	// * remove all data blocks from the FAT
	// set current block = the starting block
	uint16_t curr_block = RootDirectory[index_of_entry].first_data_block_index;
	uint16_t tmp;
	while (FAT[curr_block] != FAT_EOC)
	{
		tmp = FAT[curr_block]; // temporarily store the next block
		FAT[curr_block] = 0; // free the current block
		curr_block = tmp; // set next block to the current block
	}
	if (FAT[curr_block] == FAT_EOC)
	{
		FAT[curr_block] = 0;
	}

	// reset the struct, empty old information
	memset(&RootDirectory[index_of_entry], 0, sizeof(DirectoryTableNode));

	return 0;
}

int fs_ls(void)
{
	fprintf(stdout, "FS Ls:\n");
	for (size_t i = 0; i < FS_FILE_MAX_COUNT; i++)
	{
		if (RootDirectory[i].filename[0] != '\0')
		{
			fprintf(stdout, "file: %s, size: %d, data_blk: %d\n",
					RootDirectory[i].filename,
					RootDirectory[i].file_size,
					RootDirectory[i].first_data_block_index);
		}
	}
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

