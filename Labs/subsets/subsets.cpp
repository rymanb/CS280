#include "subsets.h"

// actual recursive function
template< typename T >
void subset_rec( std::vector<T> const& set, std::vector<T> const& subset ) 
{
    if( set.empty() ) {
        // print subset
        std::cout << "{ ";
        for( auto const& e : subset ) {
            std::cout << e << " ";
        }
        std::cout << "}" << std::endl;
    } else {
        // recursive calls
        std::vector<T> subset1 = subset;
        subset1.push_back( set[0] );
        subset_rec( std::vector<T>( set.begin()+1, set.end() ), subset1 );
        subset_rec( std::vector<T>( set.begin()+1, set.end() ), subset );

    }
}

// kickstart
template< typename T >
void subsets( std::vector<T> const& set)
{
    std::vector<T> subset;
    subset_rec( set, subset );
}
