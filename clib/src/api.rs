use clvmr::allocator::Allocator;
use clvmr::chia_dialect::ChiaDialect;
use clvmr::chia_dialect::NO_UNKNOWN_OPS as _no_unknown_ops;
use clvmr::cost::Cost;
use clvmr::run_program::run_program;
use clvmr::serde::{node_from_bytes, node_to_bytes, serialized_length_from_bytes};
use crate::lazy_node::{LazyNode, Pair};
use std::os::raw::{c_char, c_void};
use std::ffi::CString;
use std::alloc::{dealloc, Layout};

#[repr(C)]
pub struct ResultTuple {
    pub cost: Cost,
    pub node: *mut c_void, 
}

#[no_mangle]
pub extern "C" fn no_unknown_ops() -> u32 {
    _no_unknown_ops
}

#[no_mangle]
pub extern "C" fn serialized_length(program: *const u8, length: usize) -> i64 {
    let program_slice = unsafe { std::slice::from_raw_parts(program, length) };
    match serialized_length_from_bytes(program_slice) {
        Ok(len) => len as i64,
        Err(_) => -1,  
    }
}

#[no_mangle]
pub extern "C" fn run_clvm(program: *const u8, program_length: usize, args: *const u8, args_length: usize) -> *mut c_char {
    let max_cost: Cost = 1_000_000_000_000_000;
    let program_slice = unsafe { std::slice::from_raw_parts(program, program_length) };
    let args_slice = unsafe { std::slice::from_raw_parts(args, args_length) };

    let mut allocator = Allocator::new();
    let program = node_from_bytes(&mut allocator, program_slice).expect("Failed to get program from bytes");
    let args = node_from_bytes(&mut allocator, args_slice).expect("Failed to get args from bytes");

    let r = run_program(
        &mut allocator,
        &ChiaDialect::new(0),
        program,
        args,
        max_cost,
    );

    let result_str = match r {
        Ok(reduction) => {
            let bytes = node_to_bytes(&allocator, reduction.1).expect("Failed to get bytes from reduction");
            format!("{:?}", bytes)
        },
        Err(eval_err) => format!("{:?}", eval_err),
    };

    let c_str = CString::new(result_str).expect("Failed to convert to CString");
    c_str.into_raw()
}

#[no_mangle]
pub extern "C" fn run_chia_program(
    program: *const u8, 
    program_length: usize, 
    args: *const u8, 
    args_length: usize,
    max_cost: Cost, 
    flag: u32
) -> ResultTuple {
    let program_slice = unsafe { std::slice::from_raw_parts(program, program_length) };
    let args_slice = unsafe { std::slice::from_raw_parts(args, args_length) };

    let mut allocator = Allocator::new();
    let program = node_from_bytes(&mut allocator, program_slice).expect("Failed to get program from bytes");
    let args = node_from_bytes(&mut allocator, args_slice).expect("Failed to get args from bytes");
    let dialect = ChiaDialect::new(flag);

    let r = run_program(&mut allocator, &dialect, program, args, max_cost);

    match r {
        Ok(reduction) => ResultTuple {
            cost: reduction.0,
            node: Box::into_raw(Box::new(reduction.1)) as *mut c_void,
        },
        Err(_) => ResultTuple {
            cost: 0,   
            node: std::ptr::null_mut(),
        },
    }
}

/// Free memory allocated in Rust for a pointer to a node.
#[no_mangle]
pub extern "C" fn free_node_memory(ptr: *mut u8) {
    if !ptr.is_null() {
        unsafe {
            dealloc(ptr, Layout::new::<u8>());
        }
    }
}

#[no_mangle]
pub extern "C" fn lazy_node_extract_atom(node: *mut LazyNode) -> *mut c_char {
    let ln = unsafe { &*node };
    match ln.atom() {
        Some(vec) => {
            let c_str = CString::new(vec).expect("Failed to convert to CString");
            c_str.into_raw()
        },
        None => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn lazy_node_extract_pair(node: *mut LazyNode) -> *mut Pair {
    let ln = unsafe { &*node };
    ln.pair()
}

/// Free memory for ResultTuple
#[no_mangle]
pub extern "C" fn free_result_tuple_memory(result: ResultTuple) {
    // If there are other fields in ResultTuple that need deallocation, do so here.

    // Free the node memory.
    free_node_memory(result.node as *mut u8);
}

#[no_mangle]
pub extern "C" fn free_cstring_memory(ptr: *mut c_char) {
    if !ptr.is_null() {
        unsafe {
            let _ = CString::from_raw(ptr);
        }
    }
}