# UserData Update Given Name ( POST _user/api/v1/userdata/update_given_name )

Users i.e. self only can change given name for regular users, *currently disabled*

**Test Script** : `test/test_user_userdata_update_given_name.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , given_name |

## Typical Session Example

```
POST /_user/api/v1/userdata/update_given_name HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 162

{
  "sessionkey" : "gSDBzI3R1EWjXL2bl06+Y4wxPtOwI1fDAfrStFdJXZ0PepMW0XqkbiPO5i0=",
  "username" : "user001",
  "payload" : { "given_name": "test 001 by self" }
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

