#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

typedef void *Allocator;
typedef void *NodePtr;
typedef uint64_t Cost;

struct ResultTuple
{
  Cost cost;
  void *node;
};

struct LazyNode
{
  Allocator *allocator;
  NodePtr node;
};

struct Pair
{
  LazyNode *first;
  LazyNode *second;
};

extern "C"
{

  uint32_t no_unknown_ops();

  int64_t serialized_length(const uint8_t *program, uintptr_t length);

  char *run_clvm(const uint8_t *program,
                 uintptr_t program_length,
                 const uint8_t *args,
                 uintptr_t args_length);

  ResultTuple run_chia_program(const uint8_t *program,
                               uintptr_t program_length,
                               const uint8_t *args,
                               uintptr_t args_length,
                               Cost max_cost,
                               uint32_t flag);

  /// Free memory allocated in Rust for a pointer to a node.
  void free_node_memory(uint8_t *ptr);

  char *lazy_node_extract_atom(LazyNode *node);

  Pair *lazy_node_extract_pair(LazyNode *node);

  /// Free memory for ResultTuple
  void free_result_tuple_memory(ResultTuple result);

  void free_cstring_memory(char *ptr);

  void free_pair(Pair *pair);

  void free_lazy_node(LazyNode *node);

} // extern "C"
