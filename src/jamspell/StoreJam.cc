/**
 * @project zapdos
 * @file src/jamspell/StoreJam.cc
 * @author  S Roychowdhury < sroycode at gmail dot com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury
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
 *  StoreJam :  container for spellcheck implementation
 *
 */

#include "jamspell/StoreJam.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "utils/StringHelpers.hpp"

/**
 * Constructor : default
 *
 */
zpds::jamspell::StoreJam::StoreJam(std::string dbpath, std::string inpath)
{
	if (dbpath.empty()) return;
	const google::protobuf::EnumDescriptor *d = zpds::search::LangTypeE_descriptor();
	for (auto i=0 ; i < d->value_count() ; ++i ) {
		const std::string langpath{dbpath + "/" + d->FindValueByNumber(i)->name() + ".bin"};
		DLOG(INFO) << "Try Loading spellcheck : " << langpath;
		if (langpath.empty() || (!boost::filesystem::is_regular_file(langpath)) ) {
			const std::string filepath{inpath + "/" + d->FindValueByNumber(i)->name() + ".txt"};
			DLOG(INFO) << "Try Loading spellcheck : " << filepath;
			if (filepath.empty() || (!boost::filesystem::is_regular_file(filepath)) ) continue;
			TLangModel model;
			std::string alpha="abcdefghijklmnopqrstuvwxyz";
			model.Train(filepath, alpha);
			model.Dump(langpath);
			LOG(INFO) << "Created spellcheck : " << d->value(i)->name();
		}
		if (!jammap[i].LoadLangModel(langpath))
			throw zpds::InitialException("Corrupt spell file detected at " + langpath);
		LOG(INFO) << "Loaded spellcheck : " << d->value(i)->name();
	}
}

/**
 * Destructor : default
 *
 */
zpds::jamspell::StoreJam::~StoreJam()
{
}

/**
* Correct : correct the spelling
*
*/
std::string zpds::jamspell::StoreJam::Correct(::zpds::search::LangTypeE lang, std::string input) const
{
	std::wstring wtext = UTF8ToWide(input);
	std::wstring result = jammap.at(lang).FixFragment(wtext);
	return WideToUTF8(result);
}

