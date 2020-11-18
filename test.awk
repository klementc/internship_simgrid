#!/usr/bin/awk -f

BEGIN {
	print("policy,timestamp,load,nInstances,waitingReq,executingReq,execInSlot,parPerInst")
}

/POLICY/ && /stat/{
	gsub(/]/, "")
	gsub(/\[/, "")
	print($1","$2","$4","$5","$6","$7","$8","$9)
}

END {
}
