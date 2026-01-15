<?php
/**
 * Script PHP pour exporter les étudiants au format CSV
 * À placer sur votre serveur web avec accès à la base DST_DB
 *
 * Usage: https://votre-serveur.com/export_etudiants.php
 * L'Arduino téléchargera ce fichier via HTTP pour obtenir les données
 */

// Configuration de la base de données
define('DB_HOST', 'localhost');
define('DB_NAME', 'DST_DB');
define('DB_USER', 'votre_utilisateur');
define('DB_PASS', 'votre_mot_de_passe');

// Headers pour le CSV (pas d'attachment pour l'Arduino)
header('Content-Type: text/csv; charset=utf-8');
header('Cache-Control: no-cache, must-revalidate');
header('Expires: Sat, 26 Jul 1997 05:00:00 GMT');

try {
    // Connexion à la base de données
    $pdo = new PDO(
        "mysql:host=" . DB_HOST . ";dbname=" . DB_NAME . ";charset=utf8mb4",
        DB_USER,
        DB_PASS,
        [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]
    );

    // Requête pour récupérer les étudiants actifs
    $sql = "SELECT uid_rfid, nom, prenom, email, codeGroupe
            FROM etudiants
            WHERE deleted = 0
            ORDER BY nom, prenom";

    $stmt = $pdo->query($sql);

    // Ouvrir le flux de sortie
    $output = fopen('php://output', 'w');

    // Écrire l'en-tête CSV
    fputcsv($output, ['uid_rfid', 'nom', 'prenom', 'email', 'codeGroupe'], ',');

    // Écrire chaque ligne
    $count = 0;
    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
        // Nettoyer les données pour éviter les problèmes de CSV
        $uid = strtoupper(trim($row['uid_rfid']));
        $nom = trim($row['nom']);
        $prenom = trim($row['prenom']);
        $email = isset($row['email']) ? trim($row['email']) : '';
        $codeGroupe = isset($row['codeGroupe']) ? $row['codeGroupe'] : '';

        fputcsv($output, [$uid, $nom, $prenom, $email, $codeGroupe], ',');
        $count++;
    }

    fclose($output);

    // Log pour debug (si besoin)
    error_log("export_etudiants.php: $count étudiants exportés");

} catch (PDOException $e) {
    // En cas d'erreur, retourner un CSV avec un message d'erreur
    http_response_code(500);
    echo "uid_rfid,nom,prenom,email,codeGroupe\n";
    echo "ERROR,Erreur de connexion,Base de donnees,," . $e->getCode() . "\n";
}
?>
