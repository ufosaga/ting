/* The MIT License:

Copyright (c) 2008 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */ 

// (c) Ivan Gagis
// e-mail: igagis@gmail.com
// Version: 1

// Description:
//          ting is a ThreadING library

#include "ting.hpp"

//#include <iostream>

//
//  Windows specific definitions
//
#if defined(__WIN32__) || defined(WIN32)
#ifndef __WIN32__
#define __WIN32__
#endif

#include <windows.h>
typedef HANDLE T_Thread;
typedef CRITICAL_SECTION T_Mutex;
typedef HANDLE T_Semaphore;

#else //assume pthread
#define M__PTHREAD
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <ctime>
typedef pthread_t T_Thread;
typedef pthread_mutex_t T_Mutex;
typedef pthread_cond_t T_CondVar;
typedef sem_t T_Semaphore;

//TODO: implement
#endif

using namespace ting;

inline static T_Thread& CastToThread(ting::Thread::C_SystemIndependentThreadHandle& thr){
    M_TING_STATIC_ASSERT( sizeof(thr) >= sizeof(T_Thread) )
    return *reinterpret_cast<T_Thread*>(&thr);
};

//inline static const T_Thread& CastToThread(ting::Thread::C_SystemIndependentThreadHandle& thr){
//    return CastToThread(const_cast<ting::Thread::C_SystemIndependentThreadHandle&>(thr));
//};

inline static T_Mutex& CastToMutex(ting::Mutex::C_SystemIndependentMutexHandle& mut){
    M_TING_STATIC_ASSERT( sizeof(mut) >= sizeof(T_Mutex) )
    return *reinterpret_cast<T_Mutex*>(&mut);
};

#ifndef __WIN32__
inline static T_CondVar& CastToCondVar(ting::CondVar::C_SystemIndependentCondVarHandle& cond){
    M_TING_STATIC_ASSERT( sizeof(cond) >= sizeof(T_CondVar) )
    return *reinterpret_cast<T_CondVar*>(&cond);
};
#endif

inline static T_Semaphore& CastToSemaphore(ting::Semaphore::C_SystemIndependentSemaphoreHandle& sem){
    M_TING_STATIC_ASSERT( sizeof(sem) >= sizeof(T_Semaphore) )
    return *reinterpret_cast<T_Semaphore*>(&sem);
};

#ifdef __WIN32__
static DWORD __stdcall RunThread(void *data)
#elif defined(M__PTHREAD) //pthread
static void* RunThread(void *data)
#else
#error "Unknown platform"
#endif
{
    ting::Thread *thr = reinterpret_cast<ting::Thread*>(data);
    thr->Run();
    
#ifdef M__PTHREAD
    pthread_exit(0);
#endif
    return 0;
};

Thread::Thread() :
        quitFlag(false),
        isRunning(false)
{
#if defined(__WIN32__)
    CastToThread(this->thr) = NULL;
#endif
};

void Thread::Start(uint stackSize){
    //TODO: use mutex here
    if(this->isRunning){
        throw ting::Exc("Thread::Start(): Thread is already running");
    }
    
#ifdef __WIN32__
    CastToThread(this->thr) = CreateThread(NULL, static_cast<size_t>(stackSize), &RunThread, reinterpret_cast<void*>(this), 0, NULL);
    if(CastToThread(this->thr) == NULL){
        throw ting::Exc("Thread::Start(): starting thread failed");
    }
#else //pthread
    {
        pthread_attr_t attr;
        
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_attr_setstacksize (&attr, static_cast<size_t>(stackSize));
        
        if(pthread_create(&CastToThread(this->thr), &attr, &RunThread, this) !=0){
            throw ting::Exc("Thread::Start(): starting thread failed");
        }
        pthread_attr_destroy(&attr);
    }
#endif
    
    this->isRunning = true;
};

void Thread::Join(){
    //TODO: use mutex
    if(!this->isRunning)
        return;
    
#ifdef __WIN32__
    WaitForSingleObject(CastToThread(this->thr), INFINITE);
    CloseHandle(CastToThread(this->thr));
    CastToThread(this->thr) = NULL;
#else //pthread
    pthread_join(CastToThread(this->thr), 0);
    //TODO: need to close the thread handle???
#endif
};

Thread::~Thread(){
    //TODO: use mutex
    this->quitFlag = true;
    this->PushQuitMessage();
    this->Join();
};

//static
void Thread::Sleep(uint msec){
#ifdef __WIN32__
    SleepEx(DWORD(msec), FALSE);// Sleep() crashes on mingw (I do not know why), this is why I use SleepEx() here.
#else //pthreads
    if(msec == 0){
        pthread_yield();
    }else{
        usleep(msec*1000);
    }
#endif
};



ting::Exc::Exc(const char* message) throw(std::bad_alloc){
    if(message==0)
        message = "unknown exception";
    
    int len = strlen(message);
    this->msg = new char[len+1];
    memcpy(this->msg, message, len);
    this->msg[len] = 0;//null-terminate
};

ting::Exc::~Exc()throw(){
    delete[] this->msg;
};



Mutex::Mutex(){
#ifdef __WIN32__
    InitializeCriticalSection(&CastToMutex(this->mut));
#else //pthread
    pthread_mutex_init(&CastToMutex(this->mut), NULL);
#endif
};

Mutex::~Mutex(){
#ifdef __WIN32__
    DeleteCriticalSection(&CastToMutex(this->mut) );
#else //pthread
    pthread_mutex_destroy(&CastToMutex(this->mut));
#endif
};

void Mutex::Lock(){
#ifdef __WIN32__
    EnterCriticalSection(&CastToMutex(this->mut));
#else //pthread
    pthread_mutex_lock(&CastToMutex(this->mut));
#endif
};

void Mutex::Unlock(){
#ifdef __WIN32__
    LeaveCriticalSection(&CastToMutex(this->mut));
#else //pthread
    pthread_mutex_unlock(&CastToMutex(this->mut));
#endif
};


Semaphore::Semaphore(uint initialValue){
//    std::cout<<"Semaphore::Semaphore(): enter"<<std::endl;
#ifdef __WIN32__
    CastToSemaphore(this->sem) = CreateSemaphore(NULL, initialValue, 0xffffff, NULL);
    if(CastToSemaphore(this->sem) == NULL){
//        std::cout<<"Semaphore::Semaphore(): failed"<<std::endl;
        throw ting::Exc("Semaphore::Semaphore(): creating semaphore failed");
    }
#else //pthread
    if( sem_init(&CastToSemaphore(this->sem), 0, initialValue) < 0 ){
        throw ting::Exc("Semaphore::Semaphore(): creating semaphore failed");
    }
#endif
//    std::cout<<"Semaphore::Semaphore(): exit"<<std::endl;
};

Semaphore::~Semaphore(){
#ifdef __WIN32__
    CloseHandle(CastToSemaphore(this->sem));
#else //pthread
    sem_destroy(&CastToSemaphore(this->sem));
#endif
};

bool Semaphore::Wait(uint timeoutMillis){
//    std::cout<<"Semaphore::Wait(): enter"<<std::endl;
#ifdef __WIN32__
    switch( WaitForSingleObject(CastToSemaphore(this->sem), DWORD(timeoutMillis==0?INFINITE:timeoutMillis)) ){
        case WAIT_OBJECT_0:
//            std::cout<<"Semaphore::Wait(): exit"<<std::endl;
            return true;
        case WAIT_TIMEOUT:
            return false;
            break;
        default:
            throw ting::Exc("Semaphore::Wait(): wait failed");
            break;
    }
#else //pthread
    if(timeoutMillis == 0){
        int retVal;
        do{
            retVal = sem_wait(&CastToSemaphore(this->sem));
        }while(retVal == -1 && errno == EINTR);
        if(retVal < 0){
            throw ting::Exc("Semaphore::Wait(): wait failed");
        }
    }else{
        //TODO: implement use sem_timedwait
        throw ting::Exc("Semaphore::Wait(): unimplemented");
    }
#endif
    return false;
};

void Semaphore::Post(){
//    std::cout<<"Semaphore::Post(): enter"<<std::endl;
#ifdef __WIN32__
    if( ReleaseSemaphore(CastToSemaphore(this->sem), 1, NULL) == 0 ){
        throw ting::Exc("Semaphore::Post(): releasing semaphore failed");
    }
#else //pthread
    if(sem_post(&CastToSemaphore(this->sem)) < 0){
        throw ting::Exc("Semaphore::Post(): releasing semaphore failed");
    }
#endif
//    std::cout<<"Semaphore::Post(): exit"<<std::endl;
};



static inline Mutex*& CVMut(ting::CondVar* cv){
    M_TING_STATIC_ASSERT(sizeof(cv->cond) >= sizeof(Mutex*) + 2*sizeof(Semaphore*) + 2*sizeof(ting::uint));
    return *reinterpret_cast<Mutex**>(&(cv->cond));
};

static inline Semaphore*& CVSemWait(ting::CondVar* cv){
    return *reinterpret_cast<Semaphore**>( 
                reinterpret_cast<ting::byte*>(&(cv->cond)) + sizeof(Mutex*)
            );
};

static inline Semaphore*& CVSemDone(ting::CondVar* cv){
    return *reinterpret_cast<Semaphore**>( 
                reinterpret_cast<ting::byte*>(&(cv->cond)) + sizeof(Mutex*) + sizeof(Semaphore*)
            );
};

static inline ting::uint& CVNumWaiters(ting::CondVar* cv){
    return *reinterpret_cast<ting::uint*>( 
                reinterpret_cast<ting::byte*>(&(cv->cond)) + sizeof(Mutex*) + 2*sizeof(Semaphore*)
            );
};

static inline ting::uint& CVNumSignals(ting::CondVar* cv){
    return *reinterpret_cast<ting::uint*>( 
                reinterpret_cast<ting::byte*>(&(cv->cond)) + sizeof(Mutex*) + 2*sizeof(Semaphore*) + sizeof(ting::uint)
            );
};

CondVar::CondVar(){
#ifdef __WIN32__
    CVMut(this) = new Mutex();
    CVSemWait(this) = new Semaphore();
    CVSemDone(this) = new Semaphore();
    CVNumWaiters(this) = 0;
    CVNumSignals(this) = 0;
#else //pthread
    pthread_cond_init(&CastToCondVar(this->cond), NULL);
#endif
};

CondVar::~CondVar(){
#ifdef __WIN32__
    delete CVMut(this);
    delete CVSemWait(this);
    delete CVSemDone(this);
#else
    pthread_cond_destroy(&CastToCondVar(this->cond));
#endif
};

void CondVar::Wait(Mutex &mutex){
#ifdef __WIN32__
    CVMut(this)->Lock();
    ++CVNumWaiters(this);
    CVMut(this)->Unlock();
    
    mutex.Unlock();
    
//    std::cout<<"CondVar::Wait(): going to wait"<<std::endl;
    
    CVSemWait(this)->Wait();
    
    CVMut(this)->Lock();
    if(CVNumSignals(this) > 0){
        CVSemDone(this)->Post();
        --CVNumSignals(this);
    }
    --CVNumWaiters(this);
    CVMut(this)->Unlock();
    
    mutex.Lock();
#else
    pthread_cond_wait(&CastToCondVar(this->cond), &CastToMutex(mutex.mut));
#endif
};

void CondVar::Notify(){
#ifdef __WIN32__
    CVMut(this)->Lock();
    
    if(CVNumWaiters(this) > CVNumSignals(this)){
        ++CVNumSignals(this);
//        std::cout<<"CondVar::Notify(): going to post"<<std::endl;
        CVSemWait(this)->Post();
        CVMut(this)->Unlock();
        CVSemDone(this)->Wait();
    }else{
        CVMut(this)->Unlock();
    }
#else
    pthread_cond_signal(&CastToCondVar(this->cond));
#endif
};

ting::u32 ting::GetTicks(){
#ifdef __WIN32__
    //TODO: use QueryPerformanceCounter() ???
    return GetTickCount();
#else
    clock_t ticks = clock();
    u64 msec = u64(ticks) * 1000 / CLOCKS_PER_SEC;
    return u32(msec & u32(-1));
#endif
};


void Queue::PushMessage(std::auto_ptr<Message> msg){
    {
        Mutex::LockerUnlocker mutexLockerUnlocker(this->mut);
        if(this->first){
//            M_ASSERT(this->last)
//            M_ASSERT(this->last->next == 0)
            this->last = this->last->next = msg.release();
//            M_ASSERT(this->last->next == 0)
        }else{
//            M_ASSERT(msg.IsValid())
            this->last = this->first = msg.release();
        }
    }
    this->cond.Notify();
};

std::auto_ptr<Message> Queue::PeekMsg(){
    Mutex::LockerUnlocker mutexLockerUnlocker(this->mut);
    if(this->first){
        Message* ret = this->first;
        this->first = this->first->next;
        return std::auto_ptr<Message>(ret);
    }
    
    return std::auto_ptr<Message>();
};

std::auto_ptr<Message> Queue::GetMsg(){
    Mutex::LockerUnlocker mutexLockerUnlocker(this->mut);
    if(this->first){
        Message* ret = this->first;
        this->first = this->first->next;
        return std::auto_ptr<Message>(ret);
    }
    
    this->cond.Wait(this->mut);
    
//    M_ASSERT(this->first)
    
    Message* ret = this->first;
    this->first = this->first->next;
    return std::auto_ptr<Message>(ret);
};

Queue::~Queue(){
    //M_DEBUG_TRACE(<<"C_Queue::~C_Queue(): enter"<<std::endl)
    Mutex::LockerUnlocker mutexLockerUnlocker(this->mut);
    Message *msg = this->first,
            *nextMsg;
    while(msg){
        //M_DEBUG_TRACE(<<"C_Queue::~C_Queue(): msg="<<msg<<std::endl)
        nextMsg = msg->next;
        //M_DEBUG_TRACE(<<"C_Queue::~C_Queue(): nextMsg="<<nextMsg<<std::endl)
        delete msg;
        msg = nextMsg;
    }
};

void QuitMessage::Handle(){
    this->thr->quitFlag = true;
};