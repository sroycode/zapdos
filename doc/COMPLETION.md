# Completion Queries

This is intended to be a drop-in replacement for the project `https://github.com/komoot/photon`.
The endpoint `_query/api/v1/photon/{profile}` is the currently available photon-compatible endpoint. 

The reverse geocoder is with identical syntax at endpoint `_query/api/v1/notoph/{profile}` .
Each Exter user can potentially have search profiles if permitted, with different set of rules for the same endpoint. 
There is a `default` user profile for all searches which uses a pre-set .

There is now support for a separate search store for text data. Basic idea is to have local data queries which will mostly be
location based in one index and wiki data which will mostly be text based in another. This is with identical syntax as above photon
at endpoint `_query/api/v1/textdata/{profile}` though result format is different.

### API

- [Completion Type Photon](./api/query/API_QUERY_COMPLETION_PHOTON.md)
- [Completion Type Notoph aka reverse geocoder](./api/query/API_QUERY_COMPLETION_NOTOPH.md)
- [Completion Type TextData](./api/query/API_QUERY_COMPLETION_TEXTDATA.md)

### Notes
