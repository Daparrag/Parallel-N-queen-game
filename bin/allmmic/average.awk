# Compute the average of the input values taking
# away the minimum and the maximum values
#
# If there is not enough input just compute the
# classical arithmetic average

{
	for(i=1; i<=NF; i++) {
		sum += $i
		if(min == "" || $i < min) min = $i
		if(max == "" || $i > max) max = $i
	}
}
END {
	if( NF > 3) {
		print ((sum - min - max)/(NF-2))
	} else {
		if((sum/NF)==0){
		  print sum + 0.1;
		}else{
		  print sum/NF	
		}
	}
}
