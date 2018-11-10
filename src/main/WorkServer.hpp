/**
 * @project zapdos
 * @file src/work/WorkServer.hpp
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
 *  WorkServer.hpp :   Work Storage Server Headers
 *
 */
#ifndef _ZPDS_WORK_WORKSERVER_HPP_
#define _ZPDS_WORK_WORKSERVER_HPP_

#include <store/StoreBase.hpp>
#include <utils/ServerBase.hpp>

namespace zpds {
namespace work {

class WorkServer :
	virtual public zpds::utils::ServerBase,
	virtual public zpds::store::StoreBase,
	virtual public std::enable_shared_from_this<WorkServer> {

public:
	using pointer=std::shared_ptr<WorkServer>;

	/**
	* make noncopyable
	*/

	WorkServer() = delete;
	WorkServer(const WorkServer&) = delete;
	WorkServer& operator=(const WorkServer&) = delete;

	/**
	* create : static construction creates new first time
	*
	* @param stptr
	*   zpds::utils::SharedTable::pointer stptr
	*
	* @return
	*   none
	*/
	static pointer create(zpds::utils::SharedTable::pointer stptr)
	{
		pointer p(new WorkServer(stptr));
		return p;
	}

	/**
	* share : return instance
	*
	* @return
	*   none
	*/
	pointer share()
	{
		return shared_from_this();
	}


	/**
	* destructor
	*/
	virtual ~WorkServer ();

	/**
	 * GetSection : section required
	 *
	 * @return
	 *   std::string section
	 */
	const static std::string GetSection();

	/**
	 * GetRequire : parameters required
	 *
	 * @return
	 *   ParamsListT
	 */
	const static zpds::utils::ServerBase::ParamsListT GetRequire();

	/**
	* init : initialize rocksdb and others
	*
	* @param params
	*   ParamsListT parameters to init backend
	*
	* @return
	*   none
	*/
	void init(zpds::utils::ServerBase::ParamsListT params);

	/**
	* stop : shutdown
	*
	* @return
	*   none
	*/
	void stop();

private:

	/**
	* Constructor : private default Constructor
	*
	* @param stptr
	*   zpds::utils::SharedTable::pointer stptr
	*
	*/
	WorkServer(zpds::utils::SharedTable::pointer stptr);

};
} // namespace work
} // namespace zpds
#endif /* _ZPDS_WORK_WORKSERVER_HPP_ */

