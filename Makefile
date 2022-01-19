build: main.c
	gcc main.c base.c loading.c saving.c editing.c -lm -Wall -Wextra -g -o image_editor
clean: 
	rm image_editor