# RTSPM
Real Time Streaming Pattern Matching

An implementation of a real time pattern matching algorithm working in the streaming model. This implementation is based on the algorithm described in Real-Time Streaming String-Matching by Dany Breslauer and Zvi Galil.


Correctness of the implementation can be verified by running a series of tests:
```bash
$ python3 judge.py correctness
```

There exists a probability of false positives. It can be demonstrated the following way:
```bash
$ ./gen 0 4000 100
$ python3 judge.py probability 13 
```
This will create a test in the file test.in and run the algorithm with modulo 13.
The algorithm is be executed for each base from 0 to modulo-1 ([0, 12]).

The algorithm works in the streaming model, so letters are given one by one and there is no random access to either the pattern or the text. The algorithm needs O(1) operations to process a letter of the pattern and O(log m) to process the next letter of the text and report whether an occurrence of the pattern ends on that symbol.
The space complexity is O(log m) (mesured in words).


The following charts show time and memory consumption of the algorithm in comparison to a solution with random access to the pattern using KMP (hence O(m) memory consumption and O(n+m) running time):



Such charts can be created by running:
```bash
$ python3 judge.py memory <n> <smallest m> <name>
# python3 judge.py time <n> <smallest m> <name>
```
The resulting charts will be saved as name.png.
