#include "stdafx.h"
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <conio.h>
#include <ctype.h>
#include <signal.h>
#include <iostream>

#include<Windows.h>
using namespace std;
#pragma comment(lib,"pthreadVC2.lib")  

#define N 5  //消费者或者生产者的数目
#define M 10  //缓冲数目

int productin = 0; //生产者放置产品的位置
int prochaseout = 0; //消费者取产品的位置

int buff[M] = {0}; //缓冲区初始化为0，开始时没有产品。

sem_t empty_sem; // 同步信号量，当满的时候阻止生产者放产品。
sem_t full_sem; //同步信号量，当没有产品的时候阻止消费者消费。

pthread_mutex_t mutex; //互斥信号量，一次只有一个线程访问缓冲区。

int product_id = 0; //生产者id
int prochase_id = 0; //消费者id

void SignalExit(int signo)
{
    printf("程序退出%d\n",signo);
    return;
}

void PrintProduction()
{
    printf("此时的产品队列为::");
    for(int i = 0; i < M; i++ )
    {
        printf("%d  ",buff[i]);
    }
    printf("\n\n");
}

//////////////////////生产者方法////////////////////
void* Product(void* pramter)
{
    int id = ++product_id;
    while(1)
    {
        Sleep(5000); //毫秒
        sem_wait(&empty_sem); //给信号量减1操作
        pthread_mutex_lock(&mutex);
        productin = productin % M;
        printf("生产者%d在产品队列中放入第%d个产品\n\n",id,productin+1);
        buff[productin] = 1;
        PrintProduction();
        ++productin;

        pthread_mutex_unlock(&mutex); //释放互斥量对象
        sem_post(&full_sem); //给信号量的值加1操作
    }
}

//////////////消费者方法///////////////////////
void* Prochase( void* pramter )
{
    int id = ++prochase_id;
    while(1)
    {
        Sleep(7000);
        sem_wait(&full_sem);
        pthread_mutex_lock(&mutex);
        prochaseout = prochaseout % M;
        printf("消费者%d从产品队列中取出第%d个产品\n\n",id,prochaseout+1);

        buff[prochaseout] = 0;
        PrintProduction();
        ++prochaseout;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty_sem);
    }
}

int main()
{
    cout << "生产者和消费者数目都为5,产品缓冲区为10,生产者每2秒生产一个产品，消费者每5秒消费一个产品" << endl << endl;
    pthread_t productid[N];
    pthread_t prochaseid[N];

    int ret[N];

    //初始化信号量
    int seminit1 = sem_init(&empty_sem,0,M);
    int seminit2 = sem_init(&full_sem,0,0);
    if( seminit1 != 0  && seminit2 != 0 )
    {
        printf("sem_init failed !!!\n");
        return 0;
    }

    //初始化互斥信号量
    int mutexinit = pthread_mutex_init(&mutex,NULL);
    if( mutexinit != 0 )
    {
        printf("pthread_mutex_init failed !!\n");
        return 0;
    }

    //创建n个生产者线程
    for(int i = 0; i < N; i++ )
    {
        ret[i] = pthread_create( &productid[i], NULL,Product,(void*)(&i) );
        if( ret[i] != 0 )
        {
            printf("生产者%d线程创建失败!\n",i);
            return 0;
        }
    }

    //创建n个消费者线程
    for(int j = 0; j < N; j++ )
    {
        ret[j] = pthread_create(&prochaseid[j],NULL,Prochase,NULL);
        if( ret[j] != 0 )
        {
            printf("消费者%d线程创建失败\n",j);
            return 0;
        }
    }

    ///////////////////////等待线程被销毁///////////////////////////////////////////////
    for( int k = 0; k < N; k++ )
    {
        printf("销毁线程\n");
        pthread_join(productid[k],NULL);
        pthread_join(prochaseid[k],NULL);
    }
    return 0;
}
