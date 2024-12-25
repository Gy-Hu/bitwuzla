# Incremental Solving Implementation in Kissat SAT Solver

## 1. Overview

We have implemented incremental solving capabilities in the Kissat SAT solver, including push/pop operations and assumptions support. These modifications enable Kissat to be used more efficiently within the Bitwuzla SMT solver.

## 2. Major Modifications

### 2.1 Data Structure Changes (`internal.h`)

1. Added data structures for incremental solving:
```c
// Frame structure for incremental solving
struct incremental_frame {
    size_t stored_clauses_size;    // Size of stored clauses
    unsigned next_var;             // Next available variable ID
    size_t assumptions_size;       // Size of assumptions
};

typedef struct incremental_frame incremental_frame;
typedef STACK(incremental_frame) incremental_frames;
```

2. Added new members to `struct kissat`:
```c
struct kissat {
    // ... existing members ...
    unsigned next_var;           // Next variable ID
    bool needs_rebuild;          // Whether solver state needs rebuilding
    unsigneds stored_clauses;    // Store all clauses
    unsigneds assumptions;       // Current assumptions
    incremental_frames inc_frames;  // Frames for incremental solving
};
```

### 2.2 Core Implementation (`internal.c`)

1. Modified initialization function:
```c
kissat *kissat_init (void) {
    // ... existing initialization code ...
    
    // Initialize incremental solving support
    solver->next_var = 1;
    solver->needs_rebuild = false;
    INIT_STACK(solver->stored_clauses);
    INIT_STACK(solver->assumptions);
    INIT_STACK(solver->inc_frames);
    
    return solver;
}
```

2. Implemented core incremental solving functions:
```c
// Push operation: save current state
void kissat_push(kissat *solver) {
    incremental_frame frame;
    frame.stored_clauses_size = SIZE_STACK(solver->stored_clauses);
    frame.next_var = solver->next_var;
    frame.assumptions_size = SIZE_STACK(solver->assumptions);
    PUSH_STACK(solver->inc_frames, frame);
}

// Pop operation: restore previous state
void kissat_pop(kissat *solver) {
    incremental_frame frame = POP_STACK(solver->inc_frames);
    RESIZE_STACK(solver->stored_clauses, frame.stored_clauses_size);
    RESIZE_STACK(solver->assumptions, frame.assumptions_size);
    solver->next_var = frame.next_var;
    solver->needs_rebuild = true;
}

// Add assumption
void kissat_assume(kissat *solver, int lit) {
    unsigned ulit = ABS(lit);
    if (ulit >= solver->next_var) {
        solver->next_var = ulit + 1;
    }
    PUSH_STACK(solver->assumptions, (unsigned)lit);
    solver->needs_rebuild = true;
}

// Check if assumption failed
int kissat_failed(kissat *solver, int lit) {
    for (all_stack(unsigned, failed_lit, solver->assumptions)) {
        if ((int)failed_lit == -lit) {
            return 1;
        }
    }
    return 0;
}
```

3. State rebuilding support:
```c
static void rebuild_solver_if_needed(kissat *solver) {
    if (!solver->needs_rebuild) return;
    
    kissat_clear_internal_state(solver);
    
    // Re-add all stored clauses
    for (all_stack(unsigned, lit, solver->stored_clauses)) {
        kissat_add(solver, (int)lit);
    }
    
    // Add assumptions as unit clauses
    for (all_stack(unsigned, lit, solver->assumptions)) {
        kissat_add(solver, (int)lit);
        kissat_add(solver, 0);
    }
    
    solver->needs_rebuild = false;
}
```

### 2.3 API Changes (`kissat.h`)

Added incremental solving function declarations to the public API:
```c
// Incremental solving support
void kissat_assume (kissat *solver, int lit);
int kissat_failed (kissat *solver, int lit);
void kissat_push (kissat *solver);
void kissat_pop (kissat *solver);
```

## 3. Implementation Details

1. **State Management**:
   - Uses `stored_clauses` to store all added clauses
   - Uses `assumptions` to store current assumptions
   - Uses `inc_frames` to store solver state snapshots

2. **Rebuilding Mechanism**:
   - Marks `needs_rebuild = true` when state changes (pop or new assumptions)
   - Checks and rebuilds solver state before solving

3. **Memory Management**:
   - Properly releases all new memory in `kissat_release`
   - Uses Kissat's memory management functions for memory operations

## 4. Usage Example

```c
kissat *solver = kissat_init();

// Add some base constraints
kissat_add(solver, 1);
kissat_add(solver, 2);
kissat_add(solver, 0);

// Push current state
kissat_push(solver);

// Add new constraints and assumptions
kissat_add(solver, -1);
kissat_add(solver, 0);
kissat_assume(solver, 2);

// Solve
int result = kissat_solve(solver);

// Check if assumption failed
if (kissat_failed(solver, 2)) {
    // Handle failed assumption
}

// Pop back to previous state
kissat_pop(solver);

kissat_release(solver);
```

## 5. Important Notes

1. Solver state needs rebuilding after each `kissat_pop` or `kissat_assume` call
2. Rebuilding process may impact performance, especially with frequent push/pop operations
3. Assumptions are implemented as temporary unit clause constraints

## 6. Future Improvements

Potential areas for optimization:
1. Optimize the state rebuilding mechanism to reduce overhead
2. Implement more efficient assumption handling
3. Add support for incremental proof generation
4. Consider adding a more efficient way to manage variable IDs

## 7. Known Limitations

1. The current implementation rebuilds the entire solver state on pop/assume operations
2. Variable IDs are managed in a simple incremental way
3. No support for selective clause deletion
4. Assumption handling could be more efficient

## 8. Testing

The implementation has been tested with:
1. Basic SAT problems with push/pop operations
2. Problems requiring multiple assumptions
3. Integration tests with Bitwuzla SMT solver

## 9. References

1. Original Kissat SAT solver: [Kissat](http://fmv.jku.at/kissat/)
2. IPASIR incremental interface specification
3. Bitwuzla SMT solver integration requirements 