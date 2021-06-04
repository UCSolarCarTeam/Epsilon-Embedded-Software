#!/bin/bash
(cd ../ && ./format.sh EpsilonAuxBmsV2)
(make -j4 && cd Tests/ && make)
