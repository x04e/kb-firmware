#!/bin/sh
watchCmd=""
flashCmd=""

while [ "$1" != "" ]; do
    case "$1" in
        --watch-cmd)
            shift
            watchCmd="$1"
            shift
            ;;
        --flash-cmd)
            shift
            flashCmd="$1"
            shift
            ;;
    esac
done

srcSum=""
newSum=""

while true; do
    # Compile code if changes are detected
    newSum="$(find src/ -type f -exec sha256sum {} \; | sha256sum)"
    if [ "$srcSum" != "$newSum" ]; then
        printf "Compiling... "
        srcSum="$newSum"
        $watchCmd &&
            printf "\e[32mOK\e[0m\n" ||
            printf "\e[31mFAILED\e[0m\n"
    fi

    # Flash firmware if bootloader is active
    if [ -w /dev/ttyACM0 ]; then
        printf "Flashing...  "
        bash -c "$flashCmd >/dev/null" &&
            printf "\e[32mOK\e[0m\n" ||
            printf "\e[31mFAILED\e[0m\n"
        sleep 5
    fi

    sleep 1
done