#!/usr/bin/php

<!DOCTYPE html>

<html><head></head><body>

<?php
var_dump($argv);
echo "<br/>";
while (list($var,$value) = each ($_ENV)) {
    echo "$var => $value <br />";
}
?>

</body></html>