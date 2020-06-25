# UserData Update mobile ( POST _admin/api/v1/userdata/update_mobile )

Exter with rights i.e. admin or manager can change mobile for regular users

**Test Script** : `test/test_admin_userdata_update_mobile.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , mobile |

## Typical Session Example

```
POST /_admin/api/v1/userdata/update_mobile HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 161

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "primary_mobile": "+919999999999" }
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

