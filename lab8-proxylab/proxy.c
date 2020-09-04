#include "csapp.h"
#include "cache.h"

#define HTTP_VER "HTTP/1.0"
#define CACHE_INIT_LEN 5

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void usage_info( char **agrv );
void *process( void *vargp );
void first_line_parser( char *buffer,char **method,char **host,char **port,char **path );
int prepare_request( char *buffer,char *method,char *host,char *path,char *protocol );

Cache *C;

int main( int argc,char **argv )
{
    //check argument
    if( argc != 2 )
    {
        usage_info(argv);
        exit(0);
    }
    //init cache
    C = cache_init(CACHE_INIT_LEN);

    //server listenfd
    int listenfd;
    listenfd = Open_listenfd(argv[1]);

    //accept connection
    struct sockaddr client_addr;
    socklen_t client_addr_len;
    int connectfd;
    int *connectfd_copy;
    pthread_t tid;
    while( 1 )
    {
        //initialize addrlen
        client_addr_len = sizeof(client_addr);
        connectfd = Accept( listenfd,&client_addr,&client_addr_len );
        //main thread may modify connectfd in stack,use connectfd_copy in heap instead
        connectfd_copy = malloc(sizeof(connectfd_copy));
        *connectfd_copy = connectfd;
        Pthread_create( &tid,NULL,process,connectfd_copy );
    }
    close(listenfd);
    cache_free(C);
    exit(0);
}

void
usage_info( char **argv )
{
    fprintf( stderr,"usage: %s <port>\n",argv[0] );
    return;
}

//client <--> proxy <--> server
void *
process( void *vargp )
{
    int clientfd = *((int *)vargp);
    free( vargp );
    Pthread_detach(Pthread_self());
    int serverfd = 0;
    rio_t client,server;
    char *client_buffer = Malloc(MAXLINE);
    char *server_buffer = Malloc(MAX_OBJECT_SIZE);
    char *url = Malloc(MAXLINE);
    int cnt;
    int allcnt;

    //init
    Rio_readinitb( &client,clientfd );

    char *method,*host,*port,*path;
    //char ver[10];
    //GET http://127.0.0.1[:xxxx]/home.html HTTP/1.1
    Rio_readlineb( &client,client_buffer,MAXLINE );
    first_line_parser( client_buffer,&method,&host,&port,&path );

    if( strcmp( method,"GET" ) != 0 )
    {
        fprintf( stderr,"%s method not supported\n",method );
        return NULL;
    }

    //is cached?
    void *obj_data;
    int obj_size;
    *url = '\0';
    strcat( url,host );
    strcat( url,":" );
    strcat( url,port );
    obj_data = cache_search( C,url,&obj_size );
    //miss
    if( obj_data != NULL )
    {
        Rio_writen( clientfd,obj_data,obj_size );
    }
    else
    {
        //send request to server
        serverfd = Open_clientfd( host,port );
        cnt = prepare_request( server_buffer,method,host,path,HTTP_VER );
        Rio_writen( serverfd,server_buffer,cnt );

        //receive response from server and send to client
        //init
        Rio_readinitb( &server,serverfd );

        allcnt = 0;
        while( 1 )
        {
            cnt = Rio_readnb( &server,server_buffer,MAX_OBJECT_SIZE );
            if( cnt == 0 )
                break;
            Rio_writen( clientfd,server_buffer,cnt );
            allcnt += cnt;
        }
        //fill into a cache
        if( cnt <= MAX_OBJECT_SIZE )
            cache_insert( C,url,server_buffer,allcnt );
    }

    free(client_buffer);
    free(server_buffer);
    free(method);
    free(host);
    free(port);
    free(path);
    close(clientfd);
    if( serverfd != 0 )
        close(serverfd);
    return NULL;
}

void
first_line_parser( char *buffer,char **method,char **host,char **port,char **path )
{
    *method = strdup(strtok( buffer," " ));
    strtok( NULL,"/" );
    *host = strtok( NULL,"/" );
    *path = strtok( NULL," " );
    //seprate port
    *port = strchr( *host,':' );
    if( *port == NULL )
    {
        *host = strdup(*host);
        *port = strdup( "80" );
    }
    else
    {
        **port = '\0';
        *host = strdup(*host);
        *port = strdup(*port + 1);
    }
    //add path leading '/'
    char *temp = Malloc( strlen(*path) + 2 );
    temp[0] = '/';
    strcpy( temp + 1,*path );
    *path = temp;
}

//return request size
int
prepare_request( char *buffer,char *method,char *host,char *path,char *protocol )
{
    int count = 0;
    count += sprintf( buffer + count,"%s %s %s\r\n",method,path,protocol );
    count += sprintf( buffer + count,"Host: %s\r\n",host );
    count += sprintf( buffer + count,"%s",user_agent_hdr );
    count += sprintf( buffer + count,"Connection: close\r\n" );
    count += sprintf( buffer + count,"Proxy-Connection: close\r\n" );
    count += sprintf( buffer + count,"\r\n" );
    return count;
}
