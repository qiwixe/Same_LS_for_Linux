cmake_minimum_required(VERSION 4.1)
project(test-ls C)

set(CMAKE_C_STANDARD 11)

# Собираем бинарник
add_executable(test-ls main.c)

# Установка в /usr/local/bin
install(TARGETS test-ls
        RUNTIME DESTINATION /usr/local/bin)

# Удаление установленного бинарника
add_custom_target(uninstall
        COMMAND rm -f /usr/local/bin/test-ls
        COMMENT "Removing installed test-ls"
)

# Обновление бинарника
add_custom_target(update
        COMMAND ${CMAKE_COMMAND} --build . --target test-ls
        COMMAND sudo ${CMAKE_COMMAND} --build . --target install
        COMMENT "Updating test-ls"
)