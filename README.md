# Assignment 2

## Jet Devlog

30/09 - Have been very busy, Starting today.
3/10 
- Created store.c and added ability to recieve command line args. Now trying to get size of file
- 10:25 Successfully read size of file
- 10:46 added to make file
    - compile store prog
    - clean store prog
    - install module
    - uninstall module
- 10:52 successfully used sample programs to store and retrieve a file
- 2:41 Read file and converted to a set of 64 byte blocks.
- 2:53 Can write blocks to memory. Need to implement indirect indexing
- 3:53 Indirect indexing done. Need to do random allocation
- 5:30 Random allocation done 

## Will Devlog

My dog (gitlab) ate my devlog, so this is my best recreation

3/10 Started today. Also been very busy (Thanks OUSSC)

- 10:00 created retrieve.c
- 10:30 implemented command line argument handling
- 11:00 figured out how to interact with relevant inode information
- 11:30 started retrieving information from direct addressed blocks
- 12:00 Worked on using write and stdout to display file
- 1:00 Wrote block_writer method to reduce duplicate code
- 2:00 Started working on writing to files
- 3:00 Started working on how to handle the indirect addressing
- 4:00 realised that block 13 isn't always the indirect address, its the block stored at address 13
- 5:00 Bug testing and finalising
- 6:00 code comments
- 6:15 GitLab eats my dev log and makefile
- 6:20 Cry a little bit
- 6:40 Back up everything and try to reclone and repush my stuff

