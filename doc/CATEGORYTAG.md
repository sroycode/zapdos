# Setup Data Types

These are the data types that are needed for setup and control. These are not needed for the default osm and wiki data dumps.

## TagData Type

Tags are used to describe various data types. Both user and item types can have tags.
Tags need to be pre-configured to be considered in data.
Exter with admin `is_admin` can alter any tags, non admin with `can_add_tags` can alter self-created tags.

For a tag, typically, 

- `is_allowed` is to allow the tag to be updated
- `is_repeated` is for tags that can occur multiple times , default is unique.
- `is_searchable` is a tag that is allowed in search

### Control APIs

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md)
- [TagData Create](./api/admin/API_ADMIN_TAGDATA_CREATE.md)
- [TagData Upsert](./api/admin/API_ADMIN_TAGDATA_UPSERT.md)
- [TagData Update](./api/admin/API_ADMIN_TAGDATA_UPDATE.md)
- [TagData Delete](./api/admin/API_ADMIN_TAGDATA_DELETE.md)

### Read API

- [TagData GetOne](./api/admin/API_ADMIN_TAGDATA_GETONE.md)
- [TagData GetMany](./api/admin/API_ADMIN_TAGDATA_GETMANY.md)

### Notes

NA

## Category Type

Category is the basis of subscriptions and is required to be pre-configures like hosts.
Exter with admin `is_admin` can alter any category, non admin with `can_add_category` can alter self-created category.

### Control APIs

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md)
- [Category Create](./api/admin/API_ADMIN_CATEGORY_CREATE.md)
- [Category Upsert](./api/admin/API_ADMIN_CATEGORY_UPSERT.md)
- [Category Update](./api/admin/API_ADMIN_CATEGORY_UPDATE.md)
- [Category Delete](./api/admin/API_ADMIN_CATEGORY_DELETE.md)

### Read API

- [Category GetOne](./api/admin/API_ADMIN_CATEGORY_GETONE.md)
- [Category GetMany](./api/admin/API_ADMIN_CATEGORY_GETMANY.md)

### Notes

NA
