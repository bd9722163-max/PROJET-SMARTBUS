<?php
session_start();
require_once 'config.php';

// Sécurité : si l'utilisateur n'est pas connecté
if (!isset($_SESSION['id_client'])) {
    header('Location: login.php');
    exit();
}

$section = $_GET['section'] ?? 'info';
$msg_carte = "";

// Traitement de l'enregistrement de la carte RFID dans l'onglet Inscription
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['enregistrer_carte'])) {
    $uid = $_POST['uid'] ?? '';
    $statut = $_POST['statut'] ?? 'Actif';
    $date_attribution = date('Y-m-d');
    $id_client = $_SESSION['id_client'];

    if (!empty($uid)) {
        try {
            $sql = "INSERT INTO carte_rfid (uid, statut, date_attribution, id_client) VALUES (?, ?, ?, ?)";
            $stmt = $pdo->prepare($sql);
            $stmt->execute([$uid, $statut, $date_attribution, $id_client]);
            $msg_carte = "Carte RFID enregistrée avec succès !";
        } catch (PDOException $e) {
            $msg_carte = "Erreur : Cet UID de carte existe déjà.";
        }
    } else {
        $msg_carte = "Veuillez entrer l'UID de la carte.";
    }
}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Tableau de bord - SmartBus</title>
    <link rel="stylesheet" href="style.css">
    <style>
        /* Quelques ajustements spécifiques pour le contenu du dashboard */
        .dash-card form input, .dash-card form select {
            width: 100%;
            padding: 10px;
            margin: 10px 0 15px 0;
            border: 1px solid #ced4da;
            border-radius: 6px;
        }
        .dash-card form button {
            padding: 10px 20px;
            background-color: #0056b3;
            color: white;
            border: none;
            border-radius: 6px;
            cursor: pointer;
            font-weight: 600;
        }
        .dash-card form button:hover { background-color: #004085; }
    </style>
</head>
<body>

    <header class="dash-header">
        <h1>Smart TICKET  - Espace Client</h1>
        <div>
            <span>Bonjour, <strong><?php echo htmlspecialchars(strtoupper($_SESSION['prenom'] . ' ' . $_SESSION['nom'])); ?></strong> !</span>
        </div>
    </header>

    <nav class="dash-nav">
        <a href="dashboard.php?section=info" class="<?php if($section=='info') echo 'active'; ?>">Information générale</a>
        <a href="dashboard.php?section=profil" class="<?php if($section=='profil') echo 'active'; ?>">Profil</a>
        <a href="dashboard.php?section=abonnement" class="<?php if($section=='abonnement') echo 'active'; ?>">Abonnement</a>
        <a href="dashboard.php?section=inscription" class="<?php if($section=='inscription') echo 'active'; ?>">Inscription</a>
        <a href="logout.php" style="color: #ff6b6b; margin-left: auto;">Déconnexion</a>
    </nav>

    <div class="dash-container">
        <div class="dash-card">
            <?php
            switch ($section) {
                case 'profil':
                    echo "<h2>Mon Profil</h2><br>";
                    echo "<p><strong>Nom :</strong> " . htmlspecialchars($_SESSION['nom']) . "</p><br>";
                    echo "<p><strong>Prénom :</strong> " . htmlspecialchars($_SESSION['prenom']) . "</p>";
                    break;

                case 'abonnement':
                    echo "<h2>Mes Abonnements</h2><br>";
                    echo "<p>Aucun abonnement actif pour le moment. Votre carte RFID doit être enregistrée pour souscrire.</p>";
                    break;

                case 'inscription':
                    echo "<h2>Enregistrement de votre Carte RFID</h2><br>";
                    if (!empty($msg_carte)) {
                        $couleur = strpos($msg_carte, 'Erreur') !== false ? 'red' : 'green';
                        echo "<p style='color: $couleur; margin-bottom: 15px;'>$msg_carte</p>";
                    }
                    echo '<form action="dashboard.php?section=inscription" method="POST">
                            <label>UID de la Carte RFID :</label>
                            <input type="text" name="uid" placeholder="Ex: UID123456" required>
                            
                            <label>Statut de la carte :</label>
                            <select name="statut">
                                <option value="Actif">Actif</option>
                                <option value="Inactif">Inactif</option>
                            </select>
                            
                            <button type="submit" name="enregistrer_carte">Associer la Carte</button>
                          </form>';
                    break;

                case 'info':
                default:
                    echo "<h2>Informations Générales</h2><br>";
                    echo "<p>Bienvenue sur votre plateforme SmartBus. Utilisez le menu ci-dessus pour naviguer entre vos abonnements, votre profil et l'enregistrement de votre carte RFID de transport.</p>";
                    break;
            }
            ?>
        </div>
    </div>

</body>
</html>