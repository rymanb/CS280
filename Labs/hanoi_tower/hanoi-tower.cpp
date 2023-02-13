#include "hanoi-tower.h"

#include <sstream>      // std::stringstream
#include <iomanip>      // left,right,fill,setw - see handout

template< typename CALLBACK >
void hanoi_tower_1_rec( int n, CALLBACK cb, std::string source, std::string dest, std::string temp ) 
{
    std::stringstream ss;

    if (n == 1)
    {
        ss << "move disk " << n << " from " << source.data() << " to " << dest. data();
        cb( ss.str() ); // callback
    }

    else
    {
        hanoi_tower_1_rec( n-1, cb, source, temp, dest); // move n-1 disks from A to B
        ss << "move disk " << n << " from " << source.data() << " to " << dest. data();
        cb( ss.str() ); // callback
        hanoi_tower_1_rec( n-1, cb, temp, dest, source); // move n-1 disks from B to C
    }
        
}

template< typename CALLBACK >
void hanoi_tower_1( int n, CALLBACK cb ) 
{
    hanoi_tower_1_rec( n, cb, "A", "C", "B");
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
