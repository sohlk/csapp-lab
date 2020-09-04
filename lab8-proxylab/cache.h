/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/*
 * multiple cache search,one cache insert
 */
typedef struct {
    char *url;
    void *data;
    //size of data
    int size;
    //time for LRU
    time_t time;
    //lock of object time
    pthread_mutex_t lock;
} Cache_object;

typedef struct {
    Cache_object **array;
    //array len
    int len;
    //array capacity
    int cap;
    //current size of all cached data
    int size;
    //lock for entire cache
    pthread_rwlock_t lock;
} Cache;

Cache *cache_init( int len );
void cache_free( Cache *C );
//deep copy data and url
//get cache write lock
void cache_insert( Cache *C,char *url,void *data,int size );
//return data(shallow copy) or NULL if not found
//get cache write lock first,and nonblocking get object write lock
void *cache_search( Cache *C,char *url,int *size );
