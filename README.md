# TrustySketch

### File Descriptions

- `ts.h/cpp` contains the implementation of our TrustySketch.
- `cm.h/cpp` contains the implementation of CM Sketch.
- `cu.h/cpp` contains the implementation of CU Sketch.
- `as.h/cpp` contains the implementation of Augmented Sketch.
- `ss.h/cpp` contains the implementation of Space-Saving.
- `hg.h/cpp` contains the implementation of HeavyGuardian.
- `murmur3.h` contains the implementation of Murmur Hash.
- `main.cpp` is the entry of all benchmarks.

### Usages

- Modify the code of function `main()` in `main.cpp` to change parameters of experimental sketches or tasks.
- Type `make` to build benchmark program and `./benchmark` to run it.


### Notes

- All sketch classes are derived from `class Sketch` so that you can push any sketch to the vector `sk`, which is sent to the test function. 
- Before evoking test functions, you can change the size of sketches.
- Datasets can be found in the reference of the paper.