#include "bst-map.h"

#include <iostream>

// static data members
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator        
		CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::end_it        = CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(nullptr);

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const  
		CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::const_end_it  = CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const(nullptr);

// bstmap implementation

template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap() : pRoot(nullptr) {
}

//copy constructor
//in implementation file 
//method's name and return value have to be fully qualified with
//all required namespace and class names (like CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap)
//but the argument list does not have to be:
//function argument list automatically falls into the method's namespace (Koenig lookup)
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap(const BSTmap& rhs) : pRoot(nullptr) {
	*this = rhs;
}



template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::~BSTmap() {
	clear();
}

template<typename KEY_TYPE, typename VALUE_TYPE>
unsigned int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::size()
{
	// find the size of the tree
	
	return size(pRoot);
}

template<typename KEY_TYPE, typename VALUE_TYPE>
unsigned int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::size(Node* p)
{
	// find the size of the tree
	if (p == nullptr)
	{
		return 0;
	}
	else
	{
		return 1 + size(p->left) + size(p->right);
	}
}


template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::operator=(const BSTmap& rhs) {
	if (this != &rhs) {
		clear();
		for (BSTmap_iterator_const it = rhs.begin(); it != rhs.end(); ++it) {
			const Node& node = *it;
			insert(node.key, node.value);
		}
	}
	return *this;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::insert(KEY_TYPE key, VALUE_TYPE value)
{
	if (pRoot == nullptr)
	{
		pRoot = new Node(key, value, nullptr, 0, 0, nullptr, nullptr);
	}
	else
	{
		insert(pRoot, key, value);
	}
}

template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::insert(Node* p, KEY_TYPE key, VALUE_TYPE value)
{
	if (key < p->key)
	{
		if (p->left == nullptr)
		{
			p->left = new Node(key, value, p, 0, 0, nullptr, nullptr);
		}
		else
		{
			insert(p->left, key, value);
		}
	}
	else if (key > p->key)
	{
		if (p->right == nullptr)
		{
			p->right = new Node(key, value, p, 0, 0, nullptr, nullptr);
		}
		else
		{
			insert(p->right, key, value);
		}
	}
	else
	{
		p->value = value;
	}
}


template< typename KEY_TYPE, typename VALUE_TYPE >
VALUE_TYPE& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::operator[](const KEY_TYPE& key) {
	Node* p = pRoot;
	while (p != nullptr) {
		if (key < p->key) {
			p = p->left;
		}
		else if (key > p->key) {
			p = p->right;
		}
		else {
			return p->value;
		}
	}
	insert(key, VALUE_TYPE());
	return pRoot->value;
}

template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::begin() const
{
	// find the leftmost node
	Node* p = pRoot;
	if (p == nullptr)
	{
		return const_end_it;
	}
	while (p->left != nullptr)
	{
		p = p->left;
	}
	return BSTmap_iterator_const(p);
}

template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::end() const
{
	return BSTmap_iterator_const();
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::find(const KEY_TYPE& key) const {
	return BSTmap_iterator_const(find(key));
}

template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::erase(BSTmap_iterator it)
{

	// find the node to delete
	Node* p = it.p_node;
	if (p == nullptr)
	{
		return;
	}

	// if the node has no children, just delete it
	if (p->left == nullptr && p->right == nullptr)
	{
		if (p->parent == nullptr)
		{
			pRoot = nullptr;
		}
		else if (p->parent->left == p)
		{
			p->parent->left = nullptr;
		}
		else
		{
			p->parent->right = nullptr;
		}
		delete p;
		return;
	}

	// if the node has one child, replace the node with the child
	if (p->left == nullptr)
	{
		if (p->parent == nullptr)
		{
			pRoot = p->right;
		}
		else if (p->parent->left == p)
		{
			p->parent->left = p->right;
		}
		else
		{
			p->parent->right = p->right;
		}
		p->right->parent = p->parent;
		delete p;
		return;
	}
	if (p->right == nullptr)
	{
		if (p->parent == nullptr)
		{
			pRoot = p->left;
		}
		else if (p->parent->left == p)
		{
			p->parent->left = p->left;
		}
		else
		{
			p->parent->right = p->left;
		}
		p->left->parent = p->parent;
		delete p;
		return;
	}

	// if the node has two children, replace the node with the leftmost node in the right subtree
	Node* q = p->right;
	while (q->left != nullptr)
	{
		q = q->left;
	}
	p->key = q->key;
	p->value = q->value;
	if (q->parent->left == q)
	{
		q->parent->left = q->right;
	}
	else
	{
		q->parent->right = q->right;
	}
	if (q->right != nullptr)
	{
		q->right->parent = q->parent;
	}
	delete q;
}




////////////////////////////////////////////////////////////
// typename is used to help compiler to parse
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::begin() {
	if (pRoot) return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(pRoot->first());
	else       return end_it;
}

template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::end()
{
	return end_it;
}

template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const& key)
{
	Node* p = pRoot;
	while (p != nullptr)
	{
		if (key < p->key)
		{
			p = p->left;
		}
		else if (key > p->key)
		{
			p = p->right;
		}
		else
		{
			return BSTmap_iterator(p);
		}
	}
	return end_it;
}


// Node class implementation
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Node(KEY_TYPE k, VALUE_TYPE val, Node* p, int h, int b, Node* l, Node* r) : key(k), value(val), parent(p), height(h), balance(b), left(l), right(r) {
}

template< typename KEY_TYPE, typename VALUE_TYPE >
KEY_TYPE const & CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node::Key() const {
	return key;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
VALUE_TYPE & CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node::Value() {
	return value;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node::first() {
	Node* p = this;
	while (p->left) p = p->left;
	return p;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node::last() {
	Node* p = this;
	while (p->right) p = p->right;
	return p;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node::increment() {
	if (right) return right->first();
	Node* p = this;
	while (p->parent && p->parent->right == p) p = p->parent;
	return p->parent;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node::decrement() {
	if (left) return left->last();
	Node* p = this;
	while (p->parent && p->parent->left == p) p = p->parent;
	return p->parent;
}

template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::clear()
{
	if (left) left->clear();
	if (right) right->clear();
	delete this;
}



// BSTmap iterator implementation
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::BSTmap_iterator(Node* p) : p_node(p) {
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator=(const BSTmap_iterator& rhs) {
	p_node = rhs.p_node;
	return *this;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator++() {
	p_node = p_node->increment();
	return *this;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator++(int)
{
	p_node = p_node->increment();
	return *this;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator*()
{
	return *p_node;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator->()
{
	return p_node;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator==(const BSTmap_iterator& rhs)
{
	return p_node == rhs.p_node;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator::operator!=(const BSTmap_iterator& rhs)
{
	return p_node != rhs.p_node;
}




// BSTmap iterator const implementation
template< typename KEY_TYPE, typename VALUE_TYPE >
CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::BSTmap_iterator_const(Node* p) : p_node(p) {
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator=(const BSTmap_iterator_const& rhs) {
	p_node = rhs.p_node;
	return *this;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator++() {
	p_node = p_node->increment();
	return *this;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator++(int)
{
	p_node = p_node->increment();
	return *this;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node const& CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator*()
{
	return *p_node;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::Node const* CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator->()
{
	return p_node;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator==(const BSTmap_iterator_const& rhs)
{
	return p_node == rhs.p_node;
}

template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const::operator!=(const BSTmap_iterator_const& rhs)
{
	return p_node != rhs.p_node;
}








////////////////////////////////////////////////////////////
// do not change this code from here to the end of the file
/* figure out whether node is left or right child or root 
 * used in print_backwards_padded 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
char CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::getedgesymbol(const Node* node) const {
	const Node* parent = node->parent;
	if ( parent == nullptr) return '-';
	else                 return ( parent->left == node)?'\\':'/';
}

/* this is another "ASCII-graphical" print, but using 
 * iterative function. 
 * Left branch of the tree is at the bottom
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
std::ostream& CS280::operator<<(std::ostream& os, BSTmap<KEY_TYPE,VALUE_TYPE> const& map) {
	map.print(os);
	return os;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::print(std::ostream& os, bool print_value ) const {
	if (pRoot) {
		BSTmap<KEY_TYPE,VALUE_TYPE>::Node* b = pRoot->last();
		while ( b ) { 
			int depth = getdepth(b);
			int i;
			/* printf(b); */

			char edge = getedgesymbol(b);
			switch (edge) {
				case '-': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					break;
				case '\\': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					break;
				case '/': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					break;
			}
			b = b->decrement();
		}
	}
	std::printf("\n");
}

template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::clear()
{
	if (pRoot) {
		pRoot->clear();
		pRoot = nullptr;
	}
}

template<typename KEY_TYPE, typename VALUE_TYPE>
int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::getdepth(Node* p) const
{
	int depth = 0;
	while (p->parent) {
		++depth;
		p = p->parent;
	}
	return depth;
}

