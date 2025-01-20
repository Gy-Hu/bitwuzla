
/***
 * Bitwuzla: Satisfiability Modulo Theories (SMT) solver.
 *
 * Copyright (C) 2024 by the authors listed in the AUTHORS file at
 * https://github.com/bitwuzla/bitwuzla/blob/main/AUTHORS
 *
 * This file is part of Bitwuzla under the MIT license. See COPYING for more
 * information at https://github.com/bitwuzla/bitwuzla/blob/main/COPYING
 */

#ifndef BZLA_SOLVER_MULTIPLE_CORES_H_INCLUDED
#define BZLA_SOLVER_MULTIPLE_CORES_H_INCLUDED

#include "backtrack/assertion_stack.h"
#include "node/node.h"
#include "solving_context.h"

namespace bzla {

class MultipleUnsatCores
{
 public:
  MultipleUnsatCores(SolvingContext& ctx);
  ~MultipleUnsatCores() = default;

  /** Get next unsat core if exists */
  std::vector<Node> get_next_core();

 private:
  /** Reference to the solving context */
  SolvingContext& d_ctx;
  /** Store found cores */
  std::vector<std::vector<Node>> d_found_cores;
  /** Flag to indicate if the solver has been initialized */
  bool d_initialized = false;

  /** Block previously found core */
  void block_previous_core();
};

}  // namespace bzla

#endif