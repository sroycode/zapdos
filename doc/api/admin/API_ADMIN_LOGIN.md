# Admin Login ( POST _admin/api/v1/login )

Admin ( Exter ) users can login with this endpoint

- if successful it resturns a sessionkey , else error

**Test Script** : `test/test_admin_login.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `password` | user password |

## Typical Session Example

```
POST /_admin/api/v1/login HTTP/1.1
Accept: application/json, */*
Content-Type: application/json

{"name": "admin", "password": "this-is-example-only-bysrc"}

HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 72

{"name":"admin","sessionkey":"Bx2cjF3b4UKS4vsqpRuAEokywuoD0859Qr5YMg=="}
```
## Note

While using the *test scripts*, save the sessionkey in a variable for testing other functionality,

```
export sessionkey="`TESTHEADERS=0 bash test/test_admin_login.sh | jq .sessionkey | sed 's/\"//g'`"
```


