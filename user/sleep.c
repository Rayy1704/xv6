#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc,char * argv[]){
	int ticks = atoi(argv[1])*10;//converting seconds to ticks
	if(argc==2&&ticks>=0){
		pause(ticks);//calling system call to sleep
	}else{
		exit(1);//if arguments are not valid, exit with error code
	}	
	exit(0);
}
