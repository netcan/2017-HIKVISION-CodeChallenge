main: main.cpp
	clang++ -DnetcanMachine -std=c++14 -O3 -lm $< -o $@

run: main
	for i in $$(ls cases/*.txt); \
    do\
		echo "running" $$i "...";\
		log="$$(echo $$i | sed -r 's/(.*)\.txt/\1\.log/')";\
		echo `date +'%y/%m/%d %H:%M:%S'` >> $$log;\
        ./main $$i > "$$(echo $$i | sed -r 's/(.*)\.txt/\1\.out/')" 2>> $$log;\
		echo "==============================================\n" >> $$log;\
	done;\

test: main
	for i in $$(ls cases/*.txt); \
    do\
		echo "testing" $$i "...";\
		wine cases/fastDemo.exe $$i "$$(echo $$i | sed -r 's/(.*)\.txt/\1\.out/')";\
	done

