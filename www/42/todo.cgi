<?php


echo(getcwd()) // missing semicolon to trigger runtime error, should result in 500 status code
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
