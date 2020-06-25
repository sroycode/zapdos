# UserData Delete ( POST _admin/api/v1/userdata/delete )

Exter with admin `is_admin` can delete any user, non admin with `can_add_users` can delete self-created users.

**Test Script** : `test/test_admin_userdata_delete.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field name(*) only |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/userdata/delete HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 176

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001" },
  "payloads" : [
     { "name" : "user002" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "updatecount" : "2",
   "success" : true,
   "inputcount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

