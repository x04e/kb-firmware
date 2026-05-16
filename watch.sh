#!/bin/sh
watchCmd="$1"

srcSum=""
newSum=""

while true; do
    newSum="$(find src/ -type f -exec sha256sum {} \; | sha256sum)"
    if [ "$srcSum" = "$newSum" ]; then
        sleep 1
    else
        printf "Compiling... "
        srcSum="$newSum"

        $watchCmd &&
            printf "\e[32mOK\e[0m\n" ||
            printf "\e[31mFAILED\e[0m\n"
    fi
done