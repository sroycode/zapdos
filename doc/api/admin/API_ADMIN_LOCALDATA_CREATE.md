# LocalData Create( POST _admin/api/v1/localdata/create )

Exter with admin `is_admin` or `can_add_items` can create localdata. The latter can modify its own only.

**Test Script** : `test/test_admin_localdata_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed, check ItemDataT parameters|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/localdata/create HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 1019

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "A1001", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "fld_name" : "Medplus Chemists", "fld_area" : "Jadavpur", "city" : "Kolkata" },
  "payloads" : [
    { "unique_id" : "A1002", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "fld_name" : "Mannada Chemists", "fld_area" : "Jadavpur", "city" : "Kolkata" },
    { "unique_id" : "A1003", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "fld_name" : "BandBox Repairs", "fld_area" : "Garia", "city" : "Kolkata" },
    { "unique_id" : "M1004", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "fld_name" : "Mistaken Record", "fld_area" : "Jadavpur", "city" : "Kolkata" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "success" : true,
   "inputcount" : "4",
   "updatecount" : "4"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

