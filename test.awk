#!/usr/bin/awk -f

BEGIN {
	print("policy,timestamp,load,nInstances,waitingReq")
}

/POLICY/ && /stat/{
	gsub(/]/, "")
	gsub(/\[/, "")
	print($1","$2","$4","$5","$6)
}

END {
}
