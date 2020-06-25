# UserData Update Description ( POST _user/api/v1/userdata/update_description )

Users i.e. self only can change description for regular users

**Test Script** : `test/test_user_userdata_update_description.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields , name(*) , description |

## Typical Session Example

```
POST /_user/api/v1/userdata/update_description HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 166

{
  "sessionkey" : "gSDBzI3R1EWjXL2bl06+Y4wxPtOwI1fDAfrStFdJXZ0PepMW0XqkbiPO5i0=",
  "username" : "user001",
  "payload" : { "description" : "updated by user001" }
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

