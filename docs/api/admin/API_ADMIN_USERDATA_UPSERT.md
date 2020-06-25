# UserData Upsert( POST _admin/api/v1/userdata/upsert )

Exter with admin `is_admin` can upsert any user, non admin with `can_add_users` can update self-created users.

**Test Script** : `test/test_admin_userdata_upsert.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*), lang(def: EN), passkey(*), description, mobile, email, given_name, tags |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/userdata/upsert HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 374

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "passkey" : "user001p1" , "given_name" : "atest 001" , "description" : "added later" },
  "payloads" : [
     { "name" : "user003", "passkey" : "user003p1" , "given_name" : "test 003" , "tags" : [ { "name" : "goodtag" , "value" : "formal" } ] }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "inputcount" : "2",
   "success" : true,
   "updatecount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

