#!/bin/bash

MY_IP=`awk 'END{print $1}' /etc/hosts`
zpds_server -config /usr/local/etc/zapdos.conf -hrpc_thisurl "http://${MY_IP}:9094"
