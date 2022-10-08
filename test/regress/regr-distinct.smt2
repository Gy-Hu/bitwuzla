(declare-fun c0 () (_ BitVec 4))
(declare-fun c1 () (_ BitVec 4))
(declare-fun c2 () (_ BitVec 4))
(define-fun e3 () Bool (distinct c0 c1 c2))
(define-fun e11 () Bool (and (not (= c0 c1)) (not (= c0 c2)) (not (= c1 c2))))
(assert e3)
(assert (not e11))
(check-sat)
(exit)