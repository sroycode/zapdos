#!/bin/sh
if [ -z ${ZAPDOS_SOURCE} ] ; then echo "PLEASE DEFINE ZAPDOS_SOURCE"; exit 1; fi

#### hpp ####

function add_hpp_file () {
local jsfile="$1"
local name=`jq -r '.name' ${jsfile}`
local desc=`jq -r '.desc' ${jsfile}`
local lc_name=`echo $name | awk '{print tolower($0)}'`
local namespace=`jq -r '.namespace' ${jsfile}`
local hc_name=`echo "$namespace::$name" | awk 'BEGIN{FS="::"}{R=$3;gsub(/[A-Z]/,"_\&",R); print toupper($1) "_" toupper($2) toupper(R)}'`
local fname=`jq -r '.fname' ${jsfile}`
local ALIASLINE=""
if [ "${fname}" == "null" -o -z ${fname} ] ; then
	ALIASLINE="";
else
	ALIASLINE="using ${name}T=${fname}T;"
fi

cat << SHREOFF

#ifndef _${hc_name}_TABLE_HPP_
#define _${hc_name}_TABLE_HPP_

#include "StoreTable.hpp"

namespace zpds {
namespace store {
$ALIASLINE
class ${name}Table : public StoreTable<${name}T> {
public:
	using StoreTable<${name}T>::dbpointer;
	using StoreTable<${name}T>::StoreTable;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	${name}Table(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~${name}Table() {}

	/**
	* make noncopyable and remove default
	*/

	${name}Table() = delete;
	${name}Table(const std::string, const size_t) = delete;
	${name}Table(const ${name}Table&) = delete;
	${name}Table& operator=(const ${name}Table&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   ${name}T* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param pre
	*   bool non unique keys
	*
	* @return
	*   std::string key
	*/
	std::string GetKey(${name}T* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace zpds
#endif /* _${hc_name}_TABLE_HPP_ */
SHREOFF
}

#### cc ####
function add_cc_head () {
local jsfile="$1"
local name=`jq -r '.name' ${jsfile}`
local lc_name=`echo $name | awk '{print tolower($0)}'`
local uc_name=`echo $name | awk '{print toupper($0)}'`
local desc=`jq -r '.desc' ${jsfile}`
local namespace=`jq -r '.namespace' ${jsfile}`
local hc_name=`echo "$namespace::$name" | awk 'BEGIN{FS="::"}{R=$3;gsub(/[A-Z]/,"_\&",R); print toupper($1) "_" toupper($2) toupper(R)}'`

cat << SHREOFF
#include "store/${name}Table.hpp"
SHREOFF

local kno=0;
local primary_key="`jq -r '.pkey' ${jsfile}`";
local primary_name="`jq -r '.pname' ${jsfile}`";
local unique_keys="";
local index_keys="";
local keytype="`jq -r '.uindex['${kno}'].keytype' ${jsfile}`";
while [ "${keytype}" != "null" ] ; do
	if [ ${kno} -eq 0 ] ; then unique_keys="${keytype}";
	else unique_keys="${unique_keys},${keytype}"; fi
	kno=$((kno+1));
	keytype="`jq -r '.uindex['${kno}'].keytype' ${jsfile}`"
done
kno=0;
keytype="`jq -r '.sindex['${kno}'].keytype' ${jsfile}`";
while [ "${keytype}" != "null" ] ; do
	if [ ${kno} -eq 0 ] ; then index_keys="${keytype}";
	else index_keys="${index_keys},${keytype}"; fi
	kno=$((kno+1));
	keytype="`jq -r '.sindex['${kno}'].keytype' ${jsfile}`"
done

cat << SHREOFF

/**
* Constructor
*
*/
zpds::store::${name}Table::${name}Table( zpds::store::${name}Table::dbpointer trydb )
	: zpds::store::${name}Table::${name}Table(
	trydb,
	${primary_key},
	{ $unique_keys },
	{ $index_keys }
	)

{}
SHREOFF
}

function add_cc_GetSecondary () {
local jsfile="$1"
local name=`jq -r '.name' ${jsfile}`
local uc_name=`echo $name | awk '{print toupper($0)}'`
local lc_name=`echo $name | awk '{print tolower($0)}'`
local namespace=`jq -r '.namespace' ${jsfile}`
local primary_key="`jq -r '.pkey' ${jsfile}`";

cat << SHREOFF
/**
* GetKey: get a secondary key
*
*/
std::string zpds::store::${name}Table::GetKey(zpds::store::${name}T* record, zpds::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case ${primary_key}:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
SHREOFF

local kno=0;
local keytype="`jq -r '.uindex['${kno}'].keytype' ${jsfile}`";
while [ "${keytype}" != "null" ] ; do
	local t_string="";
	local i_string="";
	local ino=0;
	local iname="`jq -r '.uindex['${kno}'].keys['${ino}'].name' ${jsfile}`";
	local itype="`jq -r '.uindex['${kno}'].keys['${ino}'].type' ${jsfile}`";
	while [ ${iname} != "null" -a ${itype} != "null" ] ; do
		if [ ${ino} -eq 0 ] ; then 
			i_string="record->${iname}()"; t_string="$itype";
		else
			i_string="$i_string,record->${iname}()"; t_string="$t_string,$itype";
		fi
		ino=$((ino+1))
		iname="`jq -r '.uindex['${kno}'].keys['${ino}'].name' ${jsfile}`";
		itype="`jq -r '.uindex['${kno}'].keys['${ino}'].type' ${jsfile}`";
	done

cat << SHREOFF
	case ${keytype} : {
		key = EncodeSecondaryKey<${t_string}>(keytype , ${i_string}  );
		break;
	}
SHREOFF

	kno=$((kno+1));
	keytype="`jq -r '.uindex['${kno}'].keytype' ${jsfile}`"
done

# non unique
kno=0;
keytype="`jq -r '.sindex['${kno}'].keytype' ${jsfile}`";
while [ "${keytype}" != "null" ] ; do
	local t_string="";
	local i_string="";
	local ino=0;
	local iname="`jq -r '.sindex['${kno}'].keys['${ino}'].name' ${jsfile}`";
	local itype="`jq -r '.sindex['${kno}'].keys['${ino}'].type' ${jsfile}`";
	while [ ${iname} != "null" -a ${itype} != "null" ] ; do
		if [ ${ino} -eq 0 ] ; then 
			i_string="record->${iname}()"; t_string="$itype";
		else
			i_string="$i_string,record->${iname}()"; t_string="$t_string,$itype";
		fi
		ino=$((ino+1))
		iname="`jq -r '.sindex['${kno}'].keys['${ino}'].name' ${jsfile}`";
		itype="`jq -r '.sindex['${kno}'].keys['${ino}'].type' ${jsfile}`";
	done

cat << SHREOFF
	case ${keytype} : {
		key = (pre) ?
		      EncodeSecondaryKey<${t_string}>(keytype , ${i_string} )
			    : EncodeSecondaryKey<${t_string},uint64_t>(keytype , ${i_string},record->id() );
		break;
	}
SHREOFF

	kno=$((kno+1));
	keytype="`jq -r '.sindex['${kno}'].keytype' ${jsfile}`"
done
cat << SHREOFF
	}
	return key;
}
SHREOFF
}

# main

if [ $# -ne 1 ] ; then echo "Usage $0 FileName.js" ; exit 1 ; fi

jsfile=$1

if [ ! -f ${jsfile} ] ; then echo "${jsfile} not found" ; exit 1 ; fi

project=`jq -r '.project' ${jsfile}`
if [ "${project}" = "null" ] ; then echo "project not set" ; exit 1 ; fi

namespace=`jq -r '.namespace' ${jsfile}`
if [ "${namespace}" = "null" ] ; then echo "namespace not set" ; exit 1 ; fi

name=`jq -r '.name' ${jsfile}`
if [ "${name}" = "null" ] ; then echo "name not set" ; exit 1 ; fi

desc=`jq -r '.desc' ${jsfile}`
if [ "${desc}" = "null" ] ; then echo "desc not set" ; exit 1 ; fi

## action

add_hpp_file "$jsfile" > ${ZAPDOS_SOURCE}/include/store/${name}Table.hpp
add_cc_head "$jsfile" > ${ZAPDOS_SOURCE}/src/store/${name}Table.cc
add_cc_GetSecondary "$jsfile" >> ${ZAPDOS_SOURCE}/src/store/${name}Table.cc

cd ${ZAPDOS_SOURCE}
sh scripts/header.sh ${ZAPDOS_SOURCE}/src/store/${name}Table.cc "${name} table implementation"
sh scripts/header.sh ${ZAPDOS_SOURCE}/include/store/${name}Table.hpp "${name} table headers"

