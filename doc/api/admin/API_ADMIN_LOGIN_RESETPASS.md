# Admin Login ( POST _admin/api/v1/resetpass )

Admin ( Exter ) users can login with this endpoint

- if successful it resturns a sessionkey , else error

**Test Script** : `test/test_admin_login_resetpass.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `newpass` | new user password |
| `oldpass` | either old user password |
| `otp` | or one time password |

## Typical Session Example

```
POST /_admin/api/v1/resetpass HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 99

{"username": "admin", "oldpass": "this-is-the-old-password", "newpass": "this-is-the-new-password"}

HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 16

{"success":true}
```

## Note

- OTP not implemented directly. 
