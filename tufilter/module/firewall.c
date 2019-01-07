#include "firewall.h"


static int len, temp;

static char *msg;

struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.read = read_proc,
	.open = open_proc,
	.release = release_proc,
	.unlocked_ioctl = ioctl_filter
};

static int open_proc(struct inode *inode, struct file *file) {
	return 0;
}

static int release_proc(struct inode *inode, struct file *file) {
	return 0;
}

static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp) {

	if(count > temp) {
		count = temp;
	}
	temp = temp-count;
	copy_to_user(buf, msg, count);
	if(count == 0)
		temp=len; 
   
	return count;
}

static long ioctl_filter(struct file *f, 
                      unsigned int cmd, unsigned long arg ) {
	printk(KERN_INFO "???????????????????????");
	if( ( _IOC_TYPE( cmd ) != IOC_MAGIC ) ) { 
		printk(KERN_INFO "Bad magic number -ENOTTY");
		return -ENOTTY;
	} 
	switch( cmd ) { 
		case IOCTL_GET_STRING: 
			printk(KERN_INFO "^^^^^^^^^^^^^^^^^^^^^^^");
			//if( copy_to_user( (void*)arg, msg, _IOC_SIZE( cmd ) ) )
			if( copy_to_user( (void*)arg, msg, 13 ) ) 
				return -EFAULT; 
			break; 
      default: 
         return -ENOTTY; 
   } 
   return 0; 
}



int proc_init (void) {
 	proc_create("my_firewall", 0666, NULL, &proc_fops);
	msg = " Hello World ";	
	len = strlen(msg);
	temp = len;
	printk(KERN_INFO "1.len=%d", len);
	return 0;
}

void proc_cleanup(void) {
	remove_proc_entry("my_firewall", NULL);
}

MODULE_LICENSE("GPL"); 
module_init(proc_init);
module_exit(proc_cleanup);
