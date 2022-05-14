#ifndef BZLALS__BZLALS_H
#define BZLALS__BZLALS_H

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bzla {

class BitVector;
class RNG;

namespace ls {

struct LocalSearchMove;

class BitVectorDomain;
class BitVectorNode;

namespace test {
class TestLs;
}

class LocalSearch
{
  friend class test::TestLs;

 public:
  using NodesIdTable = std::vector<std::unique_ptr<BitVectorNode>>;
  using ParentsSet   = std::unordered_set<uint32_t>;
  using ParentsMap   = std::unordered_map<uint32_t, ParentsSet>;

  enum Result
  {
    SAT     = 10,
    UNSAT   = 20,
    UNKNOWN = 0,
  };

  enum OperatorKind
  {
    ADD,
    AND,
    ASHR,
    CONCAT,
    // DEC,
    EXTRACT,
    EQ,
    // IMPLIES,
    ITE,
    // INC,
    MUL,
    // NAND,
    // NE,
    // NEG,
    // NOR,
    NOT,
    // OR,
    // REDAND,
    // REDOR,
    // SDIV,
    SEXT,
    // SGT,
    // SGE,
    SHL,
    SHR,
    SLT,
    // SLE,
    // SREM,
    // SUB,
    UDIV,
    // UGT,
    // UGE,
    ULT,
    // ULE,
    UREM,
    // XNOR,
    XOR,
    // ZEXT,
  };

  struct
  {
    uint64_t d_nprops   = 0;
    uint64_t d_nupdates = 0;
    uint64_t d_nmoves   = 0;

    uint64_t d_nprops_inv  = 0;
    uint64_t d_nprops_cons = 0;

    uint64_t d_nconf = 0;

#ifndef NDEBUG
    struct
    {
      uint64_t d_add     = 0;
      uint64_t d_and     = 0;
      uint64_t d_ashr    = 0;
      uint64_t d_concat  = 0;
      uint64_t d_extract = 0;
      uint64_t d_eq      = 0;
      uint64_t d_ite     = 0;
      uint64_t d_mul     = 0;
      uint64_t d_not     = 0;
      uint64_t d_sext    = 0;
      uint64_t d_shl     = 0;
      uint64_t d_shr     = 0;
      uint64_t d_slt     = 0;
      uint64_t d_udiv    = 0;
      uint64_t d_ult     = 0;
      uint64_t d_urem    = 0;
      uint64_t d_xor     = 0;
    } d_ninv;
    struct
    {
      uint64_t d_add     = 0;
      uint64_t d_and     = 0;
      uint64_t d_ashr    = 0;
      uint64_t d_concat  = 0;
      uint64_t d_extract = 0;
      uint64_t d_eq      = 0;
      uint64_t d_ite     = 0;
      uint64_t d_mul     = 0;
      uint64_t d_not     = 0;
      uint64_t d_sext    = 0;
      uint64_t d_shl     = 0;
      uint64_t d_shr     = 0;
      uint64_t d_slt     = 0;
      uint64_t d_udiv    = 0;
      uint64_t d_ult     = 0;
      uint64_t d_urem    = 0;
      uint64_t d_xor     = 0;
    } d_ncons;
#endif
  } d_statistics;

  LocalSearch(uint64_t max_nprops,
              uint64_t max_nupdates,
              uint32_t seed    = 1234,
              bool ineq_bounds = false);
  ~LocalSearch();

  void set_max_nprops(uint64_t max) { d_max_nprops = max; }
  void set_max_nupdates(uint64_t max) { d_max_nupdates = max; }
  void set_ineq_bounds(bool value) { d_ineq_bounds = value; }

  uint32_t mk_node(uint32_t size);
  uint32_t mk_node(OperatorKind kind,
                   uint32_t size,
                   const std::vector<uint32_t>& children);
  uint32_t mk_indexed_node(OperatorKind kind,
                           uint32_t size,
                           uint32_t child0,
                           const std::vector<uint32_t>& indices);

  uint32_t mk_node(const BitVector& assignment, const BitVectorDomain& domain);
  uint32_t mk_node(OperatorKind kind,
                   const BitVectorDomain& domain,
                   const std::vector<uint32_t>& children);
  uint32_t mk_indexed_node(OperatorKind kind,
                           const BitVectorDomain& domain,
                           uint32_t child0,
                           const std::vector<uint32_t>& indices);

  uint32_t invert_node(uint32_t id);

  const BitVector& get_assignment(uint32_t id) const;
  void set_assignment(uint32_t id, const BitVector& assignment);
  const BitVectorDomain& get_domain(uint32_t node) const;
  // void set_domain(uint32_t node, const BitVectorDomain& domain);

  /** Fix domain bit of given node at index 'idx' to 'value'. */
  void fix_bit(uint32_t id, uint32_t idx, bool value);

  void register_root(uint32_t root);
  bool all_roots_sat() const { return d_roots.empty(); }
  uint32_t get_num_roots_unsat() const { return d_roots.size(); }

  // TODO: incremental case:
  //       - we need to be able to unregister roots (assumptions)
  //       - we might want to exclude nodes that are not in the formula from
  //         cone updates

  uint32_t get_arity(uint32_t id) const;
  uint32_t get_child(uint32_t id, uint32_t idx) const;

  Result move();

  void set_log_level(uint32_t level) { d_log_level = level; }

 private:
  /**
   * Determine if given node is an inequality (ULT or SLT).
   * @param node The node to query.
   * @return True if `node` is an inequality.
   */
  static bool is_ineq_node(const BitVectorNode* node);
  /**
   * Get node by id.
   * @param id The node id.
   * @return The node with the given id.
   */
  BitVectorNode* get_node(uint32_t id) const;
  /**
   * Determine if given node is a leaf node (its arity = 0).
   * @param node The node to query.
   * @return True if `node` is a leaf.
   */
  bool is_leaf_node(const BitVectorNode* node) const;
  /**
   * Determine if given node is a root node.
   * @param node The node to query.
   * @return True if `node` is a root.
   */
  bool is_root_node(const BitVectorNode* node) const;
  /**
   * Update information related to the root given by id.
   *
   * This removes given root from the list of unsatisfied roots , adds the root
   * to the list of unsatisfied roots if it is unsatisfied, and in either case,
   * caches the root for later updating of inequality bounds (if enabled).
   *
   * @note Roots are updated initially on registration, and during updating the
   *       assignments of the cone of influence of the input that has been
   *       updated. If given root is unsatisfied and infering inequality bounds
   *       is enabled, we have to update bounds for its children wrt. to all of
   *       its parents after the assignment of all nodes has been (re)computed.
   *       Thus, we defer updating inequality bounds for all roots to after
   *       update_roots() has been called and the assignment of all nodes is
   *       consistent (see update_roots_ineq_bounds()).
   *
   * @param id The id of the root to update.
   */
  void update_roots(uint32_t id);
  /**
   * Update min/max bounds for children of (now) false top-level
   * inequalities (cached in d_false_roots_to_update).
   *
   * This must be called after update_roots() has been called and the
   * assignment of all nodes has been computed/updated, i.e., the assignment is
   * consistent.
   */
  void update_roots_ineq_bounds();
  /**
   * Update the assignment of the given node to the given assignment, and
   * recompute the assignment of all nodes in its cone of influence
   *
   * @param node The node to update.
   * @param assignment The new assignment of the given node.
   * @return The number of updated assignments.
   */
  uint64_t update_cone(BitVectorNode* node, const BitVector& assignment);
  /**
   * Select an input and a new assignment for that input by propagating the
   * given target value `t_root` for the given root along one path towards an
   * input.
   *
   * @param root The root to propagate from.
   * @param t_root The target value of the given root.
   * @return An object encapsulating all information necessary for that move.
   */
  LocalSearchMove select_move(BitVectorNode* root, const BitVector& t_root);

  /** The random number generator. */
  std::unique_ptr<RNG> d_rng;

  /** Map from node id to nodes. */
  NodesIdTable d_nodes;
  /** The set of unsatisfied roots. */
  std::unordered_set<uint32_t> d_roots;
  /** Map nodes to their parent nodes. */
  ParentsMap d_parents;

  /** Bit-vector one of size one, the target value for each root. */
  std::unique_ptr<BitVector> d_one;

  /** The log level. */
  uint32_t d_log_level = 0;
  /** The maximum number of propagations, 0 for unlimited. */
  uint64_t d_max_nprops   = 0;
  /** The maximum number of cone updates, 0 for unlimited. */
  uint64_t d_max_nupdates = 0;
  /** The seed for the RNG. */
  uint32_t d_seed;
  /**
   * True to enable to infer bounds for top-level inequalities for value
   * computation.
   */
  bool d_ineq_bounds = false;

  /** The set of now true roots to update in update_roots_ineq_bounds(). */
  std::unordered_set<BitVectorNode*> d_true_roots_to_update;
  /** The set of now false roots to update in update_roots_ineq_bounds(). */
  std::unordered_set<BitVectorNode*> d_false_roots_to_update;
};

}  // namespace ls
}  // namespace bzla
#endif