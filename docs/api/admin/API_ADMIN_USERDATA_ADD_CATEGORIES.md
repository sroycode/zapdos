# UserData Update Tags ( POST _admin/api/v1/userdata/add_categories )

Exter with admin `is_admin` can change categories for any user, non admin with `can_add_users` can change categories for self-created users.

**Test Script** : `test/test_admin_userdata_add_categories.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , categories |

## Typical Session Example

```
POST /_admin/api/v1/userdata/add_categories HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 169

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "user001", "categories" : [ "inhouse" , "paiddata" ] }
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

