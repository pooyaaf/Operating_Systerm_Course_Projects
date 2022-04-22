MAINPARTS =  meanCalculator course class student
all: $(MAINPARTS)

% : %.c 
	gcc -o $@ $^

clean:
	rm $(MAINPARTS) result.txt

