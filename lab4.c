#include<stdio.h>
#include<pthread.h>
#include<malloc.h>
#include<semaphore.h>
#include<unistd.h>

pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;
pthread_t thread5;
pthread_t thread6;

//semaphore
sem_t SCR21;

//Signal variable
pthread_cond_t sig22 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut22 = PTHREAD_MUTEX_INITIALIZER;
int FLG22 = 0;

pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut21 = PTHREAD_MUTEX_INITIALIZER;
int FLG21 = 0;



//QUENE
int maxQueneSize = 40;
int currentQueneSize = 0;

int queneIsFull(){
	return currentQueneSize >= maxQueneSize;
}
int queneIsEmpty(){
	return currentQueneSize <= 0;
}

//mutex which control quene and signal values for fuul and empty status
pthread_mutex_t		queneMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  	sigNotEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t  	sigNotFull = PTHREAD_COND_INITIALIZER;


struct queneElem{
	struct queneElem* next;
	int number;
};

struct queneElem* queneBegin = NULL;
struct queneElem* queneEnd = NULL;

actionsWithElemOfQuine(struct queneElem* current){
}

void addElemToQuene(){
	struct queneElem* p = (struct queneElem*)malloc(sizeof(struct queneElem));
	p->next = NULL;

	if(queneBegin == NULL){
		p->number = 0;
		queneBegin = p;
	}
	else{
		p->number = queneEnd->number+1;
		queneEnd->next = p;
	}
	queneEnd = p;
}


struct queneElem* getElemFromQuene(){
	struct queneElem* p = NULL;
	p = queneBegin;
	queneBegin = queneBegin->next;
	return p;
}



//atomarri variables
int aIntVar1 = 0, aIntVar2 = 0;
unsigned aUnsVar1 = 0, aUnsVar2 = 0;
long aLongVar1 = 0, aLongVar2 = 0;
unsigned long aUnsLongVar1 = 0, aUnsLongVar2 = 0;


//thread 1
void* thread_P1(void* unused){
	printf("thread1 open\n");
	struct queneElem* currentElem = NULL;
	
	while(1){
		//Sig1+Sig2+MCR1
		pthread_mutex_lock (&queneMutex);
		while(queneIsEmpty()){
			pthread_cond_wait (&sigNotEmpty, &queneMutex);
		}

		currentElem = getElemFromQuene();
		currentQueneSize--;
		
		printf("Consumer thread1: element %d TAKEN; current queue length = %d;\n", currentElem->number, currentQueneSize);

		pthread_mutex_unlock (&queneMutex);

//		actionsWithElemOfQuene(currentElem);

		pthread_cond_broadcast (&sigNotFull);

		free (currentElem);
		
		//Open SCR21
		sem_post (&SCR21);
	}
	printf("Consumer thread1 stopped !!!\n");
	return NULL;
}

//thread 2
void* thread_P2(void* unused){
	printf("thread2 open\n");
	while(1){
		//Sig21
		pthread_mutex_lock(&mut21);
		while(!FLG21){		
			pthread_cond_wait(&sig21,&mut21);
		}
		FLG21 = 0;
		pthread_mutex_unlock(&mut21);

		//usage and modified
		printf("Usage and modified thread2\n");
		__sync_fetch_and_add(&aIntVar1, aIntVar2);				//1
		__sync_fetch_and_and(&aUnsLongVar2, aUnsLongVar1);			//4
		__sync_add_and_fetch(&aUnsVar1, aUnsVar2);				//7
		__sync_xor_and_fetch(&aLongVar1, aLongVar2);				//11
		__sync_nand_and_fetch(&aIntVar2, aIntVar1);				//12
		__sync_val_compare_and_swap(&aIntVar2, aIntVar1+89, aIntVar2-15);	//14
		
		//Sig1+Sig2+MCR1
		
		pthread_mutex_lock (&queneMutex);
		
		while(queneIsFull()){
			pthread_cond_wait (&sigNotFull, &queneMutex);
		}
		
		addElemToQuene();
		currentQueneSize++;
		printf("Producer thread 2: element %d CREATED; current queue length = %d;\n", queneEnd->number, currentQueneSize);
		
		pthread_mutex_unlock (&queneMutex);
		pthread_cond_broadcast (&sigNotEmpty);

		//Sig22
		pthread_cond_signal(&sig22);
		FLG22 = 1;
		
	}
	printf("Consumer thread2 stopped !!!\n");
	return NULL;
}

//thread 3
void* thread_P3(void* unused){
	printf("thread3 open\n");
	
	//modified
	printf("Modified thread3\n");
	aIntVar1 = 83;
	aIntVar2 = -7;
	aUnsVar1 = 651;
	aUnsVar2 = -1842;
	aLongVar1 = -7013;
	aLongVar2 = 215;
	aUnsLongVar1 = 0xDCA123;
	aUnsLongVar2 = 0xBAD718;

	while(1){
		//Sig21
		pthread_cond_signal(&sig21);		
		//modified
//		printf("Modified thread3_2\n");
	}
	printf("Consumer thread3 stopped !!!\n");
	return NULL;
}

//thread 4
void* thread_P4(void* unused){
	printf("thread4 open\n");
	while(1){
		//Sig1+Sig2+MCR1
		pthread_mutex_lock (&queneMutex);
		
		while(queneIsFull()){
			pthread_cond_wait (&sigNotFull, &queneMutex);
		}
		
		addElemToQuene();
		currentQueneSize++;
		printf("Producer thread 4: element %d CREATED; current queue length = %d;\n", queneEnd->number, currentQueneSize);
		
		pthread_mutex_unlock (&queneMutex);
		pthread_cond_broadcast (&sigNotEmpty);
//Waiting for opening SCR21
		sem_wait(&SCR21);
	}
	printf("Consumer thread4 stopped !!!\n");
	return NULL;
}

//thread 5
void* thread_P5(void* unused){
	printf("thread5 open\n");
	struct queneElem* currentElem = NULL;
	
	while(1){
		//Sig21
		pthread_mutex_lock(&mut21);
		while(!FLG21){		
			pthread_cond_wait(&sig21,&mut21);
		}
		FLG21 = 0;
		pthread_mutex_unlock(&mut21);
		
		//usage and modified
		printf("Usage and modified thread 5\n");
		__sync_add_and_fetch(&aIntVar2, aIntVar1);				//7
		__sync_xor_and_fetch(&aUnsVar1,aUnsVar2);				//11
		__sync_nand_and_fetch(&aIntVar1,aIntVar2);				//12
		__sync_bool_compare_and_swap(&aUnsVar2, aUnsVar1+654, aUnsVar1-65);	//13
		
		//Sig1+Sig2+MCR1
		pthread_mutex_lock (&queneMutex);
		while(queneIsEmpty()){
			pthread_cond_wait (&sigNotEmpty, &queneMutex);
		}

		currentElem = getElemFromQuene();
		currentQueneSize--;
		
		printf("Consumer thread5: element %d TAKEN; current queue length = %d;\n", currentElem->number, currentQueneSize);

		pthread_mutex_unlock (&queneMutex);

//		actionsWithElemOfQuene(currentElem);

		pthread_cond_broadcast (&sigNotFull);

		free (currentElem);
		
		//Close SCR21
		sem_wait(&SCR21);
		
	}
	printf("Consumer thread5 stopped !!!\n");
	return NULL;
}

//thread 6
void* thread_P6(void* unused){
	printf("thread6 open\n");
	int intSum;
	unsigned unsSum;
	long longSum;
	unsigned long unsLongSum;
	while(1){
		//Sig21
		pthread_mutex_lock(&mut21);
		while(!FLG21){		
			pthread_cond_wait(&sig21,&mut21);
		}
		FLG21 = 0;
		pthread_mutex_unlock(&mut21);

		//usage
		printf("Usage thread6\n");		
		intSum = aIntVar1 + aIntVar2;
		printf("Sum of int values is %d\n", intSum);
		unsSum = aUnsVar1 + aUnsVar2;
		printf("Sum of unsigned values is %u\n", unsSum);
		
		//Sig22		
		pthread_mutex_lock(&mut22);
		while(!FLG22){
			pthread_cond_wait(&sig22,&mut22);
		}
		pthread_mutex_unlock(&mut22);
		FLG22 = 0;		
		
		//usage
		printf("Usage thread6_2\n");
		longSum = aLongVar1 + aLongVar2;
		printf("Sum of long values is %ld\n", longSum);
		unsLongSum = aUnsLongVar1 + aUnsLongVar2;
		printf("Sum of unsigned values is %lu\n", unsLongSum);
	}
	printf("Consumer thread6 stopped !!!\n");
	return NULL;
}


int main(){
	printf("thread1 create\n");	
	pthread_create (&thread1,NULL,&thread_P1,NULL);
	printf("thread2 create\n");	
	pthread_create (&thread2,NULL,&thread_P2,NULL);
	printf("thread3 create\n");	
	//pthread_create (&thread3,NULL,&thread_P3,NULL);
	printf("thread4 create\n");	
	pthread_create (&thread4,NULL,&thread_P4,NULL);
	printf("thread5 create\n");	
	pthread_create (&thread5,NULL,&thread_P5,NULL);
	printf("thread6 create\n");	
	pthread_create (&thread6,NULL,&thread_P6,NULL);

	pthread_join(thread1,NULL);
  	pthread_join(thread2,NULL);
  	pthread_join(thread3,NULL);
  	pthread_join(thread4,NULL);
  	pthread_join(thread5,NULL);
  	pthread_join(thread6,NULL);

	return 0;
}
