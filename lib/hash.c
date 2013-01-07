#include <assert.h>
#include <stdio.h>
#include "hash.h"

static int ex_hashcmp_str(const char *s1, const char *s2)
{
  return (0==strcmp( s1, s2 )) ;
}

/*
 直接使用 網路上的 BKDR hash alg 
 */
int ex_hashfun_str(char *s)
{
  unsigned int hash = 0 ;
	char * str = (char *)s ;
  while ( * str ) {
    // equivalent to: hash = 65599*hash + (*str++);
    hash = (* str++) + (hash<<6) + (hash<<16)-hash ;
  }

  return ( hash&0x7FFFFFFF) ;
}

void ex_hash_init(ex_hashmap *hm, ex_mpool *mp, size_t _size)
{
  assert( mp!=NULL ) ;
  hm->mpool = mp ;
  hm->buckets = ( ex_hashlist ** ) ex_mpool_malloc( mp,
          sizeof (int ) * _size ) ;
  memset( hm->buckets, 0, sizeof (int ) * _size ) ;
  hm->size = _size ;
  hm->hashcmp = ( void * ) ex_hashcmp_str ;
  hm->hashfun = ( void * ) ex_hashfun_str ;
}

int ex_hash_add(ex_hashmap *hm, const void *key, const void *value)
{
  int pos = hm->hashfun( key )%hm->size ;
  ex_hashlist *nhl = ( ex_hashlist * ) ex_mpool_malloc( hm->mpool,
          sizeof (ex_hashlist) ) ;

  nhl->key = ( void * ) key, nhl->value = ( void * ) value ;
  nhl->next = hm->buckets[pos] ;
  hm->buckets[pos] = nhl ;

  return pos ;
}

void* ex_hash_find(const ex_hashmap *hm, const void *key)
{
  int pos = hm->hashfun( key )%hm->size ;
  ex_hashlist *nlh = hm->buckets[pos] ;
  void *ret = NULL ;
  while ( nlh!=NULL ) {
    if ( hm->hashcmp( nlh->key, key ) ) {
      ret = nlh->value ;
      break ;
    }
    nlh = nlh->next ;
  }
  return ret ;
}

void ex_hash_clear(ex_hashmap *hm)
{
  ex_hashlist *nl, *cl ;
  size_t i ;

  do {
    if ( hm->mpool->cflag==0 )
      break ;
    for ( i = 0 ; i<hm->size ; ++i ) {
      cl = hm->buckets[i] ;
      while ( cl!=NULL ) {
        nl = cl->next ;
        ex_mpool_free( hm->mpool, cl ) ;
        cl = nl ;
      }
    }
    ex_mpool_free( hm->mpool, hm->buckets ) ;
  } while ( 0 ) ;

  memset( hm, 0, sizeof (*hm) ) ;
}