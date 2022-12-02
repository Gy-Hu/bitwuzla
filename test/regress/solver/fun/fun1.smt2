(set-logic QF_UFBV)
(declare-fun f (Bool) Bool)
(declare-const a Bool)
(declare-const b Bool)
(assert (distinct a b))
(assert (distinct (f a) (f b)))
(assert (f a))
(assert (f (f (f b))))
(check-sat)