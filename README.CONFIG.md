# Configuration

## Section system

- accesslog : access log for daemon zpdsserver
- errorlog : error log for daemon zpdsserver
- baseurl : base URL to append to callback to this instance, this may be load-balanced/proxied and ssl offloading might happen external
- hostname : server host name
- shared_secret : key to access _admin/ functions
- dont_use_cache : if set greater than 0 memory cache is not used
- dont_create_profile : if set greater than 0 profile creation is disabled
- templatedir : ctemplate directory default should be /path-to-zpds/templates


## Section work

- datadir : location of rocksdb database
- cachesize : rocksdb cachesize in MB
- logdatadir : location of log database , - ignored if ZPDS_USE_SEPARATE_LOGDB not set
- logcachesize : log database cachesize in MB , - ignored if ZPDS_USE_SEPARATE_LOGDB not set

## Section http

- host : HTTP host for internal HTTP host
- port : HTTP port

## Section hrpc 

- host : intermachine host for internal HTTP host
- port : intermachine port
- thisurl : URL to reach this service from other machines in cluster

## Section whatwhere
- datadir : xapian store directory
- jampath : path to jamspell directory
- jinpath : path to jamspell source file for building above file 

