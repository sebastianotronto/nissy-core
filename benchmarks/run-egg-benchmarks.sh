# egg-solver built with:
#   cargo build --release --locked
#   mkdir -p tables
#   target/release/gen $i auto tables/h$i  # i from 0 to 11
#   target/release/bench tables/h$i tune   # i from 0 to 11

scr="../nissy-core/benchmarks/scrambles"
out="../nissy-core/benchmarks/results_egg.py"

get_solve_time() {
	if [ -n "$4" ]; then
		solve_all="./target/release/solve --all-optimal"
	else
		solve_all="./target/release/solve"
	fi
	$solve_all -t "$1" -c 1 --table "tables/h$2" <"$3" \
		| tail -1 | sed 's/.*cubes wall //; s/ wall.*//'
}

do_all() {
	t=$1
	name=$2

	printf '%s = {\n' "$name" >> "$out"
	for i in 6 7 8 9 10 11 ; do
		echo "Doing 17 moves for h$i with $t threads..."
		t17="$(get_solve_time $t $i "$scr/scrambles-17.txt" "$3")"
		echo "Doing 18 moves for h$i with $t threads..."
		t18="$(get_solve_time $t $i "$scr/scrambles-18.txt" "$3")"
		echo "Doing 19 moves for h$i with $t threads..."
		t19="$(get_solve_time $t $i "$scr/scrambles-19.txt" "$3")"
		printf '\t%s: {17: %s, 18: %s, 19: %s' $i $t17 $t18 $t19 >> "$out"
		if [ "$i" -ge 10 ]; then
			echo "Doing 20 moves for h$i with $t threads..."
			t20="$(get_solve_time $t $i "$scr/scrambles-20.txt" "$3")"
			printf ', 20: %s' $t20 >> "$out"
			echo "Doing superflip moves for h$i with $t threads..."
			tsf="$(get_solve_time $t $i "$scr/superflip.txt" "$3")"
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
do_all 16 "egg_all_solutions" "all"
