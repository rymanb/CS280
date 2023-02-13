/* File header
 * ===========
 * file:       lariat.cpp
 * author:     Ryman Barnett
 * email:      ryman.b@digipen.edu
 * course:     CS280
 * section:    A
 * assignment: 2
 * date:       02/3/2022
 *
 * file content:
 * this is a source file for lariat to manipulating the datastructure, requires lariat.h
 */

#include <iostream>
#include <iomanip>
#include "lariat.h"

#if 1
template <typename T, int Size>
std::ostream& operator<<(std::ostream& os, Lariat<T, Size> const& list)
{
    typename Lariat<T, Size>::LNode* current = list.head_;
    int index = 0;
    while (current) {
        os << "Node starting (count " << current->count << ")\n";
        for (int local_index = 0; local_index < current->count; ++local_index) {
            os << index << " -> " << current->values[local_index] << std::endl;
            ++index;
        }
        os << "-----------\n";
        current = current->next;
    }
    return os;
}
#else // fancier 
#endif

// constructor
template<typename T, int Size>
Lariat<T, Size>::Lariat() : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0), asize_(Size)
{
}

// copy constructor for different types
template<typename T, int Size>
template<typename T2>
Lariat<T, Size>::Lariat(const T2& rhs) : Lariat()
{
    // push back each element
    for (int i = 0; i < rhs.size_; i++)
    {
        push_back(static_cast<T>(rhs[i]));
    }
}

// copy constructor
template<typename T, int Size>
Lariat<T, Size>::Lariat(Lariat const& rhs) : Lariat()
{
    // push back each element
    for (int i = 0; i < static_cast<int>(rhs.size_); i++)
    {
        push_back(static_cast<T>(rhs[i]));
    }
}

// destructor
template<typename T, int Size>
Lariat<T, Size>::~Lariat()
{
    clear(); // clear the list
}

// assignment operator
template<typename T, int Size>
Lariat<T, Size>& Lariat<T, Size>::operator=(Lariat const& rhs)
{
    // check for self assignment
    if (rhs.head_ == head_)
    {
        return *this;
    }

    clear(); // clear the list

    // push back each element
    for (int i = 0; i < static_cast<int>(rhs.size()); i++)
    {
        push_back(static_cast<T>(rhs[i]));
    }

    return *this;
}

// assignment operator for different types
template<typename T, int Size>
template<typename T2, int Size2>
Lariat<T, Size>& Lariat<T, Size>::operator=(const Lariat<T2, Size2>& rhs)
{
    clear(); // clear the list

    // push back each element
    for (int i = 0; i < static_cast<int>(rhs.size()); i++)
    {
        push_back(static_cast<T>(rhs[i]));
    }

    return *this;
}

template<typename T, int Size>
void Lariat<T, Size>::insert(int index, const T& value)
{
    // check bounda
    if (0 > index || index > size_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    // check for empty list
    if (index == 0)
    {
        push_front(value);
    }
    else if (index == size_)
    {
        push_back(value);
    }
    else
    {
        // find the node and the index within the node
        std::tuple<LNode*, int> node_ind = findElement(index);
        LNode* node = std::get<0>(node_ind);
        int ind = std::get<1>(node_ind);

        // insert the value at the index
        LNode* newTail = insertAt(node, ind, value);
        // if a new node was created, update the tail
        if (newTail && node == tail_)
        {
            tail_ = newTail;
        }

        size_++; // increment the size
    }
}

// bracket operator, returns reference to element
template<typename T, int Size>
T& Lariat<T, Size>::operator[](int index)
{
    // check for valid index
    if (index < 0 || index >= size_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    // find the node that contains the element
    LNode* node = nullptr;
    int ind = 0;
    for (node = head_; head_; node = node->next)
    {
        if (ind + node->count > index)
        {
            break;
        }
        ind += node->count;
    }


    return node->values[index - ind]; // return the element
}


// bracket operator, returns const reference to element
template<typename T, int Size>
const T& Lariat<T, Size>::operator[](int index) const
{
    // check for valid index
    if (index < 0 || index >= size_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    // find the node that contains the element
    LNode* node = nullptr;
    int ind = 0;

    for (node = head_; head_; node = node->next)
    {
        if (ind + node->count > index)
        {
            break;
        }
        ind += node->count;
    }


    return node->values[index - ind]; // return the element

}

// returns reference to first element
template<typename T, int Size>
T& Lariat<T, Size>::first()
{
    if (!head_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    return &head_->values[0];
}

// returns const reference to first element
template<typename T, int Size>
T const& Lariat<T, Size>::first() const
{
    if (!head_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    return &head_->values[0];
}

// returns reference to last element
template<typename T, int Size>
T& Lariat<T, Size>::last()
{
    if (!head_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    return &tail_->values[tail_->count - 1];
}

// returns const reference to last element
template<typename T, int Size>
T const& Lariat<T, Size>::last() const
{
    if (!head_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    return &tail_->values[tail_->count - 1];
}

// returns size of lariat
template<typename T, int Size>
size_t Lariat<T, Size>::size(void) const
{
    return size_;
}

// pushes element to back of lariat
template<typename T, int Size>
void Lariat<T, Size>::push_back(const T& value)
{
    // if list is empty, push to front
    if (!tail_)
    {
        push_front(value);
        return;
    }

    size_++;

    // if there is room in the current node, insert
    if (tail_->count < asize_)
    {
        insertAt(tail_, tail_->count, value);
        return;

    }
    // otherwise, split the node and insert
    split(tail_);
    tail_ = tail_->next;

    if (tail_->count + 1 > asize_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    tail_->values[tail_->count++] = value;

}

// pushes element to front of lariat
template<typename T, int Size>
void Lariat<T, Size>::push_front(const T& value)
{
    // If list is empty, create a new node
    if (head_ == nullptr)
    {
        head_ = new LNode;
        nodecount_++;

        head_->prev = nullptr;
        head_->next = nullptr;
        head_->count = 0;
        
        tail_ = head_;
    }

    // If there is room in the current node, insert
    insertAt(head_, 0, value);

    // Set tail to the last node
    while (tail_->next)
    {
        tail_ = tail_->next;
    }

    size_++;

}

// pops element from back of lariat
template<typename T, int Size>
void Lariat<T, Size>::pop_back()
{

    if (tail_)
    {

        size_--;
        tail_->count--;

        // If the node is empty, delete it
        if (tail_->count <= 0)
        {
            // If there is more than one node, delete the last node
            if (nodecount_ > 1)
            {
                tail_ = tail_->prev;
                LNode* next = tail_->next;

                nodecount_--;

                if (tail_)
                {
                    tail_->next = next->next;
                }
                if (next->next)
                {
                    next->next->prev = tail_;
                }
                delete next;
            }
            else
            {
                // If there is only one node, delete it
                nodecount_--;

                if (tail_->prev)
                {
                    tail_->prev->next = tail_->next;
                }
                if (tail_->next)
                {
                    tail_->next->prev = tail_->prev;
                }
                delete tail_;

                head_ = nullptr;
                tail_ = nullptr;
            }
        }
    }
}

// pops element from front of lariat
template<typename T, int Size>
void Lariat<T, Size>::pop_front()
{

    if (head_)
    {
        // If there is more than one node, delete the first node
        MoveDown(head_, 0);
        --head_->count;
        --size_;

        if (head_->count <= 0)
        {
            if (nodecount_ > 1)
            {
                head_ = head_->next;

                LNode* prev = head_->prev;
                nodecount_--;

                if (prev->prev)
                {
                    prev->prev->next = head_;
                }
                if (prev->next)
                {
                    head_->prev = prev->prev;
                }
                delete prev;
            }
            else
            {
                // If there is only one node, delete it
                nodecount_--;

                if (head_->prev)
                {
                    head_->prev->next = head_->next;
                }
                if (head_->next)
                {
                    head_->next->prev = head_->prev;
                }
                delete head_;

                head_ = nullptr;
                tail_ = nullptr;
            }
        }
    }
}

// delete element at index
template<typename T, int Size>
void Lariat<T, Size>::erase(int index)
{
    // check bounds
    if (index < 0 || index >= size_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    // find node
    LNode* node = head_;
    int nodeIndex = 0;
    while (node != nullptr)
    {
        // if the index is in the current node, delete it
        if (nodeIndex + node->count > index)
        {
            MoveDown(node, index - nodeIndex);
            node->count--;
            size_--;
            return;
        }


        nodeIndex += node->count;
        node = node->next;
    }
}

// find index of element with value
template<typename T, int Size>
unsigned Lariat<T, Size>::find(const T& value) const
{

    // find the index of the first element with the given value
    unsigned index = 0;
    LNode* node = head_;

    while (node)
    {
        for (int i = 0; i < node->count; ++i, ++index)
        {
            if (value == node->values[i])
            {
                return index;
            }
        }

        node = node->next;
    }

    return index;
}

// shifts all data down list so there are no empty spaces
template<typename T, int Size>
void Lariat<T, Size>::compact()
{
    if (!head_)
    {
        return;
    }

    LNode* node1 = head_ ;
    LNode * node2 = head_->next;

    if (node1)
    {
        while (node2)
        {
            


            // if full move on
            if (node1->count == asize_)
            {
                node1 = node1->next;
                
            }
            else
            {

                int origCT = node2->count;
                node2->count = 0;

                for (int i = 0; i < origCT; i++)
                {
                    node1->values[node1->count++] = node2->values[i];
                    if (node1->count == asize_)
                    {
                        node1 = node1->next;
                    }
                }
            }

            node2 = node2->next;
        }

        // Remove extra nodes
        node1 = head_;
        while (node1)
        {
            if (node1->count == 0)
            {
                LNode* nxt = node1->next;

                
                
               node1->prev->next = node1->next;
               if (node1->next)
               {
                   node1->next->prev = node1->prev;
               }
                delete node1;

                this->nodecount_--;
                node1 = nxt;
                continue;
            }

                node1 = node1->next;

        }

        // update tail
        tail_ = head_;
        while (tail_->next)
        {
            tail_ = tail_->next;
        }
    }

}

// splits a node in half, and transfers data
template<typename T, int Size>
void Lariat<T, Size>::split(Lariat::LNode* node)
{
    // check if the node is full
    if (node->count == asize_)
    {
        // create a new node
        LNode* newNode = new LNode;
        nodecount_++;


        newNode->prev = nullptr;
        newNode->next = nullptr;
        newNode->count = 0;

        // update the pointers
        LNode* temp = node->next;
        node->next = newNode;
        newNode->prev = node;
        newNode->next = temp;
        if (temp)
        {
            temp->prev = newNode;
        }

        // split the values
        int half = (asize_ + 1) - ((asize_ + 1) / 2);


        for (int i = half; i < asize_; ++i)
        {
            // check bounds
            if (newNode->count + 1 > Size)
            {
                throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
            }
            newNode->values[newNode->count++] = node->values[i]; // copy the values

        }

        node->count = half; // set size
    }

}

// finds the node and index within the node
template<typename T, int Size>
std::tuple<typename Lariat<T, Size>::LNode*, int> Lariat<T, Size>::findElement(int i) const
{
    // find the node and the index within the node
    LNode* current = head_;
    int index = 0;
    while (current)
    {
        // check if the index is in the current node
        if (index + current->count > i)
        {
            return { current, i - index };
        }
        index += current->count;
        current = current->next;
    }
    return { nullptr, -1 }; // invalid
}

// move items in list up, return overflow
template<typename T, int Size>
const T* Lariat<T, Size>::MoveUp(Lariat::LNode* node, int index)
{
    // store overflow
    T overflowNum = node->values[asize_ - 1];

    if (node->count == asize_)
    {
        // shift
        for (int i = asize_ - 1; i > index; --i)
        {
            node->values[i] = node->values[i - 1];
        }

        node->values[index] = overflowNum; // reinsert overflow
        return &node->values[index];
    }
    else
    {
        // shift
        for (int i = node->count; i > index; --i)
        {
            node->values[i] = node->values[i - 1];
        }
    }

    return nullptr;
}

// items in move list down, return overflow
template<typename T, int Size>
const T* Lariat<T, Size>::MoveDown(Lariat::LNode* node, int index)
{
    // store overflow
    T overflowNum = node->values[asize_ - 1];

    if (node->count == asize_)
    {
        // shift
        for (int i = index; i < asize_ - 1; ++i)
        {
            node->values[i] = node->values[i + 1];
        }

        node->values[asize_ - 1] = overflowNum; // reinsert
        return &node->values[index];
    }
    else
    {
        // shift
        for (int i = index; i < node->count - 1; ++i)
        {
            node->values[i] = node->values[i + 1];
        }
    }

    return nullptr;
}


// insert value at given index
template<typename T, int Size>
typename Lariat<T, Size>::LNode* Lariat<T, Size>::insertAt(Lariat::LNode* node, int index, const T& val)
{
    // check bounds
    if (index < 0 || index > asize_)
    {
        throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
    }

    // should split
    if (node->count == asize_)
    {
        // shift and catch overflow
        T overflow = *(MoveUp(node, index));

        node->values[index] = val; // insert new val

        split(node); // split node

        LNode* nextNode = node->next;

        nextNode->values[nextNode->count++] = overflow; // insert overflow
        


        return node->next; // return new tail
    }
    else
    {
        // just insert otherwise
        for (int i = node->count; i > index; --i)
        {
            node->values[i] = node->values[i - 1];
        }

        node->values[index] = val;
        node->count++;

        return nullptr;
    }


}

// clears lariat fully
template<typename T, int Size>
void Lariat<T, Size>::clear(void)
{
    // Delete all nodes
    while (head_)
    {
        LNode* temp = head_->next;
        delete head_;
        head_ = temp;
    }

    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
    nodecount_ = 0;
}