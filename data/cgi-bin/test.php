#!/usr/bin/php

<!DOCTYPE html>

<html><head></head><body>

<?php
echo " <title> CGI script </title> ";
echo "   SERVER_NAME is: "; echo $_SERVER['SERVER_NAME'];
echo "  <br> GATEWAY_INTERFACE is: "; echo $_SERVER['GATEWAY_INTERFACE'];
echo "  <br> SERVER_PROTOCOL is: "; echo $_SERVER['SERVER_PROTOCOL'];
echo "  <br> SERVER_SOFTWARE is: "; echo $_SERVER['SERVER_SOFTWARE'];
?>

</body></html>