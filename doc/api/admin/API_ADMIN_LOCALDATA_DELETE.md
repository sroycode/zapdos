# LocalData Delete ( POST _admin/api/v1/localdata/delete )

Exter with admin `is_admin` can delete any localdata, non admin with `can_add_items` can delete self-created localdata.

**Test Script** : `test/test_admin_localdata_delete.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `unique_id` is needed|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/localdata/delete HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 186

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "M1004" },
  "payloads" : [
    { "unique_id" : "M1006" }
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

