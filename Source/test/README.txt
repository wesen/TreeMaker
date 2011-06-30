/*
The files in this folder are console-based main() routines that allow one
to compile and test portions of the TreeMaker model without requiring a
build of the entire GUI:

tmArrayTester.cpp -- tests the tmArray<T> container class, which serves as
the single container type used throughout TreeMaker and is helping migrate
to the C++ standard library containers. Build with the contents of the
tmPtrClasses folder.

tmDpptrTester.cpp -- tests the dangle-proof pointer classes tmDpptr<T> and
tmDpptrArray<T>. Build with the contents of the tmPtrClasses folder.

NewtonRaphsonTester.cpp -- tests the Newton Raphson solver for sets of
nonlinear equations. Build with the contents of the Solvers folder.

tmNLCOTester.cpp -- tests the nonlinear constrained optimizer classes. Build
with the contents of the tmNLCO_cfsqp and tmPtrClasses folders, but do not
directly compile the files tmNLCO_cfsqp/cfsqp.c and tmNLCO_cfsqp/qld.c (see
tmNLCO_cfsqp/README.txt for details).

tmModelTester.cpp -- compiles the TreeMaker mathematical model, reads in some
test files and performs some optimizations from the command line. 
Build with the contents of the tmModel folder, leaving out
the two files from the tmNLCO_cfsqp folder as described above.
*/
