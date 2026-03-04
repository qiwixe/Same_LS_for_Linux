#!/bin/bash
# Скрипт для удаления установленного test-ls

set -e

echo "Удаление test-ls"
sudo rm -f /usr/local/bin/test-ls > /dev/null

echo "Удаление завершено."