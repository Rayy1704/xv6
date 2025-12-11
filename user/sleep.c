#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc,char * argv[]){
	int seconds = atoi(argv[1])*10;
	if(argc==2&&seconds>=0){
		pause(seconds);
	}else{
		exit(1);
	}	
	exit(0);
}
