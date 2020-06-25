# WikiData Create( POST _admin/api/v1/wikidata/create )

Exter with admin `is_admin` or `can_add_items` can create wikidata. The latter can modify its own only.

**Test Script** : `test/test_admin_wikidata_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed, check ItemDataT parameters|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/wikidata/create HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 1171

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "A1001", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Medplus Chemists", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" },
  "payloads" : [
    { "unique_id" : "A1002", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mannada Chemists", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" },
    { "unique_id" : "A1003", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "BandBox Repairs", "summary" : "Description of this shop in Garia on wikipedia", "city" : "Kolkata" },
    { "unique_id" : "M1004", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mistaken Record", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "success" : true,
   "updatecount" : "4",
   "inputcount" : "4"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

