/********************************************************************//**
  @class utils::HeapAllocator
  Memory allocator wrapper to the standard CRT routines.

  @class utils::MemoryBlock
  Provides allocation of equal-sized objects by using a preallocated
  block of contiguous memory.

  @class utils::MemoryBlockMT
  Provides thread-safe allocation of equal-sized objects by using a
  preallocated block of contiguous memory.
  @remarks Only Acquire() and Release() are thread-safe.

  @class utils::MemoryPool
  Provides allocation of equal-sized objects by using a dynamic
  collection of memory blocks. The pool will grow or shrink as needed.

  @class utils::MemoryPoolMT
  Provides thread-safe allocation of equal-sized objects by using a
  dynamic collection of memory blocks. The pool will grow or shrink
  as needed.
  @remarks Only Acquire() and Release() are thread-safe.
*************************************************************************/

#define NDEBUG

#define uchar unsigned char

#pragma once

//#include <new>

#include <cassert>
#include <cstdlib>
#include <cmath>

#define _ALLOCATOR_RESTRICTPTR __declspec(restrict)

namespace utils
{
    /************************************************************************
      Template Class Declaration
    *************************************************************************/
    template<typename Type>
    class HeapAllocator
    {
    public:

        ///
        /// Initializes the heap allocator.
        ///
        explicit HeapAllocator ()
        : m_acquired(0),
          m_released(0)
        {
        }

        ///
        /// Initializes the heap allocator (overload for compatibility).
        ///
        explicit HeapAllocator (size_t)
        : m_acquired(0),
          m_released(0)
        {
        }

        ~HeapAllocator ()
        {
        }

        ///
        /// Acquires data.
        /// @return A pointer to the acquired data if the allocated has successfully
        /// allocated memory for the new value, NULL otherwise.
        ///
        _ALLOCATOR_RESTRICTPTR Type* Acquire ()
        {
            Type* result = (Type*)malloc(sizeof(Type));

            if (result == NULL) // Bad allocation
            {
                return NULL;
            }

            m_acquired++;
            return result;
        }

        ///
        /// Gets the count of acquired data.
        /// @return The count of acquired data.
        ///
        size_t GetAcquiredCount ()
        {
            return m_acquired; // Possible to retrieve, but leave it
        }

        ///
        /// Gets the count of released data.
        /// @return The count of released data.
        ///
        size_t GetReleasedCount ()
        {
            return m_released; // Impossible to retrieve value
        }

        ///
        /// Releases data.
        /// @param[in] object The data to be released.
        ///
        void Release (Type* object)
        {
            if (object == NULL)
            {
                return;
            }

            object->~Type();
            free(object);

            m_acquired--;
            m_released++;
        }

    protected:

        size_t m_acquired;
        size_t m_released;

    private:

        HeapAllocator (HeapAllocator const& allocator);
        HeapAllocator& operator= (HeapAllocator const& allocator);
    };

    /************************************************************************
      Template Class Declaration
    *************************************************************************/
    #define _MEMORYBLOCK_INVALID_INDEX  0xFFFFFFFF
    template<typename Type>
    class MemoryBlock
    {
    public:

        ///
        /// Initializes the memory block.
        /// @param[in] size The number of elements to be supported by the block. Must be greater
        /// than zero.
        ///
        explicit MemoryBlock (size_t size)
        : m_acquiredCount(0),
          m_chunkCount(0),
          m_free(0),
          m_elemSize((sizeof(Type) >= sizeof(size_t)) ? sizeof(Type) : sizeof(size_t))
        {
            assert(size > 0);

            m_buffer = (uchar*)malloc(size * m_elemSize);

            if (m_buffer)
            {
                m_chunkCount = size;
                _Reset(); // Initialize the buffers
            }
        }

        ~MemoryBlock ()
        {
            free(m_buffer);
        }

        ///
        /// Acquires memory to be used by a new object.
        /// @return A pointer to the acquired memory area, or NULL if the block is full.
        /// @remarks The constructor will not be called. Use 'placement new' instead to construct
        /// the object.
        ///
        _ALLOCATOR_RESTRICTPTR Type* Acquire ()
        {
            if (m_free != _MEMORYBLOCK_INVALID_INDEX)
            {
                Type* ret = (Type*)&m_buffer[m_free];
                m_free = *(size_t*)&m_buffer[m_free];
                m_acquiredCount++;
                return ret;
            }
            return NULL;
        }

        ///
        /// Checks if the specified memory address has been previously allocated by the memory block.
        /// @param[in] object Pointer with the address to be checked.
        /// @return true if the specified address has been previously allocated by the memory block,
        /// false otherwise.
        ///
        bool Contains (Type const* object) const
        {
            return (object && (uchar*)object >= m_buffer
                && (uchar*)object <= &m_buffer[m_elemSize*(m_chunkCount-1)]);
        }

        ///
        /// Gets the number of acquired elements in the block.
        /// @return The count of acquired elements in the block.
        ///
        size_t GetAcquiredCount () const
        {
            return m_acquiredCount;
        }

        ///
        /// Gets the base of an address pointing to somewhere within an object.
        /// @param[in] ptr Address pointing to somewhere within an object.
        /// @return The base address of the object, or NULL if the specified address was not allocated
        /// by this block.
        ///
        Type* GetBaseAddress (Type const* ptr) const
        {
            if (!Contains(ptr))
            {
                return NULL;
            }

            Type const* base = (Type const*)m_buffer;
            return (Type*)(base + (size_t)(ptr - base));
        }

        ///
        /// Gets the total amount of elements that are supported by the block.
        /// @return The amount of elements supported by the block.
        ///
        size_t GetCapacity () const
        {
            return m_chunkCount;
        }

        ///
        /// Gets the number of free elements in the block.
        /// @return The number of free elements in the block.
        ///
        size_t GetFreeCount () const
        {
            return (m_chunkCount - m_acquiredCount);
        }

        ///
        /// Gets the total amount of memory used by the block in bytes.
        /// @return Memory used by the block in bytes.
        ///
    /*
        uint64 GetMemoryUsage () const
        {
            return m_elemSize * m_chunkCount;
        }*/

        ///
        /// Checks if the memory block is full, that is, it cannot allocate more data.
        /// @return true if the memory block is full, false otherwise.
        ///
        bool IsFull () const
        {
            return (m_free == _MEMORYBLOCK_INVALID_INDEX);
        }

        ///
        /// Checks if the memory block is valid, that is, if it has been allocated
        /// successfully.
        /// @return true if the memory block is valid, false otherwise.
        ///
        bool IsValid () const
        {
            return (m_buffer != NULL);
        }

        ///
        /// Releases a previously acquired object and calls its destructor.
        /// @param[in] object Pointer to the memory area being released.
        ///
        void Release (Type* object)
        {
            if (object == NULL)
            {
                return;
            }
            assert(Contains(object));

            object->~Type();
            *(size_t*)object = m_free;
            m_free = (size_t)((uchar*)object - m_buffer);
            m_acquiredCount--;
        }

    protected:

        void _Reset ()
        {
            size_t i;
            for (i = 0; i < (m_chunkCount-1)*m_elemSize; i += m_elemSize)
            {
                *(size_t*)&m_buffer[i] = i + m_elemSize;
            }
            *(size_t*)&m_buffer[i] = _MEMORYBLOCK_INVALID_INDEX;

            m_free = 0;
            m_acquiredCount = 0;
        }

    protected:

        uchar* m_buffer; ///< Preallocated memory buffer.
        size_t m_chunkCount; ///< Total capacity of the buffer.
        size_t m_acquiredCount; ///< Used capacity of the buffer.
        size_t m_free; ///< Index of the next free node.
        const size_t m_elemSize; ///< Size of the chunk.

    private:

        MemoryBlock (MemoryBlock const& block);
        MemoryBlock& operator= (MemoryBlock const& block);
    };

    /************************************************************************
      Template Class Declaration
    *************************************************************************/
    template<typename Type>
    class MemoryPool
    {
        struct BlockNode
        {
            ///
            /// Initializes the block node of the memory pool.
            /// @param[in] chunkCount The chunk count of the memory block.
            ///
            BlockNode (size_t chunkCount)
            : block(chunkCount),
              next(NULL)
            {
            }

            MemoryBlock<Type> block;
            BlockNode* next;
        };

    public:
        ///
        /// Initializes the memory pool.
        /// @param[in] initialSize The number of elements to allocate initially. Must be greater
        /// than zero.
        /// @param[in] growFactor When the memory pool needs to grow, it'll allocate a new memory
        /// block with the size of the last block created scaled by this growFactor. Must be
        /// equal or greater than 1.
        ///
        explicit MemoryPool (size_t initialSize, float growFactor = 1.25f)
        : m_chunkCount(initialSize),
          m_acquired(0),
          m_blockCount(0),
          m_growCount(0),
          m_growFactor(growFactor)
        {
            assert(initialSize > 0);
            assert(growFactor >= 1.0f);

            m_root = new(std::nothrow) BlockNode(initialSize);

            if (m_root && !m_root->block.IsValid())
            {
                delete m_root;
                m_root = NULL;
            }

            m_blockCount++;
        }

        ///
        /// Releases all elements in the pool and destroys it.
        ///
        ~MemoryPool ()
        {
            BlockNode* node = m_root;

            while (node != NULL)
            {
                BlockNode* next = node->next;
                delete node;
                node = next;
            }
        }

        ///
        /// Acquires memory to be used by a new object.
        /// @return A pointer to the acquired memory area, or NULL if there is not enough memory.
        ///
        _ALLOCATOR_RESTRICTPTR Type* Acquire ()
        {
            for (BlockNode* node = m_root; node != NULL; node = node->next)
            {
                if (!node->block.IsFull())
                {
                    m_acquired++;
                    return node->block.Acquire();
                }
            }

            if (!_CreateBlock()) // Bad allocation
            {
                return NULL;
            }

            return m_root->block.Acquire();
        }

        ///
        /// Gets the number of acquired elements in the pool.
        /// @return The count of acquired elements in the pool.
        ///
        size_t GetAcquiredCount () const
        {
            return m_acquired;
        }

        ///
        /// Gets the number of memory blocks that the pool contains.
        /// @return The number of memory blocks that the pool contains.
        ///
        size_t GetBlockCount () const
        {
            return m_blockCount;
        }

        ///
        /// Gets the number of elements that the memory pool currently supports.
        /// @return The number of elements that the memory pool currently supports.
        ///
        size_t GetCapacity () const
        {
            return m_chunkCount;
        }

        ///
        /// Gets the number of free elements in the pool.
        /// @return The number of free elements in the pool.
        ///
        size_t GetFreeCount () const
        {
            return (m_chunkCount - m_acquired);
        }

        ///
        /// Gets the number of times the pool has grown.
        /// @return The number of times the pool has grown.
        ///
        size_t GetGrowCount () const
        {
            return m_growCount;
        }

        ///
        /// Gets the grow factor of the pool.
        /// @return The grow factor of the pool.
        ///
        float GetGrowFactor () const
        {
            return m_growFactor;
        }

        ///
        /// Gets the total amount of memory used by the block in bytes.
        /// @return Memory used by the block in bytes.
        ///
        /*
        uint64 GetMemoryUsage () const
        {
            uint64 totalMemory = 0;
            for (BlockNode* node = m_root; node != NULL; node = node->next)
            {
                totalMemory += node->block.GetMemoryUsage();
            }
            return totalMemory;
        }*/

        ///
        /// Checks if the memory pool is valid, that is, if it has been successfully allocated.
        /// @return true if the memory pool is valid, false otherwise.
        ///
        bool IsValid () const
        {
            return (m_root && m_root->block.IsValid());
        }

        ///
        /// Releases a previously acquired object.
        /// @param[in] object Pointer to the memory area being released.
        ///
        void Release (Type* object)
        {
            BlockNode* node;
            BlockNode* prev;

            for (prev = NULL, node = m_root; node != NULL; node = node->next)
            {
                if (node->block.Contains(object))
                {
                    m_acquired--;
                    node->block.Release(object);
                    break;
                }

                prev = node;
            }

            // If we have more than one block and the oldest one is smaller and empty, shrink the pool.
            if (prev != NULL && node->next == NULL && node->block.GetAcquiredCount() == 0
                && node->block.GetCapacity() < prev->block.GetCapacity())
            {
                delete node;
                prev->next = NULL;
                m_blockCount--;
            }
        }

        ///
        /// Sets the grow factor of the pool.
        /// @param[in] growFactor The desired grow factor of the pool. Must be equal or greater than 1.
        ///
        void SetGrowFactor (float growFactor)
        {
            assert(growFactor >= 1.0f);
            m_growFactor = growFactor;
        }

    protected:

        ///
        /// Creates a new memory block.
        /// @return true if the memory pool has successfully allocated the new block,
        /// false otherwise.
        ///
        bool _CreateBlock ()
        {
            size_t chunkCount = (size_t)ceil(m_root->block.GetCapacity() * m_growFactor);
            assert(chunkCount > 0);
            BlockNode* node = new(std::nothrow) BlockNode(chunkCount);

            if (node == NULL || !node->block.IsValid())
            {
                return false;
            }

            node->next = m_root;
            m_root = node;
            m_chunkCount += chunkCount;
            m_blockCount++;
            m_growCount++;
            return true;
        }

        BlockNode* m_root;
        size_t m_chunkCount;
        size_t m_acquired;
        size_t m_blockCount;
        size_t m_growCount;
        float m_growFactor;

    private:

        MemoryPool (MemoryPool const& pool);
        MemoryPool& operator= (MemoryPool const& pool);
    };

}

/************************************************************************
  Inline Methods
*************************************************************************/

template<typename Type>
inline _ALLOCATOR_RESTRICTPTR void* operator new (size_t, utils::HeapAllocator<Type>* alloc)
{
    return alloc->Acquire();
}

template<typename Type>
inline void operator delete (void* object, utils::HeapAllocator<Type>* alloc)
{
    return alloc->Release((Type*)object);
}

template<typename Type>
inline _ALLOCATOR_RESTRICTPTR void* operator new (size_t, utils::HeapAllocator<Type>& alloc)
{
    return alloc.Acquire();
}

template<typename Type>
inline void operator delete (void* object, utils::HeapAllocator<Type>& alloc)
{
    return alloc.Release((Type*)object);
}


template<typename Type>
inline _ALLOCATOR_RESTRICTPTR void* operator new (size_t, utils::MemoryBlock<Type>* block)
{
    return block->Acquire();
}

template<typename Type>
inline void operator delete (void* object, utils::MemoryBlock<Type>* block)
{
    return block->Release((Type*)object);
}

template<typename Type>
inline _ALLOCATOR_RESTRICTPTR void* operator new (size_t, utils::MemoryBlock<Type>& block)
{
    return block.Acquire();
}

template<typename Type>
inline void operator delete (void* object, utils::MemoryBlock<Type>& block)
{
    return block.Release((Type*)object);
}


template<typename Type>
inline _ALLOCATOR_RESTRICTPTR void* operator new (size_t, utils::MemoryPool<Type>* pool)
{
    return pool->Acquire();
}

template<typename Type>
inline void operator delete (void* object, utils::MemoryPool<Type>* pool)
{
    return pool->Release((Type*)object);
}

template<typename Type>
inline _ALLOCATOR_RESTRICTPTR void* operator new (size_t, utils::MemoryPool<Type>& pool)
{
    return pool.Acquire();
}

template<typename Type>
inline void operator delete (void* object, utils::MemoryPool<Type>& pool)
{
    return pool.Release((Type*)object);
}


#undef _ALLOCATOR_RESTRICTPTR
