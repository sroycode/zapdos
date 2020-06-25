# Project Motivation

Zapdos was originally written as a drop-in replacement for the project `https://github.com/komoot/photon` with a few modifications.

The primary use case is of people who require search/typeahead support over a reasonably large dataset with infrequent updates.

It was also designed to allow multiple search `profiles` ie multiple situations which require different business rules
with the same dataset.
An interesting example was typeahead for a local search user depending on user location type.
For example , for large cities you may want to show the nearby data before others, whereas for a small
village user you may want to use a larger radius for nearby or decide not to sort by nearness at all.
In the current implementation, this may be achieved by creating exter users called `mumbai`, `bangalore` , `somedistrict` etc
and querying that `profile` if the user lat-lon falls in that, with a catch-all `default`.

This project was later extended to provide similar typeahead for a wikipedia data.
