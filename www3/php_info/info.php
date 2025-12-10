<?php
echo "<h1>Teste de Superglobais</h1>";

echo "<h2>GET</h2>";
print_r($_GET);

echo "<h2>HEADERS</h2>";
foreach (getallheaders() as $k => $v) {
    echo "$k: $v<br>";
}

echo "<h2>SERVER</h2>";
print_r($_SERVER);
?>
