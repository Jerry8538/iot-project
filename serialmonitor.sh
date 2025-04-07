#!/bin/bash
read -p "port: " port
read -p "baudrate: " baudrate
stty -F $port $baudrate
echo
while read -r line; do
  printf "%s\n" "$line"
done < $port
