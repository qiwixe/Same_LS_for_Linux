# Same_LS_for_Linux

Тестовое задание — реализация упрощённой версии утилиты **`ls`** для Linux.

## Реализованные аргументы

- **-l** — расширенный вывод информации о файлах
- **-r** — вывод файлов в обратном порядке
- **-h** — вывод размера файлов в человеко-читаемом формате

## Установка утилиты
```bash
sudo apt update && sudo apt upgrade -y
sudo apt install git

git clone https://github.com/qiwixe/Same_LS_for_Linux.git
cd Same_LS_for_Linux

sudo ./install.sh
```
## Использование
```bash
test-ls -lhr /user/home
```
## Пример вывода
```bash
PERM       LINK OWNER    GROUP     SIZE          DATE      FILE NAME
-------------------------------------------------------------------
drwxr-xr-x    2 user     user      4.0K      Mar 05 12:40  Documents
-rw-r--r--    1 user     user      1.2K      Mar 05 12:41  file.txt