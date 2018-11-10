/**
 * @project zapdos
 * @file include/jamspell/StoreJam.hpp
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
 *  StoreJam :  container for spellchecker
 *
 */
#ifndef _ZPDS_JAMSPELL_STORE_JAM_HPP_
#define _ZPDS_JAMSPELL_STORE_JAM_HPP_

#include "store/StoreBase.hpp"

#include "jamspell/LangModel.hpp"
#include "jamspell/SpellCorrector.hpp"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <memory>
#include <functional>
#include <unordered_map>

namespace zpds {
namespace jamspell {

class StoreJam : virtual public ::zpds::store::StoreBase {

public:
	using pointer = std::shared_ptr<StoreJam>;
	using JamMapT = std::unordered_map< int , TSpellCorrector >;

	/**
	* Constructor : default
	*
	* @param dbpath
	*   std::string dbpath
	*
	* @param inpath
	*   std::string input dir for generation
	*
	*/
	StoreJam(std::string dbpath, std::string inpath=std::string());

	/**
	* make noncopyable and remove default
	*/
	StoreJam() = delete;
	StoreJam(const StoreJam&) = delete;
	StoreJam& operator=(const StoreJam&) = delete;

	/**
	* destructor
	*/
	virtual ~StoreJam ();

	/**
	* Correct : correct string
	*
	* @param input
	*   std::string input
	*
	* @param lang
	*   ::zpds::search::LangTypeE language
	*
	* @return
	*   std::string
	*/
	std::string Correct( ::zpds::search::LangTypeE lang, std::string input) const;

protected:
	JamMapT jammap;

};

} // namespace store
} // namespace zpds
#endif  // _ZPDS_JAMSPELL_STORE_JAM_HPP_

