# WikiData Create( POST _admin/api/v1/wikidata/getmany )

Exter can read wikidata. This api is for multiple record fetch using payloads

**Test Script** : `test/test_admin_wikidata_getmany.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/wikidata/getmany HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 233

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payloads" : [
    { "unique_id" : "A1002" },
    { "unique_id" : "A1003" },
    { "unique_id" : "A1004" },
    { "unique_id" : "A1005" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 705

{
   "payloads" : [
      {
         "categories" : [
            "osmdata"
         ],
         "country" : "India",
         "parent" : {},
         "ccode" : "IND",
         "city" : "Kolkata",
         "importance" : 0.39,
         "summary" : "Old Description of this shop in Jadavpur on wikipedia",
         "title" : "something",
         "unique_id" : "A1002"
      },
      {
         "categories" : [
            "osmdata"
         ],
         "country" : "India",
         "parent" : {},
         "ccode" : "IND",
         "city" : "Kolkata",
         "importance" : 0.09,
         "summary" : "Old Description of this shop in Garia on wikipedia",
         "title" : "BandBox Repairs Updated",
         "unique_id" : "A1003"
      },
      {
         "unique_id" : "A1004",
         "notfound" : true
      },
      {
         "summary" : "Description of this shop in Adugodi on wikipedia",
         "importance" : 0.09,
         "city" : "Bangalore",
         "unique_id" : "A1005",
         "title" : "Bangalore Chemists",
         "country" : "India",
         "categories" : [
            "osmdata"
         ],
         "ccode" : "IND",
         "parent" : {}
      }
   ]
}
```

## Note

- Assuming sessionkey exists

