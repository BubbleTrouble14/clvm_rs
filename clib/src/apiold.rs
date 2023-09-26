#[allow(unused_imports)]
use std::os::raw::{c_char, c_void};
use std::slice;
use std::ffi::CString;

use clvmr::allocator::Allocator;
use clvmr::chia_dialect::ChiaDialect;
use clvmr::cost::Cost;
use clvmr::run_program::run_program;
use clvmr::serde::{node_from_bytes, node_to_bytes};

// Define a ByteBuffer struct to represent a block of bytes in memory.
#[repr(C)]
pub struct ByteBuffer {
    data: *mut u8,
    len: usize,
}

impl ByteBuffer {
    // Convert a Vec<u8> into a ByteBuffer without deallocating the memory.
    fn from_vec(mut vec: Vec<u8>) -> Self {
        vec.shrink_to_fit();
        let data = vec.as_mut_ptr();
        let len = vec.len();
        std::mem::forget(vec);
        ByteBuffer { data, len }
    }
}

// The main function to run CLVM.
#[no_mangle]
pub extern "C" fn run_clvm(program_data: *const u8, program_len: usize, args_data: *const u8, args_len: usize) -> ByteBuffer {
    let program = unsafe { slice::from_raw_parts(program_data, program_len) };
    let args = unsafe { slice::from_raw_parts(args_data, args_len) };

    let max_cost: Cost = 1_000_000_000_000_000;

    let mut allocator = Allocator::new();
    let program = node_from_bytes(&mut allocator, program).unwrap();
    let args = node_from_bytes(&mut allocator, args).unwrap();

    let r = run_program(
        &mut allocator,
        &ChiaDialect::new(0),
        program,
        args,
        max_cost,
    );
    
    let result = match r {
        Ok(reduction) => node_to_bytes(&allocator, reduction.1).unwrap(),
        Err(_eval_err) => format!("{:?}", _eval_err).into_bytes(),
    };

    ByteBuffer::from_vec(result)
}

// Function to free the memory of a ByteBuffer.
#[no_mangle]
pub extern "C" fn free_buffer(buffer: ByteBuffer) {
    unsafe {
        let _ = Vec::from_raw_parts(buffer.data, buffer.len, buffer.len);
    }
}

// Define a structure to represent the result.
#[repr(C)]
pub struct ChiaProgramResult {
    cost: u64,
    node: *mut u8,
    node_len: usize,
    error: *mut c_char,
}

// The main function to run the Chia program.
#[no_mangle]
pub extern "C" fn run_chia_program(
    program_data: *const u8, program_len: usize,
    args_data: *const u8, args_len: usize,
    max_cost: u64,
    flag: u32,
) -> ChiaProgramResult {
    let program = unsafe { slice::from_raw_parts(program_data, program_len) };
    let args = unsafe { slice::from_raw_parts(args_data, args_len) };

    let mut allocator = Allocator::new();
    let program = node_from_bytes(&mut allocator, program).unwrap();
    let args = node_from_bytes(&mut allocator, args).unwrap();
    let dialect = ChiaDialect::new(flag);

    let r = run_program(&mut allocator, &dialect, program, args, max_cost);
    match r {
        Ok(reduction) => {
            let cost = reduction.0;
            let node_data = node_to_bytes(&allocator, reduction.1).unwrap();
            let node_ptr = node_data.as_ptr() as *mut u8; // Casting to *mut u8
            let node_len = node_data.len();
            std::mem::forget(node_data);
            ChiaProgramResult {
                cost,
                node: node_ptr,
                node_len,
                error: std::ptr::null_mut(),
            }
        }
        Err(_eval_err) => {
            let error_string = format!("{:?}", _eval_err);
            let error_cstring = CString::new(error_string).unwrap();
            let error_ptr = error_cstring.into_raw();
            ChiaProgramResult {
                cost: 0,
                node: std::ptr::null_mut(),
                node_len: 0,
                error: error_ptr,
            }
        }
    }
}

// Function to free the memory of the result.
#[no_mangle]
pub extern "C" fn free_chia_program_result(result: ChiaProgramResult) {
    unsafe {
        if !result.node.is_null() {
            let _ = Vec::from_raw_parts(result.node, result.node_len, result.node_len);
        }
        if !result.error.is_null() {
            let _ = CString::from_raw(result.error);
        }
    }
}
