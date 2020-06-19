#!/bin/bash
(cd ../ && ./format.sh EpsilonAuxBMS)
(make -j4 && cd Tests/ && make)
