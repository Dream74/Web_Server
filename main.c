/* 
 * File:   main.c
 * Author: Dream
 *
 * Created on 2012年11月25日, 上午 1:14
 */
#include <string.h>
#include "lib/http.h" 

int main(void)
{
	
  ex_init( ) ;
  char buf[16] ;
  while ( scanf( "%16s", buf )>0 ) {
    if ( strncmp( "quit", buf, 4 )==0 )
      break ;
    ex_sleep( 200 ) ;
  }
  ex_uninit( ) ;
  return 0 ;
}