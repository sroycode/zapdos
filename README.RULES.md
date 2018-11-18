# Rule Definitions

## rec_tagid

This is a string that provides unique id to the tag, and after the search the matched rule tagids can be found in `matched_queries`

## weight

Weight of this rule , multiple rules of same weight are all processed and they should have same `order_type` since results are sorted within all of them.

## rec_count

The max no of records to fetch for this sub rule

## oneword_length

Length of single word short queries type `I_ONEWORD` , mainly to speed up one/two alphabet searches

## desc

Human readable rule description


## distance_band

Distance band for

## distance_def

Default distance for points without latlon in above

## rec_styp

These are record sources , at this point unused .

Possible future restrictions:

- S_NONE : default
- S_OSM : restrict to OSM data
- S_MYLOCAL : restrict to user local data
- S_WIKI : restrict to wiki data
- S_MYTEXT : restrict to user text data

## limit_type

Filters

- L_NONE : default none
- L_CCODE : filter by country code
- L_CITY : filter by city
- L_PINCODE : filter by pincode
- L_GEOHASH3 : filter by geohash 3 chars
- L_GEOHASH5 : filter by geohash 5 chars
- L_GEOHASH7 : filter by geohash 7 chars
- L_GEOHASH9 : filter by geohash 9 chars ( exact location)
- L_NBRHASH3 : filter by city and suburb approximate
- L_NBRHASH5 : filter by locality or area approximate

## search_type

Search input types

- S_DEFAULT : default
- S_BEGINWITH : first word must match
- S_PARTIAL : all words match partial
- S_FULLWORD : all words match full
- S_NAME : search in name field  i.e. fld_name or title
- S_TAGS : search in tags field

## order_type

Search results order type

- O_DEFAULT : default text only
- O_DIST_BAND : distance bands within which it is by importance
- O_DIST_ONLY : distance only , actually a distance band with default 10m

## input_type

Search query input type

- I_DEFAULT : default text
- I_ONEWORD : special case of oneword input , does not fire otherwise.
