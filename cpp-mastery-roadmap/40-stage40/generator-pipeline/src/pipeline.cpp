#include "pipeline.hpp"

namespace pipeline {

std::generator<int> iota(int from, int to) {
    for (int i = from; i < to; ++i) co_yield i;
}

}  // namespace pipeline
