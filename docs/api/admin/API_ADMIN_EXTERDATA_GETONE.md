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
POST /_admin/api/v1/exterdata/getone HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 126

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "name" : "test001" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 1127

{
   "payload" : {
      "primary_email" : "test@best.com",
      "primary_mobile" : "+919999999999",
      "description" : "test001 desc",
      "name" : "test001",
      "tags" : [
         {
            "value" : "casual",
            "name" : "goodtag"
         }
      ],
      "given_name" : "test 001 again",
      "profiles" : [
         {
            "query_type" : "QRY_COMPLETION_PHOTON",
            "rules" : [
               {
                  "max_records" : "10",
                  "input_type" : "I_ONEWORD",
                  "desc" : "one word query for upto 2 chars",
                  "search_type" : "S_BEGINWITH",
                  "oneword_length" : "2",
                  "weight" : "100"
               },
               {
                  "order_type" : "O_DIST_BAND",
                  "max_records" : "10",
                  "desc" : "by distance 1km bands",
                  "distance_band" : "1000",
                  "weight" : "90"
               },
               {
                  "weight" : "80",
                  "desc" : "if not local",
                  "max_records" : "10"
               }
            ]
         },
         {
            "rules" : [
               {
                  "max_records" : "5",
                  "order_type" : "O_DIST_BAND",
                  "limit_type" : "L_NBRHASH5",
                  "weight" : "100",
                  "distance_band" : "10",
                  "desc" : "by distance only so band is small"
               }
            ],
            "query_type" : "QRY_COMPLETION_NOTOPH"
         },
         {
            "rules" : [
               {
                  "max_records" : "10",
                  "input_type" : "I_ONEWORD",
                  "oneword_length" : "2",
                  "weight" : "100",
                  "desc" : "one word query for upto 2 chars",
                  "search_type" : "S_BEGINWITH"
               },
               {
                  "weight" : "90",
                  "search_type" : "S_NAME",
                  "desc" : "find in name",
                  "max_records" : "10"
               },
               {
                  "max_records" : "10",
                  "desc" : "if not in name",
                  "weight" : "80"
               }
            ],
            "query_type" : "QRY_COMPLETION_WIKIDATA"
         }
      ]
   }
}
```

## Note

- Assuming sessionkey exists

