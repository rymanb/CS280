/* File header
 * ===========
 * file:       lariat.h
 * author:     Ryman Barnett
 * email:      ryman.b@digipen.edu
 * course:     CS280
 * section:    A
 * assignment: 2
 * date:       02/3/2022
 *
 * file content:
 * this is a header file for lariat to manipulating the datastructure, requires lariat.cpp
 */

 ////////////////////////////////////////////////////////////////////////////////
#ifndef LARIAT_H
#define LARIAT_H
////////////////////////////////////////////////////////////////////////////////

#include <string>     // error strings
#include <utility>    // error strings
#include <cstring>     // memcpy

class LariatException : public std::exception {
private:
    int m_ErrCode;
    std::string m_Description;

public:
    LariatException(int ErrCode, const std::string& Description) :
        m_ErrCode(ErrCode), m_Description(Description) {}

    virtual int code(void) const {
        return m_ErrCode;
    }

    virtual const char* what(void) const throw() {
        return m_Description.c_str();
    }

    virtual ~LariatException() throw() {
    }

    enum LARIAT_EXCEPTION { E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR };
};

// forward declaration for 1-1 operator<< 
template<typename T, int Size>
class Lariat;

template<typename T, int Size>
std::ostream& operator<< (std::ostream& os, Lariat<T, Size> const& rhs);

template <typename T, int Size>
class Lariat
{
public:

    Lariat();                   // default constructor


    Lariat(Lariat const& rhs);  // cpy

    template <typename T2>
    Lariat(T2 const& rhs);      // template cpy 
    
    ~Lariat();                  // destructor

    Lariat& operator= (Lariat const& rhs);  // Assignment operator
    template<typename T2, int Size2>
    Lariat& operator= (Lariat<T2, Size2> const& rhs);    // template assignment operator

    // inserts
    void insert(int index, const T& value);
    void push_back(const T& value);
    void push_front(const T& value);

    // deletes
    void erase(int index);
    void pop_back();
    void pop_front();

    //access
    T& operator[](int index);       // for l-values
    const T& operator[](int index) const; // for r-values
    T& first();
    T const& first() const;
    T& last();
    T const& last() const;

    unsigned find(const T& value) const;       // returns index, size (one past last) if not found

    friend std::ostream& operator<< <T, Size>(std::ostream& os, Lariat<T, Size> const& list);

    // and some more
    size_t size(void) const;   // total number of items (not nodes)
    void clear(void);          // make it empty

    void compact();             // push data in front reusing empty positions and delete remaining nodes
private:
    struct LNode { // DO NOT modify provided code
        LNode* next = nullptr;
        LNode* prev = nullptr;
        int    count = 0;         // number of items currently in the node
        T values[Size];
    };
    // DO NOT modify provided code
    LNode* head_;           // points to the first node
    LNode* tail_;           // points to the last node
    int size_;              // the number of items (not nodes) in the list
    mutable int nodecount_; // the number of nodes in the list
    int asize_;             // the size of the array within the nodes

private:

    void split(LNode* lnode);
    std::tuple<LNode*, int> findElement(int i) const;
    const T* MoveUp(LNode* node, int index);
    const T* MoveDown(LNode* node, int index);
    LNode* insertAt(LNode* node, int index, T const& val);

    // Friend
    template <typename TYPE, int OtherSize>
    friend class Lariat;


};

#include "lariat.cpp"

#endif // LARIAT_H