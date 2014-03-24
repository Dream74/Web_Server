#include <string.h>
#include <time.h>

#include "request.h"
#include "analysis.h"
#include "misc.h"
#include "io.h"
#include "cgi.h"

/* set MIME type, content length, and cache control like expire and
 * last-modified
 */
static int fileSet(char *buf, const ExHttp *pHttp)
{
  char date[35] ;
  strftime( date, 35, "%a, %d %b %Y %X GMT", localtime( &(pHttp->st.st_ctime) ) ) ;
  printf( "%s\n", date ) ;
  char expireDate[35] ;
  time_t expDate = time( NULL ) + ( time_t ) ExpireTime ;
  strftime( expireDate, 35, "%a, %d %b %Y %X GMT", localtime( &expDate ) ) ;

  return sprintf( buf, "Content-Type: %s\n"
          "Content-Length: %d\n"
          "Cache-Control: max-age=%ld\n"
          "Last-Modified: %s\n"
          "expire: %s\n"
          "ETag: %lx.%lx\n",
          get_mime_type( pHttp->url ),
          ( int ) pHttp->st.st_size,
          ( long int ) ExpireTime,
          date,
          expireDate,
          pHttp->st.st_size,
          pHttp->st.st_mtime
          ) ;
}

static int setCookie(char * buf, const ExHttp * pHttp)
{
  static size_t login_num = 0 ;
  const char * hasLogin = get_head_info( pHttp, "Cookie" ) ;
  hasLogin = (hasLogin==NULL) ? "No" : "Yes" ;

  return sprintf( buf, "Set-Cookie: loginNum=%d; YesLogin=%s\n"
          , ++login_num, hasLogin ) ;
}

const char *get_head_info(const ExHttp *pHttp, const char *key)
{
  return ex_hash_find( &(pHttp->headMap), key ) ;
}

const char *get_param_info(const ExHttp *pHttp, const char *key)
{
  return ex_hash_find( &(pHttp->paramMap), key ) ;
}

static int sendHead(const ExHttp *pHttp, char *pBuf, size_t len)
{
  size_t nLen ;

  nLen = sprintf( pBuf+len, "Server: " SERVER "\n\n" ) ;
  return ex_sock_nwrite( pHttp->sock, pBuf, len+nLen ) ;
}

static int codeSet(char *pBuf, int code)
{
  const char *c100 = "Continue" ;
  const char *c200 = "OK" ;
  const char *c201 = "Created" ;
  const char *c204 = "No Content" ;
  const char *c206 = "Partial content" ;
  const char *c304 = "Not Modified" ;
  const char *c301 = "Moved Permanently" ;
  const char *c400 = "Bad Request " ;
  const char *c404 = "Not Found" ;
  const char *c412 = "Precondition Failed" ;
  const char *c501 = "Not Implemented" ;
  const char *c505 = "HTTP Version Not Supported" ;

  const char *msg = NULL ;
  switch ( code ) {
    case 100:
      msg = c100 ;
      break ;
    case 200:
      msg = c200 ;
      break ;
    case 201:
      msg = c201 ;
      break ;
    case 204:
      msg = c204 ;
      break ;
    case 206:
      msg = c206 ;
      break ;
    case 301:
      msg = c301 ;
      break ;
    case 304:
      msg = c304 ;
      break ;
    case 400:
      msg = c400 ;
      break ;
    case 404:
      msg = c404 ;
      break ;
    case 412:
      msg = c412 ;
      break ;
    case 501:
      msg = c501 ;
      break ;
    case 505:
      msg = c505 ;
      break ;
    default:
      break ;
  }
  return sprintf( pBuf, "HTTP/1.1 %d %s\n", code, msg ) ;
}

int ex_error_reply(const ExHttp *pHttp, int stscode)
{
  char buf[128] ;
  char *pBuf = buf ;

  pBuf += codeSet( pBuf, stscode ) ;
  return sendHead( pHttp, buf, pBuf-buf ) ;
}

static void clearHttp(ExHttp *pHttp)
{
  ex_hash_clear( &(pHttp->headMap) ) ;
  ex_mpool_clear( &(pHttp->mp) ) ;
}

static int staticProcess(const ExHttp *pHttp)
{
  char buf[BUFSIZ] ;
  char *pBuf = buf ;
  int ret = 0 ;

  int code = cacheCheck( pHttp ) ;
  pBuf += codeSet( pBuf, code ) ;
  if ( code==200 ) {
    pBuf += fileSet( pBuf, pHttp ) ;
  }

  if ( strcmp( get_mime_type( pHttp->url ), "image/png" )!=0 )
    pBuf += setCookie( pBuf, pHttp ) ;

  do {
    if ( code==206 ) {
      char rangeTemp[128] ;
      char * range = ( char * ) get_head_info( pHttp, "Range" ) ;
      strncpy( ( char * ) &rangeTemp, range, 128 ) ;
      range = ( char * ) &rangeTemp ;
      SKIP( &range, '=' ) ;
      char * sRange = range ;
      SKIP( &range, '-' ) ;

      char date[35] ;
      strftime( date, 35, "%a, %d %b %Y %X GMT", localtime( &(pHttp->st.st_ctime) ) ) ;

      int startRange = (*sRange=='\0') ? -1 : atoi( sRange ) ;
      int endRange = (*range=='\0') ? -1 : atoi( range ) ;
			// printf("start :%d end :%d\n",startRange,endRange ) ;
      if ( startRange<0 ) {
        startRange = pHttp->st.st_size-endRange ;
        endRange = pHttp->st.st_size ;
      }
      else if ( endRange<0 ) {
        endRange = pHttp->st.st_size ;
      }
      pBuf += sprintf( pBuf, "connection: close\n"
              "Last-Modified: %s\n"
              "Accept-Ranges: bytes\n"
              "Content-Length: %d\n"
              "Content-Range: bytes %d-%d/%d\n",
              date,
              endRange-startRange,
              startRange,
              endRange,
              pHttp->st.st_size
              ) ;
      if ( (ret = sendHead( pHttp, buf, pBuf-buf ))<0 )
        break ;
      ret = sendFileRangeStream( pHttp, pHttp->url, startRange, endRange + 1 ) ;
      break ;
    }

    if ( (ret = sendHead( pHttp, buf, pBuf-buf ))<0 )
      break ;

    if ( code==304|| code == 412 || 'H'== *(pHttp->method) )
      break ;
    ret = sendFileStream( pHttp, pHttp->url ) ;
  } while ( 0 ) ;
  return ret ;
}

static int cgiProcess(ExHttp *pHttp)
{
  int ret = 0 ;
  void *handle = NULL ;
  do {
    if ( (handle = cgi_page_find( pHttp->url ))==NULL ) {
      errorLog( pHttp, "cgi page not find " ) ;
      ret = -1 ;
      break ;
    }
    if ( strcmp( "POST", (pHttp->method) )==0&&ex_load_body( pHttp )<0 ) {
      errorLog( pHttp, "body check error" ) ;
      ret = -2 ;
      break ;
    }
    if ( decodeParam( pHttp )<0 ) {
      errorLog( pHttp, "param decode error" ) ;
      ret = -3 ;
      break ;
    }
    if ( cgi_handler( pHttp, handle )<0 ) {
      errorLog( pHttp, "handler error" ) ;
      ret = -4 ;
    }
  } while ( 0 ) ;

  if ( strcmp( (pHttp->method), "POST" )==0&&
          pHttp->postData!=pHttp->curPos )
    free( pHttp->postData ) ;

  ex_hash_clear( &(pHttp->paramMap) ) ;
  return ret ;
}

int putProcess(const ExHttp * pHttp, int rType)
{
  static const char * c100 = "HTTP/1.1 100 Continue\n\n" ;
  size_t size ;
  int code ;
  const char * csize = get_head_info( pHttp, "Content-Length" ) ;
  size = csize==NULL ? 0 : atoi( csize ) ;
  char buf[size] ;
  char *pBuf = buf ;
  int ret = 0 ;
  ex_sock_nwrite( pHttp->sock, ( char * ) c100, strlen( c100 ) ) ;
  ex_sock_nread( pHttp->sock, ( char * ) &buf, sizeof (buf), size ) ;
  switch ( rType ) {
    case 0: /* static file */
    case 1: /* dyanamic page: '.cgi' extension */
      code = 200 ;
      writeFileStream( pHttp->url, ( char * ) &buf, size ) ;
      break ;
    case 404:
      code = 201 ;
      writeFileStream( pHttp->url, ( char * ) &buf, size ) ;
      break ;
    default:
      break ;
  }
  pBuf += codeSet( pBuf, code ) ;
  pBuf += sprintf( pBuf, "Connection: close\n" ) ;
  ret = sendHead( pHttp, buf, pBuf-buf ) ;
  return -1 ;
}

int deleteProcess(const ExHttp * pHttp, int rType)
{
  char buf[BUFSIZ] ;
  char *pBuf = buf ;
  int code ;
  switch ( rType ) {
    case 0: /* static file */
    case 1: /* dyanamic page: '.cgi' extension */
      if ( remove( pHttp->url )==0 )
        code = 200 ;
      break ;
  }

  pBuf += codeSet( pBuf, code ) ;
  strcat( pBuf, "\n\n" ) ;
  sendHead( pHttp, buf, pBuf-buf ) ;
  return -1 ;
}

static int replyHandler(ExHttp *pHttp)
{
  int rType ;
  int ret = -1 ;

  rType = checkpath( pHttp ) ;

  if ( strcmp( pHttp->method, "PUT" )==0 )
    ret = putProcess( pHttp, rType ) ;
  else if ( *pHttp->method=='D' )
    ret = deleteProcess( pHttp, rType ) ;
  else {
    switch ( rType ) {
      case 0: /* static file */
        ret = staticProcess( pHttp ) ;
        break ;
      case 1: /* dyanamic page: '.cgi' extension */
        ret = cgiProcess( pHttp ) ;
        break ;
      default:
        ex_error_reply( pHttp, rType ) ;
    }
  }
  clearHttp( pHttp ) ;
  return ret ;
}

void requestHandler(void * s)
{
  SOCKET sock = ( SOCKET ) s ;
  char recvBuf[MAX_HEADER_SIZE] ;
  char pool[512] ;
  /*
    int recvlen ;
   */
  ExHttp httpInfo ;
  ++ExContext.threadCnt ;
  httpInfo.sock = sock ;

  ex_mpool_init( &httpInfo.mp, pool, sizeof (pool) ) ;
  do {
    if ( ExContext.quitFlag ) break ;

    httpInfo.recvLen = ex_read_head( sock, recvBuf, MAX_HEADER_SIZE ) ;

    if ( httpInfo.recvLen<=0 ) break ;

    httpInfo.curPos = recvBuf ;
    recvBuf[httpInfo.recvLen] = '\0' ;
    printf( "%s", recvBuf ) ;
    // strcat(recvBuf + httpInfo.recvLen , skipBrake);
    /* if method is not implemented */
    if ( checkmethod( &httpInfo )<0 ) {
      DBG( "len: %s", httpInfo.method ) ;
      ex_error_reply( &httpInfo, 400 ) ;
      errorLog( &httpInfo, "parseURL error" ) ;
      break ;
      // ex_error_reply( &httpInfo, 501 ) ;
      // break ;
    }

    if ( parseURL( &httpInfo )<0 ) {
      DBG( "url: %s protocol %s", httpInfo.url, httpInfo.protocol ) ;
      ex_error_reply( &httpInfo, 400 ) ;
      errorLog( &httpInfo, "parseURL error" ) ;
      break ;
    }

    if ( strcmp( httpInfo.protocol, "HTTP/1.1" )!=0 ) {
      ex_error_reply( &httpInfo, 505 ) ;
      errorLog( &httpInfo, "HTTP Version Not Supported" ) ;
      break ;
    }


    /* if parse head error */
    if ( parseHeader( &httpInfo )<0 ) {
      ex_error_reply( &httpInfo, 400 ) ;
      /* bad Request */
      errorLog( &httpInfo, "parse head error" ) ;
      clearHttp( &httpInfo ) ;
      break ;
    }

    if ( strcmp( httpInfo.url, "/index.jsp" )==0 ) {
      ex_error_reply( &httpInfo, 301 ) ;
      const char * method = httpInfo.method ;
      char * message = "Location: http://127.0.0.1/index.html" ;
      size_t size = strlen( message ) ;
      if ( *method=='G'|| *method=='H' ) {
        ex_sock_nwrite( httpInfo.sock, message, size ) ;
      }
      errorLog( &httpInfo, "Moved Permanently" ) ;
      break ;
    }

    /* if reply error */
    if ( replyHandler( &httpInfo )<0 ) {
      break ;
    }

  } while ( 1 ) ;

  closesocket( sock ) ;
  --ExContext.threadCnt ;
}

int ex_send_file(ExHttp *pHttp, const char *filePath)
{
  char buf[BUFSIZ] ;
  char *pBuf = buf ;
  int ret = 0 ;
  pBuf += codeSet( pBuf, 200 ) ;
  pHttp->url = ( char * ) filePath ;
  stat( filePath, &pHttp->st ) ;
  pBuf += fileSet( pBuf, pHttp ) ;
  do {
    if ( (ret = sendHead( pHttp, buf, pBuf-buf ))<0 )
      break ;

    if ( (ret = sendFileStream( pHttp, filePath ))<0 )
      break ;
  } while ( 0 ) ;
  return ret ;
}

static int typeSet(char *buf, const char *type)
{
  return sprintf( buf, "Content-Type: %s\n", get_mime_type( type ) ) ;
}

static int lengthSet(char *buf, int len)
{
  return sprintf( buf, "Content-Length: %d\n", len ) ;
}

int ex_send_msg(ExHttp *pHttp, const char *type, const char *buf, size_t len)
{
  char hBuf[BUFSIZ] ;
  char *pBuf = hBuf ;
  int ret ;
  pBuf += codeSet( pBuf, 200 ) ;
  pBuf += typeSet( pBuf, type ) ;
  pBuf += lengthSet( pBuf, len ) ;

  do {
    if ( (ret = sendHead( pHttp, hBuf, pBuf-hBuf ))<0 )
      break ;
    if ( (ret = ex_sock_nwrite( pHttp->sock, ( char * ) buf, len ))<0 )
      break ;
  } while ( 0 ) ;
  return ret ;
}
