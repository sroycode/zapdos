# ExterData Create( POST _admin/api/v1/exterdata/create )

Exter with admin can create other exter users

**Test Script** : `test/test_admin_exterdata_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields  name(*), lang(def: EN), passkey(*), description, mobile, email, given_name, tags |
| `payloads` | array of `payload` for convenience |

## Typical Session Example

```
POST /_admin/api/v1/exterdata/getmany HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 167

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payloads" : [
     { "name" : "test001" },
     { "name" : "test002" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 1173

{
   "payloads" : [
      {
         "tags" : [
            {
               "name" : "goodtag",
               "value" : "casual"
            }
         ],
         "description" : "test001 desc",
         "primary_email" : "test@best.com",
         "given_name" : "test 001 again",
         "primary_mobile" : "+919999999999",
         "name" : "test001",
         "profiles" : [
            {
               "query_type" : "QRY_COMPLETION_PHOTON",
               "rules" : [
                  {
                     "desc" : "one word query for upto 2 chars",
                     "oneword_length" : "2",
                     "max_records" : "10",
                     "input_type" : "I_ONEWORD",
                     "search_type" : "S_BEGINWITH",
                     "weight" : "100"
                  },
                  {
                     "distance_band" : "1000",
                     "desc" : "by distance 1km bands",
                     "weight" : "90",
                     "order_type" : "O_DIST_BAND",
                     "max_records" : "10"
                  },
                  {
                     "desc" : "if not local",
                     "max_records" : "10",
                     "weight" : "80"
                  }
               ]
            },
            {
               "rules" : [
                  {
                     "distance_band" : "10",
                     "desc" : "by distance only so band is small",
                     "weight" : "100",
                     "order_type" : "O_DIST_BAND",
                     "max_records" : "5",
                     "limit_type" : "L_NBRHASH5"
                  }
               ],
               "query_type" : "QRY_COMPLETION_NOTOPH"
            },
            {
               "query_type" : "QRY_COMPLETION_WIKIDATA",
               "rules" : [
                  {
                     "desc" : "one word query for upto 2 chars",
                     "oneword_length" : "2",
                     "input_type" : "I_ONEWORD",
                     "max_records" : "10",
                     "search_type" : "S_BEGINWITH",
                     "weight" : "100"
                  },
                  {
                     "desc" : "find in name",
                     "weight" : "90",
                     "search_type" : "S_NAME",
                     "max_records" : "10"
                  },
                  {
                     "max_records" : "10",
                     "weight" : "80",
                     "desc" : "if not in name"
                  }
               ]
            }
         ]
      },
      {
         "given_name" : "test 002",
         "name" : "test002"
      }
   ]
}
```

## Note

- Assuming sessionkey exists

