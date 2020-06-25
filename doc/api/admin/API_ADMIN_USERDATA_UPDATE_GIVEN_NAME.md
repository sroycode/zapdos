# UserData Update Given Name ( POST _admin/api/v1/userdata/update_given_name )

Exter with rights i.e. admin or manager can change given_name for regular users

**Test Script** : `test/test_admin_userdata_update_given_name.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , given_name |

## Typical Session Example

```
POST /_admin/api/v1/userdata/update_given_name HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 158

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "given_name": "test 001 again" }
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

