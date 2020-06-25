# LocalData Update ( POST _admin/api/v1/localdata/update )

Exter with admin `is_admin` can update any localdata, non admin with `can_add_items` can update self-created localdata.

**Test Script** : `test/test_admin_localdata_update.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed, check ItemDataT parameters|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/localdata/update HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 594

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "A1002", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "fld_name" : "Mannada Chemists Updated", "fld_area" : "Old Jadavpur", "city" : "Kolkata" },
  "payloads" : [
    { "unique_id" : "A1003", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "fld_name" : "BandBox Repairs Updated", "fld_area" : "Old Garia", "city" : "Kolkata" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "updatecount" : "2",
   "inputcount" : "2",
   "success" : true
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

