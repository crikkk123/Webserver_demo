#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include <list>
#include <pthread.h>
#include "locker.h"
template< typename T >
class threadpool
{
public:
	/*参数thread_number是线程池中线程的数量，
	max_requests是请求队列中最多允许的、等待处理的请求数量*/
    threadpool( int thread_number = 8, int max_requests = 10000 );
    ~threadpool();
	//往请求队列中添加任务
    bool append( T* request ,int state);

private:
	//工作线程运行的函数，他不断从任务队列中取出任务并执行
    static void* worker( void* arg );
    void run();

private:
    int m_thread_number;	/*线程池中的线程数*/
    int m_max_requests;		/*请求队列中允许的最大请求数*/
    pthread_t* m_threads;	/*描述线程池的数组，其大小为m_thread_number*/
    std::list< T* > m_workqueue;/*请求队列*/
    locker m_queuelocker;	/*保护请求队列的互斥锁*/
    sem m_queuestat;		/*是否有任务需要处理*/
    bool m_stop;			/*是否结束线程*/
};

template< typename T >
threadpool< T >::threadpool( int thread_number, int max_requests ) : 
        m_thread_number( thread_number ), m_max_requests( max_requests ), m_stop( false ), m_threads( NULL )
{
    if( ( thread_number <= 0 ) || ( max_requests <= 0 ) )
    {
        throw std::exception();
    }

    m_threads = new pthread_t[ m_thread_number ];
    if( ! m_threads )
    {
        throw std::exception();
    }
	//创建thread_number个线程，并将它们设置为脱离线程
    for ( int i = 0; i < thread_number; ++i )
    {
        printf( "create the %dth thread\n", i );
        if( pthread_create( m_threads + i, NULL, worker, this ) != 0 )
        {
            delete [] m_threads;
            throw std::exception();
        }
        if( pthread_detach( m_threads[i] ) )
        {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template< typename T >
threadpool< T >::~threadpool()
{
    delete [] m_threads;
    m_stop = true;
}

template< typename T >
//向任务队列中添加一个任务
bool threadpool< T >::append( T* request , int state)
{
	//操作工作队列时一定要加锁，因为他被所有线程共享
    m_queuelocker.lock();
    if ( m_workqueue.size() > m_max_requests )
    {
        m_queuelocker.unlock();
        return false;
    }
    
    request->m_state = state;
    m_workqueue.push_back( request );
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template< typename T >
void* threadpool< T >::worker( void* arg )
{
    threadpool* pool = ( threadpool* )arg;
    pool->run();
    return pool;
}

template< typename T >
void threadpool< T >::run()
{
    while ( ! m_stop )
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if ( m_workqueue.empty() )
        {
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if ( ! request )
        {
            continue;
        }
        if(request->m_state == 0){
            request->read_once();
        }
        else if(request->m_state == 1){
            request->do_write();
        }
        
    }
}
#endif