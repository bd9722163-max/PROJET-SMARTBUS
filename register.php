<?php
require_once 'config.php';
$message = "";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $nom = $_POST['nom'] ?? '';
    $prenom = $_POST['prenom'] ?? '';
    $telephone = $_POST['telephone'] ?? '';
    $email = $_POST['adresse_mail'] ?? '';
    $pwd = $_POST['mot_de_passe'] ?? '';

    if (!empty($nom) && !empty($prenom) && !empty($telephone) && !empty($email) && !empty($pwd)) {
        $hashedPassword = password_hash($pwd, PASSWORD_DEFAULT);
        try {
            $sql = "INSERT INTO client (nom, prenom, telephone, adresse_mail, mot_de_passe) VALUES (?, ?, ?, ?, ?)";
            $stmt = $pdo->prepare($sql);
            $stmt->execute([$nom, $prenom, $telephone, $email, $hashedPassword]);
            $message = "Inscription réussie ! <a href='login.php'>Connectez-vous</a>";
        } catch (PDOException $e) {
            $message = "Erreur : Cet email est déjà utilisé.";
        }
    } else {
        $message = "Veuillez remplir tous les champs.";
    }
}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Inscription - SmartBus</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="auth-container">
        <h2>Créer un compte</h2>
        <?php if (!empty($message)) echo "<p style='color: " . (strpos($message, 'Erreur') !== false ? 'red' : 'green') . ";'>$message</p>"; ?>
        
        <form action="register.php" method="POST">
            <label>Nom :</label>
            <input type="text" name="nom" required>

            <label>Prénom :</label>
            <input type="text" name="prenom" required>

            <label>Téléphone :</label>
            <input type="text" name="telephone" required>

            <label>Adresse Mail :</label>
            <input type="email" name="adresse_mail" required>

            <label>Mot de passe :</label>
            <input type="password" name="mot_de_passe" required>

            <button type="submit">S'inscrire</button>
        </form>
        <p>Déjà un compte ? <a href="login.php">Se connecter</a></p>
    </div>
</body>
</html>