#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <ftxui/dom/elements.hpp>   // for text, gauge, operator|, flex, hbox, Element
#include <ftxui/screen/screen.hpp>  // for Screen
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/color.hpp"   // for ftxui

using namespace std::filesystem;
using namespace ftxui;
using namespace std::chrono_literals;

const size_t CPVG_BLOCK_SIZE = (2 << 12) * (2 << 5);

size_t cpvg(const char* src, const char* dst, size_t block_size = CPVG_BLOCK_SIZE)
{
	std::filesystem::path psrc(std::filesystem::canonical(src));
	std::filesystem::path pdst(std::filesystem::canonical(dst));
	std::string reset_position;

	if(!exists(psrc) || block_size == 0)
		return 0;

	if(is_directory(pdst) && (pdst.filename() != psrc.filename()))
		pdst /= psrc.filename();


	size_t size = file_size(psrc);
	std::string prefix(exists(pdst) ? " overwriting:" : " copying:" );
	std::string suffix("/" + std::to_string(block_size) + "/" + std::to_string(size));
	FILE* ifp   = fopen(psrc.c_str(), "rb");
	FILE* ofp   = fopen(pdst.c_str(), "w+b");
	void* buf   = malloc(block_size);

	size_t transfered = 0;
	for (size_t res = 0; transfered < size; ) {
		const size_t len   = std::min(size - transfered, block_size);
		const size_t blk   = (len == block_size) ? block_size : 1;
		const size_t nmemb = (len == block_size) ? 1 : len;

		if((res = fread((unsigned char*)buf, blk, nmemb, ifp)) != nmemb)
			break;
		if((res = fwrite((unsigned char*)buf, blk, nmemb, ofp)) != nmemb)
			break;

		res = (len == block_size) ? block_size : res;

		transfered += res;
		auto document = hbox({
				text(prefix),
				gauge((float)transfered / (float)(size)) | flex,
				text(" " + std::to_string(transfered) + suffix)
				});
		auto screen = Screen(100, 1);
		Render(screen, document);
		std::cout << reset_position;
		screen.Print();
		reset_position = screen.ResetPosition();
		if(res < len)
			break;
	}

	std::cout << ((transfered == size) ? " Finished!" : " Failure!") << std::endl;

	fclose(ifp);
	fclose(ofp);
	free(buf);
	return transfered;
}

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		fprintf(stderr, "Usage: %s SRC DST [BLK]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	exit(file_size(argv[1]) == cpvg(argv[1], argv[2], argc > 3 ? (size_t)strtouq(argv[3], NULL, 10) : CPVG_BLOCK_SIZE)
	     ? EXIT_SUCCESS : EXIT_FAILURE);
}

