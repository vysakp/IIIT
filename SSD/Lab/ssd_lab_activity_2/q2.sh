#!/bin/bash


file="/etc/shells"

awk -F "/" '/^\/usr/ {print $NF}' $file