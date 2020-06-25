# TagData Create( POST _admin/api/v1/tagdata/create )

Exter with admin `is_admin` or `can_add_tags` can create tags. The latter can modify its own only.

**Test Script** : `test/test_admin_tagdata_create.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `keytype`, `name` are needed, flags `is_allowed` , `is_repeated`, `is_searchable`|
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/tagdata/create HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 869

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "keytype" : "K_USERDATA", "name" : "agegroup" , "is_allowed" : true , "is_searchable" : true },
  "payloads" : [
    { "keytype" : "K_LOCALDATA", "name" : "repeatag", "is_allowed" : true, "is_repeated": true, "is_searchable" : true },
    { "keytype" : "K_LOCALDATA", "name" : "goodtag", "is_allowed" : true, "is_searchable" : true },
    { "keytype" : "K_WIKIDATA", "name" : "repeatag", "is_allowed" : true, "is_repeated": true, "is_searchable" : true },
    { "keytype" : "K_WIKIDATA", "name" : "goodtag", "is_allowed" : true, "is_searchable" : true },
    { "keytype" : "K_USERDATA", "name" : "goodtag", "is_allowed" : true, "is_searchable" : true },
    { "keytype" : "K_EXTERDATA", "name" : "badtag" },
    { "keytype" : "K_USERDATA", "name" : "badtag" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 51

{
   "updatecount" : "8",
   "success" : true,
   "inputcount" : "8"
}
```

## Note

- Assuming sessionkey exists
- Dont use both `payload` and `payloads` in same query , the example is for berevity.

