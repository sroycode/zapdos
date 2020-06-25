# UserData Get One ( POST _user/api/v1/userdata/getone )

Users can get regular data about self

**Test Script** : `test/test_user_userdata_getone.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) only |

## Typical Session Example

```
POST /_user/api/v1/userdata/getone HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 148

{
  "sessionkey" : "gSDBzI3R1EWjXL2bl06+Y4wxPtOwI1fDAfrStFdJXZ0PepMW0XqkbiPO5i0=",
  "username" : "user001",
  "payload" : { "name" : "user001" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 310

{
   "payload" : {
      "given_name" : "test 001 by self",
      "primary_mobile" : "+919999999999",
      "categories" : [
         "shilpa-paid",
         "hbo-paid"
      ],
      "name" : "user001",
      "description" : "updated by user001",
      "profiles" : [
         {
            "avatar_color" : "blue",
            "label" : "profone",
            "description" : "something here",
            "avatar_image" : "todo"
         }
      ],
      "primary_email" : "test@best.com"
   }
}
```

## Note

- Assuming sessionkey exists
