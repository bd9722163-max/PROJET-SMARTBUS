<?php
$host = '127.0.0.1';
$port = '3307'; // port MySQL  (ex: 3306)
$dbname = 'mon_site_db'; // Remplace par le nom exact de ta base de données si besoin
$username = 'root';
$password = '';

try {
    $pdo = new PDO("mysql:host=$host;port=$port;dbname=$dbname;charset=utf8", $username, $password);
    // Active le mode d'affichage des erreurs pour le débogage
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Erreur de connexion : " . $e->getMessage());
}
?>