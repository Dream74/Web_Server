/* 
 * File:   analysis.h
 * Author: Dream
 *
 * Created on 2012年12月9日, 上午 1:01
 */

#ifndef ANALYSIS_H
#define	ANALYSIS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "http.h"

/*
 *  一直讀到切到 token 這字元
 */
#define SKIP(ppBuf ,token) \
do { \
	char *cp = *(ppBuf); \
	while (*cp != (token)){ \
		++cp; \
	} \
	*cp = '\0'; \
	*(ppBuf) = ++cp; \
} while (0)

  int checkmethod(ExHttp *pHttp);
  int parseURL(ExHttp *pHttp);
  int parseHeader(ExHttp *pHttp);
  int checkpath(ExHttp *pHttp);
  int cacheCheck(const ExHttp *pHttp);
  int decodeParam(ExHttp *pHttp);
  void print_param(const ExHttp *pHttp);
#ifdef	__cplusplus
}
#endif

#endif	/* ANALYSIS_H */

