#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct ByteBuffer {
  uint8_t *data;
  uintptr_t len;
};

struct ChiaProgramResult {
  uint64_t cost;
  uint8_t *node;
  uintptr_t node_len;
  char *error;
};

extern "C" {

ByteBuffer run_clvm(const uint8_t *program_data,
                    uintptr_t program_len,
                    const uint8_t *args_data,
                    uintptr_t args_len);

void free_buffer(ByteBuffer buffer);

ChiaProgramResult run_chia_program(const uint8_t *program_data,
                                   uintptr_t program_len,
                                   const uint8_t *args_data,
                                   uintptr_t args_len,
                                   uint64_t max_cost,
                                   uint32_t flag);

void free_chia_program_result(ChiaProgramResult result);

} // extern "C"
