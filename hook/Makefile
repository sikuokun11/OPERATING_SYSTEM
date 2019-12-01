KDIR = /lib/modules/`uname -r`/build
all:
	make -C $(KDIR) M=`pwd`
	$(CC) testForUser.c -o hooksys_test
clean:
	make -C $(KDIR) M=`pwd` clean
	rm hooksys_test
