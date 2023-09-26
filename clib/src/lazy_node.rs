use clvmr::allocator::{Allocator, NodePtr, SExp};

#[repr(C)]
pub struct Pair {
    pub first: *mut LazyNode,
    pub second: *mut LazyNode,
}

#[repr(C)]
pub struct LazyNode {
    pub allocator: *mut Allocator,
    pub node: NodePtr,
}

impl LazyNode {
    pub fn pair(&self) -> *mut Pair {
        unsafe { 
            match (*self.allocator).sexp(self.node) { 
                SExp::Pair(p1, p2) => {
                    let r1 = Box::new(LazyNode::new(self.allocator, p1));
                    let r2 = Box::new(LazyNode::new(self.allocator, p2));
                    Box::into_raw(Box::new(Pair { first: Box::into_raw(r1), second: Box::into_raw(r2) }))
                },
                _ => std::ptr::null_mut(),
            }
        } 
    }

    pub fn atom(&self) -> Option<Vec<u8>> {
        unsafe {
            match (*self.allocator).sexp(self.node) { 
                SExp::Atom => Some((*self.allocator).atom(self.node).into()),  
                _ => None,
            }
        }  
    }

    pub fn new(allocator: *mut Allocator, node: NodePtr) -> Self {
        Self { allocator, node }
    }
}

#[no_mangle]
pub extern "C" fn free_pair(pair: *mut Pair) {
    if pair.is_null() {
        return;
    }
    
    unsafe {
        let pair_box = Box::from_raw(pair);
        drop(Box::from_raw(pair_box.first));
        drop(Box::from_raw(pair_box.second));
        drop(pair_box);
    }
}

#[no_mangle]
pub extern "C" fn free_lazy_node(node: *mut LazyNode) {
    if node.is_null() {
        return;
    }
    
    unsafe {
        drop(Box::from_raw(node));
    }
}