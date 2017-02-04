# Compute the average of the input values taking
# away the minimum and the maximum values
#
# If there is not enough input just compute the
# classical arithmetic average
BEGIN{
	sum=0
}
{
	for(i=1; i<=NF; i++) {
		if($i==""){
		
		}else{
		sum += $i
		}
	}
}
END {
	
		print sum/NF
	}

