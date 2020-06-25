# Item Data Types

These are the data types for actual record, this file only contains the CRUD functions.

## Common To All ItemData Types 

- The addition of any Item Type requires pre-setting of its `category`
- The tags ( if any ) also need to be pre-set up as needed for the specific types, see [TagData](./TAGDATA.md).
- Exter with admin `is_admin` can alter any data, non admin with `can_add_items` can alter self-created items.

## LocalData Type

Local Data is for mainly OSM Data and like. These are standard crud

## LocalData Parameters

| Field | Type | Description |
| ----- | ---- | ----------- |
|`unique_id` | string | unique id mandatory (*)|
|`origin` | string | data origin , "osm" or "personal"|
|`categories` | array of string | categories this belongs to, this is intended for paid data use|
|`breadcrumb` | string | breadcrumb|
|`breadcrumbs` | array of string | breadcrumbs|
|`lang` | string | language code default "EN"|
|`tags` | array of object TagDataT | extra tags for favourite mapping|
|`cansee_type` | object CanSeeTypeE | marker for paid data|
|`thumbnail` | string | thumbnail location preferred size 30x45|
|`poster` | string | default poster path, size 300x450|
|`posters` | array of string | other posters path, size 300x450|
|`landscape` | string | large poster path, size 1000x550|
|`parent` | object ItemDataT | belongs to another completely|
|`importance` | number | importance or equivalent|
|`contacts` | array of object VerifiedT | contact data|
|`ccode` | string | country code Alpha 3|
|`scode` | string | state code|
|`city` | string | city name|
|`country` | string | country|
|`state` | string | state|
|`fld_name` | string | name field|
|`fld_area` | string | area field|
|`address` | string | address|
|`pincode` | string | pin code|
|`region` | string | region |
|`accuracy` | number | accuracy of lat lon in m|
|`lat` | number | latitude|
|`lon` | number | longitude|
|`landmark` | string | landmark|
|`osm_id` | integer | osm_id|
|`osm_key` | string | osm_key like place|
|`osm_value` | string | osm_value like hamlet|
|`osm_type` | string | osm_type like N|
|`attr` | array of object TagDataT | tag attributes ported from osm|
|`geometry` | string | geometry bbox|
|`rating` | number | average rating of place|

### Control APIs

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md)
- [Local Data Create](./api/admin/API_ADMIN_LOCALDATA_CREATE.md)
- [Local Data Upsert](./api/admin/API_ADMIN_LOCALDATA_UPSERT.md)
- [Local Data Update](./api/admin/API_ADMIN_LOCALDATA_UPDATE.md)
- [Local Data Update](./api/admin/API_ADMIN_LOCALDATA_MERGE.md)
- [Local Data Delete](./api/admin/API_ADMIN_LOCALDATA_DELETE.md)

### Read API

- [Local Data GetOne](./api/admin/API_ADMIN_LOCALDATA_GETONE.md)
- [Local Data GetMany](./api/admin/API_ADMIN_LOCALDATA_GETMANY.md)

### Notes

- Recommended write functions are `upsert`, `merge` and `delete` , others are for compatibiliy.

## WikiData Type

Wiki Data is for mainly Mediawiki Data and like. These are standard crud
Recommended functions are `upsert`, `merge` and `delete` , others are for compatibiliy.

## WikiData Parameters

| Field | Type | Description |
| ----- | ---- | ----------- |
|`unique_id` | string | unique id mandatory (*)|
|`origin` | string | data origin , "mediawiki" or "personal"|
|`categories` | array of string | categories this belongs to, this is intended for paid data use|
|`breadcrumb` | string | breadcrumb|
|`breadcrumbs` | array of string | breadcrumbs|
|`lang` | string | language code default "EN"|
|`description` | string | description|
|`title` | string | display name|
|`tags` | array of object TagDataT | extra tags for favourite mapping|
|`cansee_type` | object CanSeeTypeE | marker for paid data|
|`thumbnail` | string | thumbnail location preferred size 30x45|
|`poster` | string | default poster path, size 300x450|
|`posters` | array of string | other posters path, size 300x450|
|`landscape` | string | large poster path, size 1000x550|
|`parent` | object ItemDataT | belongs to another completely|
|`importance` | number | importance or equivalent|
|`contacts` | array of object VerifiedT | contact data|
|`ccode` | string | country code Alpha 3|
|`scode` | string | state code|
|`city` | string | city name|
|`country` | string | country|
|`state` | string | state|
|`region` | string | region |
|`accuracy` | number | accuracy of lat lon in m|
|`lat` | number | latitude|
|`lon` | number | longitude|
|`geometry` | string | geometry bbox|
|`rating` | number | average rating of place|

### Control APIs

- [Admin Login](./api/admin/API_ADMIN_LOGIN.md)
- [Wiki Data Create](./api/admin/API_ADMIN_WIKIDATA_CREATE.md)
- [Wiki Data Upsert](./api/admin/API_ADMIN_WIKIDATA_UPSERT.md)
- [Wiki Data Update](./api/admin/API_ADMIN_WIKIDATA_UPDATE.md)
- [Wiki Data Update](./api/admin/API_ADMIN_WIKIDATA_MERGE.md)
- [Wiki Data Delete](./api/admin/API_ADMIN_WIKIDATA_DELETE.md)

### Read API

- [Wiki Data GetOne](./api/admin/API_ADMIN_WIKIDATA_GETONE.md)
- [Wiki Data GetMany](./api/admin/API_ADMIN_WIKIDATA_GETMANY.md)

### Notes

- Recommended write functions are `upsert`, `merge` and `delete` , others are for compatibiliy.

