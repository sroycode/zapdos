/**
 * @project zapdos
 * @file src/tools/TestSpell.cc
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2020 S Roychowdhury
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *  TestSpell.cc : zpds_spell : Spellcheck loader n test
 *
 */
#define ZPDS_DEFAULT_EXE_NAME "zpds_spell"
#define ZPDS_DEFAULT_EXE_VERSION "1.0.0"
#define ZPDS_DEFAULT_EXE_COPYRIGHT "Copyright (c) 2018-2019 S Roychowdhury"

#include <iostream>
#include "jamspell/LangModel.hpp"
#include "jamspell/SpellCorrector.hpp"

using namespace zpds::jamspell;

void PrintUsage(const char** argv)
{
	std::cerr << "Usage: " << argv[0] << " mode args" << std::endl;
	std::cerr << "    train dataset.txt resultModel.bin  - train model" << std::endl;
	std::cerr << "    correct model.bin - input sentences and get corrected one" << std::endl;
}

int Train(const std::string& datasetFile, const std::string& resultModelFile)
{
	TLangModel model;
	std::string alpha="abcdefghijklmnopqrstuvwxyz";
	model.Train(datasetFile, alpha);
	model.Dump(resultModelFile);
	return 0;
}

int Correct(const std::string& modelFile)
{
	uint64_t currtime = ZPDS_CURRTIME_MS;
	TSpellCorrector corrector;
	std::cerr << "loading model" << std::endl ;
	if (!corrector.LoadLangModel(modelFile)) {
		std::cerr << "ERROR: failed to load model" << std::endl;
		return 42;
	}
	std::cerr << "model loaded in " << ( ZPDS_CURRTIME_MS - currtime )/1000 << " s" << std::endl;
	std::cerr << ">> ";
	for (std::string line; std::getline(std::cin, line);) {
		currtime = ZPDS_CURRTIME_MS;
		std::wstring wtext = UTF8ToWide(line);
		std::wstring result = corrector.FixFragment(wtext);
		std::cerr << WideToUTF8(result);
		std::cerr << " -- took " << ( ZPDS_CURRTIME_MS - currtime ) << " ms" << std::endl;
		std::cerr << ">> ";
	}
	return 0;
}

/** main */
int main(int argc, const char** argv)
{
	if (argc<3) {
		PrintUsage(argv);
		exit(1);
	}
	std::string mode = argv[1];
	if (mode == "train") {
		if (argc < 4) {
			PrintUsage(argv);
			exit(1);
		}
		std::string datasetFile = argv[2];
		std::string resultModelFile = argv[3];
		return Train(datasetFile, resultModelFile);
	}
	else if (mode == "correct") {
		if (argc < 3) {
			PrintUsage(argv);
			exit(1);
		}
		std::string modelFile = argv[2];
		return Correct(modelFile);
	}

	return 0;
}

