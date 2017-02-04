#!/bin/bash

#
# -------------- script variables --------------
#

# settable variables
n=13
max_nw=60
executions=6
graph_dir=../docs
delay=0
timestamp=$(date +%H-%M-%S)
nw=0
exeseq=1

#
# -------------- script functions --------------
#

# Print script usage
#show the parameters of thre script and how used
#=============================================================PROVIDE INFORMATION OF THE SCRIPT============================================================================
usage() {
	echo "usage:"
	echo "  ${0##*/} [-?] [-f <matrix dimension>] [-w <max-nw>] [-e <executions>] [-d <delay>] [-p power2worker=on|off]"
	echo
	echo "  -?      print this message"
	echo "  -f <matrix dimension>"
	echo "          the dimension of the matrix"
	echo "          Defaults to 1"
	echo "  -w <max-nw>"
	echo "          maximum number of worker used. The parallel code is run using workers in the range from 1 up to this value by power of 2."
	echo "          Defaults to 16"
	echo "  -e <executions>"
	echo "          specify how many times the sequential and parallel code must be executed to get the average time."
	echo "          Defaults to 1"
	echo "  -d <delay>"
	echo "          time to wait between two consecutive executions."
	echo "          Defaults to 0"
	echo
}
#=============================================================PROVIDE VALIDATION OF THE INPUT PARAMETERS OF THE SCRIPT============================================================================
check_parameters() {
	# Check if the first argument is a positive integer
	#
	# Parameters:
	#	$1 the value to check againts
	#	$2 name of the option
	is_number() {
		local re='^[0-9]+$'
		if ! [[ $1 =~ $re ]] ; then
			echo "error: -$2 expects positive integer" >&2
			exit 1
		fi
	}

	# Check if the first argument is a positive decimal
	#
	# Parameters:
	#	$1 the value to check againts
	#	$2 name of the option
	is_decimal() {
		local re='^[0-9]+(\.[0-9]+)*$'
		if ! [[ $1 =~ $re ]] ; then
			echo "error: -$2 expects positive number (integer or decimal)" >&2
			exit 1
		fi
	}

	OPTIND=1
	while getopts ":f:w:e:d:p:?h" opt
	do
		case $opt in
		f)
			is_number $OPTARG $opt
			n=$OPTARG
			;;
		w)
			is_number $OPTARG $opt
			max_nw=$OPTARG
			;;
		e)
			is_number $OPTARG $opt
			executions=$OPTARG
			;;
		d)
			is_decimal $OPTARG $opt
			delay=$OPTARG
			;;
		\?|:)
			usage
			exit 1
			;;
		esac
	done

	shift $((OPTIND-1))
	[ "$1" = "--" ] && shift
}

# Execute the specified program $executions times
# using the current image and streamlen, with the 
# fixed global threshold. The average time of
# execution is stored in a gobal variable with the
# specified name.
#
# Parameters:
#	$1 name of the binary to execute ('par' or 'seq')
#	$2 name of the variable that will contain the result
function average() {
	local binary=$1
	local _result_var=$2
	local temp=""
	for ((i=1;i<=executions;i++))
	do
	if ((exeseq==1))
	then

	        temp+=`./$binary $n | awk 'NR==4'`
                temp+=""
                sleep $delay
                echo -n .
	else

	
		temp+=`./$binary $nw $n | awk 'NR==5'`
		temp+="" 
		sleep $delay
		echo -n .
	fi
	done
	echo
	echo $temp	
	eval $_result_var=`echo $temp | awk -f average.awk`
	echo
	echo $_result_var	
}

# Initalise the $output_file used to store the measured
# performances. This file will be used by gnuplot to 
# build the graphs.
#
# The columns index will be used to reference the source
# data in the 'create_graph_commands()' function.
#
# The file is overwritten if already exists.
create_data_file() {
	echo "# " > $output_file
	echo "# Tseq = $Tseq" >> $output_file
	echo "# " >> $output_file
	echo -e "NW \t|\tTIME\t|\tSCALAB\t|\tSPEEDUP \t|\tEFFICIENZA\n" >> $output_file
	graphs_produced+="${output_file##*/} "
}


# Add one line of data to the data file.
# Data is written following the column order used when writing the
# comment haeders in 'create_data_file()' function.
add_data_line() {
    echo -n -e "$nw \t  $avgTpar " >> $output_file	# nw and time columns
    echo -n -e "\t\t" >> $output_file
	echo -n `echo $Tpar1 $avgTpar | awk '{ print $1/$2 }'` >> $output_file	# scalability column
	echo -n -e "\t\t" >> $output_file " "
	echo -n `echo $Tseq $avgTpar | awk '{ print $1/$2 }'` >> $output_file	# speedup column
	echo -n -e "\t\t" >> $output_file
	echo `echo $Tseq $avgTpar $nw | awk '{ print ($1/$2)/$3 }'` >> $output_file
}


#
# -------------- computation --------------
#

check_parameters "$@"

if [[ -e "Nqueens_seq_main" && -e "Nqueens_pool" ]]
then
	
if [ $executions <= 3 ]
then
	echo "Warning: the average will include best and worst results"
	echo
fi
	echo "===> processing $n <==="
    	echo
        echo -n "executing sequential code"	
	average "Nqueens_seq_main" Tseq
	echo
       	output_file=$graph_dir/'result'.N=$n.time=$timestamp.txt
        create_data_file
	exeseq=0
 #       echo -e " executing parallel code with thread" >> $output_file	
#		nw=0
#		for ((j=1;nw<max_nw;j++))
		#for nw in 1 `seq 2 2 $max_nw`
#		do
#			nw=$j
#			 if (( nw>max_nw ))
#			 then
#			 nw=$max_nw
#			 fi
#			echo -n "  using $nw workers"
#			echo
#			 average "test_threads" avgTpar
#			 echo ${avgTpar}
# save the parallel execution time with 1 worker to compute scalability
#	    		if ((nw == 1))
#	    		then
#			Tpar1=$avgTpar
#	    		fi
#			echo ${Tpar1}
			
#	    	add_data_line
#	    done
		echo -e " executing farm code with thread" >> $output_file	
		nw=0
		for ((j=1;nw<max_nw;j++))
		#for nw in 1 `seq 2 2 $max_nw`
		do
			nw=$j
			 if (( nw>max_nw ))
			 then
			 nw=$max_nw
			 fi
			echo -n "  using $nw workers"
			echo
			 average "fastflow_farm" avgTpar
			 echo ${avgTpar}
# save the parallel execution time with 1 worker to compute scalability
	    		if ((nw == 1))
	    		then
			Tpar1=$avgTpar
	    		fi
			echo ${Tpar1}
			
	    	add_data_line
	    done
#	echo 
#	make clean
#	make all
#	echo
fi
