/* 
 * File:   cgi.h
 * Author: Dream
 *
 * Created on 2012年12月9日, 上午 1:03
 */

#ifndef CGI_H
#define	CGI_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "http.h"

  typedef struct {
    char *name;
    int (*callback)(ExHttp * pHttp);
  } cgi_page;


  /* add your new cgi page */
  void cgi_init();

  /* may be you have some memory to free*/
  void cgi_uninit();
  /* you can write page in C/C++ */
  int cgi_handler(ExHttp *pHttp, void *handle);

  int errorLog(ExHttp *pHttp, const char *mess);

#ifdef	__cplusplus
}
#endif

#endif	/* CGI_H */

