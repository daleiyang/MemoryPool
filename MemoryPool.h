#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
using namespace std;

namespace robot {

class MemoryPool{

private:
    static const int align = 8; 
    static const int maxBytes = 160;
    static const int nFreeLists = maxBytes / align; 
    static char * start_free;
    static char * end_free;
    static size_t heap_size;
    static pthread_mutex_t pmutex;

    union obj{
        union obj * free_list_link;
        char client_data[1];
    };
    static obj * volatile free_list[nFreeLists]; 

    class lock{
        public: 
            lock(){ pthread_mutex_lock(&pmutex);}
            ~lock(){ pthread_mutex_unlock(&pmutex);}
    };
    friend class lock;
 
    size_t RoundUp(size_t bytes);
    size_t FreeListIndex(size_t bytes);
    void * Refill(size_t n);
    char * ChunkAlloc(size_t size, int &nobjs);

public:
    MemoryPool();
    ~MemoryPool();
    void * Allocate(size_t n);
    void DeAllocate(void * p, size_t n);
}; 

} // end namespace
#endif

