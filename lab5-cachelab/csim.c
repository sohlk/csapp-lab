#include "cachelab.h"
#include <stdio.h>
#include <errno.h>
#include <string.h> //strcpy
#include <stdlib.h> //malloc
#include <getopt.h> //getopt

typedef struct arguments_def{
    char h_flag;
    char v_flag;
    int s;
    int E;
    int b;
    char *t;
}Arguments;

int hit_sum = 0,miss_sum = 0,evict_sum = 0;

// use linked list to impliment LRU
// no valid bit,exist indicate valid
// headnode->prev = tailnode
typedef struct line_def{
    struct line_def *prev;
    struct line_def *next;
    unsigned long tag;
}Line;

// linked list need to recode length
typedef struct set_def{
    int len;
    struct line_def *head;
}Set;

// pointer array
typedef Set *Cache;

void list_remove( Set *p2set,Line *p2line );
void list_insert( Set *p2set,Line *p2line );

// exit on error
void parse( Arguments *p2args,int argc,char **argv );

// exit on error and -h
FILE* process_args( Arguments *p2args );

// run simulation and printSummary,handle -v
void run_sim( Arguments args,FILE *trace );

// search cache and optionally load cache
void exec_op( Cache cache,unsigned long addr,Arguments args );

Cache init_cache( Arguments args );

// insert at tail
// return 0 if not full before insert
// return 1 if full,need to evict head node
int cache_load( Cache cache,unsigned long addr,Arguments args );

// return 0 if hit
// return 1 if miss
// O(n),can use hash to improve effciency
int search_cache( Cache cache,unsigned long addr,Arguments args );

int main( int argc,char **argv )
{
    Arguments args = (Arguments){.h_flag = 0, .v_flag = 0, .s = 0, .E = 0, .b = 0,\
    .t = NULL};
    parse( &args,argc,argv );
    FILE *trace = process_args( &args );
    run_sim( args,trace );
    return 0;
}

void
parse( Arguments *p2args,int argc,char **argv )
{
    int opt;
    while( (opt = getopt( argc,argv,"hvs:E:b:t:" )) != -1 )
    {
        switch( opt )
        {
            case 'h':
                p2args->h_flag = 1;
                break;
            case 'v':
                p2args->v_flag = 1;
                break;
            case 's':
                p2args->s = atoi( optarg );
                break;
            case 'E':
                p2args->E = atoi( optarg );
                break;
            case 'b':
                p2args->b = atoi( optarg );
                break;
            case 't':
                p2args->t = malloc( strlen( optarg ) );
                strcpy( p2args->t,optarg );
                break;
            default:
                break;
        }
    }
    return;
}

FILE*
process_args( Arguments *p2args )
{
    FILE *trace = NULL;
    if( p2args->h_flag == 1 || p2args->s == 0 || p2args->E == 0 || \
     p2args->b == 0 || p2args->t == NULL )
    {
        printf( "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
                "Options:\n"
                  "-h         Print this help message.\n"
                  "-v         Optional verbose flag.\n"
                  "-s <num>   Number of set index bits.\n"
                  "-E <num>   Number of lines per set.\n"
                  "-b <num>   Number of block offset bits.\n"
                  "-t <file>  Trace file.\n"

                "Examples:\n"
                  "linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
                  "linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n" );
        exit( 0 );
    }
    else if( (trace = fopen( p2args->t,"r" )) == NULL )
    {
        perror( "open" );
        exit( 0 );
    }
    return trace;
}

void
run_sim( Arguments args,FILE *trace )
{
    char op;
    unsigned long addr;
    int size;
    char v = args.v_flag;

    Cache cache = init_cache( args );
    while( (fscanf( trace," %c%lx,%d",&op,&addr,&size )) != EOF )
    {
        switch( op )
        {
            case 'I':
                break;
            case 'M':
                if( v ) printf( "M %lx,%d",addr,size );
                exec_op( cache,addr,args );
                exec_op( cache,addr,args );
                printf( "\n" );
                break;
            case 'L':
                if( v ) printf( "L %lx,%d",addr,size );
                exec_op( cache,addr,args );
                printf( "\n" );
                break;
            case 'S':
                if( v ) printf( "S %lx,%d",addr,size );
                exec_op( cache,addr,args );
                printf( "\n" );
                break;
            default:
                break;
        }
    }
    printSummary( hit_sum,miss_sum,evict_sum );
    return;
}

Cache
init_cache( Arguments args )
{
    int s = args.s;
    Cache cache = malloc( (1 << s) * sizeof( *cache ) );
    int i;
    for( i = 0;i < s;i ++ )
    {
        cache[i].head = NULL;
        cache[i].len = 0;
    }
    return cache;
}

void
exec_op( Cache cache,unsigned long addr,Arguments args )
{
    char v = args.v_flag;
    if( (search_cache( cache,addr,args )) == 0 )//hit
    {
        hit_sum ++;
        if( v ) printf( " hit" );
    }
    else
    {
        miss_sum ++;
        if( v ) printf( " miss" );
        if( (cache_load( cache,addr,args )) == 1 )//eviction
        {
            evict_sum ++;
            if( v ) printf( " eviction" );
        }
    }
    return;
}

int
cache_load( Cache cache,unsigned long addr,Arguments args )
{
    int s = args.s;
    int b = args.b;
    int E = args.E;
    unsigned long tag = addr >> (s + b);
    int index = (addr >> b) & ((1 << s) - 1);
    Line *head = cache[index].head;
    Line *temp = NULL;
    if( cache[index].len == E )//evict
    {
        head->tag = tag;
        list_remove( cache + index,head );
        list_insert( cache + index,head );
        return 1;
    }
    else//add
    {
        temp = malloc( sizeof(Line) );
        temp->tag = tag;
        list_insert( cache + index,temp );
        return 0;
    }
}

int
search_cache( Cache cache,unsigned long addr,Arguments args )
{
    int s = args.s;
    int b = args.b;
    unsigned long tag = addr >> (s + b);
    int index = (addr >> b) & ((1 << s) - 1);
    Line *p2line = cache[index].head;
    while( p2line != NULL )
    {
        if( p2line->tag == tag )//hit,need update list
        {
            list_remove( cache + index,p2line );
            list_insert( cache + index,p2line );
            return 0;
        }
        else
            p2line = p2line->next;
    }
    return 1;
}

void
list_remove( Set *p2set,Line *p2line )
{
    p2set->len --;
    if( p2set->head == p2line )//head node
    {
        if( p2line->next == NULL )
            p2set->head = NULL;
        else
        {
            p2set->head = p2line->next;
            p2line->next->prev = p2line->prev;
        }
    }
    else if( p2set->head->prev == p2line )//tail node
    {
        p2set->head->prev = p2line->prev;
        p2line->prev->next = NULL;
    }
    else
    {
        p2line->prev->next = p2line->next;
        p2line->next->prev = p2line->prev;
    }
    return;
}

void
list_insert( Set *p2set,Line *p2line )
{
    p2set->len ++;
    if( p2set->head == NULL )//empty
    {
        p2set->head = p2line;
        p2line->prev = p2line;
        p2line->next = NULL;
    }
    else
    {
        p2set->head->prev->next = p2line;
        p2line->prev = p2set->head->prev;
        p2set->head->prev = p2line;
        p2line->next = NULL;
    }
    return;
}
