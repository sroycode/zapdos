# UserData Update Email ( POST _admin/api/v1/userdata/update_email )

Exter with rights i.e. admin or manager can change email for regular users

**Test Script** : `test/test_admin_userdata_update_email.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , email |

## Typical Session Example

```
POST /_admin/api/v1/userdata/update_email HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 160

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "primary_email": "test@best.com" }
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

