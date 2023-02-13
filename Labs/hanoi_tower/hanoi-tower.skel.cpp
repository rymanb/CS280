#include "hanoi-tower.h"

#include <sstream>      // std::stringstream
#include <iomanip>      // left,right,fill,setw - see handout

template< typename CALLBACK >
void hanoi_tower_1_rec( ... ) 
{
    std::stringstream ss;

    if ( n == 1 ) {
        ...
    }

    ...
    ss << ...; // your message
    cb( ss.str() ); // callback
    ...
}

template< typename CALLBACK >
void hanoi_tower_1( int n, CALLBACK cb ) 
{
    hanoi_tower_1_rec( .... );
}

////////////////////////////////////////////////////////////////////////////////
template< typename CALLBACK >
void hanoi_tower_2( int n, CALLBACK cb ) 
{
    hanoi_tower_1( n, cb ); // if not doing extra credit just leave it like this
}

////////////////////////////////////////////////////////////////////////////////
template< typename CALLBACK >
void hanoi_tower_3( int n, CALLBACK cb ) 
{
    hanoi_tower_1( n, cb ); // if not doing extra credit just leave it like this
}
