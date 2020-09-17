#!/usr/bin/php

<!DOCTYPE html>

<html><head></head><body>

<?php
while (list($var,$value) = each ($_ENV)) {
    echo "$var => $value <br />";
}
?>

</body></html>