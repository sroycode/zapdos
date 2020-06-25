# Category Create( POST _admin/api/v1/category/getmany )

Exter can read category. This api is for multiple record fetch using payloads

**Test Script** : `test/test_admin_category_getmany.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/category/getmany HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 569

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payloads" : [
    { "name" : "inhouse" , "description" : "inhouse production" },
    { "name" : "osmdata", "description" : "default osm data" },
    { "name" : "wikidata", "description" : "default wiki data" },
    { "name" : "alwaysfree", "description" : "always free host bundle" },
    { "name" : "paiddata", "description" : "example of paid data" , "cansee_type" : "C_PAID" , "cansee_price" : 100 },
    { "name" : "mistake1", "description" : "to demonstrate delete" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 943

{
   "payloads" : [
      {
         "updated_at" : "1591964352715",
         "name" : "inhouse",
         "manager" : "admin",
         "created_at" : "1591964351890",
         "id" : "19",
         "description" : "inhouse production updated"
      },
      {
         "name" : "osmdata",
         "updated_at" : "1591964351890",
         "description" : "default osm data",
         "created_at" : "1591964351890",
         "id" : "14",
         "manager" : "admin"
      },
      {
         "manager" : "admin",
         "created_at" : "1591964351890",
         "id" : "15",
         "description" : "default wiki data",
         "updated_at" : "1591964351890",
         "name" : "wikidata"
      },
      {
         "updated_at" : "1591964352715",
         "name" : "alwaysfree",
         "created_at" : "1591964351890",
         "id" : "16",
         "manager" : "admin",
         "description" : "always free host bundle updated"
      },
      {
         "name" : "paiddata",
         "cansee_price" : 100,
         "description" : "example of paid data",
         "manager" : "admin",
         "created_at" : "1591964351890",
         "cansee_type" : "C_PAID",
         "updated_at" : "1591964351890",
         "id" : "17"
      },
      {
         "manager" : "admin",
         "created_at" : "1591964351890",
         "is_deleted" : true,
         "id" : "18",
         "description" : "to demonstrate delete",
         "updated_at" : "1591964353117",
         "name" : "mistake1"
      }
   ]
}
```
## Note

- Assuming sessionkey exists

