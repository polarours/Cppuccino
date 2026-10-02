#include "binary_heap.hpp"

#include <functional>
#include <iostream>

int main() {
    binary_heap::BinaryHeap<int> maxHeap;  // std::less -> max on top
    for (int v : {5, 3, 8, 1, 9}) maxHeap.push(v);

    std::cout << "max-heap pop order:";
    while (!maxHeap.empty()) std::cout << " " << maxHeap.pop();
    std::cout << "\n";

    binary_heap::BinaryHeap<int, std::greater<int>> minHeap;
    minHeap.heapify({5, 3, 8, 1, 9});
    std::cout << "min-heap pop order:";
    while (!minHeap.empty()) std::cout << " " << minHeap.pop();
    std::cout << "\n";
    return 0;
}
