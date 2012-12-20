/* 
 * File:   io.h
 * Author: Dream
 *
 * Created on 2012年12月8日, 下午 10:57
 */

#ifndef IO_H
#define	IO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "http.h"
  int ex_sock_nread(SOCKET sock, char *buf, size_t bufsize, size_t n);
  int ex_read_head(SOCKET sock, char *buf, size_t bufsize);
  int ex_sock_nwrite(SOCKET sock, char *buf, size_t n);
  int writeFileStream(const char * filePath, char * buf, size_t filesize);
  int sendFileStream(const ExHttp *pHttp, const char *filePath);
  int ex_load_body(ExHttp *pHttp);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */

