# ExterData Update QueryProf profiles ( POST _admin/api/v1/exterdata/add_profiles )

Exter with admin `is_admin` can update profiles for exter, exter can update own profiles.

**Test Script** : `test/test_admin_exterdata_add_profiles.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  , profiles |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/add_profiles HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 2139

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001",
    "profiles" : [
    {
      "query_type" : "QRY_COMPLETION_PHOTON",
      "rules" : [
        {
          "weight" : 100, "max_records" : 10, "oneword_length" : 2,
          "desc" : "one word query for upto 2 chars",
          "limit_type" : "L_NONE", "search_type" : "S_BEGINWITH", "order_type" : "O_DEFAULT", "input_type" : "I_ONEWORD"
        },
        {
          "weight" : 90, "max_records" : 10,
          "desc" : "by distance 1km bands",
          "distance_band" : 1000,
          "limit_type" : "L_NONE", "search_type" : "S_DEFAULT", "order_type" : "O_DIST_BAND", "input_type" : "I_DEFAULT"
        },
        {
          "weight" : 80, "max_records" : 10,
          "desc" : "if not local",
          "limit_type" : "L_NONE", "search_type" : "S_DEFAULT", "order_type" : "O_DEFAULT", "input_type" : "I_DEFAULT"
        }
      ]
    },
    {
      "query_type" : "QRY_COMPLETION_NOTOPH",
      "rules" : [
        {
          "weight" : 100, "max_records" : 5,
          "desc" : "by distance only so band is small",
          "distance_band" : 10,
          "limit_type" : "L_NBRHASH5", "search_type" : "S_DEFAULT", "order_type" : "O_DIST_BAND", "input_type" : "I_DEFAULT"
        }
      ]
    },
    {
      "query_type" : "QRY_COMPLETION_WIKIDATA",
      "rules" : [
        {
          "weight" : 100, "max_records" : 10, "oneword_length" : 2,
          "desc" : "one word query for upto 2 chars",
          "limit_type" : "L_NONE", "search_type" : "S_BEGINWITH", "order_type" : "O_DEFAULT", "input_type" : "I_ONEWORD"
        },
        {
          "weight" : 90, "max_records" : 10,
          "desc" : "find in name",
          "limit_type" : "L_NONE", "search_type" : "S_NAME", "order_type" : "O_DEFAULT", "input_type" : "I_DEFAULT"
        },
        {
          "weight" : 80, "max_records" : 10,
          "desc" : "if not in name",
          "limit_type" : "L_NONE", "search_type" : "S_DEFAULT", "order_type" : "O_DEFAULT", "input_type" : "I_DEFAULT"
        }
      ]
    }
    ]
  } 
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

