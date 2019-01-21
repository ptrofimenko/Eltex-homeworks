#include "firewall.h"
#include "../filter.h"


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
	int n = n_filters;
	int i;
	int shift = 0;
	int statlen;					  
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
			/*вывод получпенного фильтра в логи*/
			printk(KERN_INFO "type = %d transport = %d port = %d dis/en = %d ip = %s", 
			temp_filter->type, temp_filter->transport, temp_filter->port, 
			temp_filter->disable_enable, temp_filter->ip);
			
			if (temp_filter->disable_enable == DIS) {
				disable_filter();
			}
			if (temp_filter->disable_enable == EN && n_filters < LIMIT) {
				enable_filter();
			}
			
			printk(KERN_NOTICE "n_filters = %d", n_filters);
			break;
			/*отправка пользователю количества установленных фильтров*/
		case IOCTL_GET_NFILTERS:
				
				put_user(n, (int __user *)arg);
				printk(KERN_INFO "nfilters sent to user (%d)", n_filters);
				break;
				
		case IOCTL_GET_STATLEN:
			
			msg = (char *) kmalloc(1000 * sizeof(char), GFP_KERNEL);
			/*формирование строки со статистикой*/
			for (i = 0; i < LIMIT; i++) {
				if(filters[i].type == 1) {
					if(filters[i].transport == UDP) {
						shift += sprintf(msg + shift, "proto = udp ");
					}
					else {
						shift += sprintf(msg + shift, "proto = tcp ");
					}
					shift += sprintf(msg + shift, "ip = %s port = %d number of banned packets = %d\n",
					filters[i].ip, filters[i].port, filters[i].disable_enable);
				}
			}
			
			/*отправка длины строки со статистикой*/
			put_user(strlen(msg), (int __user *)arg);
			break;
		case IOCTL_GET_STAT:
			statlen = strlen(msg);
			if( copy_to_user( (void*)arg, msg, statlen ) )
				return -EFAULT;
			printk(KERN_INFO "%s ---- %d", msg, statlen);
			kfree(msg); 
			break;
				
   }
   /* костыльный printk, так как последний printk при вызове ioctl
	* прописывается только при следующем вызове ioctl, при этом 
	* системное время записи соответствует тому, в которое она должна была
	* прописаться, а реальное совпадает с новым вызовом ioctl */
	printk(KERN_INFO "");
	return 0; 
}

/*Устанавливает фильтр из временной структуры*/ 
void enable_filter() {
	int i;
	/*флаг = 1 если отправленный фильтр уже установлен*/
	int flag = 0;
	/*проверка, установлен ли уже полученный фильтр*/
	
	for(i = 0; i < LIMIT; i++) {
		if(filters[i].type == 1 
		&& filters[i].transport == temp_filter->transport
		&& filters[i].port == temp_filter->port) {
			/* добавлена дополнительная проверка, так как с NULL указателями
			 * strcmp работает некорректно и приводит к ошибкам */
			if(temp_filter->ip == NULL && filters[i].ip == NULL) {
				flag = 1;
				break;
			}
			if (temp_filter->ip != NULL && filters[i].ip != NULL) {
				if (strcmp(filters[i].ip, temp_filter->ip) == 0) {
					flag = 1;
					break;
				}
			}
			
		}
	}
	if(flag == 0) {	
		/*установка фильтра в первую свободную ячейку*/
		for(i = 0; i < LIMIT; i++) {
			if(filters[i].type == 0) {
				filters[i].transport = temp_filter->transport;
				filters[i].port = temp_filter->port;
				
				if(temp_filter->ip != NULL) {
					filters[i].ip = (char *) kmalloc(60 * sizeof(char), GFP_KERNEL);
					strncpy(filters[i].ip, temp_filter->ip, 60);
				}
				else {
					filters[i].ip = NULL;
				}
				filters[i].disable_enable = 0;
				/*увеличиваем счётчик установленных фильтров*/
				n_filters++;
				filters[i].type = 1;
				printk(KERN_NOTICE "FILTER ENABLED");
				break;
			}
		}
	}
}

void disable_filter() {
	int i;
	for(i = 0; i < LIMIT; i++) {
		if(filters[i].type == 1 
		&& filters[i].transport == temp_filter->transport
		&& filters[i].port == temp_filter->port) {
			/* добавлена дополнительная проверка, так как с NULL указателями
			 * strcmp работает некорректно и приводит к ошибкам */
			if(temp_filter->ip == NULL && filters[i].ip == NULL) {
				filters[i].type = 0;
				/*уменьшение счётчика установленных фильтров*/
				printk(KERN_NOTICE "FILTER DISABLED");
				n_filters--;
			}
			if (temp_filter->ip != NULL && filters[i].ip != NULL) {
				if (strcmp(filters[i].ip, temp_filter->ip) == 0) {
					filters[i].type = 0;
					kfree(filters[i].ip);
					printk(KERN_NOTICE "FILTER DISABLED");
					n_filters--;
				}
			}
			
		}
	}
}

static unsigned int hook_filter(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
	struct iphdr *ip_h;
	struct udphdr *udp_h;
	int i;
	
	if(!skb)
		return NF_ACCEPT;

	ip_h = (struct iphdr *) skb_network_header(skb);
	
	for(i = 0; i < LIMIT; i++) {
		if(filters[i].type == 1) {
			if((ip_h->protocol == IPPROTO_UDP && filters[i].transport == UDP)
			|| (ip_h->protocol == IPPROTO_TCP && filters[i].transport == TCP)) {
				if(ip_h->saddr == inet_addr(filters[i].ip) || filters[i].ip == NULL) {
					udp_h = (struct udphdr *) skb_transport_header(skb);
					if(ntohs(udp_h->source) == filters[i].port || filters[i].port == 0) {
						/* используем поле disable_enable для счётчика 
						 * количества заблокированных пакетов по текущему
						 * фильтру */
						filters[i].disable_enable++;
						return NF_DROP;
					}
				}
			}
		}
	}
	
	return NF_ACCEPT;
	
}

unsigned int inet_addr(char *str)	{ 
	int a,b,c,d; 
	char arr[4]; 
    sscanf(str,"%d.%d.%d.%d",&a,&b,&c,&d); 
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d; 
    return *(unsigned int*)arr; 
} 

int proc_init (void) {
 	int i;
 	int ret;
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
	
	nfho_out.hook = hook_filter;
    nfho_out.hooknum = NF_INET_LOCAL_OUT;
    nfho_out.pf = PF_INET;
    nfho_out.priority = NF_IP_PRI_FIRST;
	
	nfho_in.hook = hook_filter;
    nfho_in.hooknum = NF_INET_LOCAL_IN;
    nfho_in.pf = PF_INET;
    nfho_in.priority = NF_IP_PRI_FIRST;
    
    /*Регистрация сетевых хуков*/
    ret = nf_register_net_hook(&init_net, &nfho_out);
    if (ret) {
		printk(KERN_INFO "could not register netfilter hook\n");
	}
	
	ret = nf_register_net_hook(&init_net, &nfho_in);
    if (ret) {
		printk(KERN_INFO "could not register netfilter hook\n");
	}
	
	
	return 0;
}

void proc_cleanup(void) {
	nf_unregister_net_hook(&init_net, &nfho_in);
	nf_unregister_net_hook(&init_net, &nfho_out);
	remove_proc_entry("my_firewall", NULL);
}

MODULE_LICENSE("GPL"); 
module_init(proc_init);
module_exit(proc_cleanup);
