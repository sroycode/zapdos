# TagData Create( POST _admin/api/v1/tagdata/getone )

Exter can read tagdata. This api is for single record fetch using payload

**Test Script** : `test/test_admin_tagdata_getone.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Field `name` is needed |

## Typical Session Example

```
POST /_admin/api/v1/tagdata/getone HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 154

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "keytype" : "K_LOCALDATA", "name" : "repeatag" }
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 198

{
   "payload" : {
      "keytype" : "K_LOCALDATA",
      "manager" : "admin",
      "name" : "repeatag",
      "updated_at" : "1591964349407",
      "is_repeated" : true,
      "created_at" : "1591964349407",
      "is_searchable" : true,
      "id" : "4",
      "is_allowed" : true
   }
}
```

## Note

- Assuming sessionkey exists

