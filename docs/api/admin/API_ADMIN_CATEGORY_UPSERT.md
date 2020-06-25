# Category Upsert ( POST _admin/api/v1/category/upsert )

Exter with admin `is_admin` can upsert any category, non admin with `can_add_category` can alter self-created category.

**Test Script** : `test/test_admin_category_upsert.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `name` are needed, `description` is optional |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/category/upsert HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 330

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "someother" , "description" : "example of upsert"},
  "payloads" : [
    { "name" : "inhouse" , "description" : "inhouse production upsert" },
    { "name" : "mistake2", "description" : "to demonstrate delete" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "inputcount" : "3",
   "updatecount" : "3",
   "success" : true
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

