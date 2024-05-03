find . -regex '.*\.\(cpp\|hpp\|cc\|c\|h\)' -exec clang-format -style=file -i {} \;
