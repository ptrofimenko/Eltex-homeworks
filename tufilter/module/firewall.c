#include "firewall.h"


static int len, temp;

static char *msg;

struct file_operations proc_fops = {
	read: read_proc,
	ioctl: ioctl_filter
};

static int read_proc(struct file *filp, char *buf, size_t count, loff_t *offp) {

	if(count > temp) {
		count = temp;
	}
	temp = temp-count;
	copy_to_user(buf, msg, count);
	if(count == 0)
		temp=len; 
   
	return count;
}

static int ioctl_filter( struct inode *n, struct file *f, 
                      unsigned int cmd, unsigned long arg ) {
	if( ( _IOC_TYPE( cmd ) != IOC_MAGIC ) ) 
      return -ENOTTY; 
   switch( cmd ) { 
      case IOCTL_GET_STRING: 
         if( copy_to_user( (void*)arg, hello_str, _IOC_SIZE( cmd ) ) ) 
            return -EFAULT; 
         break; 
      default: 
         return -ENOTTY; 
   } 
   return 0; 
}



int proc_init (void) {
 	proc_create("hello", 0, NULL, &proc_fops);
	msg = " Hello World ";	
	len = strlen(msg);
	temp = len;
	printk(KERN_INFO "1.len=%d", len);
	return 0;
}

void proc_cleanup(void) {
	remove_proc_entry("hello", NULL);
}

MODULE_LICENSE("GPL"); 
module_init(proc_init);
module_exit(proc_cleanup);
