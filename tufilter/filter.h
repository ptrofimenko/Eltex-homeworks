#ifndef __FILTER_H__
#define __FILTER_H__

#define SHOW 23
#define SET 24
#define TCP 33
#define UDP 34
#define DIS 5
#define EN 6

#define LIMIT 10

/* Путь к proc файлу модуля.
 * В коде модуля он не используется, но я посчитал, что лучше будет 
 * вынести его в общий заголовочный файл, так как это повысит удобство
 * пользования модуля клиентом (не придется искать путь самостоятельно) */
#define DEVPATH "/proc/my_firewall"

#define IOC_MAGIC    'k' 
#define IOCTL_GET_NFILTERS _IOR( IOC_MAGIC, 0, int)
#define IOCTL_GET_STAT _IOR( IOC_MAGIC, 1, char *)
#define IOCTL_GET_STATLEN _IOR( IOC_MAGIC, 3, int)
#define IOCTL_SEND_FILTER _IOW( 'h', 2, filter_struct ) 

#endif
