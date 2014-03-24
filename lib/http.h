/* 
 * File:   http.h
 * Author: Dream
 *
 * Created on 2012年11月27日, 下午 4:09
 */

#ifndef HTTP_H
#define	HTTP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <sys/stat.h>

#include "mempool.h"
#include "hash.h"

#define _DEBUG 

#define SERVER "Dream web server/0.1"

#define INDEXFILE "index.html"
#define ROOTPATH "www"
#define CGIEXT		".cgi"

  /* for cache usage, default is ony-day. */
#define ExpireTime 	(24 * 60 * 60)

  /* default listen port */
#define SERVER_PORT 80

  /* max pending-socket number */
#define SERVER_BACKLOG 5

  /* socket receive timeout, 500ms may be ok */
#define  EX_SOCK_RTT		500

  /* the max thread number */
#define EX_MAX_THREADS 		10

  /* we use it when read/write file */
#define PAGE_SIZE		4096

#define MAX_HEADER_SIZE (2 * 1024)	

#define MAX_POST_SIZE 		(8 * 1024)

  /* the max size we read socket once */
#define EX_TCP_QUAN 		(2 * 1024)
  /* max heads in http/1.1 support max:64.
   * but here we only support 40*/
#define	MAX_HTTP_HEADERS	40

  typedef struct {
    char *method;
    char *url;
    char *protocol;

    union {
      char *queryString; /* for GET */
      char *postData; /* for POST */
    };
    ex_mpool mp;
    ex_hashmap headMap;
    ex_hashmap paramMap;

    /* the file stat(if static file) */
    struct stat st;

    SOCKET sock;
    int recvLen;
    char *curPos;
    char *paramEndPos;
  } ExHttp;

  /*static*/
  struct {
    ex_hashmap mimeMap; // reposent mine value
    ex_hashmap pageMap;
    volatile int threadCnt;
    volatile int quitFlag;
  } ExContext;

#if !defined(MIN)
#define MIN(a ,b) ((a)<(b)?(a):(b))
#endif

#if !defined(MAX)
#define MAX(a ,b) ((a)>(b)?(a):(b))
#endif

  const char *get_mime_type(const char *path);
  void *cgi_page_find(const char *pageName);
  void cgi_page_add(const char * pageName, void *);

  // 初始建立 thread http socket 
  void ex_init(void);
  //  Sleep of ms
  void ex_sleep(unsigned int ms);
  // 關閉所有的 socket
  void ex_uninit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* HTTP_H */

