
#include<stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
using namespace std;
vector<int> result,task_idx,exit_idx;
int threadid = 0,idx = 0;

pthread_mutex_t  mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	cond = PTHREAD_COND_INITIALIZER;
pthread_key_t thread_data_key;

void tfun1(int arg){
	int &thread_data = *(int*)pthread_getspecific(thread_data_key);
	printf("hello from thread %d:thread_data = %d\n",arg,thread_data);
	thread_data = arg*10;
}
void tfun2(int arg){
	int  &thread_data = *(int*)pthread_getspecific(thread_data_key);
	thread_data += arg;

}

void * threadfun(void *arg){
	int id = *(int*)arg;
	int thread_data = -1;
	int i,taskid;

	pthread_mutex_lock(&mut);
	task_idx[id] = threadid++;
	while(threadid!=task_idx.size()) pthread_cond_wait(&cond,&mut);
	pthread_mutex_unlock(&mut);
	taskid = task_idx[id];
	if(taskid == task_idx.size()-1) pthread_cond_broadcast(&cond);

	pthread_setspecific(thread_data_key,&thread_data);


		puts("start error");
		printf("%d %d %d\n",result[0],task_idx[0],exit_idx[0]);

	tfun1(id);
	tfun2(taskid + 1);
	result[id] = thread_data;
	pthread_mutex_lock(&mut);
		puts("is something wrong1");
	exit_idx[id] = idx++;
	pthread_mutex_unlock(&mut);

		puts("is something wrong2");
		sleep(1);

	if(idx == task_idx.size()){
		pthread_mutex_destroy(&mut);
		pthread_cond_destroy(&cond);
		pthread_key_delete(thread_data_key);
		printf("output by task %d\ntaskid  :",id);
		for(i=0;i<task_idx.size();i++) printf("%3d",task_idx[i]);
		printf("\nresult ");
		for(i = 0;i<task_idx.size();i++) printf("%3d",result[i]);
		printf("\nexit order:");
		for(i = 0;i<task_idx.size();i++) printf("%2d",exit_idx[i]);
		puts("");
	}
	pthread_exit(NULL);
}
void * test(void *arg)
{
	int id = *(int*)arg;
	sleep(1);
	printf("%d\n",id );
	pthread_exit(NULL);
}
int main(int argc,char * argv[])
{

	int i,thread_num;
	pthread_attr_t attr;

	vector<pthread_t> threads;
	vector<int> crt_idx,exit_idx;

	if(argc!=2){
		thread_num = sysconf(_SC_NPROCESSORS_ONLN);
		printf("default thread_num = %d\n",thread_num);

	} else sscanf(argv[1],"%d",&thread_num);

	threads.resize(thread_num);
	crt_idx.resize(thread_num);
	result.resize(thread_num);
	task_idx.resize(thread_num);
	exit_idx.resize(thread_num);
	
	for(i = 0;i<thread_num;i++) result[i] = task_idx[i] = exit_idx[i] = -1;
	pthread_key_create(&thread_data_key,NULL);

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	for(i = 0;i<thread_num;i++){
		crt_idx[i] = i;
		pthread_create(&threads[i],&attr,threadfun,&crt_idx[i]);
	}

		sleep(4);
		pthread_exit(0);

}

