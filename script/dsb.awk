#!/usr/bin/awk -f

BEGIN {
	print("timestamp")
}

/FINISHED REQUEST/{
	print($8)
}

END {
}
