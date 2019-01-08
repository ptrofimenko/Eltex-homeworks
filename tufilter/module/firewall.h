#ifndef FIREWALL_H
#define FIREWALL_H

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/proc_fs.h> 
#include <linux/kobject.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <linux/sched.h>
#include <linux/udp.h> 
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h> 
#include <linux/skbuff.h>
#include <linux/net.h>

#define SHOW 23
#define SET 24
#define TCP 33
#define UDP 34
#define DIS 5
#define EN 6

#define LIMIT 10
#define IOC_MAGIC    'k' 
#define IOCTL_GET_STRING _IOR( IOC_MAGIC, 0, char * )
#define IOCTL_SEND_TRANSPORT _IOW( IOC_MAGIC, 1, int) 
#define IOCTL_SEND_FILTER _IOW( 'h', 2, filter_struct ) 

//структура для хранения фильтра
typedef struct {
	int type;
	int transport;
	int port;
	int disable_enable;
	char *ip;
} filter_struct;
 
int proc_init (void);
void proc_cleanup(void);
static int open_proc(struct inode *, struct file *);
static int release_proc(struct inode *, struct file *);
static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp);
static long ioctl_filter(struct file *f, unsigned int cmd, unsigned long arg);

void enable_filter(void);
void disable_filter(void);
 

#endif
