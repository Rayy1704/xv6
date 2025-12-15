#include "kernel/types.h"
#include "user/user.h"

typedef struct example4 { // struct to encapsulate data like in example 4
    uint64 p;
    int i;
    short h;
    char c;
} example4_t;

static uint64 read_u64(char *p) { //function to read 8 bytes and convert to uint64
    uint64 v = 0;
    for(int j = 0; j < 8; j++){
        v |= (uint64)(unsigned char)p[j] << (8*j);//adding first 8 bits then left shifting by 8 bits for next byte
    }// |= is used to or the shifted value into v 
    return v;
}

static unsigned read_u32(char *p) {//function to read 4 bytes and convert to unsigned int
    unsigned v = 0;
    for(int j = 0; j < 4; j++){
        v |= (unsigned)(unsigned char)p[j] << (8*j);
    }
    return v;
}

static unsigned short read_u16(char *p) {//function to read 2 bytes and convert to unsigned short
    unsigned short v = 0;
    for(int j = 0; j < 2; j++){
        v |= (unsigned short)(unsigned char)p[j] << (8*j);
    }
    return v;
}

static char read_char(char *p) {// function to read char for consistence
    return p[0];
}

void memdump(char * fmt,char * data){
    char * ptr = data;//pointer to data to traverse
    int i = 0;//index for format string
    while(fmt[i]){//as long as there are format specifiers
        char f = fmt[i];
        if(f == 'i'){//integer format case
            unsigned v = read_u32(ptr);
            printf("%d\n", (int)v);
            ptr += 4;
        } else if(f == 'p'){//pointer format case
            uint64 v64 = read_u64(ptr);
            printf("%lx\n", v64);
            ptr += 8;
        } else if(f == 'h'){//short format case
            unsigned short vhs = read_u16(ptr);
            printf("%d\n", (int)vhs);
            ptr += 2;
        } else if(f == 'c'){//char format case
            char vc = read_char(ptr);
            printf("%c\n", vc);
            ptr += 1;
        } else if(f == 's'){//string format case with pointer
            uint64 addr = read_u64(ptr);
            char *sptr = (char*)addr;
            printf("%s\n", sptr);
            ptr += 8;
        } else if(f == 'S'){//string format case with inline string
            printf("%s\n", ptr);
            ptr += strlen(ptr) + 1;
        } else {
            //skip any other format
            i--;
        }
        i++;
    }
}

int main(int argc, char * argv[]){
    if(argc<=1){
        //examples
        printf("Example 1:\n");
        memdump("hh", (char*)(short[]){61810, 2025});
        
        printf("Example 2:\n");
        memdump("s", (char*)(char*[]){"a string"});

        printf("Example 3:\n");
        memdump("S", "another");
        example4_t example4_instance = {(uint64)"deadc0de", 42, 12345, 'Z'};
        printf("Example 4:\n");
        memdump("pihc", (char*)&example4_instance);

        printf("Example 5:\n");
        memdump("Sccccc", "hello\0world");
        exit(1);
    }else if(argc==2){//for pipelining case (e.g. echo deadc0de | memdump hhcccc)
        char buf[512];
        int n = read(0, buf, sizeof(buf));
        if(n > 0){
            memdump(argv[1], buf);
        }
    }else if(argc==3){//regular case with format and data arguments
        char * fmt = argv[1];
        char * data = argv[2];
        memdump(fmt, data);
    }else{//invalid usage
        printf("Usage: memdump <format> <data>\n");
    }
    exit(0);
}
