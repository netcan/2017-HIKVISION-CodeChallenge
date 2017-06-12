main: main.cpp
	clang++ -DnetcanMachine -std=c++14 -g -lm $< -o $@

test: main
	for i in $$(ls cases/*.txt); \
    do\
		echo "testing " $$i "...";\
        ./main $$i > "$$(echo $$i | sed -r 's/(.*)\.txt/\1result\.out/')";\
		wine cases/fastDemo.exe $$i "$$(echo $$i | sed -r 's/(.*)\.txt/\1result\.out/')";\
	done
