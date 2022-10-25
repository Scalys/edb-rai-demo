#!/bin/bash

#echo ":: switch to $1"

cd $(dirname "$0")
cd classes
ln -sfT $1 current
ln -sfT $1 current

