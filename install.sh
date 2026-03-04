#!/bin/bash
# Скрипт для сборки и установки test-ls

set -e  # Прерывать скрипт при любой ошибке

sudo apt update
sudo apt install -y cmake

echo "Создаём папку сборки..."
mkdir -p build
cd build

echo "Конфигурируем проект с помощью CMake..."
cmake ..

echo "Собираем проект..."
make

echo "Устанавливаем test-ls в /usr/local/bin..."
sudo make install

echo "Установка завершена. Вы можете вызвать 'test-ls'."