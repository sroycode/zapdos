# LocalData Create( POST _admin/api/v1/localdata/getone )

Exter can read localdata. This api is for single record fetch using payload

**Test Script** : `test/test_admin_localdata_getone.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed|

## Typical Session Example

```
POST /_admin/api/v1/localdata/getone HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 129

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "unique_id" : "A1001" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 240

{
   "payload" : {
      "importance" : 0.29,
      "tags" : [
         {
            "value" : "yet another",
            "name" : "repeatag"
         }
      ],
      "categories" : [
         "osmdata"
      ],
      "country" : "India",
      "fld_name" : "Medplus Chemists",
      "fld_area" : "Jadavpur",
      "ccode" : "IND",
      "unique_id" : "A1001",
      "city" : "Kolkata",
      "parent" : {}
   }
}
```

## Note

- Assuming sessionkey exists

