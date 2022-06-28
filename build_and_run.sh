#!/bin/bash

(mkdir build || true) &&
cd build &&
cmake ../ &&
make -j2 &&
(./Client & ./Server) # Print everything in the same terminal
