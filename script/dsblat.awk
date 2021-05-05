#!/usr/bin/awk -f

BEGIN {
}

/FINISHED REQUEST/{
	print(va","vb","vc","vd","$12)
}

END {
}
