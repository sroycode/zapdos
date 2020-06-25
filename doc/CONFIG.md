# Configuration

## Section system

- accesslog : access log for daemon zpds_server
- errorlog : error log for daemon zpds_server
- hostname : server host name
- shared_secret : key to access _admin/ functions, can override by commandline
- templatedir : ctemplate directory default should be /path-to-zpds/templates
- max_fetch_records: limits max records fetched ( currently unused )

## Section work

- default_adminpass: default admin password for first time creation of admin, can override by commandline
- datadir : location of rocksdb database
- cachesize : rocksdb cachesize in MB ( use at least 8192 on production )
- logdatadir : location of log database , - ignored if XSTR_USE_SEPARATE_LOGDB not set
- logcachesize : log database cachesize in MB , - ignored if XSTR_USE_SEPARATE_LOGDB not set

## Section http

- host : HTTP host for external
- port : HTTP port

## Section hrpc 

- host : host for inter machine access , should not be exposed outside LAN
- port : port
- thisurl : URL to reach this service from other machines in cluster

## Section xapian
- datadir : xapian store directory
- jampath : path to jamspell directory , the spell file for EN is EN.bin (generated)
- jinpath : path to jamspell source file EN.txt for building above file 
