# ExterData Update Passkey ( POST _admin/api/v1/exterdata/update_passkey )

Exter with admin and self can change passkey

**Test Script** : `test/test_admin_exterdata_update_passkey.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , passkey |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/update_passkey HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 151

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001", "passkey" : "test001p1" }
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

