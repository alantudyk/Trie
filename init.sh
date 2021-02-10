#!/bin/sh

echo
sudo gem install ruby-xz --conservative || exit 1

# echo
# sudo apt install -y liblzma-dev || exit 1

echo
cc -flto -O3 -s make_trie.c -o make_trie &&
cc -flto -O3 -s words.c -o words &&
./make_trie ||
echo 'Something went wrong.'
