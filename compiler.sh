#!/bin/bash
cd $HOME
icpc -std=c++11 -DNO_DEFAULT_MAPPING -O3 -finline-functions -DNDEBUG Project_SPM2015/Sequencial/queen.c Project_SPM2015/Sequencial/tree.c Project_SPM2015/Sequencial/Nqueens_seq_main.c  -o Project_SPM2015/bin/test_seq  -g -debug -I. -pthread
icpc -std=c++11 -DNO_DEFAULT_MAPPING -O3 -finline-functions -DNDEBUG Project_SPM2015/multithread/queen.c Project_SPM2015/multithread/tree.c Project_SPM2015/multithread/threadpool.c Project_SPM2015/multithread/Nqueens_pool.c -o Project_SPM2015/bin/test_threads  -g -debug -I. -pthread
icpc -std=c++11 -DNO_DEFAULT_MAPPING  -I /home/spm1501/public/fastflow -O3 -finline-functions -DNDEBUG -o Project_SPM2015/bin/test_fast -g -debug Project_SPM2015/fastflow_farm/queen.c Project_SPM2015/fastflow_farm/tree.c Project_SPM2015/fastflow_farm/fastflow_farm.cpp  -pthread
cd Project_SPM2015/bin/