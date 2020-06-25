# Project Zapdos

Search engine for OSM location data, wiki data and maybe more.

The steps to compile are in [here](./COMPILE.md). After compilation it builds these:

| Program | Description |
| ---------- | ----------- |
`zpds_server` | This is the main server executable , for syntax use `zpds_server -helpshort` |
`zpds_adddata` | Tool to add data in bulk, for syntax use `zpds_adddata -helpshort` |
`zpds_dumpdb` | Debugging tool for database, dumps all entries, never use in production |
`zpds_extractwiki` | To extract data from mediawiki dump , for syntax use `zpds_extractwiki -helpshort` |
`zpds_spell` | Testing program for spellchecker, for syntax use `zpds_spell` without params |
`zpds_xapindex` | One time build larger indexes with hundreds of millions of records, for syntax use `zpds_xapindex -helpshort` |

# Main Server `zpds_server`

This is the main application that runs as the search engine. As of now , it supports locational data and plain text data.
Currently, the system has admin users called `Exter` who can add or manipulate data. A default `admin` account with
default password as set in config or runtime parameter as well as another called `default` with the default
search profile are auto-created at system start.  For details see [Exter Data Type](./USERDATA.md#exter-data-type).

Typically after install, if you are working only with OSM or Wiki data, you can just start populating the same.
See see [data](./BULKDATA.md).

The `test` folder contains testing scripts for all apis which are described in the links below.

Several features of this system are built as groundwork for future development and other related projects using this base.

## Key Features

- For populating osm data, see [data](./BULKDATA.md).
- For high-available cluster, see [replication](./REPLICATION.md).
- For configuration, see [configuration](./CONFIGURATION.md).
- For using the spellchecker, see [spellchecker](./SPELLCHECKER.md).
- For finding out why this project exists, see [motivation](./MOTIVATION.md).

## Query Endpoints

- [Completion Queries](./COMPLETION.md#completion-queries) for the query endpoints.

## Common APIs

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md) login as admin user , needed for all api calls
- [User Login](./api/user/API_USER_LOGIN.md) login as regular user , needed for all api calls

## Setup Data Types

- [Tag Data Type](./CATEGORYTAG.md#tagdata-type) is for tag handling. Note that tags need to be configured to be used in input.
- [Category Data Type](./CATEGORYTAG.md#category-type) is for handling category.

## User Data Types

- [Exter Data Type](./USERDATA.md#exter-data-type) is for Exter ( aka admin) handling. Only those with admin rights can create exter.
- [User Data Type](./USERDATA.md#user-data-type) is for regular users , signed-in users if you need.

## Item Data Types

- [Local Data Type](./ITEMDATA.md#localdata-type) is for each item of type local data.
- [Wiki Data Type](./ITEMDATA.md#wikidata-type) is for wiki data entries.


There is a set of helper scripts to auto login as `admin` or `user01` after latter is created.

## Who can change what

- [ServiceDefine.hpp](../include/store/ServiceDefine.hpp) contains the rules.


# Tool `zpds_adddata` 

# Tool `zpds_dumpdb` 

# Tool `zpds_extractwiki` 

# Tool `zpds_spell` 

# Tool `zpds_xapindex` 
