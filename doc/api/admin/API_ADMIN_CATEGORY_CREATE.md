# category Create( POST _admin/api/v1/category/create )

Exter with admin `is_admin` or `can_add_category` can create category. The latter can modify its own only.

**Test Script** : `test/test_admin_category_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `name` is needed, `description` is optional |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/category/create HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 579

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "inhouse" , "description" : "inhouse production" },
  "payloads" : [
    { "name" : "osmdata", "description" : "default osm data" },
    { "name" : "wikidata", "description" : "default wiki data" },
    { "name" : "alwaysfree", "description" : "always free host bundle" },
    { "name" : "paiddata", "description" : "example of paid data" , "cansee_type" : "C_PAID" , "cansee_price" : 100 },
    { "name" : "mistake1", "description" : "to demonstrate delete" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "inputcount" : "6",
   "updatecount" : "6",
   "success" : true
}
```
## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

