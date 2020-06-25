# Completion TextData( POST _query/api/v1/textdata/{profile} )

Public text lookup query in simple format  by 'profile' , the 'default' profile is used here.

**Test Script** : `test/test_query_completion_textdata.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `q` | query text |
| `lang` | language default EN |
| `f` | set 1 if fulltext aka no partial |
| `limit` | page size aka no of results |
| `lon`| detected longitude ( not used) |
| `lat`| detected latitude (not used) |
| `ccode` | country code (not used)  |
| `city` | city (not used) |

## Typical Session Example

```
GET /_query/api/v1/textdata/default?q=chem&limit=10 HTTP/1.1
Accept: */*



HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 196

{
   "records" : [
      {
         "summary" : "Description of this shop in Jadavpur on wikipedia",
         "title" : "Medplus Chemists"
      },
      {
         "summary" : "Description of this shop in Adugodi on wikipedia",
         "title" : "Bangalore Chemists"
      }
   ]
}
```

## Note

NA
