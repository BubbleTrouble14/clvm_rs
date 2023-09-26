#include <iostream>
#include "clvm.h" // Include the generated header
#include <gtest/gtest.h>

// Define your test cases
TEST(ChiaProgramTest, Test1)
{
    const uint8_t program_data[] = {0xff, 0x01, 0x7f};
    const uint8_t args_data[] = {0x80};
    uintptr_t program_len = sizeof(program_data);
    uintptr_t args_len = sizeof(args_data);

    // Adjusted to use ResultTuple instead of the previous struct
    ResultTuple result = run_chia_program(program_data, program_len, args_data, args_len, 100000000000, 0);

    // Assuming the node pointer holds binary data and the first byte of this data is what you want to compare
    if (result.node != nullptr)
    {
        LazyNode *lazyNode = reinterpret_cast<LazyNode *>(result.node);
        char *maybeAtom = lazy_node_extract_atom(lazyNode);
        if (maybeAtom != nullptr)
        {
            auto first_byte_of_atom = static_cast<uint8_t>(maybeAtom[0]);
            EXPECT_EQ(static_cast<int>(first_byte_of_atom), 127);
            free_cstring_memory(maybeAtom); // Remember to free after use
        }
        else
        {
            // Handle case where it's not an atom, potentially a pair.
        }
    }
    else
    {
        FAIL() << "result.node is nullptr";
    }

    free_result_tuple_memory(result);
}

TEST(ChiaProgramTest, TestPlusOneAndThree)
{
    const uint8_t program_data[] = {0xff, 0x10, 0xff, 0x01, 0xff, 0xff, 0x01, 0x03, 0x80};
    const uint8_t args_data[] = {0x02};
    uintptr_t program_len = sizeof(program_data);
    uintptr_t args_len = sizeof(args_data);

    ResultTuple result = run_chia_program(program_data, program_len, args_data, args_len, 100000000000, 0);
    if (result.node != nullptr)
    {
        LazyNode *lazyNode = reinterpret_cast<LazyNode *>(result.node);
        char *maybeAtom = lazy_node_extract_atom(lazyNode);
        if (maybeAtom != nullptr)
        {
            auto first_byte_of_atom = static_cast<uint8_t>(maybeAtom[0]);
            EXPECT_EQ(static_cast<int>(first_byte_of_atom), 5);
            free_cstring_memory(maybeAtom);
        }
        else
        {
            FAIL() << "Expected atom, got pair or invalid type";
        }
    }
    else
    {
        FAIL() << "result.node is nullptr";
    }
    free_result_tuple_memory(result);
}

TEST(ChiaProgramTest, TestPlusSevenAndThreeNested)
{
    const uint8_t program_data[] = {0xff, 0x10, 0xff, 0x07, 0xff, 0xff, 0x01, 0x03, 0x80};
    const uint8_t args_data[] = {0xff, 0x80, 0xff, 0x80, 0x02};
    uintptr_t program_len = sizeof(program_data);
    uintptr_t args_len = sizeof(args_data);

    ResultTuple result = run_chia_program(program_data, program_len, args_data, args_len, 100000000000, 0);
    if (result.node != nullptr)
    {
        LazyNode *lazyNode = reinterpret_cast<LazyNode *>(result.node);
        char *maybeAtom = lazy_node_extract_atom(lazyNode);
        if (maybeAtom != nullptr)
        {
            auto first_byte_of_atom = static_cast<uint8_t>(maybeAtom[0]);
            EXPECT_EQ(static_cast<int>(first_byte_of_atom), 5);
            free_cstring_memory(maybeAtom);
        }
        else
        {
            FAIL() << "Expected atom, got pair or invalid type";
        }
    }
    else
    {
        FAIL() << "result.node is nullptr";
    }
    free_result_tuple_memory(result);
}

TEST(ChiaProgramTest, TestDivmod)
{
    const uint8_t program_data[] = {0xff, 0x14, 0xff, 0xff, 0x01, 0x05, 0xff, 0xff, 0x01, 0x81, 0xfd, 0x80};
    const uint8_t args_data[] = {0x80};
    uintptr_t program_len = sizeof(program_data);
    uintptr_t args_len = sizeof(args_data);

    ResultTuple result = run_chia_program(program_data, program_len, args_data, args_len, 100000000000, 0);
    if (result.node != nullptr)
    {
        LazyNode *lazyNode = reinterpret_cast<LazyNode *>(result.node);
        Pair *maybePair = lazy_node_extract_pair(lazyNode);
        if (maybePair != nullptr)
        {
            char *firstAtom = lazy_node_extract_atom(maybePair->first);
            char *secondAtom = lazy_node_extract_atom(maybePair->second);
            if (firstAtom != nullptr && secondAtom != nullptr)
            {
                auto first_byte_of_first_atom = static_cast<int8_t>(firstAtom[0]);
                auto first_byte_of_second_atom = static_cast<int8_t>(secondAtom[0]);
                EXPECT_EQ(static_cast<int>(first_byte_of_first_atom), -2);  // This may need to be adjusted depending on data format
                EXPECT_EQ(static_cast<int>(first_byte_of_second_atom), -1); // This may need to be adjusted depending on data format
                free_cstring_memory(firstAtom);
                free_cstring_memory(secondAtom);
            }
            else
            {
                FAIL() << "Expected both atoms in the pair, but one or both are missing or invalid";
            }
            free_pair(maybePair);
        }
        else
        {
            FAIL() << "Expected pair, got atom or invalid type";
        }
    }
    else
    {
        FAIL() << "result.node is nullptr";
    }
    free_result_tuple_memory(result);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    // // Test Case: Test '(q . 127)' '()'
    // // (q . 127)
    // const uint8_t program_data[] = {0xff, 0x01, 0x7f};
    // // ()
    // const uint8_t args_data[] = {0x80};

    // uintptr_t program_len = sizeof(program_data);
    // uintptr_t args_len = sizeof(args_data);

    // std::cout << "Running the test case for '(q . 127)' '()'" << std::endl;

    // // 100,000,000,000
    // const uint64_t max_cost = 100000000000;
    // const uint32_t flag = 0; // Assuming the flag is uint32_t from the header definition

    // // Call the Rust FFI function
    // ChiaProgramResult result = run_chia_program(program_data, program_len, args_data, args_len, max_cost, flag);

    // // Check for errors
    // if (result.error != nullptr)
    // {
    //     std::cerr << "Error: " << result.error << std::endl;
    //     // Free the memory of the result struct after using it
    //     free_chia_program_result(result);
    //     return -1; // Return an error code
    // }
    // else
    // {
    //     // Use the result
    //     std::cout << "Cost: " << result.cost << std::endl;

    //     // Printing the node data. Assuming it's not a null-terminated string, but binary data.
    //     std::cout << "Node Data:";
    //     for (uintptr_t i = 0; i < result.node_len; ++i)
    //     {
    //         std::cout << " " << static_cast<int>(result.node[i]); // Display as integers
    //     }
    //     std::cout << std::endl;

    //     // Free allocated memory
    //     free_chia_program_result(result);
    // }

    // return 0;
}
