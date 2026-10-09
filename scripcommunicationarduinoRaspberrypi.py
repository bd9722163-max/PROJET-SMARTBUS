import serial
import mysql.connector
import time

# --- CONFIGURATION BASE DE DONNEES MARIADB ---
DB_CONFIG = {
    'host': 'localhost',
    'user': 'Aida',          #  C'est le nom de L'utilisateur cree dans MariaDB
    'password': 'votre_mot_de_passe', # c'est le de mot de passe de l'utilisateur creer
    'database': 'smartbus_db'
}

# --- CONFIGURATION PORT SERIE USB ---
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

def initialiser_systeme():
    """Initialise les connexions MariaDB et Serie."""
    try:
        db = mysql.connector.connect(**DB_CONFIG)
        print("[DATABASE] Connexion reussie a MariaDB.")
    except mysql.connector.Error as err:
        print(f"[ERREUR DB] Impossible de se connecter a la base : {err}")
        exit(1)

    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2) # Attente d'initialisation du port serie
        print(f"[SERIAL] Communication ouverte sur {SERIAL_PORT}.")
    except Exception as e:
        print(f"[ERREUR SERIE] Impossible d'ouvrir le port {SERIAL_PORT} : {e}")
        exit(1)

    return db, ser

def traiter_carte(uid_scanne, db, ser):
    """Interroge la base de donnees et envoie la commande a l'Arduino."""
    cursor = db.cursor()
    print(f"\n[SCAN] UID recu de l'Arduino : {uid_scanne}")

    # Requete SQL pour verifier l'abonnement du passager
    query = "SELECT nom_passager, statut_abonnement FROM abonnements WHERE uid = %s"
    cursor.execute(query, (uid_scanne,))
    resultat = cursor.fetchone()

    # Traitement de la reponse
    if resultat:
        nom_passager, statut = resultat
        if statut == "VALIDE":
            print(f" -> RESULTAT : ACCES ACCORDE à {nom_passager}")
            # Format envoye : OK:<Nom_Abonne>
            reponse = f"OK:{nom_passager}\n"
        else:
            print(f" -> RESULTAT : ACCES REFUSE (Abonnement {statut})")
            reponse = "NOK:Abonn. Expire\n"
    else:
        print(" -> RESULTAT : ACCES REFUSE (Carte non enregistree)")
        reponse = "NOK:Inconnu\n"

    # Envoi de la reponse d'instruction a l'Arduino
    ser.write(reponse.encode('utf-8'))
    print(f"[LIAISON SERIE] Reponse transmise : {reponse.strip()}")
    cursor.close()

def main():
    db, ser = initialiser_systeme()
    print("\n==========================================")
    print("      SYSTEME SMARTBUS PRETS À L'EMPLOI   ")
    print("==========================================")

    try:
        while True:
            # Verification de la presence de donnees sur le port serie
            if ser.in_waiting > 0:
                uid = ser.readline().decode('utf-8').strip().upper()
                if uid:
                    traiter_carte(uid, db, ser)
            time.sleep(0.05)

    except KeyboardInterrupt:
        print("\n[FIN] Arret du serveur SmartBus.")
    finally:
        ser.close()
        db.close()

if __name__ == "__main__":
    main()