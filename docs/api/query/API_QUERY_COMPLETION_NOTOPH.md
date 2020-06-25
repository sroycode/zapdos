# Completion Notoph( POST _query/api/v1/notoph/{profile} )

Public reverse lookup query in photon format  by 'profile' , the 'default' profile is used here.

**Test Script** : `test/test_query_completion_notoph.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `lang` | language default EN |
| `limit` | page size aka no of results |
| `lon`| detected longitude |
| `lat`| detected latitude |
| `ccode` | country code ( filter) |
| `city` | city  ( filter) |

## Typical Session Example

```
GET /_query/api/v1/notoph/default?limit=10&lon=88.88888&lat=22.22222 HTTP/1.1
Accept: */*



HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 544

{
   "type" : "FeatureCollection",
   "features" : [
      {
         "properties" : {
            "city" : "Kolkata",
            "country" : "India",
            "name" : "something,Old Jadavpur"
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
            "city" : "Kolkata",
            "country" : "India",
            "name" : "Medplus Chemists,Jadavpur"
         },
         "geometry" : {
            "type" : "Point",
            "coordinates" : [
               88.88888,
               22.22222
            ]
         },
         "type" : "Feature"
      },
      {
         "properties" : {
            "name" : "BandBox Repairs Updated,Old Garia",
            "country" : "India",
            "city" : "Kolkata"
         },
         "geometry" : {
            "type" : "Point",
            "coordinates" : [
               88.88888,
               22.22222
            ]
         },
         "type" : "Feature"
      }
   ]
}
```

## Note

NA
