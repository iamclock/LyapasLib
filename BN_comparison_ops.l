main(/)
	
	@+L1(100)
	@+L2(100)
	@+F3(100) *** @+L3(100)
	
	OQ3 @'0xFF443 0xFF443'>F3
	
	OQ1=Q2=i
	*big_int_input(L1, F3, i/L1, i)
	*big_int_input(L2, F3, i/L2, i)
	
	*** *BN_eq(L1, L2/b) ?(b=1)1 ?=7
	*** *BN_less(L1, L2/b) ?(b=1)2 ?=7
	*** *BN_gr(L1, L2/b) ?(b=1)3 ?=7
	*** *BN_leq(L1, L2/b) ?(b=1)4 ?=7
	*** *BN_greq(L1, L2/b) ?(b=1)5 ?=7
	*** *BN_neq(L1, L2/b) ?(b=1)6 ?=7
	P1 /'Equals\n'>C ?=7
	P2 /'Less\n'>C ?=7
	P3 /'Greater\n'>C ?=7
	P4 /'Less or Equals\n'>C ?=7
	P5 /'Greater or Equals\n'>C ?=7
	P6 /'Not Equals\n'>C
	
	P7
**

*** проверка, что 2 больших числа равны
BN_eq(L1, L2/b)
	Ob=i
	?(Q1<Q2)2 ?(Q2<Q1)2 Q1=n
	P1 ?(L1i#L2i)2 Di ?(i<n)1 Db
	P2
**

*** проверка, что большое число L1 больше числа L2
BN_gr(L1, L2/b)
	Ob=i
	?(Q1<Q2)2 Db ?(Q2<Q1)2 Ob Q1=n
	P1 ?(L1i<L2i)2 Db ?(L2i<L1i)2 Ob Di ?(i<n)1
	P2
**

*** проверка, что большое число L1 меньше числа L2
BN_less(L1, L2/b)
	Ob=i
	?(Q1>Q2)2 Db ?(Q2>Q1)2 Ob Q1=n
	P1 ?(L1i>L2i)2 Db ?(L2i>L1i)2 Ob Di ?(i<n)1
	P2
**

*** проверка, что большое число L1 больше ил равно числу L2
BN_greq(L1, L2/b)
	Ob=i
	?(Q1<Q2)2 Db ?(Q2<Q1)2 Ob Q1=n
	P1 ?(L1i<L2i)2 Di ?(i<n)1 Db
	P2
**

*** проверка, что большое число L1 меньше или равно числу L2
BN_leq(L1, L2/b)
	Ob=i
	?(Q1>Q2)2 Db ?(Q2>Q1)2 Ob Q1=n
	P1 ?(L1i>L2i)2 Di ?(i<n)1 Db
	P2
**

*** проверка, что 2 больших числа не равны
BN_neq(L1, L2/b)
	Ob=i Db
	?(Q1<Q2)2 ?(Q2<Q1)2 Q1=n
	P1 ?(L1i#L2i)2 Di ?(i<n)1 Ob
	P2
**