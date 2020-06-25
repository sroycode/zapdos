# TagData Create( POST _admin/api/v1/tagdata/getmany )

Exter can read tagdata. This api is for multiple record fetch using payloads

**Test Script** : `test/test_admin_tagdata_getmany.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/tagdata/getmany HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 273

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payloads" : [
    { "keytype" : "K_LOCALDATA", "name" : "goodtag" },
    { "keytype" : "K_LOCALDATA", "name" : "repeatag" },
    { "keytype" : "K_USERDATA", "name" : "badtag" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 513

{
   "payloads" : [
      {
         "is_allowed" : true,
         "manager" : "admin",
         "id" : "5",
         "is_searchable" : true,
         "updated_at" : "1591964349407",
         "name" : "goodtag",
         "created_at" : "1591964349407",
         "keytype" : "K_LOCALDATA"
      },
      {
         "is_repeated" : true,
         "id" : "4",
         "is_allowed" : true,
         "manager" : "admin",
         "keytype" : "K_LOCALDATA",
         "is_searchable" : true,
         "updated_at" : "1591964349407",
         "name" : "repeatag",
         "created_at" : "1591964349407"
      },
      {
         "keytype" : "K_USERDATA",
         "is_deleted" : true,
         "id" : "10",
         "created_at" : "1591964349407",
         "name" : "badtag",
         "updated_at" : "1591964350658",
         "manager" : "admin"
      }
   ]
}
```

## Note

- Assuming sessionkey exists

