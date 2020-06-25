# TagData Delete ( POST _admin/api/v1/tagdata/delete )

Exter with admin `is_admin` can delete any tags, non admin with `can_add_tags` can delete self-created tags.

**Test Script** : `test/test_admin_tagdata_delete.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payload` | Object. Fields `keytype`, `name` are needed |
| `payloads` | array of `payload` |

## Typical Session Example

```
POST /_admin/api/v1/tagdata/delete HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 227

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payload" : { "keytype" : "K_EXTERDATA", "name" : "badtag" },
  "payloads" : [
    { "keytype" : "K_USERDATA", "name" : "badtag"  }
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

