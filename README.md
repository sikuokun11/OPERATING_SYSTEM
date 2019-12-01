# BÁO CÁO
 ĐỒ ÁN 2 HỆ ĐIỀU HÀNH
LINUX KERNEL MODULE
& SYSTEM CALL HOOKING

*		MÔ TẢ TỔ CHỨC, THIẾT KẾ CỦA LINUX KERNEL PHÁT SINH SỐ NGẪU NHIÊN

1) Cấu trúc:
- Chương trình sẽ gồm file testForUser.c để gọi xuống kernel module được tạo để phát sinh số ngẫu nhiên 
- Kernel module được tạo sẽ nằm bên dưới kernel space trong file getRandomNumber.c, sẽ tiến hành tạo ra số ngẫu nhiên và gởi lại cho user khi được yêu cầu
-Ngoài ra ta sẽ tiến hành tạo thêm Makefile và Kbuild để biên dịch chương trình
![11](https://imgur.com/zOKNH94)
 



2) Các tham số, biến môi trường có trong kernel module phát sinh số ngẫu nhiên
a) Struct file_operators:
struct file_operations fops=
{
 	.open=dev_open;
.read=dev_read;
.release=dev_release;
}
- Mục đích: Khai báo biến một struct file_operators. Mục địch của struct này là để lưu các chức năng thực thi của kernel module yêu cầu khi open(mỗi khi mở kernel ra ), read(mỗi khi phát sinh random number và gởi đến user),release(mỗi khi phát sinh xong số và đóng thiết bị)
b) Struct _randomNum_drv:
struct _randomNum_drv 
{
	Struct class* dev_class_randomNum;// Lưu kết quả trả về của hàm class_create
	Struct device* dev_randomNum;//Lưu kết quả trả về của ham device_create
}randomNum_drv;

- Mục đích: Khai báo struct randomNum_drv để lưu các trường device class và device khi ta tiến hành khởi tạo device.

c) static int majorNumber:
-Mục đích: biến này để lưu giá trị nhận được khi khởi tạo ngẫu nhiên major number cho device

d) static int numberOpens:
- Mục đích: biến này để lưu giá trị của số lần ta gởi yêu cầu và nhận số random number tới kernel module(mỗi lần được yêu cầu và kernel module gởi một số random number thì biến này sẽ tăng lên một đơn vị)
3) Các hàm quan trọng có trong kernel module phát sinh số ngẫu nhiên

•	static int __init randomNumber_driver_init(void): hàm khởi tạo device phát sinh số ngẫu nhiên

•	static void __exit  randomNumber_driver_exit(void): hàm giải phóng device class, device driver,hủy và xóa character device 

•	static int dev_open(struct inode *inodep, struct file *filep): hàm tính số lần mà user gởi yêu cầu vào kernel để nhận số random

•	static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset): hàm phát sinh số ngẫu nhiên và gởi đến user

•	static int dev_release(struct inode *inodep, struct file *filep) : hàm đóng device

•	int register_chrdev (unsigned int  major, const char *  name,const struct file_operations *  fops): hàm này nhận vào major là một số khác 0(nếu muốn cấp phát tĩnh) hoặc bằng 0 nếu muốn cấp phát động major number, name là tên của character device và một file_operators là struct chứa các thao tác trên device để đăng kí major number cho character device. Thành công nếu kếu quả trả về lớn hơn 0


•	struct class * class_create (struct module *  owner,const char *  name): hàm này để tạo device class, nhận vào tham số là struct module *  owner trỏ đến module đang khởi tạo và const char *  name là tên của device class. Thành công nếu kếu quả trả về khác NULL


•	struct device * device_create (struct class *  class, struct device *  parent,dev_t  devt,const char *  fmt,...):  hàm này để khởi tạo character device , tham số nhận vào là struct class *  class chứa device class vừa khởi tạo, struct device *  parent chứa parent struct của device(nếu không có thì truyền vào NULL), dev_t  devt chứa character device với major number vừa khởi tạo bên trên và minor number MKDEV(majorNumber, 0), const char *  fmt chứa tên của device.


•	void device_destroy (struct class *  class, dev_t  devt): hàm hủy device được tạo trước đó bởi device_create, nhận vào tham số struct class *  class là con trỏ trỏ đến device class đã đăng kí và dev_t  devt là character device với major number đã tạo trước đó


•	void class_destroy (struct class *  cls): hàm hủy device class đã khởi tạo, tham số nhận vào là struct class *  cls là class đã đăng kí trước đó qua hàm class_create


•	void get_random_bytes(void *buf,  int nbytes ): hàm này sẽ tạo phát sinh ra một số ngẫu nhiên có kích thước nbytes và lưu vào trong buf


•	unsigned long copy_to_user (	void __user *  to,const void *  from,unsigned long  n):  hàm này sẽ truyền giá trị from là gía trị của random number đã phát sinh ở trên qua hàm get_random_bytes vào to với kích thước n là kích thước của random number và gởi đến user. Nếu kết quả trả về bằng 0 thì gởi thành công và ngược lại
2.3.1	Thiết kế kernel module
Hàm static int __init randomNumber_driver_init(void):
o	Hàm này sẽ được thực thi khi gọi module_init(randomNumber_driver_init)
o	Đầu tiên sẽ tiến hành khởi tạo major number động cho device file thông qua việc gọi hàm register_chrdev(0, DEVICE_NUM_NAME, &fops)
o	Nếu kết quả trả về của major number < 0 sẽ thất bại và kết thúc hàm
o	Ngược lại nếu thành công sẽ tiếp tục khởi tạo device class qua hàm randomNum_drv.dev_class_randomNum=class_create(THIS_MODULE,DEVICE_CLASS_NAME). Nếu kết quả trả về là NULL thì khởi tạo thất bại và gọi hàm unregister_chrdev(majorNumber, DEVICE_NAME) để hủy device number đã cấp trước đó
o	Nếu khởi tạo device class thành công, ta tiếp tục khởi tạo character device qua hàm device_create(randomNum_drv.dev_class_randomNum,NULL,MKDEV(majorNumber, 0),NULL,DEVICE_NAME).

o	Nếu thất bại gọi hàm class_destroy(randomNum_drv.dev_class_randomNum) để hủy bỏ device class đã tạo trước đó. Nếu thành công sẽ thông báo khởi tạo thành công driver.
Hàm static void __exit randomNumber_driver_exit(void):
o	Hàm này sẽ được thực thi khi gọi module_exit(randomNumber_driver_exit), tiến hành hủy device class,device driver và character device trước khi thoát
o	Tiến hành hủy bỏ  device driver đã đăng kí qua hàm device_destroy(randomNum_drv.dev_class_randomNum,MKDEV(majorNumber, 0))
o	Sau đó, hủy bỏ class device bằng class_destroy(randomNum_drv.dev_class_randomNum)
o	Sau cùng hủy đăng kí và xóa character device qua unregister_chrdev(majorNumber,DEVICE_NAME)
o	Hiển thị thông báo giải phóng thành công



Hàm static int dev_open(struct inode *inodep, struct file *filep):
o	Hàm nhận vào tham số là một con trỏ struct idode*(để chỉ thao tác thuộc về hệ thống) và struct file *(để chỉ thac tác thuộc về phía user)
o	Mỗi lần có theo tác diễn ra(mỗi lần user yêu cầu thực hiện việc gởi và nhận random number) thì biến môi trường numberOpens sẽ tăng lên 1 (tức là sẽ tính số lần mà user đã yêu cầu truy cập vào device)
o	Sau cùng hiện thị thông báo
Hàm static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset):
o	Hàm nhận vào 4 tham số là struct file* filep(con trỏ trỏ đến file object ,thao tác phía user),char *buffer (con trỏ trỏ đến buffer chứa data), size_t len(chứa kích thước buffer), và con trỏ loff_t* offset(chứa offset nếu yêu cầu)

o	Hàm sẽ tạo ra một số ngẫu nhiên thông qua hàm có sẵn là get_random_bytes(&number, sizeof(number))
o	 Sau đó tiến hành copy giá trị number ngẫu nhiên vừa tạo vào buffer và gởi đến user qua hàm copy_to_user(buffer, &number, sizeof(number))
o	Nếu kết quả trả về của hàm trên là 0(tức thành công) sẻ hiển thị thông báo thành công và giá trị số đã gởi đến user
o	Ngược lại,hiển thị thông báo thất bại nếu có lỗi xảy ra
Hàm static int dev_release(struct inode *inodep, struct file *filep):
o	Tham số nhận vào vẫn là struct inode* inodep (thao tác với phía hệ thống),struct file*filep(file thao tác với phía user)
o	Hàm này chỉ đơn giản là thực hiện việc hiển thị thông báo đóng device sau khi thực hiện xong các thao tác trên device

2.3.2	Thiết kế file thực thi phía user
o	Thiết bị device tạo ra có tên là randomNumberDevice và được chứa trong thư mục /dev/randomNumberDevice
o	Ta tiến hành mở file chứa device với mục đích read thông qua hàm open("/dev/randomNumberDevice", O_RDONLY).Kết quả trả về không âm sẽ thành công và ngược lại
o	Sau đó đọc giá trị random number qua hàm ssize_t read(int fd, void *buf, size_t count) và hiển thị giá trị number nhận được

