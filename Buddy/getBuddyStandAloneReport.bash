#!/bin/bash -f

./generateBuddyReport.bash	BBS_TRAD	2	507	65536	> check_BBS_TRAD_2_507_65536
./generateBuddyReport.bash	BBS_TRAD	4	507	65536	> check_BBS_TRAD_4_507_65536
./generateBuddyReport.bash	BBS_TRAD	8	507	65536	> check_BBS_TRAD_8_507_65536
./generateBuddyReport.bash	BBS_TRAD	16	507	65536	> check_BBS_TRAD_16_507_65536
./generateBuddyReport.bash	BBS_TRAD	32	507	65536	> check_BBS_TRAD_32_507_65536
./generateBuddyReport.bash	BBS_TRAD	64	507	65536	> check_BBS_TRAD_64_507_65536
./generateBuddyReport.bash	BBS_TRAD	128	507	65536	> check_BBS_TRAD_128_507_65536
./generateBuddyReport.bash	BBS_TRAD	256	507	65536	> check_BBS_TRAD_256_507_65536
./generateBuddyReport.bash	BBS_NAPOT	2	507	65536	> check_BBS_NAPOT_2_507_65536
./generateBuddyReport.bash	BBS_NAPOT	4	507	65536	> check_BBS_NAPOT_4_507_65536
./generateBuddyReport.bash	BBS_NAPOT	8	507	65536	> check_BBS_NAPOT_8_507_65536
./generateBuddyReport.bash	BBS_NAPOT	16	507	65536	> check_BBS_NAPOT_16_507_65536
./generateBuddyReport.bash	BBS_NAPOT	32	507	65536	> check_BBS_NAPOT_32_507_65536
./generateBuddyReport.bash	BBS_NAPOT	64	507	65536	> check_BBS_NAPOT_64_507_65536
./generateBuddyReport.bash	BBS_NAPOT	128	507	65536	> check_BBS_NAPOT_128_507_65536
./generateBuddyReport.bash	BBS_NAPOT	256	507	65536	> check_BBS_NAPOT_256_507_65536
./generateBuddyReport.bash	BBS_CAMB	2	507	65536	> check_BBS_CAMB_2_507_65536
./generateBuddyReport.bash	BBS_CAMB	4	507	65536	> check_BBS_CAMB_4_507_65536
./generateBuddyReport.bash	BBS_CAMB	8	507	65536	> check_BBS_CAMB_8_507_65536
./generateBuddyReport.bash	BBS_CAMB	16	507	65536	> check_BBS_CAMB_16_507_65536
./generateBuddyReport.bash	BBS_CAMB	32	507	65536	> check_BBS_CAMB_32_507_65536
./generateBuddyReport.bash	BBS_CAMB	64	507	65536	> check_BBS_CAMB_64_507_65536
./generateBuddyReport.bash	BBS_CAMB	128	507	65536	> check_BBS_CAMB_128_507_65536
./generateBuddyReport.bash	BBS_CAMB	256	507	65536	> check_BBS_CAMB_256_507_65536
./generateBuddyReport.bash	BBS_RCPT	2	507	65536	> check_BBS_RCPT_2_507_65536
./generateBuddyReport.bash	BBS_RCPT	4	507	65536	> check_BBS_RCPT_4_507_65536
./generateBuddyReport.bash	BBS_RCPT	8	507	65536	> check_BBS_RCPT_8_507_65536
./generateBuddyReport.bash	BBS_RCPT	16	507	65536	> check_BBS_RCPT_16_507_65536
./generateBuddyReport.bash	BBS_RCPT	32	507	65536	> check_BBS_RCPT_32_507_65536
./generateBuddyReport.bash	BBS_RCPT	64	507	65536	> check_BBS_RCPT_64_507_65536
./generateBuddyReport.bash	BBS_RCPT	128	507	65536	> check_BBS_RCPT_128_507_65536
./generateBuddyReport.bash	BBS_RCPT	256	507	65536	> check_BBS_RCPT_256_507_65536
./generateBuddyReport.bash	BBS_cRCPT	2	507	65536	> check_BBS_cRCPT_2_507_65536
./generateBuddyReport.bash	BBS_cRCPT	4	507	65536	> check_BBS_cRCPT_4_507_65536
./generateBuddyReport.bash	BBS_cRCPT	8	507	65536	> check_BBS_cRCPT_8_507_65536
./generateBuddyReport.bash	BBS_cRCPT	16	507	65536	> check_BBS_cRCPT_16_507_65536
./generateBuddyReport.bash	BBS_cRCPT	32	507	65536	> check_BBS_cRCPT_32_507_65536
./generateBuddyReport.bash	BBS_cRCPT	64	507	65536	> check_BBS_cRCPT_64_507_65536
./generateBuddyReport.bash	BBS_cRCPT	128	507	65536	> check_BBS_cRCPT_128_507_65536
./generateBuddyReport.bash	BBS_cRCPT	256	507	65536	> check_BBS_cRCPT_256_507_65536


FILES=`ls | grep check_`
for file in $FILES
do
	printf "%-40s" $file
	grep Allocation $file | awk '{printf "%-10.2f ", $2}'
	grep De-allocation $file | awk '{printf "%-10.2f ", $2}'
	grep "Free Nodes" $file | awk '{printf "%-10d ", $NF}'
	grep "allocated node" $file | awk '{printf "%-10d ", $NF}'
	grep AvgAddrCovrPTE $file | awk '{printf "%-10.2f \n", $NF}'
done