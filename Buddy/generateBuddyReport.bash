#!/bin/bash -f

#$1: buddy type
make $1 BLOCKSIZE_INIT=$2 NUM_REQUESTED_PAGES=$3 NUM_TOTAL_PAGES=$4

numOfTestCase=100

for((i = 0; i < $numOfTestCase; i++))
do
	./run > result_${i}
done

FILES=`ls | grep result_*`
allocation=0
deallocation=0
step=0
for file in $FILES
do
	allocation=$(grep Allocations $file | awk -v allo=$allocation '{print allo+$6}')
	deallocation=$(grep De-allocations $file | awk -v deallo=$deallocation '{print deallo+$6}')
	step=$(($step + 1))
done

allocation=$(echo | awk -v allo=$allocation -v step=$step '{print (allo / step)}')
deallocation=$(echo | awk -v deallo=$deallocation -v step=$step '{print (deallo / step)}')

nCurNum=$(grep nCurNum result_0 | tail -n 1 | awk '{print $NF}')

nFreeNodes=$(grep FreeNodes result_0 | awk '{print $NF}')

AvgAddrCovrPTE=$(grep AvgAddrCovrPTE result_0 | awk '{print $NF}')

#echo $1 with NUM_REQUESTED_PAGES = $2, BLOCKSIZE_INIT = $3, NUM_NODE = $(($4 / $3))
echo $1 with NUM_REQUESTED_PAGES = $3, BLOCKSIZE_INIT = $2
echo "Number of Free Nodes:     " $nFreeNodes
echo "Number of allocated node: " $nCurNum
echo "AllocationTime:           " $allocation
echo "De-allocationTime:        " $deallocation
echo "AvgAddrCovrPTE:           " $AvgAddrCovrPTE

for((i = 0; i < $numOfTestCase; i++))
do
	rm -f result_${i}
done
