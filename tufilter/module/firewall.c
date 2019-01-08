#include "firewall.h"


static int len, temp;
/*временная структура для приема фильтра от пользователя*/
static filter_struct *temp_filter;
/*установленные фильтры*/
static filter_struct filters[LIMIT];
/*счётчик установленных фильтров*/
static int n_filters = 0; 

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
	int transport;
	temp_filter = kmalloc(sizeof(filter_struct), GFP_KERNEL);
	printk(KERN_INFO "IOCTL call");
	/*if( ( _IOC_TYPE( cmd ) != IOC_MAGIC ) ) { 
		printk(KERN_INFO "Bad magic number -ENOTTY");
		return -ENOTTY;
	}*/ 
	switch( cmd ) { 
		case IOCTL_SEND_FILTER:
			if (copy_from_user(temp_filter, (void*)arg, sizeof(filter_struct)))
				return -EFAULT;
			printk(KERN_INFO "type = %d transport = %d port = %d dis/en = %d ip = %s", 
			temp_filter->type, temp_filter->transport, temp_filter->port, temp_filter->disable_enable, temp_filter->ip);
			if (temp_filter->disable_enable == DIS) {
				disable_filter();
			}
			else {
				enable_filter();
			}
			break;
		case IOCTL_GET_STRING: 
			printk(KERN_INFO "^^^^^^^^^^^^^^^^^^^^^^^");
			if( copy_to_user( (void*)arg, msg, strlen(msg) ) )
			//if( copy_to_user( (void*)arg, msg, 13 ) ) 
				return -EFAULT; 
			break;
		case IOCTL_SEND_TRANSPORT:
			get_user(transport, (int*)arg);
			printk(KERN_INFO "!!!!!transport = %d!!!!!!!!!", transport);
			break;
				
		default:
			printk("default ioctl");
			break; 
   }
   return 0; 
}

/*Устанавливает фильтр из временной структуры*/ 
void enable_filter() {
	int i;
	for(i = 0; i < LIMIT; i++) {
		/*установка фильтра в первую свободную ячейку*/
		if(filters[i].type == 0) {
			filters[i].transport = temp_filter->transport;
			filters[i].port = temp_filter->port;
			strcpy(filters[i].ip, temp_filter->ip);
			/*увеличиваем счётчик установленных фильтров*/
			n_filters++;
			printk(KERN_INFO "FILTER ENABLED");
			break;
		}
	}
}

void disable_filter() {
	int i;
	for(i = 0; i < LIMIT; i++) {
		if(filters[i].type == 1 
		&& filters[i].transport == temp_filter->transport
		&& filters[i].port == temp_filter->port
		&& (strcmp(filters[i].ip, temp_filter->ip) == 0)) {
			filters[i].type = 0;
			/*уменьшение счётчика установленных фильтров*/
			printk(KERN_INFO "FILTER DISABLED");
			n_filters--;
		}
	}
}


int proc_init (void) {
 	int i;
 	proc_create("my_firewall", 0666, NULL, &proc_fops);
	msg = "Hello, this is my simple firewall";	
	len = strlen(msg);
	temp = len;
	/* поле type используется для обозначения свободна ли ячейка фильтра,
	 * 0 - свободна, 1 - занята,
	 * инициализируем все нулями */
	for (i = 0; i < LIMIT; i++) {
		filters[i].type = 0;
	}
	return 0;
}

void proc_cleanup(void) {
	kfree(temp_filter);
	remove_proc_entry("my_firewall", NULL);
}

MODULE_LICENSE("GPL"); 
module_init(proc_init);
module_exit(proc_cleanup);
