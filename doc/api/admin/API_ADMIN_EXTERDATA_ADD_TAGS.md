# ExterData Update Tags ( POST _admin/api/v1/exterdata/add_tags )

Exter with admin `is_admin` can update tags for exter.

**Test Script** : `test/test_admin_exterdata_add_tags.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , tags |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/add_tags HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 183

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001", "tags" : [ { "name" : "goodtag", "value" : "casual" } ] }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 16

{
   "success" : true
}
```

## Note

- Assuming sessionkey exists

