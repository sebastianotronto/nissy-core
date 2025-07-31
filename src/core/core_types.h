#define MOVES_STRUCT_MAXLEN 1000

typedef struct {
	cube_t cube;
	uint8_t orientation;
} oriented_cube_t;

typedef struct {
	size_t nnormal;
	size_t ninverse;
	uint8_t normal[MOVES_STRUCT_MAXLEN];
	uint8_t inverse[MOVES_STRUCT_MAXLEN];
} moves_struct_t;
