# Replication

1. Simple replication is supported. 

2. Basically other machine(s) can be started pointing to a master and they will replicate from it. 

3. Writes always happen at master. 

4. If the master goes down , one of the slaves will get promoted to master.

5. To enable replication `hrpc_thisurl` ( OR `thisurl` in section `hrpc` of config ) has to be set to the reachable host:port from 
other machines . For both started on localhost , one at 9091/9092 , other at 9081/9082, the below holds.

```
./zpds_server --config ../etc/zpds2.conf  --jinpath=/path/to/directory \
--master="http://127.0.0.1:9092" --hrpc_thisurl="http://127.0.0.1:9082" 
```

