main: main.cpp
	clang++ -DnetcanMachine -std=c++14 -O3 -lm $< -o $@

test: main
	for i in $$(ls cases/*.txt); \
    do\
		echo "testing " $$i "...";\
        ./main $$i > "$$(echo $$i | sed -r 's/(.*)\.txt/\1\.out/')";\
		wine cases/fastDemo.exe $$i "$$(echo $$i | sed -r 's/(.*)\.txt/\1\.out/')";\
	done

test100: main
	for i in $$(ls cases100/*.txt); \
    do\
		echo "testing " $$i "...";\
        ./main $$i > "$$(echo $$i | sed -r 's/(.*)\.txt/\1\.out/')";\
		wine cases/fastDemo.exe $$i "$$(echo $$i | sed -r 's/(.*)\.txt/\1\.out/')";\
	done
