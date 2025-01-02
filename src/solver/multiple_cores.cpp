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
  block_previous_cores();

  if (d_ctx.solve() == Result::UNSAT)
  {
    auto core = d_ctx.get_unsat_core();
    d_found_cores.push_back(core);
    return core;
  }
  return std::vector<Node>();
}

void
MultipleUnsatCores::block_previous_cores()
{
  NodeManager& nm = d_ctx.env().nm();
  for (const auto& core : d_found_cores)
  {
    // Create NOT nodes for each core element
    std::vector<Node> not_nodes;
    for (const Node& n : core)
    {
      if (n.type().is_bool())
      {
        not_nodes.push_back(nm.mk_node(node::Kind::NOT, {n}));
      }
    }

    // Build a binary tree of OR nodes using indices
    if (!not_nodes.empty())
    {
      std::function<Node(size_t, size_t)> build_or_tree =
        [&](size_t start, size_t end) -> Node {
          if (end - start == 1)
          {
            return not_nodes[start];
          }
          else if (end - start == 2)
          {
            return nm.mk_node(node::Kind::OR, {not_nodes[start], not_nodes[start + 1]});
          }
          else
          {
            size_t mid = start + (end - start) / 2;
            Node left = build_or_tree(start, mid);
            Node right = build_or_tree(mid, end);
            return nm.mk_node(node::Kind::OR, {left, right});
          }
        };

      Node blocking = build_or_tree(0, not_nodes.size());
      d_ctx.assert_formula(blocking);
    }
  }
}

}  // namespace bzla