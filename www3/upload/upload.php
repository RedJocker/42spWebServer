<?php
if (!isset($_FILES['arquivo'])) {
    echo "Nenhum arquivo enviado";
    exit;
}

$file = $_FILES['arquivo'];
$dest = "uploads/" . basename($file['name']);

if (!move_uploaded_file($file['tmp_name'], $dest)) {
    echo "Falha ao mover arquivo";
    exit;
}

echo "Arquivo recebido: " . htmlspecialchars($file['name']);
?>
