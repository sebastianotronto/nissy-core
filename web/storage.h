#include <string>
#include <fstream>

namespace storage {
	bool read(std::string, size_t, char *);
	bool write(std::string, size_t, const char *);
}
