# UserData Types ( User and Exter )

1. There are two types of users in the system 
- User is a end user having access to data, based on subscriptions.
- Exter to perform admin tasks like creating users and managing their access and permissions.
- Exter can also have query profiles other than system default.

## User Data Type

This is the general user, who has access to a set of items.
Exter with admin `is_admin` can alter any user, non admin with `can_add_users` can alter self-created users.
The expected workflow is exter with `can_add_users` will create users and assign their host visibility.

### Privileges

Subscription Types

### Control APIs

For admin

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md)
- [UserData Create](./api/admin/API_ADMIN_USERDATA_CREATE.md)
- [UserData Upsert](./api/admin/API_ADMIN_USERDATA_UPSERT.md)
- [UserData Update](./api/admin/API_ADMIN_USERDATA_UPDATE.md)
- [UserData Delete](./api/admin/API_ADMIN_USERDATA_DELETE.md)
- [UserData Update Description](./api/admin/API_ADMIN_USERDATA_UPDATE_DESCRIPTION.md)
- [UserData Update Given Name](./api/admin/API_ADMIN_USERDATA_UPDATE_GIVEN_NAME.md)
- [UserData Update Email](./api/admin/API_ADMIN_USERDATA_UPDATE_EMAIL.md)
- [UserData Update Mobile](./api/admin/API_ADMIN_USERDATA_UPDATE_MOBILE.md)
- [UserData Update Passkey](./api/admin/API_ADMIN_USERDATA_UPDATE_PASSKEY.md)
- [UserData Add Tags](./api/admin/API_ADMIN_USERDATA_ADD_TAGS.md)
- [UserData Add Categories](./api/admin/API_ADMIN_USERDATA_ADD_CATEGORIES.md)
- [UserData Read Many](./api/admin/API_ADMIN_USERDATA_GETMANY.md)
- [UserData Read One](./api/admin/API_ADMIN_USERDATA_GETONE.md)

For User

- [User Login](./api/user/API_USER_LOGIN.md)
- [Reset Password](./api/user/API_USER_LOGIN_RESETPASS.md)
- [UserData Get One](./api/user/API_USER_USERDATA_GETONE.md)
- [UserData Update Description](./api/user/API_USER_USERDATA_UPDATE_DESCRIPTION.md)
- [UserData Update Email](./api/user/API_USER_USERDATA_UPDATE_EMAIL.md) *currently disabled*
- [UserData Update Given Name](./api/user/API_USER_USERDATA_UPDATE_GIVEN_NAME.md) *currently disabled*
- [UserData Update Given Mobile](./api/user/API_USER_USERDATA_UPDATE_MOBILE.md) *currently disabled*

### Note

NA

## Exter Data Type

This is mainly a special user type to act as admin or updater.
These can be admin or external users, having privileges to create and modify users and content
Exter users with `admin` privileges can do everything , else the privileges are defined by flags as below.

### Privileges

The following privileges exist.

| Privilege | Description |
| ---- | ----------- |
| `admin` | `admin` is the highest type which can literally do anything including create other exters and set rights. |
| `can_add_tags` | can create tags and modify the ones it created |
| `can_add_category` | can create category and modify the ones it created |
| `can_add_users` | can create users and modify the ones it created |
| `can_add_items` | can create item data and modify the ones it created |
| `can_add_profiles` | can expose owned profiles for search |

### Control APIs

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md)
- [Reset Password](./api/admin/API_ADMIN_LOGIN_RESETPASS.md)
- [ExterData Create](./api/admin/API_ADMIN_EXTERDATA_CREATE.md)
- [ExterData Upsert](./api/admin/API_ADMIN_EXTERDATA_UPSERT.md)
- [ExterData Update](./api/admin/API_ADMIN_EXTERDATA_UPDATE.md)
- [ExterData Delete](./api/admin/API_ADMIN_EXTERDATA_DELETE.md)
- [ExterData Update Passkey](./api/admin/API_ADMIN_EXTERDATA_UPDATE_PASSKEY.md)
- [ExterData Update Given Name](./api/admin/API_ADMIN_EXTERDATA_UPDATE_GIVEN_NAME.md)
- [ExterData Update Description](./api/admin/API_ADMIN_EXTERDATA_UPDATE_DESCRIPTION.md)
- [ExterData Update Email](./api/admin/API_ADMIN_EXTERDATA_UPDATE_EMAIL.md)
- [ExterData Update Mobile](./api/admin/API_ADMIN_EXTERDATA_UPDATE_MOBILE.md)
- [ExterData Add Tags](./api/admin/API_ADMIN_EXTERDATA_ADD_TAGS.md)

- [ExterData Get One](./api/admin/API_ADMIN_EXTERDATA_GETONE.md)
- [ExterData Get Many](./api/admin/API_ADMIN_EXTERDATA_GETMANY.md)

### Note

Since admin can add other admin it is advised not to share it beyond site administrators.

