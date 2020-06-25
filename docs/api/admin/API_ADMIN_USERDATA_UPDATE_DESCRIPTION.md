# UserData Update Description ( POST _admin/api/v1/userdata/update_description )

Exter with rights i.e. admin or manager can change description for regular users

**Test Script** : `test/test_admin_userdata_update_description.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields , name(*) , description

## Typical Session Example

```
POST /_admin/api/v1/userdata/update_description HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 158

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "description" : "user001 desc" }
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

