#include<linux/module.h>/* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h>/*Thu vien nay de cap phat dong/giai phong device number*/
#include<linux/device.h>/*Thu vien nay de chua cac ham phuc vu cho vien tao ra device file*/
#include <linux/random.h> // get_random_bytes
#include <linux/uaccess.h>          // Thu vien nay dung cho ham copy_from_user
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel(kernel works)
#include <linux/types.h>  

//Khai bao thong tin device
#define DRIVER_LINCENSE 	"GPL"
#define DRIVER_AUTHOR  		"HA DUY TAN<dreamleage1999@gmail.com>"
#define DRIVER_DESC    		"A module get random number"
#define DRIVER_VERSION 		"0.1"
#define DEVICE_NUM_NAME 	"randomNum_Num"
#define DEVICE_CLASS_NAME 	"randomNum_class"
#define DEVICE_NAME 		"randomNumberDevice"
#define MAX 1000

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static int    numberOpens = 0;
static int majorNumber;//Khai bao maijor number

struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .release = dev_release,
};/*Khai bao file operators structure*/

struct _randomNum_drv
{
	
	struct class *dev_class_randomNum; /*Truong nay de luu ket qua tra ve cua ham class_create*/
	struct device *dev_randomNum;/*Truong nay de luu ket qua tra ve cua ham device_create */
}randomNum_drv;
/*Ham khoi tao driver get random number*/
static int __init randomNumber_driver_init(void)
{
	
	// Cap phat dong major number
	majorNumber = register_chrdev(0, DEVICE_NUM_NAME, &fops);
	if (majorNumber < 0){
		printk(KERN_ALERT "Failed to register a major number for random number module!!\n");
		goto failed_res_major;
	}

	printk(KERN_INFO "Registered a major number %d for random number module!!\n", majorNumber);

	



	/*Tao device file*/
	randomNum_drv.dev_class_randomNum=class_create(THIS_MODULE,DEVICE_CLASS_NAME);
	/*Tao class*/
	if(randomNum_drv.dev_class_randomNum==NULL)
	{
	printk(KERN_ALERT"Failed to create a device class for random number module");
	goto failed_res_classnum;
	}	
	
	printk(KERN_INFO "Created a device class for random number module successfully\n");
	randomNum_drv.dev_randomNum=device_create(randomNum_drv.dev_class_randomNum,NULL,MKDEV(majorNumber, 0),NULL,DEVICE_NAME);
	
	/*Tao device*/
	if(IS_ERR(randomNum_drv.dev_randomNum))
	{
	printk(KERN_ALERT"Failed to create a device for random number module");
	goto failed_res_devicenum;
	}
	printk(KERN_INFO "Created a device for random number module successfully\n");
	
	printk(KERN_INFO"Initialized random number module successfully\n");
	return 0;
	
	failed_res_devicenum:/*Truong hop tao device loi*/
	class_destroy(randomNum_drv.dev_class_randomNum);/*Goi ham huy device class da tao truoc do*/

	failed_res_classnum:/*Truong hop tao device class loi*/
	unregister_chrdev(majorNumber, DEVICE_NAME);/*Goi ham huy device number da cap truoc do*/

	failed_res_major:
	return majorNumber;
	
}
static void __exit randomNumber_driver_exit(void)
{
	/*Xoa bo device file*/
	device_destroy(randomNum_drv.dev_class_randomNum,MKDEV(majorNumber, 0));/*Huy bo device*/
	class_destroy(randomNum_drv.dev_class_randomNum);/*Huy bo class device*/


	/*Giai phong device number*/
	unregister_chrdev(majorNumber,DEVICE_NAME);
	printk(KERN_INFO"Remove random number module successfully!\n");
}

/** Tang bien dem moi lan goi 1 so ngau
 * nhien tu device den user  
 *  Bien inodep A  la con tro pointer tro den inode object (dinh nghia trong thu vien linux/fs.h)
 *  Bien filep A la con tro tro den file object (dinh nghia trong thu vien linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "Device random number has been opened %d time(s)\n", numberOpens);
   return 0;
}
/*Ham nay duoc goi khi doc data duoc goi tu thiet bi den user */
 /*  send the buffer string to the user and captures any errors.
 *  Bien filep A la con tro tro den file object (dinh nghia trong linux/fs.h)
 *  Bien buffer la con tro tro den buffer buffer chua data
 *  Bien len chua do dai buffer
 *  Bien offset chua offset neu yeu cau
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int check_error;
   	int number;
	
	get_random_bytes(&number, sizeof(number)); //Ham nay tra ve mot so ngau nhien
	number %= MAX;

	// copy_to_user tra ve 0 neu thanh cong
	

	check_error = copy_to_user(buffer, &number, sizeof(number));//goi buffer string den user va kiem tra loi

	if (check_error == 0){
		printk(KERN_INFO "Device sent a random number with value %d to the user \n",number);
		return 0;
	}
	else{
		printk(KERN_ALERT "Failed to send a random number to the user\n");
		return -EFAULT;
	}
}
 


 
/** Ham dong thiet bi 
 *  Bien inodep A la con tro tro den inode object (dinh nghia trong linux/fs.h)
 *  Bien filep A la con tro tro den file object (dinh nghia trong linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Device get random number closed successfully !\n"); //Dong device
   return 0;
}
/********************************* OS specific - END ********************************/

module_init(randomNumber_driver_init);
module_exit(randomNumber_driver_exit);

MODULE_LICENSE(DRIVER_LINCENSE); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cua module */
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */
