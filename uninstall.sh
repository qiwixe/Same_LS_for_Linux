#!/bin/bash
# Скрипт для удаления установленного test-ls

set -e

echo "Удаляем test-ls из /usr/local/bin..."
sudo rm -f /usr/local/bin/test-ls

echo "Удаление завершено."