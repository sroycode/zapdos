# WikiData Create( POST _admin/api/v1/wikidata/getone )

Exter can read wikidata. This api is for single record fetch using payload

**Test Script** : `test/test_admin_wikidata_getone.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `unique_id` is needed|

## Typical Session Example

```
POST /_admin/api/v1/wikidata/getone HTTP/1.1
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
Content-Length: 278

{
   "payload" : {
      "categories" : [
         "wikidata"
      ],
      "tags" : [
         {
            "name" : "repeatag",
            "value" : "yet another"
         }
      ],
      "parent" : {},
      "city" : "Kolkata",
      "ccode" : "IND",
      "importance" : 0.29,
      "title" : "Medplus Chemists",
      "unique_id" : "A1001",
      "summary" : "Description of this shop in Jadavpur on wikipedia",
      "country" : "India"
   }
}
```

## Note

- Assuming sessionkey exists

