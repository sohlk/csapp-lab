#include "csapp.h"
#include "cache.h"

static void cache_insert_internal( Cache *C,int index,char *url,void *data,int size );
static void cache_delete_internal( Cache *C,Cache_object *obj );
static void cache_evict( Cache *C );
static void cache_expand( Cache *C );

Cache *
cache_init( int len )
{
    Cache *C = malloc(sizeof(*C));
    C->array = malloc(sizeof(*C->array) * len);
    C->len = 0;
    C->cap = len;
    C->size = 0;
    pthread_rwlock_init( &C->lock,NULL );

    int i;
    for( i = 0;i < len;i ++ )
        C->array[i] = NULL;
    return C;
}

void
cache_free( Cache *C )
{
    int i;
    for( i = 0;i < C->len;i ++ )
    {
        if( C->array[i] != NULL )
            cache_delete_internal( C,C->array[i] );
    }
    free(C->array);
    pthread_rwlock_destroy(&C->lock);
    free(C);
}

void
cache_insert( Cache *C,char *url,void *data,int size )
{
    //if will reach MAX_CACHE_SIZE,evict until enough to insert
    pthread_rwlock_wrlock(&C->lock);
    while( C->size + size > MAX_CACHE_SIZE )
    {
        cache_evict(C);
    }
    //need expand?
    if( C->len == C->cap )
    {
        cache_expand(C);
    }
    //find empty element
    int i;
    for( i = 0;i < C->len;i ++ )
    {
        if( C->array[i] == NULL)
            break;
    }
    cache_insert_internal( C,i,url,data,size );
    pthread_rwlock_unlock(&C->lock);
}

void *
cache_search( Cache *C,char *url,int *size )
{
    pthread_rwlock_rdlock(&C->lock);
    int i;
    for( i = 0;i < C->len;i ++ )
    {
        if( C->array[i] != NULL )
        {
            if( !strcmp( url,C->array[i]->url ) )
            {
                *size = C->array[i]->size;
                //not strict LRU
                if( !pthread_mutex_trylock(&C->array[i]->lock) )
                {
                    C->array[i]->time = time(NULL);
                    pthread_mutex_unlock(&C->array[i]->lock);
                }
                pthread_rwlock_unlock(&C->lock);
                return C->array[i]->data;
            }
        }
    }
    pthread_rwlock_unlock(&C->lock);
    return NULL;
}

static void
cache_insert_internal( Cache *C,int index,char *url,void *data,int size )
{
    Cache_object *obj = malloc(sizeof(*obj));
    obj->url = strdup(url);
    obj->data = malloc(size);
    memcpy( obj->data,data,size );
    obj->size = size;
    obj->time = time(NULL);
    pthread_mutex_init( &obj->lock,NULL );
    C->array[index] = obj;
    C->len ++;
    C->size += size;
}

static void
cache_delete_internal( Cache *C,Cache_object *obj )
{
    C->len --;
    C->size -= obj->size;
    free(obj->url);
    free(obj->data);
    pthread_mutex_destroy(&obj->lock);
    free(obj);
}

//alredy hold Cache write lock
static void cache_evict( Cache *C )
{
    int i,target;
    time_t min = 0;
    for( i = 0;i < C->len;i ++ )
    {
        if( C->array[i] != NULL )
        {
            if( min == 0 )
            {
                min = C->array[i]->time;
                target = i;
            }
            else if( min > C->array[i]->time )
            {
                min = C->array[i]->time;
                target = i;
            }
        }
    }
    cache_delete_internal( C,C->array[target] );
}

//alredy hold Cache write lock
//cap *= 2
static void
cache_expand( Cache *C )
{
    C->cap *= 2;
    C->array = realloc( C->array,sizeof(*C->array) * (C->cap) );
}
