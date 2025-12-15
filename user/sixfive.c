#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_delimiter(char c) {
    return (c == '-' || c == '\r' || c == '\t' || c == '\n' || c == '.' || c == '/' || c == ',' || c == ' ');
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}


void process_buffer(char *buf, int n) {
    int i = 0;
    
    while (i < n) {
        //skipping delimiters
        while (i < n && is_delimiter(buf[i])) {
            i++;
        }
        //checking if we reached a valid token or the end of buffer
        if (i >= n) break;
        
        //reading a token 
        int start = i;
        int num = 0;
        int valid = 1;
        
        while (i < n && !is_delimiter(buf[i])) {//reading until a delimiter is found
            if (is_digit(buf[i])) {
                num = num * 10 + (buf[i] - '0');//converting individual char digits to int
                i++;
            } else {
                //if a char is not a digit, mark as invalid
                valid = 0;
                while (i < n && !is_delimiter(buf[i])) {//skipping rest of invalid token
                    i++;
                }
                break;
            }
        }
        
        // Check if number is divisible by 6 or 5
        if (valid && i > start && (num % 6 == 0 || num % 5 == 0)) {
            printf("%d\n", num);
        }
    }
}

int main(int argc, char *argv[]) {
    char buf[512];
    int n;
    int fd;
    
    if (argc < 2) {
        printf("Please input a valid file name.\n");
        exit(1);
    } else {
        if ((fd = open(argv[1], 0)) < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[1]);
            exit(1);
        }
            
        while ((n = read(fd, buf, sizeof(buf))) > 0) {
            process_buffer(buf, n);           
        }
        
        close(fd);
    }
    
    exit(0);
}
