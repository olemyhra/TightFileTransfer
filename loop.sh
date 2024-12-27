#!/bin/bash

for i in {1..50};
do
	start_time=`date +%s`
	./tft_client test.txt
	end_time=`date +%s`
	echo run time is `expr $end_time - $start_time`
done
