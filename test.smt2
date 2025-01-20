(set-logic QF_BV)
(set-option :produce-unsat-cores true)
(set-option :produce-models true)

(define-sort myBitVec () (_ BitVec 41))
(define-fun next ((vin myBitVec)) myBitVec (concat (bvadd ((_ extract 0 0) vin) ((_ extract 3 3) vin)) ((_ extract 40 1) vin)))

(define-fun DCeq ((inVec myBitVec) (andVec myBitVec) (rhsVec myBitVec)) Bool (= (bvand inVec andVec) rhsVec))

;;;;;; DECLARES ;;;;;;

;;; LFSR steps
(declare-fun s0 () myBitVec)
(declare-fun s1 () myBitVec)
(declare-fun s2 () myBitVec)
(declare-fun s3 () myBitVec)
(declare-fun s4 () myBitVec)

;;; Vectors
(declare-fun t0 () myBitVec)
(declare-fun t1 () myBitVec)
(declare-fun t2 () myBitVec)

;;; MATCH bools
(declare-fun m_t0_s0 () Bool)
(declare-fun m_t0_s1 () Bool)
(declare-fun m_t0_s2 () Bool)
(declare-fun m_t0_s3 () Bool)
(declare-fun m_t0_s4 () Bool)
(declare-fun m_t1_s0 () Bool)
(declare-fun m_t1_s1 () Bool)
(declare-fun m_t1_s2 () Bool)
(declare-fun m_t1_s3 () Bool)
(declare-fun m_t1_s4 () Bool)
(declare-fun m_t2_s0 () Bool)
(declare-fun m_t2_s1 () Bool)
(declare-fun m_t2_s2 () Bool)
(declare-fun m_t2_s3 () Bool)
(declare-fun m_t2_s4 () Bool)

;;;;;; STEPS ;;;;;;
(assert (= s1 (next s0)))
(assert (= s2 (next s1)))
(assert (= s3 (next s2)))
(assert (= s4 (next s3)))

;;;;;; MATCHES ;;;;;;
(assert (= m_t0_s0 (DCeq s0 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t0_s1 (DCeq s1 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t0_s2 (DCeq s2 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t0_s3 (DCeq s3 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t0_s4 (DCeq s4 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t1_s0 (DCeq s0 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t1_s1 (DCeq s1 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t1_s2 (DCeq s2 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t1_s3 (DCeq s3 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t1_s4 (DCeq s4 #b11111111111111111111111111111111000000001 #b11001000101101110000001100000011000000000)))
(assert (= m_t2_s0 (DCeq s0 #b11111111111111111111111111111111000000001 #b01100111011000110001100000101000000000000)))
(assert (= m_t2_s1 (DCeq s1 #b11111111111111111111111111111111000000001 #b01100111011000110001100000101000000000000)))
(assert (= m_t2_s2 (DCeq s2 #b11111111111111111111111111111111000000001 #b01100111011000110001100000101000000000000)))
(assert (= m_t2_s3 (DCeq s3 #b11111111111111111111111111111111000000001 #b01100111011000110001100000101000000000000)))
(assert (= m_t2_s4 (DCeq s4 #b11111111111111111111111111111111000000001 #b01100111011000110001100000101000000000000)))

(assert (distinct s0 (_ bv0 41)))

;;;;;; MATCH ALL 3 VECTORS IN 5 STEPS ;;;;;;
(assert (! (or m_t1_s0 m_t1_s1 m_t1_s2 m_t1_s3 m_t1_s4) :named C1))
(assert (! (or m_t2_s0 m_t2_s1 m_t2_s2 m_t2_s3 m_t2_s4) :named C2))
(assert (! (or m_t0_s0 m_t0_s1 m_t0_s2 m_t0_s3 m_t0_s4) :named C0))


(check-sat)

(get-unsat-core)

(exit)