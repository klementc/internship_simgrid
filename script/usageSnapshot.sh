#!/bin/sh


DATE=`date +%Y-%m-%d_%H-%M-%S`

if [[ "snap" = $1 ]]
then
  if [[ "network" = $2 ]]
  then
    csvOut="snapshot_network_${DATE}.csv"
    echo "containerName,containerID,containerPID,bytesSent,bytesReceived" > ${csvOut}
    echo "Taks snapshot of network usage at time $DATE"
    containers=$(sudo docker ps | awk '{if(NR>1) print $NF,$1}')
    echo "$containers"
    IFS=$'\n'
    for container in $containers
    do
      contName=$(echo $container | awk '{print $1}')
      contID=$(echo $container | awk '{print $2}')
      contPID=`docker inspect -f '{{ .State.Pid }}' $contID`

      echo "Container $contName has pid $contPID, fetch its network usage"
      output=`cat /proc/${contPID}/net/dev | grep eth0`
      echo $output
      bytesReceived=$(echo $output | awk '{print $2}')
      bytesSent=$(echo $output | awk '{print $10}')
      echo "received: $bytesReceived sent: $bytesSent"

      echo "${contName},${contID},${contPID},${bytesSent},${bytesReceived}" >> ${csvOut}
    done
  fi
elif [[ "substract" = $1 ]]
then
  csvOut="diff_$(echo $3 | awk -F  "." '{print $1}')_$(echo $2 | awk -F  "." '{print $1}').csv"
  echo "substract between files $2 and $3, put to $csvOut"
  # for each container, substract values from $2 to the values of $3
  containers=$(tail -n+2 $2)
  echo "containerName,sentDiff,receivedDiff" > ${csvOut}
  for container in $containers
  do
    contName=$(echo $container | awk -F "," '{print $1}')
    bytesReceivedTotal=$(echo $container | awk -F "," '{print $5}')
    bytesSentTotal=$(echo $container | awk -F "," '{print $4}')

    toSub=$(cat $3 | grep $contName)
    bytesReceivedToSub=$(echo $toSub | awk -F "," '{print $5}')
    bytesSentToSub=$(echo $toSub | awk -F "," '{print $4}')
    echo "$"
    echo "$bytesReceivedTotal - $bytesReceivedToSub "
    echo "$bytesSentTotal - $bytesSentToSub "

    recRes=$(($bytesReceivedTotal - $bytesReceivedToSub))
    sendRes=$(($bytesSentTotal - $bytesSentToSub))

    echo "${contName},${sendRes},${recRes}" >> ${csvOut}

  done
fi