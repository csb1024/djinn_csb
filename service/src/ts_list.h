#ifndef LIST
#define LIST
#include <list>
#include <vector>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include "request.h"

using namespace std;


template <class T> 
class TSList
{
public:
TSList();
~TSList();
void pushBack(T &t);
void popFront();
unsigned int getLength();
bool isEmpty();
T* at(const int index);
T* getFront();
void eraseElement(int index);

private: 
vector<T*> _list;
mutex mtx;
};

template <class T> TSList<T>::TSList(){
}

template <class T> TSList<T>::~TSList(){
}

template<class T> void TSList<T>::eraseElement(int index){
//check if index is in right range, return null otherwise
lock_guard<mutex> lock(mtx);
if (index <0 || index >= _list.size()){ 
#ifdef DEBUG
fprintf(stderr, "index %d out of range for eraseElement()\n", index);
#endif
return;
}
_list.erase(_list.begin()+index);
}

template <class T> void TSList<T>::pushBack(T &t)
{
lock_guard<mutex> lock(mtx);
_list.push_back(&t);
}

template <class T> unsigned int TSList<T>::getLength(){
lock_guard<mutex> lock(mtx);
return _list.size();
}


template <class T> bool TSList<T>::isEmpty(){
return _list.empty();
}

template <class T> T* TSList<T>::getFront(){
lock_guard<mutex> lock(mtx);
return  _list.front();
}
template <class T> T* TSList<T>::at(int index){
//check if index is in right range, return null otherwise
lock_guard<mutex> lock(mtx);
if (index <0 || index >= _list.size()){ 
#ifdef DEBUG
fprintf(stderr, "index %d out of range for at()\n", index);
#endif 
return NULL;
}
return  _list.at(index);
}



#endif 
