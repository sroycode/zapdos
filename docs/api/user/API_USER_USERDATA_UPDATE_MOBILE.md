# UserData Update mobile ( POST _user/api/v1/userdata/update_mobile )

Users ( self ) can change mobile for regular users, *currently disabled*

**Test Script** : `test/test_user_userdata_update_mobile.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , mobile |

## Typical Session Example

```
POST /_user/api/v1/userdata/update_mobile HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 163

{
  "sessionkey" : "gSDBzI3R1EWjXL2bl06+Y4wxPtOwI1fDAfrStFdJXZ0PepMW0XqkbiPO5i0=",
  "username" : "user001",
  "payload" : { "primary_mobile": "+919900000099" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 42

{
   "errorcode" : 0,
   "error" : "Unknown API used"
}
```

## Note

- Assuming sessionkey exists

