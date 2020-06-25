# UserData Update Profiles ( POST _user/api/v1/userdata/add_profiles )

Users ( self ) can add and change profiles by label

**Test Script** : `test/test_user_userdata_add_profiles.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  label(*) , description, avatar_image, avatar_color, tags |

## Typical Session Example

```
POST /_user/api/v1/userdata/add_profiles HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 270

{
  "sessionkey" : "gSDBzI3R1EWjXL2bl06+Y4wxPtOwI1fDAfrStFdJXZ0PepMW0XqkbiPO5i0=",
  "username" : "user001",
  "payload" : {
    "profiles" : [
      { "label" : "profone", "description" : "something here", "avatar_image" : "todo", "avatar_color" : "blue" }
    ]
  }
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

