/* 
 * File:   request.h
 * Author: Dream
 *
 * Created on 2012年12月8日, 下午 8:36
 */

#ifndef REQUEST_H
#define	REQUEST_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "http.h" 

  void requestHandler(void * s);
  const char *get_head_info(const ExHttp *pHttp, const char *key);
  const char *get_param_info(const ExHttp *pHttp, const char *key);
  int ex_send_file(ExHttp *pHttp, const char *filePath);
  int ex_send_msg(ExHttp *pHttp, const char *type, const char *buf, size_t len);
#ifdef	__cplusplus
}
#endif

#endif	/* REQUEST_H */

