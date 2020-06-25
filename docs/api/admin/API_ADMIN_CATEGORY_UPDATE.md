# Category Update ( POST _admin/api/v1/category/update )

Exter with admin `is_admin` can update any category, non admin with `can_add_category` can update self-created category.

**Test Script** : `test/test_admin_category_update.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `name` is needed, `description` is optional |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/category/update HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 275

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "inhouse" , "description" : "inhouse production updated"},
  "payloads" : [
    { "name" : "alwaysfree", "description" : "always free host bundle updated" }
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

