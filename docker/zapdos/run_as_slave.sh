#!/bin/bash

MY_IP=`awk 'END{print $1}' /etc/hosts`
zpds_server -config ${ZAPDOS_SOURCE}/zapdos.conf -hrpc_thisurl "http://${MY_IP}:9092" $1
