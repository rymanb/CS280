/* File header
 * ===========
 * file:       ObjectAllocator.cpp
 * author:     Ryman Barnett
 * email:      ryman.b@digipen.edu
 * course:     CS280
 * section:    A
 * assignment: 2
 * date:       01/17/2022
 *
 * file content:
 * this is a source file for ObjectAllocator to handle allocating/freeing objects, requires ObjectAllocator.h
 */

#include "ObjectAllocator.h" // include the header file
#include <cstring> // include cstring for memset

/*!
*  \brief  calculate the aligned size
*
*  \param  size the size to align
*  \param  alignment the alignment
*
*  \return the aligned size
*/
size_t align(size_t size, size_t alignment)
{
	// if alignment is 0, return size
	if (!alignment)
		return size;
		
	// if size is already aligned, return size
	if (size % alignment == 0)
		return size;

	// return aligned size
	return size + alignment - size % alignment;
}

/*!
*  \brief  constructor for the ObjectAllocator
*
*  \param  ObjectSize the size of the object
*  \param  config the configuration
*/

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config) : PageList_(0), FreeList_(0), Config_(config), Stats_{}
{
	// size of our object
	Stats_.ObjectSize_ = ObjectSize;

	// calculate the header size
	size_t headerSize = sizeof(void*) + Config_.HBlockInfo_.size_ + Config_.PadBytes_;
	// calculate the object size
	size_t innerSize = ObjectSize + Config_.HBlockInfo_.size_ + (Config_.PadBytes_ * 2);

	// allign the header size
	HeaderSize_ = align(headerSize, Config_.Alignment_);
	// allign the object size
	InnerSize_ = align(innerSize, Config_.Alignment_);

	// calculate the difference between the header size and the object size for the alignment
	size_t inner_diff = InnerSize_ - innerSize;
	size_t header_diff = HeaderSize_ - headerSize;

	// set the alignment sizes
	Config_.InterAlignSize_ = static_cast<unsigned int>(inner_diff);
	Config_.LeftAlignSize_ =  static_cast<unsigned int>(header_diff);

	size_t objS = Config_.ObjectsPerPage_ * InnerSize_; // calculate the object size
	size_t allign = Config_.LeftAlignSize_ - Config_.InterAlignSize_; // calculate the alignment size

	// calculate the page size
	Stats_.PageSize_ = sizeof(void*) + objS + allign;

	// allocate a new page
	allocate_new_page();
}

/*!
*  \brief destructor for the ObjectAllocator
*/
ObjectAllocator::~ObjectAllocator()
{
	// delete all pages
	GenericObject* curr = PageList_;
	while (curr)
	{
		GenericObject* next = curr->Next;

		delete[] curr;

		curr = next;
	}
}

/*!
*  \brief  allocate an object
*
*  \param  label the label for the object
*
*  \return the new object
*/
void* ObjectAllocator::Allocate(const char* label)
{
	GenericObject* freeObject = nullptr;

	// if using cpp memory manager, allocate a new object
	if (Config_.UseCPPMemManager_)
	{
		try
		{
			freeObject = reinterpret_cast<GenericObject*>(new unsigned char[Stats_.ObjectSize_]);

		}
		catch (std::bad_alloc&)
		{
			throw OAException(OAException::E_NO_MEMORY, "No memory available"); // throw exception if no memory available
		}
	}
	else
	{
		// if freelist is empty, allocate a new page
		if (FreeList_ == nullptr) {
			allocate_new_page();
		}

		// get the first free object
		freeObject = FreeList_;
		FreeList_ = FreeList_->Next;

		// debug
		if (Config_.DebugOn_)
		{
			std::memset(freeObject, ALLOCATED_PATTERN, Stats_.ObjectSize_);
		}
	}

	// update stats
	Stats_.ObjectsInUse_++;
	if (Stats_.ObjectsInUse_ > Stats_.MostObjects_) {
		Stats_.MostObjects_ = Stats_.ObjectsInUse_;
	}

	Stats_.FreeObjects_--;
	Stats_.Allocations_++;

	// initialize the header block
	if (!Config_.UseCPPMemManager_)
	{
		InitializeHeaderBlock(freeObject, Config_.HBlockInfo_.type_, label);
	}

	return freeObject; // return the object
}

/*!
*  \brief  free an object
*
*  \param  Object the object to free
*/
void ObjectAllocator::Free(void* Object)
{
	Stats_.Deallocations_++; // increment deallocations
	--Stats_.ObjectsInUse_; // decrement objects in use

	// if using cpp memory manager, delete the object
	if (Config_.UseCPPMemManager_)
	{
		delete[] reinterpret_cast<char*>(Object);
		return;
	}

	// convert to GenericObject
	GenericObject* freeObject = reinterpret_cast<GenericObject*>(Object);

	FreeHeaderBlock(freeObject, Config_.HBlockInfo_.type_);

	// if debug is on, check for corrupted padding
	if (Config_.DebugOn_)
	{
		// check corrupted padding
		if (Config_.PadBytes_ > 0) 
		{

			bool corrupt = false; // corrupted flag

			// check padding
			for (size_t i = 0; i < Config_.PadBytes_; ++i)
			{
				// check left padding
				if (*(GetLeftAlignBlock(freeObject) + i) != ObjectAllocator::PAD_PATTERN)
				{
					corrupt = true;
				}
				// check right padding
				if (*(GetRightAlignBlock(freeObject) + i) != ObjectAllocator::PAD_PATTERN)
				{
					corrupt = true;
				}
			}

			// throw exception if corrupted
			if (corrupt)
			{
				throw OAException(OAException::E_CORRUPTED_BLOCK, "Corrupted Padding");
			}
		}

		// check corrupted boundaries
		CheckBoundaries(reinterpret_cast<unsigned char*>(freeObject));

		// set the object to freed pattern
		std::memset(Object, FREED_PATTERN, Stats_.ObjectSize_);
	}

	// put the object on the freelist
	freeObject->Next = nullptr;
	put_on_freelist(freeObject);
}

/*!
*  \brief  calls the callback function for each object in use
*
*  \param  fn the callback function
*
*  \return the number of bytes in use
*/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
	unsigned bytes = 0; // number of bytes in use
	GenericObject* page = PageList_; // current page

	// iterate through all pages
	while (page)
	{
		char* data = reinterpret_cast<char*>(page) + HeaderSize_; // data pointer

		// iterate through all objects
		for (size_t i = 0; i < Config_.ObjectsPerPage_; ++i)
		{
			// get the object
			GenericObject* objectData = reinterpret_cast<GenericObject*>(data + i * InnerSize_);

			// if the object is used, call the callback
			if (IsUsed(objectData))
			{
				++bytes;
				fn(objectData, Stats_.ObjectSize_);

			}
		}

		page = page->Next;
	}

	return bytes; // return the number of bytes in use
}

/*!
*  \brief  calls the callback function for each corrupted object
*
*  \param  fn the callback function
*
*  \return the number of corrupted objects
*/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
	// if debug is off, return 0
	if (!Config_.DebugOn_)
	{
		return 0;
	}

	// if no padding, return 0 cant check
	if (!Config_.PadBytes_)
	{
		return 0;
	}

	unsigned corrupted = 0; // number of corrupted pages
	GenericObject* page = PageList_; // current page


	// iterate through all pages
	while (page)
	{
		unsigned char* pPage = reinterpret_cast<unsigned char*>(page); // current page

		// iterate through all objects
		for (size_t i = 0; i < Config_.ObjectsPerPage_; ++i)
		{
			GenericObject* data = reinterpret_cast<GenericObject*>(pPage + HeaderSize_ + i * InnerSize_); // current object

			unsigned char* left = GetLeftAlignBlock(data); // left padding
			unsigned char* right = GetRightAlignBlock(data); // right padding

			bool valid = true; // is the padding valid

			// iterate through all padding bytes
			for (size_t i = 0; i < Config_.PadBytes_; ++i)
			{
				// if any byte is not the pad pattern, the padding is invalid
				if (*(left + i) != ObjectAllocator::PAD_PATTERN)
				{
					valid = false;
					break;
				}
				if (*(right + i) != ObjectAllocator::PAD_PATTERN)
				{
					valid = false;
					break;
				}
			}

			// if the padding is invalid, call the callback
			if (!valid)
			{
				fn(data, Stats_.ObjectSize_);
				++corrupted;
				continue;
			}
		}

		page = page->Next;
	}

	return corrupted; // return the number of corrupted pages
}

/*!
*  \brief  frees all empty pages
*
*  \return the number of freed pages
*/
unsigned ObjectAllocator::FreeEmptyPages(void)
{
	// check if there are pages
	if (!PageList_)
	{
		return 0;
	}
	
	unsigned empty = 0; // number of empty pages
	GenericObject* walk = PageList_; // walk the list
	GenericObject* prev = nullptr; // previous page

	// remove empty pages
	while (walk )
	{
		if (IsPageEmpty(walk))
		{
			++empty;

			// remove page
			if (walk == PageList_)
			{
				PageList_ = walk->Next;
				FreePage(walk);
				walk = PageList_;
			}
			else
			{
				prev->Next = walk->Next;
				FreePage(walk);
				walk = prev->Next;
			}
		}
		else
		{
			prev = walk;
			walk = walk->Next;
		}

	}

	return empty; // number of pages freed
}

/*!
*  \brief  did the user implement extra credit
*
*  \return true if implemented, false otherwise
*/
bool ObjectAllocator::ImplementedExtraCredit(void)
{
	return true;
}

/*!
*  \brief  sets the debug state
*
*  \param  State the new debug state
*/
void ObjectAllocator::SetDebugState(bool State)
{
	Config_.DebugOn_ = State;
}

/*!
*  \brief  gets a pointer to the free list
*
*  \return a pointer to the free list
*/
const void* ObjectAllocator::GetFreeList(void) const
{
	return FreeList_;
}

/*!
*  \brief  gets a pointer to the page list
*
*  \return a pointer to the page list
*/
const void* ObjectAllocator::GetPageList(void) const
{
	return PageList_;
}

/*!
*  \brief  gets the object allocator configuration
*
*  \return the object allocator configuration
*/
OAConfig ObjectAllocator::GetConfig(void) const
{
	return Config_;
}

/*!
*  \brief  gets the object allocator statistics
*
*  \return the object allocator statistics
*/
OAStats ObjectAllocator::GetStats(void) const
{
	return Stats_;
}

/*!
*  \brief  allocates a new page
*/
void ObjectAllocator::allocate_new_page(void)
{
	// check for max pages
	if (Stats_.PagesInUse_ == Config_.MaxPages_) {
		throw OAException(OAException::OA_EXCEPTION::E_NO_PAGES, "No free pages");
	}

	// allocate a new page
	GenericObject* newPage = nullptr;
	// catch the exception
	try {
		newPage = reinterpret_cast<GenericObject*>(new GenericObject*[Stats_.PageSize_]());
		Stats_.PagesInUse_++;
	}
	catch (std::bad_alloc& e) {
		throw OAException(OAException::E_NO_MEMORY, "No memory available to allocate new page.");
	}

	// set debug info
	if (Config_.DebugOn_) {
		std::memset(newPage, ALIGN_PATTERN, Stats_.PageSize_);
	}

	// set the page list
	newPage->Next = PageList_;
	PageList_ = newPage;

	// offset to the first objects data
	unsigned char* DataStart = reinterpret_cast<unsigned char*>(newPage) + HeaderSize_;

	for (unsigned i = 0; i < Config_.ObjectsPerPage_; ++i) {
		// set the free list
		put_on_freelist(DataStart);

		// set the debug info
		if (Config_.DebugOn_) {
			std::memset(DataStart + sizeof(void*), UNALLOCATED_PATTERN, Stats_.ObjectSize_ - sizeof(void*));
			std::memset(DataStart - Config_.PadBytes_, PAD_PATTERN, Config_.PadBytes_);
			std::memset(DataStart + Stats_.ObjectSize_, PAD_PATTERN, Config_.PadBytes_);
		}

		// set header
		void* header = reinterpret_cast<void*>(DataStart - Config_.PadBytes_ - Config_.HBlockInfo_.size_);
		std::memset(header, 0, Config_.HBlockInfo_.size_);

		DataStart += InnerSize_;
	}

}

/*!
*  \brief  puts an object on the free list
*
*  \param  Object the object to put on the free list
*/
void ObjectAllocator::put_on_freelist(void* Object)
{
	// set the free list
	GenericObject* newObject = reinterpret_cast<GenericObject*>(Object);
	GenericObject* oldFreeList = FreeList_;
	FreeList_ = newObject;
	newObject->Next = oldFreeList;

	// update stats
	++Stats_.FreeObjects_;
}

/*!
*  \brief  checks if an object is in use
*
*  \param  object the object to check
*
*  \return true if the object is in use, false otherwise
*/
bool ObjectAllocator::IsUsed(GenericObject* object) const
{
	// get address of the header
	unsigned char* header = reinterpret_cast<unsigned char*>(object) - Config_.PadBytes_ - Config_.HBlockInfo_.size_;

	switch (Config_.HBlockInfo_.type_)
	{
	case OAConfig::HBLOCK_TYPE::hbNone:
	{
		GenericObject* freelist = FreeList_; // get the free list
		
		// check if the object is on the free list
		while (freelist)
		{
			if (freelist == object)
			{
				return true; // object is on the free list
			}
			
			freelist = freelist->Next;
		}
		return false; // object is not on the free list
	}
	case OAConfig::hbBasic:
	{
		unsigned int* alocNum = reinterpret_cast<unsigned int*>(header);
		
		// check if the object is allocated
		bool* flag = reinterpret_cast<bool*>(alocNum + 1);
		return *flag;
		break;
	}
	case OAConfig::hbExtended:
	{
		unsigned short* counter = reinterpret_cast<unsigned short*>(header + Config_.HBlockInfo_.additional_);


		unsigned int* alocNum = reinterpret_cast<unsigned int*>(counter + 1);

		// check if the object is allocated
		bool* flag = reinterpret_cast<bool*>(alocNum + 1);
		return *flag;
		break;
	}
	case OAConfig::hbExternal:
	{
		// check if the object has a header
		return *reinterpret_cast<unsigned char*>(object) - Config_.HBlockInfo_.size_ - Config_.PadBytes_;
		break;
	}
	default:
		return false;
		break;
	}
}

/*!
*  \brief  checks if a page is empty
*
*  \param  page the page to check
*
*  \return true if the page is empty, false otherwise
*/
bool ObjectAllocator::IsPageEmpty(GenericObject* page) const
{
	GenericObject* walk = FreeList_; // walk the free list
	unsigned free = 0; // count the free objects
	
	// walk the free list
	while (walk)
	{
		// check if the object is in the page
		if (reinterpret_cast<unsigned char*>(walk) >= reinterpret_cast<unsigned char*>(page))
		{
			if (reinterpret_cast<unsigned char*>(walk) < reinterpret_cast<unsigned char*>(page) + Stats_.PageSize_)
			{
				++free; // increment the free count

				// if the free count is equal to the objects per page, the page is empty
				if (free >= Config_.ObjectsPerPage_)
				{
					return true;
				}
			}
		}
		
		walk = walk->Next;
	}
	
	return false; // the page is not empty
}

/*!
*  \brief  frees a page
*
*  \param  page the page to free
*/
void ObjectAllocator::FreePage(GenericObject* page)
{
	GenericObject* walk = FreeList_; // walk the free list
	GenericObject* prev = nullptr; // keep track of the previous node
	
	// walk the free list
	while (walk)
	{
		// check if the object is in the page
		if (reinterpret_cast<unsigned char*>(walk) >= reinterpret_cast<unsigned char*>(page))
		{
			if (reinterpret_cast<unsigned char*>(walk) < reinterpret_cast<unsigned char*>(page) + Stats_.PageSize_)
			{
				// remove the object from the free list
				if (prev == nullptr)
				{
					FreeList_ = walk->Next;

				}
				else
				{
					prev->Next = walk->Next;
				}

				--Stats_.FreeObjects_;
			}
			else
			{
				prev = walk; // update the previous node
			}
			
		}
		else
		{
			prev = walk; // update the previous node
		}

		walk = walk->Next; // move to the next node
	}

	// remove the page from the page list
	delete[] reinterpret_cast<char*>(page);
	--Stats_.PagesInUse_;
}

/*!
*  \brief  initializes a header block for an object
*
*  \param  object the object to initialize
*  \param  headerType the type of header block to initialize
*  \param  label_ the label to use for the header block
*/
void ObjectAllocator::InitializeHeaderBlock(GenericObject* object, OAConfig::HBLOCK_TYPE headerType, const char* label_)
{
	// get header address
	unsigned char* header = reinterpret_cast<unsigned char*>(object) - Config_.PadBytes_ - Config_.HBlockInfo_.size_;

	switch (headerType) {
	case OAConfig::hbBasic:
	{
		// set the allocation number
		unsigned int* alocNum = reinterpret_cast<unsigned*>(header);
		*alocNum = Stats_.Allocations_;

		// set the flag
		bool* flag = reinterpret_cast<bool*>(alocNum + 1);
		*flag = true;
		break;
	}
	case OAConfig::hbExtended:
	{
		// set the counter
		unsigned short* counter = reinterpret_cast<unsigned short*>(header + Config_.HBlockInfo_.additional_);
		++(*counter);

		// set the allocation number
		unsigned int* alocNum = reinterpret_cast<unsigned*>(counter + 1);
		*alocNum = Stats_.Allocations_;

		// set the flag
		bool* flag = reinterpret_cast<bool*>(alocNum + 1);
		*flag = true;
		break;
	}
	case OAConfig::hbExternal:
	{
		// get adress of the memory block
		MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

		try
		{
			// create a new memory block
			*info = new MemBlockInfo();
			(*info)->in_use = true;
			(*info)->alloc_num = Stats_.Allocations_;

			if (label_)
			{
				try
				{
					// allocate our label
					(*info)->label = new char[strlen(label_) + 1];
				}
				catch (std::bad_alloc& e)
				{
					throw OAException(OAException::E_NO_MEMORY, "No memory for label.");
				}

				// copy the label
				strcpy((*info)->label, label_);
			}

		}
		catch (std::bad_alloc& e)
		{
			throw OAException(OAException::E_NO_MEMORY, "No memory for MemBlockInfo.");
		}
		break;
	}
	default:
		break;
	}
}

/*!
*  \brief  frees a header block
*
*  \param  object the object to free
*  \param  headerType the type of header block to free
*/
void ObjectAllocator::FreeHeaderBlock(GenericObject* object, OAConfig::HBLOCK_TYPE headerType)
{
	// start at the beginning of the header block
	unsigned char* header =  reinterpret_cast<unsigned char*>(object) - Config_.PadBytes_ - Config_.HBlockInfo_.size_;

	switch (headerType) {
	case OAConfig::hbBasic:
	{
		// set number of allocations to 0
		unsigned int* alocNum = reinterpret_cast<unsigned int*>(header);
		*alocNum = 0;

		// clear the flag
		bool* flag = reinterpret_cast<bool*>(alocNum + 1);

		if (*flag == false)
		{
			throw OAException(OAException::E_MULTIPLE_FREE, "Double Free");
		}
		
		*flag = false;
		break;
	}
	case OAConfig::hbExtended:
	{
		unsigned short* counter = reinterpret_cast<unsigned short*>(header + Config_.HBlockInfo_.additional_);

		// set number of allocations to 0
		unsigned int* alocNum = reinterpret_cast<unsigned int*>(counter + 1);
		*alocNum = 0;

		// clear the flag
		bool* flag = reinterpret_cast<bool*>(alocNum + 1);

		if (*flag == false)
		{
			throw OAException(OAException::E_MULTIPLE_FREE, "Double Free");
		}
		
		*flag = false;
		break;
	}
	case OAConfig::hbExternal:
	{
		// get the pointer to the MemBlockInfo
		MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);
		if (*info)
		{
			// check if the block is in use
			if ((*info)->in_use == false)
			{
				throw OAException(OAException::E_MULTIPLE_FREE, "Double Free");
			}
			
			if ((*info)->label)
			{
				delete[](*info)->label; // delete the label
			}
			
			delete* info; // delete the MemBlockInfo
			*info = nullptr;
		}
		break;
	}
	default:
		// for no header block
		// check if the block is in use
		unsigned char* freeObjectChar = reinterpret_cast<unsigned char*>(object) + Stats_.ObjectSize_ - 1; // get the last byte of the object
		if (*freeObjectChar == FREED_PATTERN)
		{
			throw OAException(OAException::E_MULTIPLE_FREE, "Double Free");
		}
		
		break;
	}
}

/*!
*  \brief  checks the boundaries of an object
*
*  \param  address the address to check
*/
void ObjectAllocator::CheckBoundaries(unsigned char* address) const
{	
	GenericObject* page = PageList_; // start at the beginning of the page list
	
	// check if the address is in the page list
	while (page)
	{	// check if the address is in the page
		if (address >= reinterpret_cast<unsigned char*>(page))
		{
			if (address < reinterpret_cast<unsigned char*>(page) + Stats_.PageSize_)
			{
				// check if the address is in the object
				if (static_cast<size_t>(address - reinterpret_cast<unsigned char*>(page)) < HeaderSize_)
				{
					throw OAException{ OAException::E_BAD_BOUNDARY, "Address not on a boundary" };
				}

				// check if the address is on a boundary
				if ((address - reinterpret_cast<unsigned char*>(page) - HeaderSize_) % InnerSize_ != 0)
				{
					throw OAException{ OAException::E_BAD_BOUNDARY, "Address not on a boundary" };
				}

				return;
			}
		}

		// move to the next page
		page = page->Next;
	}

	// if the address is not in the page list, throw an exception
	throw OAException{ OAException::E_BAD_BOUNDARY, "Address not on a boundary" };
	
}

/*!
*  \brief  gets the left sides address of an object
*
*  \param  object the object to get the address of
*
*  \return the left aligned address of the object
*/
unsigned char* ObjectAllocator::GetLeftAlignBlock(GenericObject* object) const
{
	// get the address of left alligned block
	return reinterpret_cast<unsigned char*>(object) - Config_.PadBytes_;
}

/*!
*  \brief  gets the right sides address of an object
*
*  \param  object the object to get the address of
*
*  \return the right aligned address of the object
*/
unsigned char* ObjectAllocator::GetRightAlignBlock(GenericObject* object) const
{
	// get the address of right alligned block
	return reinterpret_cast<unsigned char*>(object) + Stats_.ObjectSize_;
}
