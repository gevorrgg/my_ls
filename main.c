#include <ls.h>

int main(const int argc, const char* argv[])
{
	return ls(argc - 1, argv + 1) == LS_SUCCESS ? 0 : 1;
}
