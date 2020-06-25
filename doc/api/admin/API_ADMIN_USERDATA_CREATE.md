# UserData Create( POST _admin/api/v1/userdata/create )

Exter with admin `is_admin` or `can_add_users` can create users. The latter can modify its own only.

**Test Script** : `test/test_admin_userdata_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*), lang(def: EN), passkey(*), description, mobile, email, given_name, tags |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/userdata/create HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 282

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "passkey" : "user001p1" , "given_name" : "user 001" },
  "payloads" : [
     { "name" : "user002", "passkey" : "user002p1" , "given_name" : "user 002" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "inputcount" : "2",
   "updatecount" : "2",
   "success" : true
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

