#!/bin/bash
# Скрипт для сборки и установки test-ls

set -e  # Прерывать скрипт при любой ошибке

sudo apt-get update > /dev/null
sudo apt-get install -y cmake > /dev/null

mkdir -p build > /dev/null
cd build > /dev/null
cmake .. > /dev/null
make > /dev/null

sudo make install > /dev/null

echo "Установка завершена. Вы можете вызвать 'test-ls'."