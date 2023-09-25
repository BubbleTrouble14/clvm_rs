#include <iostream>
#include "clvm.h" // Include the generated header

int main()
{
    // Test Case: Test '(q . 127)' '()'
    // (q . 127)
    const uint8_t program_data[] = {0xff, 0x01, 0x7f};
    // ()
    const uint8_t args_data[] = {0x80};

    uintptr_t program_len = sizeof(program_data);
    uintptr_t args_len = sizeof(args_data);

    std::cout << "Running the test case for '(q . 127)' '()'" << std::endl;

    // 100,000,000,000
    const uint64_t max_cost = 100000000000;
    const uint32_t flag = 0; // Assuming the flag is uint32_t from the header definition

    // Call the Rust FFI function
    ChiaProgramResult result = run_chia_program(program_data, program_len, args_data, args_len, max_cost, flag);

    // Check for errors
    if (result.error != nullptr)
    {
        std::cerr << "Error: " << result.error << std::endl;
        // Free the memory of the result struct after using it
        free_chia_program_result(result);
        return -1; // Return an error code
    }
    else
    {
        // Use the result
        std::cout << "Cost: " << result.cost << std::endl;

        // Printing the node data. Assuming it's not a null-terminated string, but binary data.
        std::cout << "Node Data:";
        for (uintptr_t i = 0; i < result.node_len; ++i)
        {
            std::cout << " " << static_cast<int>(result.node[i]); // Display as integers
        }
        std::cout << std::endl;

        // Free allocated memory
        free_chia_program_result(result);
    }

    return 0;
}
