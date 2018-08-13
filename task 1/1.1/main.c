#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define N 5
#define T 4
#define CARGO 500

void* loader(void *);
void* buyer(void *);

pthread_mutex_t mutex[N];

int count_complete = 0;

typedef struct {
	int *shops;
} data;

int main() {
	int *shops = (int *)malloc(N), i;
 
	pthread_t threads[T];
	int status;
	
	data mydata;
	
	/*Заполнение массива магазинов случайными значениями в пределах 3000*/
	srand(time(NULL));
	for (i = 0; i < N; i++) { shops[i] = rand() % 3000; }
	
	mydata.shops = &shops[0];
	for(i = 0; i < N; i++) {
		pthread_mutex_init(&mutex[i], NULL);
	}
	/*создание потоков-покупателей*/
	for(i = 0; i < 3; i++) {
		status = pthread_create(&threads[i], NULL, buyer, (void *)&mydata);
		if (status != 0) { printf("Can't create thread"); } 
	}
	
	/*создание потока-погрузчика*/
	i++;
	status = pthread_create(&threads[i], NULL, loader, (void *)&mydata);
	if (status != 0) { printf("Can't create thread"); } 
	
	/*Ждём насыщение потоков-покупателей*/
	for(i = 0; i < 3; i++) {
		status = pthread_join(threads[i], NULL);
		if (status != 0) { printf("Can't join thread. status = %d\n", status); } 
	}
	status = pthread_join(threads[T], NULL);
	if (status != 0) { printf("Can't join thread. status = %d\n", status); }
	return 0;
}

/*Код погрузика*/
void* loader(void *args) {
	printf("I'm a loader\n");
	//printf("%d", count_complete);
	data *mydata = (data *)args;
	int i = 0, number = 0;
	
	srand(time(NULL));
	/*погрузчик работает до тех пор пока количество закончивших покупателей 
	 * (count_complete) не станет равно общему количеству покупателей (T - 1)*/
	while (count_complete < T - 1) {
		/*рандомим номер магазина, каждому магазину соответствует свой мютекс*/
		number = rand() % N;
		pthread_mutex_lock(&mutex[number]);
		mydata->shops[number] += CARGO;
		for (i = 0; i < N; i++) {
			printf("shop %d = %d |", i, mydata->shops[i]);
		}
		printf("\n");
		pthread_mutex_unlock(&mutex[number]);
		sleep(2);
	}
		
	return 0;
}

/*Код покупателя*/
void* buyer(void *args) {
	printf("I'm a buyer\n");
	data *data1 = (data *)args;
	int i, j, progress = 0;
	
	while (progress < 15000) {
		for(i = 0; i < N; i++) {
			pthread_mutex_lock(&mutex[i]);
			
			if(data1->shops[i] > 0) {
				/*Если в магазине меньше чем нужно - покупатель выносит всё*/
				if(15000 - progress >= data1->shops[i]) {	
					progress += data1->shops[i];
					data1->shops[i] = 0;
				}
				/*Иначе ровно столько, сколько требуется*/
				else {
					data1->shops[i] -= (15000 - progress);
					progress = 15000;
				}
				
				for (j = 0; j < N; j++) {
					printf("shop %d = %d |", j, data1->shops[i]);
				}
				printf(" %d\n", progress);
			}
			pthread_mutex_unlock(&mutex[i]);
		}
		sleep(3);
	}
	count_complete++;
	return 0;
}


