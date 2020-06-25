# UserData Update Tags ( POST _admin/api/v1/userdata/add_tags )

Exter with admin `is_admin` can change tags any user, non admin with `can_add_users` can change tags self-created users.

**Test Script** : `test/test_admin_userdata_add_tags.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*) , tags |

## Typical Session Example

```
POST /_admin/api/v1/userdata/add_categories HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 173

{
  "sessionkey" : "posmrqBIGn4BPnuuVcKCMF1faKmXT4UU7T1DGg==",
  "username" : "admin",
  "payload" : { "name" : "user001", "categories" : [ "shilpa-paid" , "hbo-paid" ] }
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

