(set-logic QF_BV)
(set-info :status unsat)
(declare-const v0 (_ BitVec 8))
(declare-const v1 (_ BitVec 8))
(assert (= #b1 (bvor (bvor (ite (distinct (bvxnor v0 v1) (bvsub (bvsub (bvand v0 v1) (bvor v0 v1)) (_ bv1 8))) #b1 #b0) (ite (distinct (bvxnor v0 v1) (bvadd (bvand v0 v1) (bvnot (bvor v0 v1)))) #b1 #b0)) (bvor (ite (distinct (bvor v0 v1) (bvadd (bvand v0 (bvnot v1)) v1)) #b1 #b0) (ite (distinct (bvand v0 v1) (bvsub (bvor (bvnot v0) v1) (bvnot v0))) #b1 #b0)))))
(check-sat)