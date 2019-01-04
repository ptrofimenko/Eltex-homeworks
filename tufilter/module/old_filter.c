#include "firewall.h"

static unsigned int firewall(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
	struct iphdr *ip_h;
	struct udphdr *udp_h;
	if(!skb)
		return NF_ACCEPT;
	if(etx_value == 3435) {
		printk(KERN_INFO "Port = %d\n", etx_value);
	}
	ip_h = (struct iphdr *) skb_network_header(skb);
	if(ip_h->protocol == IPPROTO_UDP) {
		//udp_h = (struct udphdr *) (skb_transport_header(skb) + ip_hdrlen(skb));
		udp_h = (struct udphdr *) skb_transport_header(skb);
		printk(KERN_INFO "port of pack = %d\n", ntohs(udp_h->dest));
		printk(KERN_INFO "port of pack(not mod) = %d\n", udp_h->dest);
		if(ntohs(udp_h->dest) == etx_value) {
			printk(KERN_INFO "package dropped \n");
			return NF_DROP;
		}
	}
	return NF_ACCEPT;
}
 
static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
	printk(KERN_INFO "Sysfs - Read!!!\n");
    return sprintf(buf, "%d", etx_value);
}
 
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count)
{
    printk(KERN_INFO "Sysfs - Write!!!\n");
    sscanf(buf,"%d",&etx_value);
    return count;
}
 
static int etx_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device File Opened...!!!\n");
    return 0;
}
 
static int etx_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device File Closed...!!!\n");
    return 0;
}
 
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Read function\n");
    return 0;
}
static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{       
	printk(KERN_INFO "Write Function\n");
    return 0;
}
 
 
static int __init etx_driver_init(void)
{
	int ret;
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0) {
		printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
	}
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
    /*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);
 
    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_class;
    }
 
    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }
 
    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        printk(KERN_INFO "Cannot create the Device 1\n");
        goto r_device;
    }
 
    /*Creating a directory in /sys/kernel/ */
    kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);
 
    /*Creating sysfs file for etx_value*/
    if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
        printk(KERN_INFO"Cannot create sysfs file......\n");
        goto r_sysfs;
	}
    printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        
    /*Init of nf_hook_ops*/
    nfho.hook = firewall;
    nfho.hooknum = NF_INET_LOCAL_OUT;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;
        
        
        
    /*register hook*/
    ret = nf_register_net_hook(&init_net, &nfho);
    if (ret) {
		printk(KERN_INFO "could not register netfilter hook\n");
		goto r_hook;
	}

        
    return 0;
 
r_sysfs:
    kobject_put(kobj_ref); 
    sysfs_remove_file(kernel_kobj, &etx_attr.attr);
 
r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    cdev_del(&etx_cdev);
r_hook:
	nf_unregister_net_hook(&init_net, &nfho);
	return -1;
}
 
void __exit etx_driver_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    kobject_put(kobj_ref); 
    sysfs_remove_file(kernel_kobj, &etx_attr.attr);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");

