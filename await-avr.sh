#!/bin/sh
maxAttempts=20
attempts=0

printf "Waiting for AVR to be writable."
while [ $attempts -le $maxAttempts ] && [ ! -w /dev/ttyACM0 ]; do
    attempts=$(( attempts + 1 ))
    printf "."
    sleep 1
done
printf "\n"

if [ ! -w /dev/ttyACM0 ]; then
    printf "AVR was not writable after %d seconds\n" "$attempts" >&2
    exit 1
fi
