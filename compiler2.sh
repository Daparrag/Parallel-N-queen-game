#!/bin/bash
icpc -std=c++11 -DNO_DEFAULT_MAPPING -O3 -finline-functions -DNDEBUG Sequencial/queen.c Sequencial/tree.c Sequencial/Nqueens_seq_main.c  -o bin/test_seq  -g -debug -I. -pthread
icpc -std=c++11 -DNO_DEFAULT_MAPPING -O3 -finline-functions -DNDEBUG multithread/queen.c multithread/tree.c multithread/threadpool.c multithread/Nqueens_pool.c -o bin/test_threads  -g -debug -I. -pthread
icpc -std=c++11 -DNO_DEFAULT_MAPPING  -I /home/spm1501/public/fastflow -O3 -finline-functions -DNDEBUG -o bin/test_fast -g -debug fastflow_farm/queen.c fastflow_farm/tree.c fastflow_farm/fastflow_farm.cpp  -pthread
cd bin/