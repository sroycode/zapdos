# ExterData Update Email ( POST _admin/api/v1/exterdata/update_email )

Exter with admin and self can change email

**Test Script** : `test/test_admin_exterdata_update_email.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , email |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/update_email HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 160

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001", "primary_email": "test@best.com" }
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

