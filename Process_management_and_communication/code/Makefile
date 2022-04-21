MAINPARTS =  meanCalculator course class student
all: $(MAINPARTS)

% : %.c utility.c
	gcc -o $@ $^

clean:
	rm $(MAINPARTS) result.txt

