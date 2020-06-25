# UserData Update Passkey ( POST _admin/api/v1/userdata/update_passkey )

Exter with rights i.e. admin or manager can change passkey for regular users

**Test Script** : `test/test_admin_userdata_update_passkey.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , passkey |

## Typical Session Example

```
POST /_admin/api/v1/userdata/update_passkey HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 151

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "passkey" : "user001p1" }
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

