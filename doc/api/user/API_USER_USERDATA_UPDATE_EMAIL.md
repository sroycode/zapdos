# UserData Update Email ( POST _user/api/v1/userdata/update_email )

Users i.e. self only can change email for regular users , *currently disabled*

**Test Script** : `test/test_user_userdata_update_email.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , email

## Typical Session Example

```
POST /_user/api/v1/userdata/update_email HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 164

{
  "sessionkey" : "gSDBzI3R1EWjXL2bl06+Y4wxPtOwI1fDAfrStFdJXZ0PepMW0XqkbiPO5i0=",
  "username" : "user001",
  "payload" : { "primary_email": "test@best11.com" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 42

{
   "error" : "Unknown API used",
   "errorcode" : 0
}
```

## Note

- Assuming sessionkey exists

