<?php
if (!isset($_GET['go'])) {
    echo '<a href="?go=1">Clique para redirecionar</a>';
    exit;
}

header("Location: /index1.html");
exit;
?>
