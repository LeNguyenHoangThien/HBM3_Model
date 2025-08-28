BBS_TRAD:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"BBS" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

BBS_NAPOT:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"BBS" -D"NAPOT" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

BBS_CAMB:
	g++  -g -Wall -fno-stack-protector -D"QUARTER_FETCH" -D"BBS" -D"CAMB_ENABLE" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

BBS_RCPT:
	g++  -g -Wall -fno-stack-protector -D"QUARTER_FETCH" -D"BBS" -D"RCPT_ENABLE" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

BBS_cRCPT:
	g++  -g -Wall -fno-stack-protector -D"QUARTER_FETCH" -D"BBS" -D"cRCPT_ENABLE" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

BBS_ANCH:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"BBS" -D"AT_ENABLE" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp


BBS_PCAD:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"BBS" -D"PCAD" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp


RBS_TRAD:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"RBS" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)" -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp


RBS_PCAD:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"RBS" -D"PCAD" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)"  -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

PBS_PCAD:
	g++  -g -Wall -fno-stack-protector -D"SINGLE_FETCH" -D"PBS" -D"PCAD" -D"MAX_ORDER=9" -D"NUM_TOTAL_PAGES=$(NUM_TOTAL_PAGES)" -D"NUM_REQUESTED_PAGES=$(NUM_REQUESTED_PAGES)" -D"BLOCKSIZE_INIT=$(BLOCKSIZE_INIT)"  -o run ./BuddyTop.cpp ./CQueue.cpp ./CFreeList.cpp ./CBlockList.cpp ./CBlockArray.cpp ./CPageArray.cpp ./CBuddy.cpp ./main.cpp ../Top.cpp

run:
	./run

clean:
	rm -f run
