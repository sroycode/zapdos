# ExterData Delete ( POST _admin/api/v1/exterdata/delete )

Exter with admin can delete other exter users

**Test Script** : `test/test_admin_exterdata_delete.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field name(*) only |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/delete HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 176

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001" },
  "payloads" : [
     { "name" : "test002" }
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

