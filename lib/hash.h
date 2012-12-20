/* 
 * File:   hash.h
 * Author: Dream
 *
 * Created on 2012年12月9日, 下午 7:26
 */

#ifndef HASH_H
#define	HASH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mempool.h" 

  typedef struct _slist {
    void *key;
    void *value;
    struct _slist *next;
  } ex_hashlist;

  typedef struct {
    ex_hashlist **buckets;
    size_t size;
    ex_mpool *mpool;
    int (*hashfun)(const void *key);
    int (*hashcmp)(const void *lkey, const void *rkey);
  } ex_hashmap;


  void ex_hash_init(ex_hashmap* hm, ex_mpool *mp, size_t _size);
  int ex_hash_add(ex_hashmap *hm, const void *key, const void *value);

  void ex_hash_clear(ex_hashmap *hm);
  void* ex_hash_find(const ex_hashmap *hm, const void *key);
#ifdef	__cplusplus
}
#endif

#endif	/* HASH_H */

