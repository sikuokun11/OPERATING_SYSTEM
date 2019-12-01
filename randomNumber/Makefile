KDIR = /lib/modules/`uname -r`/build
all:
	make -C $(KDIR) M=`pwd` modules
	$(CC) testForUser.c -o test 
clean:
	make -C $(KDIR) M=`pwd` clean
	rm test
