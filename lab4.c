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

//Signal variable
pthread_cond_t sig22 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut22a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut22b = PTHREAD_MUTEX_INITIALIZER;



//QUENE
int maxQueneSize = 80;
int currentQueneSize = 0;

int queneIsFull(){
	return currentQueneSize >= maxQueneSize;
}
int queneIsEmpty(){
	return currentQueneSize <= 0;
}

//mutex which control quene and signal values for fuul and empty status
pthread_mutex_t		MCR1 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  	Sig1NotEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t  	Sig2NotFull = PTHREAD_COND_INITIALIZER;


struct queneElem{
	struct queneElem* next;
	int number;
};

struct queneElem* queneBegin = NULL;
struct queneElem* queneEnd = NULL;

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
	while(1){
		printf("thread_P1\n");	
		//Sig1+Sig2+MCR1

		//Open SCR21
			}
	return NULL;
}

//thread 2
void* thread_P2(void* unused){
	while(1){
		printf("thread_P2\n");	
		//Sig21
		
		//usage and modified
		printf("Usage and modified thread2");
		__sync_fetch_and_add(&aIntVar1, aIntVar2);				//1
		__sync_fetch_and_and(&aUnsLongVar2, aUnsLongVar1);			//4
		__sync_add_and_fetch(&aUnsVar1, aUnsVar2);				//7
		__sync_xor_and_fetch(&aLongVar1, aLongVar2);				//11
		__sync_nand_and_fetch(&aIntVar2, aIntVar1);				//12
		__sync_val_compare_and_swap(&aIntVar2, aIntVar1+89, aIntVar2-15);	//14
		
		//Sig1+Sig2+MCR1
		//Sig22
		pthread_mutex_lock(&mut22a);
		pthread_cond_signal(&sig22);
		pthread_mutex_unlock(&mut22a);
		
	}
	return NULL;
}

//thread 3
void* thread_P3(void* unused){
	
	//modified
	printf("Modified thread3");
	aIntVar1 = 83;
	aIntVar2 = -7;
	aUnsVar1 = 651;
	aUnsVar2 = -1842;
	aLongVar1 = -7013;
	aLongVar2 = 215;
	aUnsLongVar1 = 0xDCA123;
	aUnsLongVar2 = 0xBAD718;

	while(1){
		printf("thread_P3\n");	
		//Sig21		
		//modified
		printf("Modified thread3_2");
	}
	return NULL;
}

//thread 4
void* thread_P4(void* unused){
	while(1){
		printf("thread_P4\n");	
		//Sig1+Sig2+MCR1
		//Waiting for opening SCR21
	}
	return NULL;
}

//thread 5
void* thread_P5(void* unused){
	while(1){
	printf("thread_P5\n");	
		//Sig21
		
		//usage and modified
		printf("Usage and modified thread5");
		__sync_add_and_fetch(&aIntVar2, aIntVar1);				//7
		__sync_xor_and_fetch(&aUnsVar1,aUnsVar2);				//11
		__sync_nand_and_fetch(&aIntVar1,aIntVar2);				//12
		__sync_bool_compare_and_swap(&aUnsVar2, aUnsVar1+654, aUnsVar1-65);	//13
		
		//Sig1+Sig2+MCR1
		//Close SCR21
	}
	return NULL;
}

//thread 6
void* thread_P6(void* unused){
	int intSum;
	unsigned unsSum;
	long longSum;
	unsigned long unsLongSum;
	while(1){
		printf("thread_P6\n");	
		//Sig21

		//usage
		printf("Usage thread6");		
		intSum = aIntVar1 + aIntVar2;
		printf("Sum of int values is %d\n", intSum);
		unsSum = aUnsVar1 + aUnsVar2;
		printf("Sum of unsigned values is %u\n", unsSum);
		
		//Sig22		
		pthread_mutex_lock(&mut22b);
		pthread_cond_wait(&sig22,&mut22b);
		pthread_mutex_unlock(&mut22b);

		//usage
		printf("Usage thread6_2");
		longSum = aLongVar1 + aLongVar2;
		printf("Sum of long values is %ld\n", longSum);
		unsLongSum = aUnsLongVar1 + aUnsLongVar2;
		printf("Sum of unsigned values is %lu\n", unsLongSum);
	}
	return NULL;
}


int main(){
	printf("thread1 create");	
	//pthread_create (&thread1,NULL,&thread_P1,NULL);
	printf("thread2 create");	
	pthread_create (&thread2,NULL,&thread_P2,NULL);
	printf("thread3 create");	
	//pthread_create (&thread3,NULL,&thread_P3,NULL);
	printf("thread4 create");	
	//pthread_create (&thread4,NULL,&thread_P4,NULL);
	printf("thread5 create");	
	//pthread_create (&thread5,NULL,&thread_P5,NULL);
	printf("thread6 create");	
	pthread_create (&thread6,NULL,&thread_P6,NULL);

	pthread_join(thread1,NULL);
  	pthread_join(thread2,NULL);
  	pthread_join(thread3,NULL);
  	pthread_join(thread4,NULL);
  	pthread_join(thread5,NULL);
  	pthread_join(thread6,NULL);

	return 0;
}
