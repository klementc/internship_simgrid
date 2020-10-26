#!/usr/bin/awk -f

BEGIN {
	print("timestamp,load,nInstances")
}

/POLICY/ && /stat/{
	gsub(/]/, "")
	print($2", "$4", "$5)
}

END {
}
