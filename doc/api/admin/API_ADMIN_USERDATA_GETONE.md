# UserData Get One ( POST _admin/api/v1/userdata/getone )

Exter with rights can get regular user data

**Test Script** : `test/test_admin_userdata_getone.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) only |

## Typical Session Example

```
POST /_admin/api/v1/userdata/getone HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 126

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 235

{
   "payload" : {
      "description" : "user001 desc",
      "given_name" : "test 001 again",
      "categories" : [
         "inhouse",
         "paiddata"
      ],
      "name" : "user001",
      "tags" : [
         {
            "value" : "casual",
            "name" : "goodtag"
         }
      ],
      "primary_email" : "test@best.com",
      "primary_mobile" : "+919999999999"
   }
}
```

## Note

- Assuming sessionkey exists
