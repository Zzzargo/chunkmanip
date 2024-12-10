#!/bin/bash

YELLOW="\033[1;33m"
RESET="\033[0m"

zip -r tema2.zip libchunk/*.{c,h} Makefile

README=$(find . -maxdepth 1 -type f | grep -i readme)

if [ $? -eq 0 ]; then
    zip -u tema2.zip "${README}"
else
    echo -e "\n${YELLOW}Warning: No README.md file found.${RESET}"
fi
