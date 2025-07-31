#!/bin/sh

args="$@"

getsolutions() {
	nissy solve -p $args "$1" |
	while read -r solution; do
		printf '\t\t"%s\\n"\n' "$solution"
	done
}

printf "struct {\n\tchar *scramble;\n\tchar *solutions;\n} s[] = {\n"

i=0
while read -r scramble; do
	solutions="$(getsolutions "$scramble")"
	printf '[%s] = {\n\t.scramble = "%s",\n' "$i" "$scramble"
	printf '\t.solutions =\n%s\n' "$solutions"
	printf '},\n'
	i=$((i+1))
done

printf "};\n"
