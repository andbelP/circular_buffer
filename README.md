# Circular Buffer (STL-Compatible Container)

Personal project implementing an STL-compatible circular buffer (ring buffer) container.

## Goal

Implement an STL-compatible container for a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer). The container provides a configurable maximum size and optional capacity growth.

## Requirements

The container is a class template parameterized by the stored value type and an allocator. The maximum number of stored elements is provided via the constructor. It partially satisfies the following standard named requirements for STL-compatible containers:

- [Container](https://en.cppreference.com/w/cpp/named_req/Container)
- [SequenceContainer](https://en.cppreference.com/w/cpp/named_req/SequenceContainer) with the following methods intentionally not provided:
  - emplace
  - assign_range
  - emplace_front
  - emplace_back
  - prepend_range
- [ReversibleContainer](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer)
- [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)
- Provides a [RandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)

Additionally, the container provides the following operations with expected time complexity:

| Operation   | Complexity |
|-------------|------------|
| clear       | O(N)       |
| push_back   | O(1)       |
| pop_back    | O(1)       |
| push_front  | O(1)       |
| pop_front   | O(1)       |
| insert      | O(N)       |
| erase       | O(N)       |

## Tests

The above requirements are covered by tests using [Google Test](http://google.github.io/googletest).

### Local Coverage Check

You can measure test coverage locally. Install `gcovr`, build the project with coverage enabled, and run the `coverage` target:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DWITH_COVERAGE=ON
cmake --build build
cmake --build build --target coverage
```

After completion, a detailed report will be available at `build/coverage.html`.

## Optional Capacity Growth

The container can optionally grow its maximum size. When the buffer reaches its current capacity, the maximum size doubles (similar to `std::vector`).

To enable tests for the expandable buffer, set `RUN_EXT_TESTS` to `ON` in `tests/CMakeLists.txt`.

## Restrictions

- Standard containers and adapters must not be used.
