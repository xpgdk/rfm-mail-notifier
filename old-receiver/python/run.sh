#!/bin/sh

while /bin/true; do
	./receiver.py >> log 2>> stdout
done
