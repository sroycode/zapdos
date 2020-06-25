# WikiData Delete ( POST _admin/api/v1/wikidata/delete )

Exter with admin `is_admin` can delete any wikidata, non admin with `can_add_items` can delete self-created wikidata.

**Test Script** : `test/test_admin_wikidata_delete.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `unique_id` is needed|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/wikidata/delete HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 186

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : 
    { "unique_id" : "M1004" },
  "payloads" : [
    { "unique_id" : "M1006" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "success" : true,
   "inputcount" : "2",
   "updatecount" : "2"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

