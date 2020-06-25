# User Login ( POST _user/api/v1/login )

Users can login with this endpoint

- if successful it resturns a sessionkey , else error

**Test Script** : `test/test_user_login.sh`

## Parameters

| Input | Description |
| ---- | ----------- |
| `username` | user name |
| `password` | user password |

## Typical Session Example

```
POST /_user/api/v1/login HTTP/1.1
Accept: application/json, */*
Content-Type: application/json
Content-Length: 48

{"username": "user001", "password": "user001p1"}

HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 98

{"username":"user001","sessionkey":"5mcn9yLQ0PQQRx6h49ZmMZOHyMSHhnSTMjo0et37Qsc+XaVusFN7oJVgKyA="}
```
## Note

While using the *test scripts*, save the sessionkey in a variable for testing other functionality,

```
export sessionkey="`TESTHEADERS=0 bash test/test_user_login.sh | jq .sessionkey | sed 's/\"//g'`"
```


