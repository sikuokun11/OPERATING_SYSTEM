#include <stdio.h>
#include <stdlib.h>
#include <errno.h>   //Thong bao loi
#include <unistd.h>  // THu vien nay dung cho ham read random number from device
#include <fcntl.h>   // Chi dinh che do  O_RDONLY(chi doc)

int main()
{
	int fd, number;

	printf("Starting Get Random Number Device\n");

	// Mo device voi muc dich doc so ngau nhien

	fd = open("/dev/randomNumberDevice", O_RDONLY); 
	//printf("%d",fd);
	if (fd < 0){
		perror("Failed to open the device!!");//Truong hop mo that bai
		return errno;
	}

	printf("Getting number from randomNumberDevice\n");
	read(fd, &number, sizeof(number));
	//Nhan so ngau nhien va luu vao bien number
	
	printf("Random number received from device is %d\n", number);
	//Hien thi so ngau nhien nhan duoc
	printf("Close the program random number!!\n");
	//Dong chuong trinh
	return 0;       	
}
