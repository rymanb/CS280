#include "phone2word.h"


void all_combinations( 
        std::string const& phone_number,      // number to convert
        std::map<char,std::string> & keypad,  // mapping on the keypad - I may use a non-standard mapping
        std::vector< std::string > & result,  // vector of strings to return
        std::string & current,                // current string being built
        unsigned int index                             // index into the phone number
        )
{
    if( index == phone_number.size() )
    {
        result.push_back( current );
        return;
    }

    char digit = phone_number[index];
    std::string letters = keypad[digit];
    for( unsigned i = 0; i < letters.size(); ++i )
    {
        current.push_back( letters[i] );
        all_combinations( phone_number, keypad, result, current, index + 1 );
        current.pop_back();
    }
}

std::vector< std::string > all_combinations( 
        std::string const& phone_number,      // number to convert
        std::map<char,std::string> & keypad   // mapping on the keypad - I may use a non-standard mapping
        )
{
// use a recursive function to generate all possible combinations
// of letters that can be generated from the phone number
// the function will return a vector of strings

    std::vector< std::string > result;
    std::string current;
    all_combinations( phone_number, keypad, result, current, 0 );
    return result;

}