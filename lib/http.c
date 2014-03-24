#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "misc.h"
#include "request.h"
#include "hash.h"
#include "cgi.h"
DWORD dwThreadIdArray ;
HANDLE hThreadArray ;

const char *get_mime_type(const char *path)
{
  const char *extension ;
  const char *ret = NULL ;
  if ( path!=NULL&&
          (extension = strrchr( path, '.' ))!=NULL ) {
    ret = ex_hash_find( &ExContext.mimeMap, ++extension ) ;
  }
  return (ret!=NULL) ? ret : ("text/plain") ;
}

void ex_http_start(void)
{
  /*** Initial WSA ***/
  int WSAState ;
  WSADATA wsaData ;

  WSAState = WSAStartup( MAKEWORD( 2, 2 ), (LPWSADATA)&wsaData ) ;
  if ( WSAState ) {
    char meserr[64] ;
    sprintf( meserr, "Initial WSA Error!, error code:%d\n", WSAState ) ;
    perror( meserr ) ;
    exit( EXIT_FAILURE ) ;
  }

  /*** Create a Socket ***/
  SOCKET server ;
  server = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ;
  if ( server==INVALID_SOCKET ) {
    perror( "Error occurred in socket()" ) ;
    WSACleanup( ) ;
    exit( EXIT_FAILURE ) ;
  }

  /*** Inital host address and port ***/
  struct sockaddr_in local, cli_addr ;

  local.sin_family = AF_INET ;
  local.sin_addr.s_addr = INADDR_ANY ;
  local.sin_port = htons( SERVER_PORT ) ;

  /*** bind Socket ***/
  if ( bind( server, ( struct sockaddr* ) &local, sizeof (local) )==SOCKET_ERROR ) {
    perror( "Error occurred in bind()" ) ;
    WSACleanup( ) ;
    closesocket( server ) ;
    exit( EXIT_FAILURE ) ;
  }

  /*** Create socket listener ***/
  if ( listen( server, SERVER_BACKLOG )==SOCKET_ERROR ) {
    perror( "Error occurred in listen()" ) ;
    WSACleanup( ) ;
    closesocket( server ) ;
    exit( EXIT_FAILURE ) ;
  }

  if ( chdir( ROOTPATH )!=0 ) {
    perror( "Direct " ROOTPATH "not find" ) ;
    exit( EXIT_FAILURE ) ;
  }

  /*** Create a SOCKET for accepting incoming ***/
  SOCKET AcceptSocket ;
  printf( "#Waiting for client to connect...\n" ) ;

  int cli_size = sizeof ( cli_addr) ;
  while ( 1 ) {
    if ( ExContext.quitFlag )
      break ;

    if ( (AcceptSocket = accept( server,
            ( struct sockaddr* ) &cli_addr,
            ( int * ) &cli_size ))==INVALID_SOCKET )
      continue ;

    while ( 1 ) {
      if ( ExContext.threadCnt<EX_MAX_THREADS ) {
        printf( "#Connected from %s:%d.\n", inet_ntoa( cli_addr.sin_addr ), ntohs( cli_addr.sin_port ) ) ;
        CreateThread( NULL, 0, ( void * ) requestHandler, ( void * ) AcceptSocket, 0, NULL ) ;
        break ;
      }
      else
        ex_sleep( 50 ) ;
    }
  }
  printf( "Connected Close : %d\n", WSAGetLastError( ) ) ;

  /*** close socket ***/
  closesocket( server ) ;
  WSACleanup( ) ;
}

void* cgi_page_find(const char *pageName)
{
  return ex_hash_find( &ExContext.pageMap, pageName ) ;
}

static int ex_hashcasecmp_str(const char *s1, const char *s2)
{
  return (0==strcasecmp( s1, s2 )) ;
}

/*
 感覺寫得很 high 結果發現好像根本不是這樣弄.....
 */
void ex_init(void)
{

  int i = -1 ;
  static char pool[2048] ;
  static ex_mpool mpool ;

  ex_mpool_init( &mpool, pool, sizeof (pool) ) ;
  ex_hash_init( &ExContext.mimeMap, &mpool, 97 ) ;
  ex_hash_init( &ExContext.pageMap, &mpool, 97 ) ;
  ExContext.mimeMap.hashcmp = ( void * ) ex_hashcasecmp_str ;
  ExContext.pageMap.hashcmp = ( void * ) ex_hashcasecmp_str ;

  /* add MIME type map */
  while ( mmt[++i].ext!=NULL ) {
    ex_hash_add( &ExContext.mimeMap, mmt[i].ext, mmt[i].type ) ;
  }

  ExContext.quitFlag = 0 ;
  ExContext.threadCnt = 0 ;

  cgi_init( ) ;
  if ( (hThreadArray = CreateThread( NULL, 0, ( void * ) ex_http_start, NULL, 0, NULL ))==NULL ) {
    perror( "Create Thread Error\n" ) ;
    return ;
  }

}

void ex_sleep(unsigned int ms)
{
  // window api use ms
  Sleep( ms ) ;
}

void cgi_page_add(const char * pageName, void *f)
{
  ex_hash_add( &ExContext.pageMap, pageName, f ) ;
}

void ex_uninit(void)
{
  ExContext.quitFlag = 1 ;
#ifndef _DEBUG
  while ( ExContext.threadCnt>0 )
    ex_sleep( EX_SOCK_RTT ) ;
#endif
	
  cgi_uninit( ) ;

  ex_hash_clear( &ExContext.mimeMap ) ;
  ex_hash_clear( &ExContext.pageMap ) ;
  printf( "eServ terminated.\n" ) ;
}