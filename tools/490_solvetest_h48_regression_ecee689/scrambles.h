struct {
	char *scramble;
	char *solutions;
} s[] = {
[0] = {
	.scramble = "U L' B' L2 B' R2 U B U L2 B L' U'",
	.solutions = "U L B' L2 U' B' U' R2 B L2 B L U'\n"
},
{
	.scramble = "", /* End-of-list signal */
}
};
