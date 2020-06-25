# LocalData Upsert ( POST _admin/api/v1/localdata/upsert )

Exter with admin `is_admin` can upsert any localdata, non admin with `can_add_items` can update self-created localdata.

**Test Script** : `test/test_admin_localdata_upsert.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed, check ItemDataT parameters|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/localdata/merge HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 318

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "A1001", "importance" : 0.29, "tags" : [ { "name" : "repeatag" , "value" : "yet another" } ] },
  "payloads" : [
    { "unique_id" : "A1002", "importance" : 0.39, "fld_name" : "something" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "inputcount" : "2",
   "updatecount" : "2",
   "success" : true
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

