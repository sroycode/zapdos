# WikiData Upsert ( POST _admin/api/v1/wikidata/upsert )

Exter with admin `is_admin` can upsert any wikidata, non admin with `can_add_items` can update self-created wikidata.

**Test Script** : `test/test_admin_wikidata_upsert.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed, check ItemDataT parameters|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/wikidata/upsert HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 651

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "A1005", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Bangalore Chemists", "summary" : "Description of this shop in Adugodi on wikipedia", "city" : "Bangalore" },
  "payloads" : [
    { "unique_id" : "M1006", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mistaken Record2", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "success" : true,
   "updatecount" : "2",
   "inputcount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.
