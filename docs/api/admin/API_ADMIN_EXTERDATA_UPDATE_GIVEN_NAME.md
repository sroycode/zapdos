# ExterData Update Given Name ( POST _admin/api/v1/exterdata/update_given_name )

Exter with admin and self can change given name

**Test Script** : `test/test_admin_exterdata_update_given_name.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , given_name |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/update_given_name HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 158

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001", "given_name": "test 001 again" }
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

