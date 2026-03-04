#!/bin/bash
# Скрипт для обновления test-ls
# Пересобирает и заменяет бинарник

set -e

echo "Создаём/переходим в папку сборки..."
mkdir -p build
cd build

echo "Конфигурируем проект с помощью CMake..."
cmake ..

echo "Собираем проект..."
make

echo "Обновляем test-ls в /usr/local/bin..."
sudo make install

echo "Обновление завершено."