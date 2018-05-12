#include "MemoryPool.h"

namespace robot {

//init static members
char * MemoryPool::start_free = 0;
char * MemoryPool::end_free = 0;
size_t MemoryPool::heap_size = 0;
MemoryPool::obj * volatile MemoryPool::free_list[nFreeLists] = {0, 0, 0, 0, 0, 0, 0, 0, 
                                                                0, 0, 0, 0, 0, 0, 0, 0,
                                                                0, 0, 0, 0};
pthread_mutex_t MemoryPool::pmutex = PTHREAD_MUTEX_INITIALIZER;

//function
MemoryPool::MemoryPool() {}
MemoryPool::~MemoryPool() {}

size_t MemoryPool::RoundUp(size_t bytes){ 
    return (((bytes) + align - 1) & ~(align - 1)); 
}

size_t MemoryPool::FreeListIndex(size_t bytes){ 
    return (((bytes) + align - 1)/align - 1);
}
 
void * MemoryPool::Allocate(size_t n)
{
    if(n == 0) return NULL;

    obj * volatile * my_free_list;
    obj * result;

    if(n > (size_t)maxBytes){ 
        void *result = malloc(n);
        if (0 == result) { cerr << "out of memory" << endl; exit(1);}
        //cout << "Malloc " << n <<" bytes from heap." << endl;  
        return result;
    }

    my_free_list = free_list + FreeListIndex(n);
    lock lock_instance;
    result = *my_free_list;
    if(result == 0){
        void *r = Refill(RoundUp(n));
        //cout << "Refill get " << n << ". Round is: " << RoundUp(n) << ". Freelist is: " << FreeListIndex(n) << endl;
        return r;
    }
    *my_free_list = result -> free_list_link;
    //cout << "Freelist get " << n << " bytes." << " RoundUp is: " << RoundUp(n) << ". Freelist is: " << FreeListIndex(n) << endl;
    return (result);
}

void MemoryPool::DeAllocate(void *p, size_t n)
{
    if(p == NULL) return;

    obj * q = (obj *)p;
    obj * volatile * my_free_list;

    if(n > (size_t)maxBytes){ 
        free(p); 
        //cout << "Free " << n <<" bytes." << endl;  
        return; 
    }

    my_free_list = free_list + FreeListIndex(n);
    lock lock_instance;
    q -> free_list_link = *my_free_list;
    *my_free_list = q;

    //cout << "DeAllocate n is: " << n << ". Round Up: " << RoundUp(n) << ". FreeListIndex:" << FreeListIndex(n) << endl;
}

void * MemoryPool::Refill(size_t n)
{
    int nobjs = 20;
    char * chunk = ChunkAlloc(n, nobjs);
    obj * volatile * my_free_list;
    obj * result;
    obj * current_obj, * next_obj;
    int i;

    if(1 == nobjs) return(chunk);
    my_free_list = free_list + FreeListIndex(n);

    result = (obj *)chunk;
    *my_free_list = next_obj = (obj *)(chunk + n);
    for(i = 1; ; i++){
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if(nobjs - 1 == i){
            current_obj -> free_list_link = 0;
            break;
        } else {
            current_obj -> free_list_link = next_obj;
        }
    } 
    return(result);
}

char * MemoryPool::ChunkAlloc(size_t size, int& nobjs)
{
    char * result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;

    if(bytes_left >= total_bytes){
        result = start_free;
        start_free += total_bytes;
        return(result);
    } else if (bytes_left >= size) {
        nobjs = bytes_left/size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return(result);
    } else {
        size_t bytes_to_get = 2 * total_bytes + RoundUp(heap_size >> 4);
        if(bytes_left > 0){
            obj * volatile * my_free_list = free_list + FreeListIndex(bytes_left);
            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }
        start_free = (char *)malloc(bytes_to_get);
        //cout << "Malloc size is: " << size << ". Bytes to get:" << bytes_to_get <<"." << endl;  
        if(0 == start_free){
            int i;
            obj * volatile * my_free_list, * p;
            for(i = size; i <= maxBytes; i += align){
                my_free_list = free_list + FreeListIndex(i);
                p = *my_free_list;
                if(0 != p){
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return(ChunkAlloc(size, nobjs));
                }
            }
            end_free = 0;
            start_free = (char *)malloc(bytes_to_get);
            //cout << "malloc " << bytes_to_get <<" bytes." << endl;  
            if (0 == start_free) { cerr << "out of memory" << endl; exit(1);}
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return(ChunkAlloc(size, nobjs));
    }
}

} //end namespace

