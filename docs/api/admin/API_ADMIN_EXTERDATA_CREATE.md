# ExterData Create( POST _admin/api/v1/exterdata/create )

Exter with admin can create other exter users

**Test Script** : `test/test_admin_exterdata_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*), lang(def: EN), passkey(*), description, mobile, email, given_name, tags |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/create HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 280

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001", "passkey" : "test001p" , "given_name" : "test 001" },
  "payloads" : [
     { "name" : "test002", "passkey" : "test002p" , "given_name" : "test 002" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "success" : true,
   "updatecount" : "2",
   "inputcount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

