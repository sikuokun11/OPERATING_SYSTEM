#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/fdtable.h>

#define DRIVER_LINCENSE 	"GPL"
#define DRIVER_AUTHOR  		"HCMUS_PROJECT2_1712148_1712147_1712126"
#define DRIVER_DESC    		"Hooking into syscall read and syscall write"
#define DRIVER_VERSION 		"0.1"

unsigned long	**sys_call_table;

asmlinkage int ( *original_write ) ( unsigned int, const char __user *, size_t );
asmlinkage long ( *original_open ) (const char __user *, int, mode_t );

asmlinkage int	write(unsigned int fd,const char __user *buf,size_t count )
{
	if (strcmp(current->comm ,"hooksys_test")==0)
	{
  		printk( KERN_INFO "Hooking into syscall write\n ");
		char *tmp;
		char *pathname;
		struct file *file;
		struct files_struct *files = current->files;
		struct path *path ;
		
		spin_lock(&files->file_lock);
		file = fcheck_files(files, fd);
		if (!file) {
		    spin_unlock(&files->file_lock);
		    return -ENOENT;
		}

		path = &file->f_path;
		path_get(path);
		spin_unlock(&files->file_lock);

		tmp = (char *)__get_free_page(GFP_KERNEL);

		if (!tmp) {
		    path_put(path);
		    return -ENOMEM;
		}

		pathname = d_path(path, tmp, PAGE_SIZE);
		path_put(path);

		if (IS_ERR(pathname)) {
		    free_page((unsigned long)tmp);
		    return PTR_ERR(pathname);
		}

		printk(KERN_INFO"[Hooking]Wrote into file: %s\n",pathname);
		printk(KERN_INFO"[Hooking]Wrote: %ld bytes\n",count);

		free_page((unsigned long)tmp);
	}
  return (*original_write )(fd,buf,count);
}


asmlinkage long	open(const char __user * pathname, int flags, mode_t mode )
{
 	 long res;
	if (strcmp(current->comm ,"hooksys_test")==0)
	{
  		printk( KERN_INFO "Hooking into syscall open\n");
 		printk( KERN_INFO "Process excute: %s\n",current->comm);
		printk( KERN_INFO "Opened file: %s\n",pathname);
	}

	res = (*original_open)(pathname,flags,mode);
  	return res;
}


static void allow_write(void)
{

  write_cr0(read_cr0() & ~0x10000);
}

static void disallow_write( void )
{

  write_cr0(read_cr0()| 0x10000);
}

static int init_mod(void)
{

	printk(KERN_INFO"[HOOKING]: LOADED MODULE SUCCESSFULLY!!");

	sys_call_table= (unsigned long **)kallsyms_lookup_name("sys_call_table");



	original_write = (void *) sys_call_table[__NR_write];
	original_open = (void *) sys_call_table[__NR_open];

	allow_write();

	sys_call_table[__NR_write] = (unsigned long *) write;


	sys_call_table[__NR_open] =(unsigned long *)open;
	disallow_write();

  return 0;
}


static void exit_mod(void)
{
	printk(KERN_INFO"[HOOKING]: UNLOADED MODULE SUCCESSFULLY!!");
 	allow_write();
	sys_call_table[__NR_write] = (unsigned long *)original_write; 
	sys_call_table[__NR_open] = (unsigned long *)original_open;
	disallow_write();
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE(DRIVER_LINCENSE); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cua module */
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */
