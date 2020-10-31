#!/bin/sh

echo '<html>'

echo '<head> <title> CGI script </title> </head> <body>'

echo "   SERVER_NAME is: <b> $SERVER_NAME </b> <br>"
echo "   GATEWAY_INTERFACE is: <b> $GATEWAY_INTERFACE </b> <br>"
echo "   SERVER_PROTOCOL is: <b> $SERVER_PROTOCOL </b> <br>"
echo "   SERVER_SOFTWARE is: <b> $SERVER_SOFTWARE </b> <br>"

echo '</html>'