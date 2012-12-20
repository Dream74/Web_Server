/* 
 * File:   mempool.h
 * Author: Dream
 *
 * Created on 2012年12月9日, 下午 7:50
 */

#ifndef MEMPOOL_H
#define	MEMPOOL_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <string.h>

  typedef struct {
    char *begin; /* start pos */
    size_t len; /* capacity */
    int index; /* curIndex */
    int cflag; /* clear flag */
  } ex_mpool;

  void ex_mpool_init(ex_mpool *pmp, char *begin, size_t len);
  void *ex_mpool_malloc(ex_mpool *pmp, size_t mlen);
  void ex_mpool_free(ex_mpool *pmp, void *p);
  void ex_mpool_clear(ex_mpool *pmp);
#ifdef	__cplusplus
}
#endif

#endif	/* MEMPOOL_H */

