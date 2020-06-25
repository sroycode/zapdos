# WikiData Update ( POST _admin/api/v1/wikidata/update )

Exter with admin `is_admin` can update any wikidata, non admin with `can_add_items` can update self-created wikidata.

**Test Script** : `test/test_admin_wikidata_update.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed, check ItemDataT parameters|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/wikidata/update HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 668

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "A1002", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mannada Chemists Updated", "summary" : "Old Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" },
  "payloads" : [
    { "unique_id" : "A1003", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "BandBox Repairs Updated", "summary" : "Old Description of this shop in Garia on wikipedia", "city" : "Kolkata" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "updatecount" : "2",
   "success" : true,
   "inputcount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

