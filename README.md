# TinyFileSystem
A program which uses basic I/O to access a virtual disk drive. 

The disk is divided into 16 logical blocks, and each block is composed of 16 bytes. So in total, a

TFS-Drive file is always 256 bytes in size. Data is stored in big-endian bit ordering. A legal file name is a single lowercase letter, and valid directory names are single uppercase letters (ASCII representation). The root directory is stored in block 0, and each directory can hold at most 8 entries (files or directories). A directory consumes exactly one block of disk space. Each directory contains a bitmap of its blockpointers to indicate which are valid. The root directory also contains a bitmap of the free space for the disk


import LP tp - Copy a file stored in the regular file system to the current TFS-Drive.

export tp LP - copy a file stored in the current virtual disk to the regular file system.

ls tp - list the contents of the given directory of the TFS-Drive

display - shown the raw contents of the current TFS-Drive, as a table of 16 rows and columns where each entry is a 2 character hexadecimal number.

open LP - open an existing TFS-Drive file, and close the TFS-file currently in use, if any. Fails if the file does not exist.

create LP - create a new, empty TFS-Drive file. (You must initialize the root directory) Fails if the file already exists.

exit - end the program

mkdir tp - Create a new directory in the TFS-Drive.

rm tp - Remove a file or directory from the TFS-Drive. Fails if the file or directory does not exist, or if the directory is not empty.
