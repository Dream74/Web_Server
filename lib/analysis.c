#include <string.h>
#include <time.h>
#include "analysis.h"
#include "misc.h"
#include "hash.h"
#include "request.h"

/*
 * 去除特殊字元
 * 0x20 之前 ascii 是 特殊字元
 */
#define TRIM_HEAD(ppbuf) \
do { \
	char *cp = *(ppbuf); \
	while (*cp <= 0x20) { \
		++cp; \
	} \
	*(ppbuf) = cp; \
} while (0) 

/* trim left-brother's tail */
#define  TRIMI_LB_TAIL(pBuf) \
do { \
	char *cp = (pBuf);\
	while (*(--cp) <=0x20){ \
		*cp = '\0'; \
	} \
} while (0)

/* trim left-brother's end pos */
static char *find_lb_end(char *pBuf) {
  while (*(--pBuf) <= 0x20)
    /* wait */;
  return ++pBuf;
}

enum http_methods {
  METHOD_HEAD = 0,
  METHOD_GET,
  METHOD_POST,
  METHOD_PUT,
  METHOD_DELETE,
  METHOD_SUPPORTED
};

static const char *methods[METHOD_SUPPORTED] = {
  [METHOD_HEAD] = "HEAD",
  [METHOD_GET] = "GET",
  [METHOD_POST] = "POST",
  [METHOD_PUT] = "PUT",
  [METHOD_DELETE] = "DELETE"
};

int checkmethod(ExHttp *pHttp) {
  char *pBuf = pHttp->curPos;
  int ret = -1;
  int i;

  pHttp->method = pBuf;
  SKIP(&pBuf, ' ');

  pHttp->curPos = pBuf;
  for (i = 0; i < METHOD_SUPPORTED; ++i) {
    if (0 == strcmp(methods[i], pHttp->method)) {
      ret = i;
      break;
    }
  }
  return ret;
}

static int ex_hashcasecmp_str(const char *s1, const char *s2) {
  return (0 == strcasecmp(s1, s2));
}

/* the first letter */
static int ex_hashfun_uchar(const char *s) {
  return (unsigned char) (*s);
}

int checkpath(ExHttp *pHttp) {
  int ret = 404;
  char *ext = NULL;
  /* Prevent SQL injection */
  do {
    if (strstr(pHttp->url, "..") != NULL) {
      DBG("may be SQL injection..");
      ret = 400;
      break;
    }

    if (*(pHttp->url) == '/')
      ++(pHttp->url);
    if (*(pHttp->url) == '\0')
      pHttp->url = (char *) INDEXFILE;

    if (stat(pHttp->url, &pHttp->st) == 0) {
      ret = 0;
    } else {
      ext = strchr(pHttp->url, '.');
      if (ext != NULL && strcmp(ext, CGIEXT) == 0) {
        /* we only use string suffixing .cgi to decide
         * cgipage
         */
        ret = 1;
      }
    }
  } while (0);

  return ret;
}

int checkURL(const char * pUrl) {
  int check = -1;

  // return check ;
  return 1;
}

int parseURL(ExHttp *pHttp) {
  /* we should treat URL carefully to prevent security issues. */
  char *ePos = NULL;
  char *pBuf = pHttp->curPos;
  TRIM_HEAD(&pBuf);
  pHttp->url = pBuf;
  SKIP(&pBuf, ' ');
  TRIMI_LB_TAIL(pBuf);

  TRIM_HEAD(&pBuf);
  pHttp->protocol = pBuf;
  SKIP(&pBuf, '\n');
  TRIMI_LB_TAIL(pBuf);

  pHttp->curPos = pBuf;
  pHttp->queryString = NULL;
  if (*(pHttp->method) == 'G') {
    /*if empty ,index file*/
    if (*(pHttp->url + 1) == '\0') {
      pHttp->url = (char *) INDEXFILE;
    } else if ((ePos = strchr(pHttp->url, '?')) != NULL) {
      *ePos = '\0';
      pHttp->queryString = ++ePos;
      pHttp->paramEndPos = find_lb_end(pHttp->protocol);
      DBG("Get Query %s", pHttp->queryString);
    }
  }

  // is always OK
  return checkURL(pHttp->url);
}

void print_param(const ExHttp *pHttp) {
  size_t i;
  ex_hashlist *cList;
  for (i = 0; i < pHttp->paramMap.size; ++i) {
    cList = pHttp->paramMap.buckets[i];
    while (cList != NULL) {
      printf("%s:%s\n", (char *) cList->key,
              (char *) cList->value);
      cList = cList->next;
    }
  }
}

int cacheCheck(const ExHttp *pHttp) {
  int retCode = 200;
  char buf[64];
  const char * oldETag = get_head_info(pHttp, "If-None-Match");
  const char * modifiedSince = get_head_info(pHttp, "If-Modified-Since");
  const char * ifMatchETag = get_head_info(pHttp, "If-Match");
  const char * ifUnmodSince = get_head_info(pHttp, "If-Unmodified-Since");
  // const char * ifRange = get_head_info( pHttp, "If-Range" ) ;
  char * range = get_head_info(pHttp, "Range");

  sprintf(buf, "%lx.%lx", pHttp->st.st_size, pHttp->st.st_mtime);
  char date[35];
  strftime(date, 35, "%a, %d %b %Y %X GMT", localtime(&(pHttp->st.st_ctime)));
  do {
    if ((ifMatchETag != NULL &&
            (strncmp(ifMatchETag, buf, 64) != 0 || strncmp(ifMatchETag, "*", 2) == 0)) ||
         ifUnmodSince != NULL && strncmp(ifUnmodSince, date, 35) != 0) {
      retCode = 412;
      break;
    }

    if (range != NULL) {
      char rangeTemp[256];
      strncpy(&rangeTemp, range, 255);
      range = (char *) &rangeTemp;
      SKIP(&range, '=');
      char * sRange = range;
      SKIP(&range, '-');
      if (atoi(sRange) < pHttp->st.st_size && atoi(range) < pHttp->st.st_size) {
        retCode = 206;
        break;
      }
      retCode = 304;
      break;
    }

    if (oldETag == NULL || modifiedSince == NULL)
      break;
    if (strncmp(oldETag, buf, 64) == 0 && strncmp(modifiedSince, date, 35) == 0)
      retCode = 304;
  } while (0);
  return retCode;
}

int parseHeader(ExHttp *pHttp) {
  char *pBuf = pHttp->curPos;
  int i = -1;
  char *key, *val;

  /* set bucket size 8 would be efficient in most cases. */
  ex_hash_init(&(pHttp->headMap), &(pHttp->mp), 8);
  pHttp->headMap.hashfun = (void *) ex_hashfun_uchar;
  pHttp->headMap.hashcmp = (void *) ex_hashcasecmp_str;

  while (*pBuf >= 0x20 && ++i < MAX_HTTP_HEADERS) {
    key = pBuf;
    SKIP(&pBuf, ':');
    TRIMI_LB_TAIL(pBuf);

    TRIM_HEAD(&pBuf);
    val = pBuf;
    SKIP(&pBuf, '\n');
    TRIMI_LB_TAIL(pBuf);

    ex_hash_add(&(pHttp->headMap), key, val);
  }


  /* if heads num exceed or SKIP run flash ,return error */
  if (++i <= MAX_HTTP_HEADERS && pBuf - pHttp->method < pHttp->recvLen) {

    /* skip the blank line if POST is used.
     * We now reach postdata-head */
    SKIP(&pBuf, '\n');
    pHttp->curPos = pBuf;
  } else {
    i = -1;
  }
  return i;
}

int decodeParam(ExHttp *pHttp) {
  static const char *skipBrake = "&=&=";
  char *pBuf, *endPos;
  char backup[8];
  char *key, *val;
  int ret = 0;

  DBG("params: \n%s\n", pHttp->postData);
  pBuf = pHttp->queryString;
  endPos = pHttp->paramEndPos;

  ex_hash_init(&(pHttp->paramMap), &(pHttp->mp), 32);

  if (pBuf == NULL || *pBuf == '\0')
    return 0;

  memcpy(backup, endPos, 4);
  memcpy(endPos, skipBrake, 4);
  do {
    key = pBuf;
    SKIP(&pBuf, '=');
    val = pBuf;
    SKIP(&pBuf, '&');

    /*
     我們取消了 前面 url decode 原本 project 支援 utf-8
    if ( url_decode( key, key, 1 )<0||
            url_decode( val, val, 1 )<0 ) {
      ret = -1 ;
      break ;
    }
     */

    /* the key should be null but val could be */
  } while (*key != '\0' &&
          ex_hash_add(&(pHttp->paramMap), key, val) >= 0 &&
          pBuf < endPos);
  memcpy(endPos, backup, 4);

  return (ret == 0 && pBuf == ++endPos) ? 1 : -1;
}