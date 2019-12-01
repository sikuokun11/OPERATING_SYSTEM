#include<stdio.h>
#include<errno.h>  
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>   // O_RDONLY
#include <unistd.h> 



int main(){
    
    printf("Open file successfully\n");
    int fd = open("test",O_WRONLY | O_CREAT | O_TRUNC, 0644);     
    if (fd < 0) {
        perror("Failed to open the device...!!");
        return errno;
    } 
	
    write(fd,"K17 FIT HCMUS",strlen("K17 FIT HCMUS"));
    close(fd);//Dong file
    printf("Exit program succefully\n");
    return 0;
}
