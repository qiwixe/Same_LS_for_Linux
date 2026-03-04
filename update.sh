#!/bin/bash
# Скрипт для обновления test-ls
# Пересобирает и заменяет бинарник

set -e

git fetch origin > /dev/null
git pull origin > /dev/null

mkdir -p build > /dev/null
cd build > /dev/null
cmake .. > /dev/null
make > /dev/null
sudo make install > /dev/null

echo "Обновление завершено."