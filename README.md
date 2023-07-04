# Program Description
Authors: Jet Hughes, Will Murrel, Zhiyi Huang

This project implements a set of programs which can save and retrieve a file using an indexed allocation file system.

The program takes command line arguments for store and retrieve features.

# Usage
- `./store-prog [FILE] ` Store a program up to 4864 bytes if file large that 4864 will caurse to Truncated and extra file will not been stored.
- `./store-prog [FILE] -r ` Store the program with random allocation.
- `./retrieve-prog` Retrieve a program and print it to the standard output
- `./retrieve-prog [FILE]` Retrieve the program from storage and save it to a newly created file.
- `./debug ` Display the file as stored in the file system.
- `./clean ` Reset what is displayed in the file system, which is helpful when using `./debug`