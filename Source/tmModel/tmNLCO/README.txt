/*
This folder contains classes for nonlinear constrained optimization, which
is used in several places within TreeMaker.

TreeMaker is written to support several different implementations of NLCO,
which trade off speed, robustness, and license restrictions. (I will probably
eventually downselect to one optimizer.)

The class tmNLCO_base defines the interface with which all optimizers must
comply. Each optimization library is supplemented with a class tmNLCO_XXXXX,
where XXXXX is the name of the library. This class adapts the optimizer to
TreeMaker.

Class tmNLCO_base and class tmDifferentiableFn also provide some support for
benchmarking (recording numbers of calls to objectives, constraints, and their
gradients) and for constructing "pure" test problems from specific origami
configurations for benchmarking.

Currently, the optimizers under consideration are:

(1) CFSQP + tmNLCO_cfsqp. CFSQP is a code written at the University of
Maryland and taken over by AEM Design (Atlanta, GA). It is the optimizer
used in TreeMaker 4.0. It is very fast and efficient. However, the code
is proprietary to AEM design and cannot be redistributed and the executable
can only be distributed for non-profit usage.

(2) ALM + tmNLCO_alm. This is an updated version of the Augmented Lagrangian
Multiplier code I originally developed for TreeMaker 3.0. The algorithm is
1970s-vintage and it is much slower than CFSQP. However, it's a bit more
robust against getting trapped in spurious infeasible configurations. Since
I wrote the code myself, the code is fully distributable.

(3) wnlib + tmNLCO_wnlib. This uses the wnnlp routine from the wnlib library, 
written by Will Naylor and Bill Chapman. The code is is freely distributable.
It is faster than ALM, slower than CFSQP. However, in some of my sample tests
it had a tendency to terminate before convergence was complete.

The following optimizers have not yet been adapted or benchmarked:

(4) HCL, The Hilbert Class Library.

(5) conmin_cpp

(6) donlp2

(7) hqp

(8) RFSQP. I've received an evaluation copy of RFSQP (from the makers of
CFSQP), which I'll be trying out. This will not be redistributable.

*/
