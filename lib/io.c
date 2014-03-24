#include <assert.h>
#include <stdio.h>
#include "io.h"
#include "request.h"
#include "misc.h"

static int ex_sock_recv(SOCKET sock, char *buf, size_t bufsize)
{
  int ret ;
  do {
    ret = recv( sock, buf, bufsize, 0 ) ;
  } while ( ret<0 ) ;
  return ret ;
}

int ex_sock_nwrite(SOCKET sock, char *buf, size_t n)
{
  size_t nLeft = n ;
  size_t nExite = 0 ;
  int ret ;
  while ( nLeft>0 ) {
    ret = send( sock, buf+nExite, nLeft, 0 ) ;
    if ( ret<=0 )
      break ;
    else {
      nLeft -= ret ;
      nExite += ret ;
    }
  }
  return nExite==n ? ( int ) n : -1 ;
}

/**
 * Head message End !?
 * @param pBuf buf index 0
 * @param cPos this recv buf start index
 * @param len   recv buf len
 * @return is  head End 
 */
static int isHeadEnd(char *pBuf, char *cPos, int len)
{
  int i = -1 ;
  while ( ++i<len ) {
    if ( '\n'== *cPos ) {
      if ( (cPos-1>=pBuf&&'\n'== *(cPos-1))||
              (cPos-2>=pBuf&&'\r'== *(cPos-1)&&'\n'== *(cPos-2))
              ) {
        return i ;
      }
    }
    ++cPos ;
  }
  return -1 ;
}

int ex_read_head(SOCKET sock, char *buf, size_t bufsize)
{
  int nRead = 0 ;
  int rsize ;
  int flag = -1 ;
  do {
    rsize = ex_sock_recv( sock, buf+nRead, bufsize-nRead ) ;

    if ( rsize<=0 ) break ;

    flag = isHeadEnd( buf, buf+nRead, rsize ) ;
    nRead += rsize ;
  } while ( flag<0 ) ;

  return (flag<0) ? -1 : nRead ;
}

int ex_sock_nread(SOCKET sock, char *buf, size_t bufsize, size_t n)
{
  size_t nRead = 0 ;
  size_t nLeft = n ;
  int ret ;
  assert( bufsize>=n ) ;
  while ( nLeft>0 ) {
    ret = ex_sock_recv( sock, buf+nRead,
            MIN( nLeft, EX_TCP_QUAN ) ) ;
    if ( ret<=0 )
      break ;
    else {
      nRead += ret ;
      nLeft -= ret ;
    }
  }
  return nRead==n ? ( int ) n : -1 ;
}

int ex_load_body(ExHttp *pHttp)
{
  int ret ;
  int cLen, rLen, sLen ;
  const char *pType, *pLen ;
  do {
    /* "x-www-form-urlencoded" */
    pType = get_head_info( pHttp, "Content-Type" ) ;
    pLen = get_head_info( pHttp, "Content-Length" ) ;
    if ( pType==NULL||pLen==NULL ) {
      ret = -1 ;
      break ;
    }

    /* content length */
    cLen = atoi( pLen ) ;
    if ( cLen<=0||cLen>MAX_POST_SIZE ) {
      ret = -2 ;
      break ;
    }
    rLen = pHttp->curPos-pHttp->method+cLen ;
    sLen = pHttp->recvLen-(pHttp->curPos-pHttp->method) ;

    DBG( "cLen, rLen, sLen: %d %d %d\n", cLen, rLen, sLen ) ;
    if ( cLen==sLen ) {
      DBG( "just ok..\n" ) ;
      pHttp->postData = pHttp->curPos ;
      ret = 1 ;
    }
    else if ( cLen>sLen ) {
      if ( cLen+pHttp->curPos-pHttp->method>MAX_HEADER_SIZE ) {
        pHttp->postData = malloc( cLen+8 ) ;
        memmove( pHttp->postData, pHttp->curPos, sLen ) ;
      }
      ret = ex_sock_nread( pHttp->sock,
              pHttp->postData+sLen,
              cLen-sLen, cLen-sLen ) ;
      if ( cLen-sLen==ret ) {
        ret = 2 ;
      }
      else
        ret = -3 ;

    }
    else {
      ret = -4 ;
    }
  } while ( 0 ) ;
  if ( ret>0 ) {
    pHttp->paramEndPos = pHttp->postData+cLen ;
    *(pHttp->paramEndPos) = '\0' ;
  }
  DBG( "contentType: %d\n", ret ) ;
  return ret ;
}

int writeFileStream(const char * filePath, char * buf, size_t filesize)
{
  FILE * fd = fopen( filePath, "wb" ) ;
  int rSize = 0 ;
  assert( fd>=0 ) ;
  rSize = fwrite( buf, 1, filesize, fd ) ;
  fclose( fd ) ;
  return rSize ;
}

int sendFileRangeStream(const ExHttp *pHttp, const char *filePath, int start, int end)
{
  // printf( "Range Start :%d Range End :%d\n", start, end ) ;

  char buf[PAGE_SIZE] ;
  int rSize ;
  int ret = 0 ;
  FILE * fd = fopen( filePath, "rb" ) ;
  assert( fd>=0 ) ;

  int fReadSum = 0 ;
  /*
   * skip before char 
   */
  int skipsum = start ;
  int readNum = (skipsum<PAGE_SIZE) ? skipsum : PAGE_SIZE ;
  while ( (rSize = fread( ( char * ) &buf, 1, readNum, fd ))>0 ) {
    fReadSum += rSize ;
    skipsum -= rSize ;
    readNum = (skipsum<PAGE_SIZE) ? skipsum : PAGE_SIZE ;
  }

  // printf( "skipNum :%d\n", fReadSum ) ;
  int readSum = end - fReadSum ;
  readNum = (readSum<PAGE_SIZE) ? readSum : PAGE_SIZE ;
  while ( (rSize = fread( ( char * ) &buf, 1, readNum, fd ))>0 ) {
    if ( (ret = ex_sock_nwrite( pHttp->sock, ( char * ) &buf, rSize ))<0 )
      break ;
    fReadSum += rSize ;
    readSum -= rSize ;
    readNum = (readSum<PAGE_SIZE) ? readSum : PAGE_SIZE ;
  }

 // printf( "readNum :%d\n", fReadSum ) ;
  fclose( fd ) ;
  return ret ;
}

int sendFileStream(const ExHttp *pHttp, const char *filePath)
{
  char buf[PAGE_SIZE] ;
  int rSize ;
  int ret = 0 ;
  FILE * fd = fopen( filePath, "rb" ) ;
  assert( fd>=0 ) ;
  while ( (rSize = fread( ( char * ) &buf, 1, sizeof (buf), fd ))>0 ) {
    if ( (ret = ex_sock_nwrite( pHttp->sock, ( char * ) &buf, rSize ))<0 )
      break ;
  }
  fclose( fd ) ;
  return ret ;
}