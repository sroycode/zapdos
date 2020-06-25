# Category Create( POST _admin/api/v1/category/getone )

Exter can read category. This api is for single record fetch using payload

**Test Script** : `test/test_admin_category_getone.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `name` is needed |

## Typical Session Example

```
POST /_admin/api/v1/category/getone HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 127

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "paiddata" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 196

{
   "payload" : {
      "created_at" : "1591964351890",
      "manager" : "admin",
      "name" : "paiddata",
      "cansee_type" : "C_PAID",
      "description" : "example of paid data",
      "id" : "17",
      "cansee_price" : 100,
      "updated_at" : "1591964351890"
   }
}
```
## Note

- Assuming sessionkey exists

