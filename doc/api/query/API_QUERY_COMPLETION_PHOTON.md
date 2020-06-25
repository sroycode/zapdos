# Completion Photon( POST _query/api/v1/photon/{profile} )

Public place lookup query in photon format  by 'profile' , the 'default' profile is used here.

**Test Script** : `test/test_query_completion_photon.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `q` | query text |
| `lang` | language default EN |
| `f` | set 1 if fulltext aka no partial |
| `limit` | page size aka no of results |
| `lon`| detected longitude |
| `lat`| detected latitude |
| `ccode` | country code |
| `city` | city  |

## Typical Session Example

```
GET /_query/api/v1/photon/default?q=chem&limit=10&lon=88.88888&lat=22.22222 HTTP/1.1
Accept: */*



HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 376

{
   "features" : [
      {
         "properties" : {
            "city" : "Kolkata",
            "country" : "India",
            "name" : "Medplus Chemists,Jadavpur"
         },
         "type" : "Feature",
         "geometry" : {
            "type" : "Point",
            "coordinates" : [
               88.88888,
               22.22222
            ]
         }
      },
      {
         "properties" : {
            "city" : "Bangalore",
            "country" : "India",
            "name" : "Bangalore Chemists,Adugodi"
         },
         "geometry" : {
            "coordinates" : [
               77.77777,
               12.22222
            ],
            "type" : "Point"
         },
         "type" : "Feature"
      }
   ],
   "type" : "FeatureCollection"
}
```

## Note

NA
