#!/bin/bash

mkdir -p /tmp/s_server
cd /tmp/s_server
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes
openssl s_server -tls1 -key key.pem -cert cert.pem -accept 4433
