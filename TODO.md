# TODO
### *We will use this file to supplement our planning and development process.
### *This file also contains notes and snippets we need during development.

## BEFORE SUBMISSION:
0. DISABLE debugging print flags (i.e `PRINT_ERROR`) in `fs.c`
1. Delete `./test/ref_program` folder and update `.gitignore`
2. Remove debugging files in the `./libfs/Makefile` if any.
3. Remove `instructions` folder.
4. Delete this file.
5. Do a final git push. Then begin `git bundle` process.
---

## Phase 1:

## Phase 2:

## Phase 3:

## Phase 4:


## Example Output #1:
---
### Before removing `stupid.txt`
```
-----DISK DATA BEGIN-----

---SUPER BLOCK---
Signature: ECS150FS 
Total amount of blocks of virtual disk: 8198
Root directory block index: 5
Data block start index: 6
Amount of data blocks: 8192
Number of blocks for FAT: 4

---FAT TABLE: first 10 items---
Index 0: 65535
Index 1: 65535
Index 2: 65535
Index 3: 4
Index 4: 5
Index 5: 6
Index 6: 65535
Index 7: 0
Index 8: 0
Index 9: 0

---ROOT DIR: first 3 items---
Filename [0]: file.txt
Filesize [0]: 2198
Index of first data block [0]: 1

Filename [1]: stupid.txt
Filesize [1]: 1988
Index of first data block [1]: 2

Filename [2]: above.txt
Filesize [2]: 16195
Index of first data block [2]: 3

-----DISK DATA END-----
```
### After removing `stupid.txt`
```
-----DISK DATA BEGIN-----

---SUPER BLOCK---
Signature: ECS150FS 
Total amount of blocks of virtual disk: 8198
Root directory block index: 5
Data block start index: 6
Amount of data blocks: 8192
Number of blocks for FAT: 4

---FAT TABLE: first 10 items---
Index 0: 65535
Index 1: 65535
Index 2: 0
Index 3: 4
Index 4: 5
Index 5: 6
Index 6: 65535
Index 7: 0
Index 8: 0
Index 9: 0

---ROOT DIR: first 3 items---
Filename [0]: file.txt
Filesize [0]: 2198
Index of first data block [0]: 1

Filename [1]: 
Filesize [1]: 1988
Index of first data block [1]: 0

Filename [2]: above.txt
Filesize [2]: 16195
Index of first data block [2]: 3

-----DISK DATA END-----
```
### After adding `stupid.txt` again
```
-----DISK DATA BEGIN-----

---SUPER BLOCK---
Signature: ECS150FS 
Total amount of blocks of virtual disk: 8198
Root directory block index: 5
Data block start index: 6
Amount of data blocks: 8192
Number of blocks for FAT: 4

---FAT TABLE: first 10 items---
Index 0: 65535
Index 1: 65535
Index 2: 65535
Index 3: 4
Index 4: 5
Index 5: 6
Index 6: 65535
Index 7: 0
Index 8: 0
Index 9: 0

---ROOT DIR: first 3 items---
Filename [0]: file.txt
Filesize [0]: 2198
Index of first data block [0]: 1

Filename [1]: stupid.txt
Filesize [1]: 1988
Index of first data block [1]: 2

Filename [2]: above.txt
Filesize [2]: 16195
Index of first data block [2]: 3

-----DISK DATA END-----
```
## Verdict: Root Directory is definately an array as well!
---