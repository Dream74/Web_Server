#include <stdio.h>
#include <assert.h>
#include "request.h"
#include "analysis.h"
#include "cgi.h"

int cgi_page_sum(ExHttp *pHttp)
{
  const char *lAdd, *rAdd ;
  int sum ;
  char buf[32] ;
  printf( "\n--add.cgi--\n" ) ;

  print_param( pHttp ) ;
  lAdd = get_param_info( pHttp, "lAdd" ) ;
  rAdd = get_param_info( pHttp, "rAdd" ) ;
  sum = atoi( lAdd )+atoi( rAdd ) ;

  sprintf( buf, "%d", sum ) ;
  ex_send_msg( pHttp, NULL, buf, strlen( buf ) ) ;
  return 0 ;
}

int cgi_page_txt(ExHttp *pHttp)
{
  printf( "\n--txt.cgi--\n" ) ;
  print_param( pHttp ) ;
  ex_send_file( pHttp, "hello.txt" ) ;

  return 0 ;
}

int cgi_page_login(ExHttp *pHttp)
{
  const char *smsg = "login success" ;
  const char *emsg = "login error" ;
  static const char *user = "eserv" ;
  static const char *passwd = "passwd" ;

  const char *pRet = emsg ;

  const char *pUser, *pPasswd ;
  printf( "\n--login.cgi--\n" ) ;
  print_param( pHttp ) ;

  pUser = get_param_info( pHttp, "user" ) ;
  pPasswd = get_param_info( pHttp, "passwd" ) ;
  if ( strcmp( user, pUser )==0&&strcmp( passwd, pPasswd )==0 ) {
    pRet = smsg ;
  }
  ex_send_msg( pHttp, NULL, pRet, strlen( pRet ) ) ;

  return 0 ;
}

int cgi_page_gallery(ExHttp *pHttp)
{
  static int count = 0 ;
  char buf[40] ;
  printf( "\n--gallery.cgi--\n" ) ;

  print_param( pHttp ) ;

  count++ ;
  if ( count>3 ) {
    sprintf( buf, "All of pictures are shown. Reset" ) ;
    count = 0 ;
  }
  else {
    sprintf( buf, "No. %d<br /><img src='%d.jpg' />", count, count ) ;
  }
  ex_send_msg( pHttp, NULL, buf, strlen( buf ) ) ;
  return 0 ;
}


cgi_page cgi_pages[] = {
  {
    .name = "sum.cgi",
    .callback = cgi_page_sum,
  },

  {
    .name = "txt.cgi",
    .callback = cgi_page_txt,
  },

  {
    .name = "login.cgi",
    .callback = cgi_page_login,
  },

  {
    .name = "gallery.cgi",
    .callback = cgi_page_gallery,
  },
} ;

void cgi_init()
{
  size_t i ;
  for ( i = 0 ; i < sizeof (cgi_pages)/sizeof (cgi_page) ; i++ )
    cgi_page_add( cgi_pages[i].name, cgi_pages[i].callback ) ;
}

void cgi_uninit()
{
}

int cgi_handler(ExHttp *pHttp, void *handle)
{
  int (*pf)(ExHttp *) = handle ;

  return pf( pHttp ) ;
}

int errorLog(ExHttp *pHttp, const char *mess)
{
  assert( pHttp ) ;
  printf( "%s\n", mess ) ;
  return 0 ;
}
