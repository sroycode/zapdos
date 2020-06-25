# Login Reset Passkey ( POST _user/api/v1/login/resetpass )

Users i.e. self only can change passkey for regular users

**Test Script** : `test/test_user_login_resetpass.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `newpass` | new password  |
| `oldpass` | either old password  |
| `otp` | or one time password  |

## Typical Session Example

```
POST /_admin/api/v1/resetpass HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 104

{"username": "user001", "oldpass": "this-was-the-old-password", "newpass": "this-is-the-new-password"}

HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 16

{"success":true}
```
