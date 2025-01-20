/***
 * Bitwuzla: Satisfiability Modulo Theories (SMT) solver.
 *
 * Copyright (C) 2024 by the authors listed in the AUTHORS file at
 * https://github.com/bitwuzla/bitwuzla/blob/main/AUTHORS
 *
 * This file is part of Bitwuzla under the MIT license. See COPYING for more
 * information at https://github.com/bitwuzla/bitwuzla/blob/main/COPYING
 */

#include "solver/multiple_cores.h"

namespace bzla {

MultipleUnsatCores::MultipleUnsatCores(SolvingContext& ctx) : d_ctx(ctx) {}

std::vector<Node>
MultipleUnsatCores::get_next_core()
{
  if (d_initialized)
  {
    // Block the previous core
    block_previous_core();
  }
  else
  {
    // Initialize: assert the negation of all original assertions
    NodeManager& nm = d_ctx.env().nm();
    backtrack::AssertionView& assertions = d_ctx.assertions();
    size_t size = assertions.size();
    for (size_t i = assertions.begin(); i < size; ++i)
    {
      d_ctx.assert_formula(nm.mk_node(node::Kind::NOT, {assertions[i]}));
    }
    d_initialized = true;
  }

  Result result = d_ctx.solve();

  if (result == Result::SAT)
  {
    // No more cores exist
    return std::vector<Node>();
  }
  else if (result == Result::UNSAT)
  {
    auto core = d_ctx.get_unsat_core();
    d_found_cores.push_back(core);
    return core;
  }
  else
  {
    // Unknown result, treat as no more cores
    return std::vector<Node>();
  }
}

void
MultipleUnsatCores::block_previous_core()
{
  if (d_found_cores.empty())
  {
    return;
  }

  const auto& core = d_found_cores.back();
  NodeManager& nm = d_ctx.env().nm();
  
  // Create OR of core elements
  std::vector<Node> core_nodes;
  for (const Node& n : core)
  {
    if (n.type().is_bool())
    {
      core_nodes.push_back(n);
    }
  }

  if (!core_nodes.empty())
  {
    Node blocking = nm.mk_node(node::Kind::OR, core_nodes);
    d_ctx.assert_formula(blocking);
  }
}

}  // namespace bzla