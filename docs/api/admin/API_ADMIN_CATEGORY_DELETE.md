# Category Delete ( POST _admin/api/v1/category/delete )

Exter with admin `is_admin` can delete any category, non admin with `can_add_category` can delete self-created category.

**Test Script** : `test/test_admin_category_delete.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `name` is needed, `description` is optional |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/category/delete HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 177

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "mistake1" },
  "payloads" : [
    { "name" : "mistake2" }
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


