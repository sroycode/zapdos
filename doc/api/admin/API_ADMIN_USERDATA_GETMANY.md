# UserData Get Many ( POST _admin/api/v1/userdata/getmany )

Exter with rights can get regular users in bulk

**Test Script** : `test/test_admin_userdata_getmany.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `sessionkey` | user session key |
| `payloads` | Array of Object. Fields  name(*) only |

## Typical Session Example

```
POST /_admin/api/v1/userdata/getmany HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 167

{
  "sessionkey" : "6Hqia6axlaLQZ6gNEIn8L5babDPANTfPsfu31Q==",
  "username" : "admin",
  "payloads" : [
     { "name" : "user001" },
     { "name" : "user002" }
  ]
}


HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 281

{
   "payloads" : [
      {
         "categories" : [
            "inhouse",
            "paiddata"
         ],
         "primary_email" : "test@best.com",
         "tags" : [
            {
               "name" : "goodtag",
               "value" : "casual"
            }
         ],
         "given_name" : "test 001 again",
         "description" : "user001 desc",
         "primary_mobile" : "+919999999999",
         "name" : "user001"
      },
      {
         "name" : "user002",
         "given_name" : "user 002"
      }
   ]
}
```

## Note

- Assuming sessionkey exists
