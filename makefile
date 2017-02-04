# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------
# icpc -std=c++11 -DNO_DEFAULT_MAPPING -O0 -finline-functions -DNDEBUG Project_SPM2015/multithread/queen.c Project_SPM2015/multithread/tree.c Project_SPM2015/multithread/threadpool.c Project_SPM2015/multithread/Nqueens_pool.c -o Project_SPM2015/bin/test_threads  -g -debug -I. -pthread

FF_ROOT		= $(HOME)/fastflow
CXX		= g++ -std=c++11
INCLUDES	= -I $(FF_ROOT) 
CXXFLAGS  	= -Wall -DNO_DEFAULT_MAPPING
FF_RESULT	= bin
FF_MMIC		= bin/allmmic
FF_DOCS		= docs
FF_THREAD	= Multithread
FF_SEQ		= Sequencial
FF_FASTFLW	= Nqueens_fastflow

LDFLAGS 	=  -pthread
OPTFLAGS	=  -O3 -finline-functions -DTRACE_FASTFLOW
OPTFLAG         =  -O3 -finline-functions -DTRACE_FASTFLOW
SOURCEC1	= Seq_Nqueens
SOURCEC2	= threadpool
SOURCEC3	= Chess_Board

SOURCE1		= Nqueen_Pool
SOURCE2		= Farm_Fast
SOURCE3		= main_seq


IXX		= icpc -std=c++11
LDIFLAGS	= -pthread -mmic

.PHONY: allg++  clean
.SUFFIXES: .c


%: %.c
allhost: $(FF_THREAD)/$(SOURCE1).cpp $(FF_FASTFLW)/$(SOURCE2).cpp  $(FF_SEQ)/$(SOURCE3).cpp
	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_RESULT)/$(SOURCE1) $(FF_THREAD)/$(SOURCE1).cpp  $(FF_THREAD)/$(SOURCEC2).c $(FF_THREAD)/$(SOURCEC3).cpp $(LDFLAGS)
	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_RESULT)/$(SOURCE2) $(FF_FASTFLW)/$(SOURCE2).cpp $(FF_FASTFLW)/$(SOURCEC1).cpp $(FF_FASTFLW)/$(SOURCEC3).cpp $(LDFLAGS)
	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_RESULT)/$(SOURCE3) $(FF_SEQ)/$(SOURCE3).cpp $(FF_SEQ)/$(SOURCEC1).cpp $(FF_SEQ)/$(SOURCEC3).cpp $(LDFLAGS)
#	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_RESULT)/$(SOURCE4) $(FF_THREAD)/$(SOURCE4).c $(FF_THREAD)/$(SOURCEC1).c $(FF_THREAD)/$(SOURCEC2).c  $(FF_THREAD)/$(SOURCEC3).c $(LDFLAGS)

allmmic: $(FF_THREAD)/$(SOURCE1).cpp $(FF_FASTFLW)/$(SOURCE2).cpp  $(FF_SEQ)/$(SOURCE3).cpp

	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_MMIC)/$(SOURCE1) $(FF_THREAD)/$(SOURCE1).cpp $(FF_THREAD)/$(SOURCEC2).c $(FF_THREAD)/$(SOURCEC3).cpp $(LDIFLAGS)
	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_MMIC)/$(SOURCE2) $(FF_FASTFLW)/$(SOURCE2).cpp $(FF_FASTFLW)/$(SOURCEC1).cpp $(FF_FASTFLW)/$(SOURCEC3).cpp $(LDIFLAGS)
	$(IXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $(FF_MMIC)/$(SOURCE3) $(FF_SEQ)/$(SOURCE3).cpp $(FF_SEQ)/$(SOURCEC1).cpp $(FF_SEQ)/$(SOURCEC3).cpp $(LDIFLAGS)
clean:
	rm -f $(FF_RESULT)/$(SOURCE1) $(FF_RESULT)/$(SOURCE2) $(FF_RESULT)/$(SOURCE3)
	rm -f $(FF_MMIC)/$(SOURCE1) $(FF_MMIC)/$(SOURCE2) $(FF_MMIC)/$(SOURCE3)
	rm -f $(FF_DOCS)/graphs/*.txt

