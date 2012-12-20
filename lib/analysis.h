/* 
 * File:   analysis.h
 * Author: Dream
 *
 * Created on 2012�~12��9��, �W�� 1:01
 */

#ifndef ANALYSIS_H
#define	ANALYSIS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "http.h"
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

