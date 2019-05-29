# REPORT #4 - Custom File System
### Authors:
1. Hashoush, Adam - 916173432
2. Shafi, Mohamed Yoosuf - 916606873
---
## DESIGN AND IMPLEMENTATION:
Three structures were created to complete the file system. The structures were 
for the Superblock, Directory Table Node, and Opened File Node. The superblock 
contains a signature, total number of blocks counter, root directory block 
index, data block start index, number of data blocks counter, number of FAT 
blocks counter and padding. The packed attribute was attached to both the 
superblock struct and the Directory Table Node struct to avoid any padding to 
variables allocation space. The Root Directory is made up of an array of 
Directory Table Node structures. Each Directory Table Node contains the 
filename, file size, first data block index and padding. Lastly, the Opened File
Table is made up of an array of Opened File Node structures. Each of these 
structures contain a pointer to its metadata in the structure’s respective Root 
Directory. The Opened File Node structs also have the offset, blocks traversed 
and the seeked block of the file it represents.

## HELPER FUNCTIONS:
The `count_root_dir_nodes()` function returns count of how many files in root 
directory. It is used in `fs_create()` to see if the root directory is full or 
not so a new file can be created. The `count_fat_entries()` functions returns 
number of entries in the FAT. It is used in the `add_fat_entry()` helper 
function to see if there is space available in the FAT for another entry.
`seek_blocks()`: seeks to the index of the block to read/write based on offset 
provided. Returns -1 if offset is not valid. The `add_fat_entry()` function  
adds an entry to the FAT index and updates EOF block. If no entries are 
available then the function returns -1. It is used in the `fs_write()` function 
to update the FAT when writing to a new block.

## PHASE 1:
The `fs_mount()` function opens the virtual disk file and mounts the file 
system. First calls `block_disk_open()` and reads the content of superblock. 
Then it checks that the signature is “ECS150FS”. Next, memory is allocated for 
the FAT and information is copied over from the FAT of the disk. The root 
directory of the disk is also copied over. Lastly the file descriptor array is 
initialized. The `fs_umount()` function checks if a virtual disk was open by 
using `block_disk_count()` and checks if there are still any open file 
descriptors. If there are still open files then the function returns -1. 
Otherwise, the function copies the FAT blocks, the root directory blocks, and 
the superblock to the disk by using `block_write()`. Lastly, 
`block_disk_close()` is called to close the disk and the FAT is freed. The 
`fs_info()` function prints out information of the current disk such as the 
total block count, FAT block count, root directory block count, data block 
count, data block start index, fat_free ratio and the root directory free ratio.

## PHASE 2:
The `fs_create()` function first checks if there is a virtual disk open, 
if the root directory is full, if the filename length is valid and that no other
file exists with the same name. Then it searches for an empty entry in the root 
directory and copies the filename over, sets the initial file size and sets the 
first data block to FAT_EOC. In the `fs_delete()` function, first, it checks if 
a virtual disk is open, if the filename is invalid and if the file is open. Then
it finds the match entry in the root directory and empties the data blocks 
associated with the file by setting them equal to 0. In `fs_ls()` function, it 
first checks if a virtual disk is open. Then all the file’s names, sizes and 
first data block indexes are printed out that are not empty when iterating 
through the root directory.

## PHASE 3:
The `fs_open()` function first checks if the open file table is full and if the 
filename is valid and exists in the root directory. Then the first free entry 
from the open file table is found and the information of the new entry is 
updated. Lastly the number of total files open is incremented and index of the 
entry is returned. The `fs_close()` checks the file descriptor to see if it is 
valid and then the entry in the open file table is reset/emptied. Lastly the 
number of total files open is decremented. The `fs_stat()` function simply 
checks to see if the file descriptor passed as an argument to the function is 
valid and, if so, its file size is returned. The `fs_lseek()` function checks 
the file descriptor to see if it is valid and then the `seek_blocks()` helper 
function is used to check if the offset is valid before changing to the new 
offset of the file descriptor.

## PHASE 4:
The `fs_read()` function checks if the file descriptor is valid. Then the 
starting block index and the offset of the block are found and checked for 
validation. A while loop is used to read the whole current block to a temporary 
buffer. Based off the location of the offset the data is saved to the buffer 
from the temporary buffer. A count of how many bytes have been saved to buffer 
is used to tell if the read is complete or if we must move on to the next block 
to continue reading. The `fs_write()` function is implemented in similar way as 
the `fs_read()` function. The file descriptor, starting block index and the 
offset of the block are checked for validation. A while loop is again used to 
read the whole block. Then the block is overwritten starting from the offset. A 
count of how many bytes have been written are kept track of to see if it is 
necessary to continue to the next block. If the end of the file is reached a new
block is added, if possible, to continue writing the buffer.

## TEST CASES:
The tester we made focuses on reading and writing at different offsets. We first
seek to some point in the middle of a file and overwrite what is currently in 
the file for multiple blocks. We also attempt error cases like trying to read a 
larger number of bytes than what is contained in the file. Lastly, we use assert 
for some unit tests like trying to unmount a disk while there are still open 
file descriptors, creating a file with too long of a filename and closing a file
with an invalid file descriptor.
Note: our 'disk.fs' contained a text file (size of 16,308 bytes). The commands
`off_read` and `rewrite` in `fs_testsuite.c` are designed to operate on that 
particular text file.

## SOURCES
1. https://www.gnu.org/software/libc/manual/
