// examples/pitfall-memory-fragmentation.cpp
// Demonstrates heap fragmentation from interleaved frees and the arena/pool fix.
// Compile with: g++ -std=c++17 -o pitfall_memory_fragmentation pitfall-memory-fragmentation.cpp

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory_resource>
#include <vector>

namespace {

constexpr int kBlocks = 1000;         // number of small allocations
constexpr int kSizeSpread = 8;        // block sizes cycle through 1..8 ints
constexpr std::size_t kArenaBytes = 64 * 1024;

// Counts the traffic that goes to the general purpose heap.
class CountingResource final : public std::pmr::memory_resource {
public:
    std::size_t allocations = 0;
    std::size_t deallocations = 0;

private:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        ++allocations;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }

    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override {
        ++deallocations;
        std::pmr::new_delete_resource()->deallocate(ptr, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

int blockInts(int index) { return 1 + (index % kSizeSpread); }

// A simple model of the heap: every block is either live or free, and the
// free blocks sit in the gaps between the live ones.
struct HeapModel {
    std::vector<bool> live;

    std::size_t total_free_bytes() const {
        std::size_t total = 0;
        for (std::size_t i = 0; i < live.size(); ++i) {
            if (!live[i]) {
                total += static_cast<std::size_t>(blockInts(static_cast<int>(i))) * sizeof(int);
            }
        }
        return total;
    }

    // Bytes in the biggest run of consecutive free blocks.
    std::size_t largest_hole_bytes() const {
        std::size_t largest = 0;
        std::size_t current = 0;
        for (std::size_t i = 0; i < live.size(); ++i) {
            if (live[i]) {
                current = 0;
            } else {
                current += static_cast<std::size_t>(blockInts(static_cast<int>(i))) * sizeof(int);
                largest = std::max(largest, current);
            }
        }
        return largest;
    }
};

}  // namespace

// --- 1. The fragmented heap: allocate many blocks, free every other one ---

void fragmentedHeap() {
    std::cout << "--- Fragmented heap: free every other block ---\n";

    CountingResource heap;
    std::pmr::polymorphic_allocator<int> alloc(&heap);

    HeapModel model;
    model.live.reserve(kBlocks);

    std::vector<int*> blocks;
    blocks.reserve(kBlocks);
    for (int i = 0; i < kBlocks; ++i) {
        int* p = alloc.allocate(static_cast<std::size_t>(blockInts(i)));
        p[0] = i;
        blocks.push_back(p);
        model.live.push_back(true);
    }

    // Free every other block: the surviving blocks wedge the holes apart.
    for (std::size_t i = 0; i < blocks.size(); i += 2) {
        alloc.deallocate(blocks[i], static_cast<std::size_t>(blockInts(static_cast<int>(i))));
        model.live[i] = false;
    }

    std::cout << "general heap allocations:   " << heap.allocations << "\n";
    std::cout << "general heap deallocations: " << heap.deallocations << "\n";
    std::cout << "blocks still live:          " << (blocks.size() - heap.deallocations) << "\n";
    std::cout << "total free bytes:           " << model.total_free_bytes() << "\n";
    std::cout << "largest contiguous hole:    " << model.largest_hole_bytes() << " bytes\n";

    const std::size_t request = 64;  // a 64 byte request
    std::cout << "a " << request << " byte request fits in a hole: "
              << (model.largest_hole_bytes() >= request ? "yes" : "NO")
              << " (yet " << model.total_free_bytes() << " bytes are free in total)\n";

    // Release the survivors so the process leaves no garbage behind.
    for (std::size_t i = 1; i < blocks.size(); i += 2) {
        alloc.deallocate(blocks[i], static_cast<std::size_t>(blockInts(static_cast<int>(i))));
    }
}

// --- 2. The fix: one arena for the whole workload, freed in bulk ---

void arenaAllocation() {
    std::cout << "\n--- Arena allocation: one region, bulk free ---\n";

    // The arena is a single contiguous block that owns every allocation below.
    std::vector<char> arena(kArenaBytes);
    CountingResource general_heap;
    std::pmr::monotonic_buffer_resource pool(arena.data(), arena.size(), &general_heap);
    std::pmr::polymorphic_allocator<int> alloc(&pool);

    std::vector<int*> blocks;
    blocks.reserve(kBlocks);
    std::size_t used_bytes = 0;
    for (int i = 0; i < kBlocks; ++i) {
        std::size_t count = static_cast<std::size_t>(blockInts(i));
        int* p = alloc.allocate(count);
        p[0] = i;
        blocks.push_back(p);
        used_bytes += count * sizeof(int);
    }

    // No per block free: the whole arena is reclaimed in one step when it
    // goes out of scope, so no holes can form.
    std::cout << "general heap allocations for " << kBlocks << " blocks: " << general_heap.allocations
              << "\n";
    std::cout << "arena bytes requested: " << used_bytes << " of " << kArenaBytes << "\n";
    std::cout << "arena bytes left over: " << (kArenaBytes - used_bytes) << "\n";
    std::cout << "first and last value: " << blocks.front()[0] << ", " << blocks.back()[0] << "\n";
}

// --- 3. Prefer contiguous containers over many small allocations ---

void contiguousContainer() {
    std::cout << "\n--- One contiguous container instead of many pointers ---\n";

    CountingResource scattered_heap;
    {
        std::pmr::polymorphic_allocator<int> alloc(&scattered_heap);
        std::vector<int*> blocks;
        blocks.reserve(kBlocks);
        for (int i = 0; i < kBlocks; ++i) {
            int* p = alloc.allocate(static_cast<std::size_t>(blockInts(i)));
            p[0] = i;
            blocks.push_back(p);
        }
        std::cout << "scattered blocks: " << scattered_heap.allocations << " allocations\n";
        for (std::size_t i = 0; i < blocks.size(); ++i) {
            alloc.deallocate(blocks[i], static_cast<std::size_t>(blockInts(static_cast<int>(i))));
        }
    }

    CountingResource vector_heap;
    {
        std::pmr::vector<int> contiguous(&vector_heap);
        contiguous.reserve(static_cast<std::size_t>(kBlocks) * kSizeSpread);
        for (int i = 0; i < kBlocks; ++i) {
            for (int j = 0; j < blockInts(i); ++j) {
                contiguous.push_back(i);
            }
        }
        std::cout << "contiguous vector: " << vector_heap.allocations << " allocation, "
                  << contiguous.size() << " elements\n";
    }
}

int main() {
    fragmentedHeap();
    arenaAllocation();
    contiguousContainer();

    std::cout << "\n=== All memory fragmentation scenarios completed ===\n";
    return 0;
}
