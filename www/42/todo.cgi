<?php

echo(getcwd());
echo("---<br><pre>");
var_export($_SERVER);
echo("</pre><br>---");

echo("---<br><pre>");
var_export($_REQUEST); 
echo("</pre><br>---");

echo("---<br><pre>");
$entityBody = file_get_contents('php://input'); // raw body
var_export($entityBody);
echo("</pre><br>---");

echo("<br><br><br><br><br><br><br><br><br>");
//phpinfo();
