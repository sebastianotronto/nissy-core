scr="../nissy-core/benchmarks/scrambles"
out="../nissy-core/benchmarks/results_egg.py"

get_solve_time() {
	./target/release/solve -t "$1" --table "tables/h$2" <"$3" \
		| tail -1 | sed 's/.*cubes wall //; s/ wall.*//'
}

do_all() {
	echo "doing all $1 $2"
	t=$1
	name=$2
	printf '%s = {\n' "$name" >> "$out"
	for i in 6 7 8 9 10 11 ; do
		echo "Doing 17 moves for h$i with $t threads..."
		t17="$(get_solve_time $t $i "$scr/scrambles-17.txt")"
		echo "Doing 18 moves for h$i with $t threads..."
		t18="$(get_solve_time $t $i "$scr/scrambles-18.txt")"
		echo "Doing 19 moves for h$i with $t threads..."
		t19="$(get_solve_time $t $i "$scr/scrambles-19.txt")"
		printf '\t%s: {17: %s, 18: %s, 19: %s' $i $t17 $t18 $t19 >> "$out"
		if [ "$i" -ge 10 ]; then
			echo "Doing 20 moves for h$i with $t threads..."
			t20="$(get_solve_time $t $i "$scr/scrambles-20.txt")"
			printf ', 20: %s' $t20 >> "$out"
			echo "Doing superflip moves for h$i with $t threads..."
			tsf="$(get_solve_time $t $i "$scr/superflip.txt")"
			printf ', "superflip": %s},\n' $tsf >> "$out"
		else
			printf '},\n' >> "$out"
		fi
	done
	printf '}\n\n' >> "$out"
}

printf '' > "$out"

do_all 1 "egg_single_thread"
do_all 4 "egg_4_threads"
do_all 16 "egg_16_threads"
