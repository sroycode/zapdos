# UserData Update( POST _admin/api/v1/userdata/update )

Exter with admin `is_admin` can update any user, non admin with `can_add_users` can update self-created users.

**Test Script** : `test/test_admin_userdata_update.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields name(*), lang(def: EN), passkey(*), description, mobile, email, given_name, tags |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/userdata/update HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 383

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "passkey" : "user001p1" , "given_name" : "updated 001" , "description" : "added in update" },
  "payloads" : [
     { "name" : "user003", "passkey" : "user003p1" , "given_name" : "updated 003" , "tags" : [ { "name" : "goodtag" , "value" : "formal" } ] }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "success" : true,
   "inputcount" : "2",
   "updatecount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

