#include <stdio.h>
#include <assert.h>
#include <utility>
#include <cstdint>
#include <unistd.h>

/// <summary>
/// Machine word size. Depending on the architecture,
/// can be 4 or 8 bytes.
/// </summary>
using word_t = intptr_t;

/*
 * Mode for searching a free block.
 */
enum class SearchMode {
    FirstFit,
    NextFit
};

/// <summary>
/// Allocated block of memory. Contains the object header structure,
/// and the actual payload pointer.
/// </summary>
struct Block {
	// -----------------------
	// 1. Object header

	/* 
	 * Block Size
	 */
	size_t size;

	/* 
	 * Whether this block is currently used.
	 */
	bool used;

	/*
	 * Next block in the list.
	 */
	Block* next;

	// ----------------------
	// 2. User data

	/*
	 * Payload pointer
	 */
	word_t data[1];

};

/*
 * Heap start. Initialized on first allocation.
 */
static Block *heapStart = nullptr;

/*
 * Current top. Updated on each allocation.
 */
static auto top = heapStart;

/* Previously found block. Updated in nextFit. */
static Block *searchStart = heapStart;

/* Search mode currently used. */
static auto searchMode = SearchMode::FirstFit;

/* Reset the heap to the original position. */
void resetHeap() {
    // If already reset.
    if (heapStart == nullptr) {
        return;
    }

    // Rollback to the beginning.
    brk(heapStart);

    heapStart = nullptr;
    top = nullptr;
    searchStart = nullptr;
}

/* Initializes the heap, and the search mode. */
void initHeap(SearchMode mode) {
    searchMode = mode;
    resetHeap();
}

/*
 * Returns the object header.
 */
Block *getHeader(word_t *data) {
	return (Block *)((char *)data + sizeof(std::declval<Block>().data) - sizeof(Block));
}

/*
 * Aligns the size by the machine word.
 * Alignment allows faster access, usually by size of the machine word.
 * If a user requests to allocate, say, 6 bytes, we actually allocate 8
 * bytes. Allocating 4 bytes can result either to 4 bytes on 32 bit, or 
 * 8 bytes on 64 bit architectures.
 */
inline size_t align(size_t n) {
	return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

/*
 * Returns total allocation size, reserving in addition the space for
 * the Block structure (object header + first data word).
 * 
 * Since the 'word_t data[1]' already allocates one word inside the Block
 * structure, we decrease it from the size request: if a user allocates
 * only one word, it's fully in the Block struct.
 */
inline size_t allocSize(size_t size) {
	return size + sizeof(Block) - sizeof(std::declval<Block>().data);
}

/*
 * Requests(maps) memory from OS.
 * Used only when no free blocks available in our linked list of blocks.
 * Otherwise, reuse a free block.
 */
Block *requestFromOS(size_t size) {
	// Current heap break.
	auto block = (Block *)sbrk(0);				// (1), beginning position of newly allocated block.

	// OOM.
	if (sbrk(allocSize(size)) == (void *)-1) {	// (2), passing the amount of bytes on which we should
		return nullptr;							// increase the break position. If call results in (void *)-1,
	}											// signal about OOM, return nullptr. Otherwise, return the
												// obtained address in (1) of the allocated block.
	return block;
}

/*
 * Frees a previously allocated block.
 */
void free(word_t *data) {
    auto block = getHeader(data);
    block->used = false;
}

/*
 * First fit algorithm
 * 
 * Returns the first free block which fits the size.
 */
Block *firstFit(size_t size) {
    auto block = heapStart;

    while (block != nullptr) {
        if (block->used == true || block->size < size) {
            block = block->next;
            continue;
        }
        else {
            return block;
        }        
    }
    return nullptr;
}

/*
 * Tries to find a block of a needed size.
 */
Block *findBlock(size_t size) {
    return firstFit(size);
}

/*
 * Allocates a block of memory of at least 'size' bytes.
 * At least because of padding or alignment.
 */
word_t *alloc(size_t size) {
	size = align(size);

    // Search for an available free block:
    if (auto block = findBlock(size)) {
        return block->data;
    }

    // If block not found in the free list, request from OS:
	auto block = requestFromOS(size);

	block->size = size;
	block->used = true;

	// Init heap.
	if (heapStart == nullptr) {
		heapStart = block;
	}

	// Chain the blocks
	if (top != nullptr) {
		top->next = block;
	}

	top = block;

	// User payload:
	return block->data;
}

// void experimentAlignment() {
// 	// Assuming 64 bit architecture
// 	auto ret = align(1); // 8
// 	printf("64 bit Alignment of 1 bytes: %d bytes, expected 8 bytes\n", (int)ret);
// 	ret = align(3); // 8
// 	printf("64 bit Alignment of 3 bytes: %d bytes, expected 8 bytes\n", (int)ret);
// 	ret = align(8); 	 // 8
// 	printf("64 bit Alignment of 8 bytes: %d bytes, expected 8 bytes\n", (int)ret);
// 	ret = align(12);	 // 16
// 	printf("64 bit Alignment of 12 bytes: %d bytes, expected 16 bytes\n", (int)ret);
// 	ret = align(16);	 // 16
// 	printf("64 bit Alignment of 16 bytes: %d bytes, expected 16 bytes\n", (int)ret);

// 	// Assuming 32 bit architecture
// 	ret = align(3); 	 // 4
// 	printf("32 bit Alignment of 3 bytes: %d bytes, expected 4 bytes\n", (int)ret);
// 	ret = align(8); 	 // 8
// 	printf("32 bit Alignment of 8 bytes: %d bytes, expected 8 bytes\n", (int)ret);
// 	ret = align(12);	 // 12
// 	printf("32 bit Alignment of 12 bytes: %d bytes, expected 12 bytes\n", (int)ret);
// 	ret = align(16);	 // 16
// 	printf("32 bit Alignment of 16 bytes: %d bytes, expected 16 bytes\n", (int)ret);
// }

// void experimentAllocation() {
// 	// Test case 1: Alignment
// 	// A request for 3 bytes is aligned to 8.

// 	auto p1 = alloc(3);
// 	auto p1b = getHeader(p1);
// 	assert(p1b->size == sizeof(word_t));

// 	// Test case 2: Exact amount of aligned bytes.
// 	auto p2 = alloc(8);
// 	auto p2b = getHeader(p2);
// 	assert(p2b->size == 8);

//     // Test case 3: Free the object.
//     free(p2);
//     assert(p2b->used == false);

//     // Test case 4: The block is reused.
//     auto p3 = alloc(8);
//     auto p3b = getHeader(p3);
//     assert(p3b->size == 8);
//     assert(p3b == p2b); // Reused!

// 	printf("All assertions passed!\n");
// }