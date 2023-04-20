target := a.x

flags :=
flags += -std=c++20
flags += -fmax-errors=4
flags += -g
flags += -O3

incl :=
incl += -I${SCIDF}/src

link :=

main:
	g++ ${flags} ${incl} main.cc -o ${target} ${link}

clean:
	rm -f ${target}

run: main
	./${target}
