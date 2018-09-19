#ifndef QUEUE
#define QUEUE
#include <queue>
#include <deque>
#include <vector>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include "request.h"

using namespace std;


template <class T> 
class TSQueue
{
public:
TSQueue();
~TSQueue();
void pushBack(T &t);
void popFront();
unsigned int getLength();
bool isEmpty();
T* at(const int index);
T* getFront();
void eraseElement(int index);

private: 
deque<T*> _queue;
mutex mtx;
};

template <class T> TSQueue<T>::TSQueue(){
}

template <class T> TSQueue<T>::~TSQueue(){
}

template<class T> void TSQueue<T>::eraseElement(int index){
//check if index is in right range, return null otherwise
lock_guard<mutex> lock(mtx);
if (index <0 || index >= _queue.size()){ 
#ifdef DEBUG
fprintf(stderr, "index %d out of range for eraseElement()\n", index);
#endif
return;
}
_queue.erase(_queue.begin()+index);
}

template <class T> void TSQueue<T>::pushBack(T &t)
{
lock_guard<mutex> lock(mtx);
_queue.push_back(&t);
}

template <class T> unsigned int TSQueue<T>::getLength(){
lock_guard<mutex> lock(mtx);
return _queue.size();
}


template <class T> bool TSQueue<T>::isEmpty(){
return _queue.empty();
}

template <class T> T* TSQueue<T>::getFront(){
lock_guard<mutex> lock(mtx);
return  _queue.front();
}

template <class T> void TSQueue<T>::popFront(){
lock_guard<mutex> lock(mtx);
_queue.pop_front();
}
template <class T> T* TSQueue<T>::at(int index){
//check if index is in right range, return null otherwise
lock_guard<mutex> lock(mtx);
if (index <0 || index >= _queue.size()){ 
#ifdef DEBUG
fprintf(stderr, "index %d out of range for at()\n", index);
#endif 
return NULL;
}
return  _queue.at(index);
}



#endif 
