#!/bin/bash

for i in {1..50};
do
	./tft_client file2send
	./tft_client data.txt
done
