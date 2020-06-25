# ExterData Update( POST _admin/api/v1/exterdata/update )

Exter with admin can update other exter users

**Test Script** : `test/test_admin_exterdata_update.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields name(*), lang(def: EN), passkey(*), description, mobile, email, given_name, tags |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/update HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 381

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001", "passkey" : "test001p" , "given_name" : "updated 001" , "description" : "added in update" },
  "payloads" : [
     { "name" : "test003", "passkey" : "test003p" , "given_name" : "updated 003" , "tags" : [ { "name" : "goodtag" , "value" : "formal" } ] }
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

