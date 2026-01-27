#include "SimpleParser.hpp"
#include "SimpleGenerator.hpp"

#include <fstream>

static std::vector<char> ReadWholeFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file)
		throw std::runtime_error("Cannot open file: " + filename);

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> result(size);

	if (!file.read(result.data(), size))
		throw std::runtime_error("Failed to read file: " + filename);

	return result;
}

int main(int argc, const char** argv)
{
#if 1
	// Check command line arguments
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <unity_version_num> <input_file> <output_file>\n";
		return 1;
	}

	uint64_t unity_version_num = std::stoull(argv[1]);
	std::string input_path = argv[2];
	std::string output_path = argv[3];
#else
	uint64_t unity_version_num = 2022308945;
	std::string input_path = "../../../../GameAssembly/include/System/Threading/ThreadPool.hpp";
	std::string output_path = "test.out";
#endif // 0

	auto input_conetent = ReadWholeFile(input_path);
	auto result = SimpleParse(unity_version_num, input_conetent.data(), input_conetent.size());

	if (!result.has_value())
		return 1;

	std::ofstream output_file(output_path);

	if (!output_file.is_open())
	{
		std::cerr << "Error: Cannot open output file: " << output_path << "\n";
		return 1;
	}

	output_file << SimpleGenerate(*result);

	return 0;
}
