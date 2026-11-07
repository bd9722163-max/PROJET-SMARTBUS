<?php
session_start();
require_once 'config.php';

$message = "";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $email = $_POST['adresse_mail'] ?? '';
    $pwd = $_POST['mot_de_passe'] ?? '';

    if (!empty($email) && !empty($pwd)) {
        $sql = "SELECT * FROM client WHERE adresse_mail = ?";
        $stmt = $pdo->prepare($sql);
        $stmt->execute([$email]);
        $client = $stmt->fetch();

        if ($client && password_verify($pwd, $client['mot_de_passe'])) {
            $_SESSION['id_client'] = $client['id_client'];
            $_SESSION['nom'] = $client['nom'];
            $_SESSION['prenom'] = $client['prenom'];
            header('Location: dashboard.php');
            exit();
        } else {
            $message = "Email ou mot de passe incorrect.";
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
    <title>Connexion - SmartBus</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="auth-container">
        <h2>Connexion</h2>
        <?php if (!empty($message)) echo "<p style='color: red; text-align: center; margin-bottom: 15px;'>$message</p>"; ?>
        
        <form action="login.php" method="POST">
            <label>Adresse Mail :</label>
            <input type="email" name="adresse_mail" required>

            <label>Mot de passe :</label>
            <input type="password" name="mot_de_passe" required>

            <button type="submit">Se connecter</button>
        </form>
        <p>Pas encore de compte ? <a href="register.php">S'inscrire</a></p>
    </div>
</body>
</html>