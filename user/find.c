#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*fmt_name(char *path)
{
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)//start from end go backwards until slash
    ;
  p++;//moveahead of slash
  return p;//returnseverything after the last slash
}

void find(char *path, char *target, char** args)//take current path and target filename
{
  char buf[512], *p;//buffer to hold path
  int fd;//file descriptor
  struct dirent de;
  struct stat st;//file information structure

  if((fd = open(path, O_RDONLY)) < 0){//open file 
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){//fills data related to file into st
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){//checking against file type
  case T_FILE:
    if(strcmp(fmt_name(path), target) == 0){//if file name is same as target then print path
      if(args){//if exec command is provided
        // 1. Prepare a new argument list
        char *new_argv[32]; // Use MAXARG from param.h if included, or a safe size like 32
        int i = 0;
        // Copy the original command (e.g., "echo", "hi")
        while(args[i] != 0 && i < 30) { 
          new_argv[i] = args[i];
          i++;
        }
        // 2. Append the found file path
        new_argv[i] = path; 
        i++;
        
        // 3. Null terminate the list
        new_argv[i] = 0;
        if(fork() == 0){//create child process
          exec(new_argv[0], new_argv);//execute command with arguments
          exit(0);
        } else {
          wait(0);//parent waits for child to finish
        }
      }else{
        printf("%s\n", path);//print path
      }
    }
    break;

  case T_DIR://in case file is directory
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){//check for path length overflow
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);      //copy current path to buffer
    p = buf + strlen(buf);//pointer to end of buffer
    *p++ = '/';     //append slash to buffer
    
    while(read(fd, &de, sizeof(de)) == sizeof(de)){//read directory entries one by one
      if(de.inum == 0)//if inode number is 0 then entry is invalid
        continue;

      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)//skip current and parent directory entries
        continue;

      memmove(p, de.name, DIRSIZ);//append entry name to path
      p[DIRSIZ] = 0;

      find(buf, target,args);//recursive call to find for new path
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc > 4 && strcmp(argv[3],"-exec")==0){
    find(argv[1],argv[2],&argv[4]);
  }
  else if(argc == 2){
     find(".", argv[1],0); 
  } else if(argc == 3){
     find(argv[1], argv[2],0);
  }else{
    printf("Usage: find [path] target_name [-exec command]\n");
    exit(1);
  }
  
  exit(0);
}