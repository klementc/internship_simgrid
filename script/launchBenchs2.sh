duration=30

csv=res0505_8c.csv
csvlat=reslat0505_8c.csv
echo "ratio,nbn,perc,load,lat">${csvlat}
#echo "ratio,nbn,perc,load,reqPerSec,lat">${csv}

set -x
for load in `seq 1 100 2000`
do
for rat in 100 # 50 70 80 90 100
do
  for perc in 100
	do
		for nbn in 1 2 #1 #2
		do
		file=out_${nbn}_${load}_${perc}.log
		./generated ../../config_files/platforms/two_hosts_100p_8c.xml ../../config_files/configServices/config${nbn}node_${rat}.csv ${load} > $file 2>&1
#		reqPerSec=`awk  -f dsb2.awk $file`
		awk -v va="$rat" -v vb="$nbn" -v vc="$perc" -v vd="$load" -v ve="$reqPerSec" -f dsblat.awk $file >> $csvlat
#		echo "${rat},${nbn},${perc},${load}," >> ${csv}
#		awk -f dsb.awk $file > ${file}.csv
		done
	done
done
done
