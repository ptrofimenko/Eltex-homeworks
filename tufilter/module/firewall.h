#ifndef FIREWALL_H
#define FIREWALL_H

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

#define ERR(...) fprintf( stderr, "\7" __VA_ARGS__ ), exit( EXIT_FAILURE )





#define IOC_MAGIC    'h' 
#define IOCTL_GET_STRING _IOR( IOC_MAGIC, 0, char * ) 
 
int proc_init (void);
void proc_cleanup(void);
static int open_proc(struct inode *, struct file *);
static int release_proc(struct inode *, struct file *);
static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp);
static long ioctl_filter(struct file *f, unsigned int cmd, unsigned long arg);
 

#endif
