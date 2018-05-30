#include<stdio.h>
#include<pthread.h>
#include<malloc.h>
#include<semaphore.h>

//threads
pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;
pthread_t thread5;
pthread_t thread6;

//time control
int queneFullTimes = 0;
int queneEmptyTimes = 0;
int A[6] = {0,0,0,0,0,0};
int timeFlag = 0;

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
int maxQueneSize = 10;
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
	printf("1.OPEN\n");
	struct queneElem* currentElem = NULL;
	
	while(1){
		if(queneEmptyTimes == 2 && A[3]==1){
			pthread_cond_broadcast (&sigNotEmpty);
			currentQueneSize++;
			break;
		}

		//Sig1+Sig2+MCR1
		pthread_mutex_lock (&queneMutex);
		while(queneIsEmpty() && (queneEmptyTimes != 2)){
			pthread_cond_wait (&sigNotEmpty, &queneMutex);
			if(queneIsEmpty && (queneFullTimes - 1 == queneEmptyTimes) && queneEmptyTimes < 2){
				queneEmptyTimes++;
			}
		}
		timeFlag = 1;

		if(!timeFlag || queneEmptyTimes == 2){
			usleep(20);		
		}

		currentElem = getElemFromQuene();
		currentQueneSize--;
		
		printf("1.Consumer: element %d TAKEN; current queue length = %d;\n", 
							currentElem->number, currentQueneSize);

		pthread_mutex_unlock (&queneMutex);

		//actionsWithElemOfQuene(currentElem);

		pthread_cond_broadcast (&sigNotFull);
		free (currentElem);
		
		//Open SCR21
		sem_post (&SCR21);
		printf("1.Open SCR21\n");
	}
	printf("1.STOP !!!\n");
	A[0] = 1;	
	return NULL;
}

//thread 2
void* thread_P2(void* unused){
	printf("2.OPEN\n");
	while(1){
		if(queneFullTimes == 2 && A[5]==1){
			break;
		}	
		
		//Sig21
		pthread_mutex_lock(&mut21);
		while(!FLG21){		
			printf("2.Wait sig21\n");
			pthread_cond_wait(&sig21,&mut21);
		}
		FLG21 = 0;
		pthread_mutex_unlock(&mut21);

		//usage and modified
		printf("2.Use and modified CR2\n");
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
			if(queneIsFull && (queneFullTimes == queneEmptyTimes) && queneFullTimes < 2){
				queneFullTimes++;
				if(queneFullTimes == 2){
					break;
				}
			}			
		}
		timeFlag = 0;
		if(queneEmptyTimes == 2 && A[5] == 1){
			pthread_mutex_unlock (&queneMutex);
			break;
		}

		if(timeFlag || queneEmptyTimes == 2){
			usleep(20);		
		}

		addElemToQuene();
		currentQueneSize++;
		printf("2.Producer: element %d CREATED; current queue length = %d;\n", 
							queneEnd->number, currentQueneSize);
		
		pthread_mutex_unlock (&queneMutex);
		pthread_cond_broadcast (&sigNotEmpty);

		//Sig22
		printf("2.send sig22\n");
		pthread_cond_signal(&sig22);
		FLG22 = 1;
	}
	printf("2.STOP !!!\n");
	A[1] = 1;	
	return NULL;
}

//thread 3
void* thread_P3(void* unused){
	printf("3.OPEN\n");
	
	//modified
	printf("3.Modified CR2\n");
	aIntVar1 = 83;
	aIntVar2 = -7;
	aUnsVar1 = 651;
	aUnsVar2 = -1842;
	aLongVar1 = -7013;
	aLongVar2 = 215;
	aUnsLongVar1 = 0xDCA123;
	aUnsLongVar2 = 0xBAD718;

	while(1){
		if(A[1] == 1 && A[4] == 1 && A[5] == 1){
			break;
		}
		//Sig21
		usleep(3);	
		printf("3.Send sig21\n");		
		pthread_cond_broadcast(&sig21);
		FLG21 = 1;
		//modified
		__sync_fetch_and_add(&aIntVar1, aIntVar2);				//1
		__sync_fetch_and_and(&aUnsLongVar2, aUnsLongVar1);			//4
		
	}
	printf("3.STOP !!!\n");
	A[2] = 1;	
	return NULL;
}

//thread 4
void* thread_P4(void* unused){
	printf("4.OPEN\n");

	while(1){
		if(queneFullTimes == 2){
			pthread_cond_broadcast (&sigNotFull);
			currentQueneSize--;
			break;
		}

		//Sig1+Sig2+MCR1
		pthread_mutex_lock (&queneMutex);
		
		while(queneIsFull()){
			pthread_cond_wait (&sigNotFull, &queneMutex);
			if(queneIsFull && queneFullTimes == queneEmptyTimes && queneFullTimes < 2){
				queneFullTimes++;
			}
		}
		timeFlag = 0;

		if(timeFlag || queneEmptyTimes == 2){
			usleep(20);		
		}
		addElemToQuene();
		currentQueneSize++;
		printf("4.Producer: element %d CREATED; current queue length = %d;\n", 
							queneEnd->number, currentQueneSize);
		
		pthread_mutex_unlock (&queneMutex);
		pthread_cond_broadcast (&sigNotEmpty);
		
		//Waiting for opening SCR21
		sem_wait(&SCR21);
		
		printf("4.waiting SCR21\n");
	}
	printf("4.STOP !!!\n");
	A[3] = 1;	
	return NULL;
}

//thread 5
void* thread_P5(void* unused){
	printf("5.OPEN\n");
	struct queneElem* currentElem = NULL;
	
	while(1){
		//Sig21
		pthread_mutex_lock(&mut21);
		while(!FLG21){		
			printf("5.wait SIG21\n");
			pthread_cond_wait(&sig21,&mut21);
		}
		FLG21 = 0;
		pthread_mutex_unlock(&mut21);
		
		//usage and modified
		printf("5.Use and modified CR2\n");
		__sync_add_and_fetch(&aIntVar2, aIntVar1);				//7
		__sync_xor_and_fetch(&aUnsVar1,aUnsVar2);				//11
		__sync_nand_and_fetch(&aIntVar1,aIntVar2);				//12
		__sync_bool_compare_and_swap(&aUnsVar2, aUnsVar1+654, aUnsVar1-65);	//13
		
		//Sig1+Sig2+MCR1
		pthread_mutex_lock (&queneMutex);
		while(queneIsEmpty() && (queneEmptyTimes != 2)){			
			pthread_cond_wait (&sigNotEmpty, &queneMutex);
			if(queneIsEmpty && (queneFullTimes - 1 == queneEmptyTimes) && queneEmptyTimes < 2){
				queneEmptyTimes++;
			}
		}
		if(queneEmptyTimes == 2){
			pthread_mutex_unlock (&queneMutex);
			break;
		}
		timeFlag = 1;
		if(!timeFlag || queneEmptyTimes == 2){
			usleep(20);		
		}
		currentElem = getElemFromQuene();
		currentQueneSize--;
		
		printf("5.Consumer: element %d TAKEN; current queue length = %d;\n", 
						currentElem->number, currentQueneSize);

		pthread_mutex_unlock (&queneMutex);
		
		pthread_cond_broadcast (&sigNotFull);
		free (currentElem);
		
		//Close SCR21
		sem_close(&SCR21);
		printf("5.Close SCR21\n");

	}
	printf("5.STOP !!!\n");
	A[4] = 1;	
	return NULL;
}

//thread 6
void* thread_P6(void* unused){
	printf("6.OPEN\n");
	while(1){
		if(queneEmptyTimes == 2){
			break;
		}
		//Sig21
		pthread_mutex_lock(&mut21);
		while(!FLG21){		
			printf("6.wait sig21\n");
			pthread_cond_wait(&sig21,&mut21);
		}
		FLG21 = 0;
		pthread_mutex_unlock(&mut21);

		//usage
		printf("6.Use CR2\n");		
		__sync_fetch_and_add(&aIntVar1, aIntVar2);				//1
		__sync_fetch_and_and(&aUnsLongVar2, aUnsLongVar1);			//4
		
		//Sig22		
		pthread_mutex_lock(&mut22);
		printf("6.wait sig22\n");
		while(!FLG22){
			pthread_cond_wait(&sig22,&mut22);
		}
		pthread_mutex_unlock(&mut22);
		FLG22 = 0;		
		
		//usage
		printf("6.Use CR2\n");
		__sync_fetch_and_add(&aIntVar1, aIntVar2);				//1
		__sync_fetch_and_and(&aUnsLongVar2, aUnsLongVar1);			//4		
	}
	printf("6.STOP!!!\n");
	A[5] = 1;	
	return NULL;
}


int main(){
	printf("thread1 create\n");	
	pthread_create (&thread1,NULL,&thread_P1,NULL);
	printf("thread2 create\n");	
	pthread_create (&thread2,NULL,&thread_P2,NULL);
	printf("thread3 create\n");	
	pthread_create (&thread3,NULL,&thread_P3,NULL);
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
