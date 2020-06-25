# LocalData Create( POST _admin/api/v1/localdata/getmany )

Exter can read localdata. This api is for multiple record fetch using payloads

**Test Script** : `test/test_admin_localdata_getmany.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/localdata/getmany HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 233

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payloads" : [
    { "unique_id" : "A1002" },
    { "unique_id" : "A1003" },
    { "unique_id" : "A1004" },
    { "unique_id" : "A1005" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 594

{
   "payloads" : [
      {
         "categories" : [
            "osmdata"
         ],
         "parent" : {},
         "ccode" : "IND",
         "unique_id" : "A1002",
         "fld_area" : "Old Jadavpur",
         "country" : "India",
         "importance" : 0.39,
         "fld_name" : "something",
         "city" : "Kolkata"
      },
      {
         "ccode" : "IND",
         "unique_id" : "A1003",
         "fld_area" : "Old Garia",
         "categories" : [
            "osmdata"
         ],
         "parent" : {},
         "city" : "Kolkata",
         "country" : "India",
         "fld_name" : "BandBox Repairs Updated",
         "importance" : 0.09
      },
      {
         "notfound" : true,
         "unique_id" : "A1004"
      },
      {
         "city" : "Bangalore",
         "country" : "India",
         "importance" : 0.09,
         "fld_name" : "Bangalore Chemists",
         "ccode" : "IND",
         "unique_id" : "A1005",
         "fld_area" : "Adugodi",
         "categories" : [
            "osmdata"
         ],
         "parent" : {}
      }
   ]
}
```

## Note

- Assuming sessionkey exists

