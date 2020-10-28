#!/usr/bin/awk -f

BEGIN {
	print("policy,timestamp,load,nInstances")
}

/POLICY/ && /stat/{
	gsub(/]/, "")
	gsub(/\[/, "")
	print($1","$2","$4","$5)
}

END {
}
