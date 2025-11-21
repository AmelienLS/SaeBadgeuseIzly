# SAE Badgeuse (Arduino UNO R4)

Ce projet fournit un firmware pour badgeuse basée sur Arduino UNO R4 (Minima ou WiFi). Les passages de badge sont tamponnés localement et exposés via un protocole série minimal afin d’être transmis à une base de données ou à une application distante.

## Structure du projet

- `src/main.cpp` – point d’entrée du firmware : boucle principale, tampon d’événements et commandes série.
- `include/types.h` – constantes partagées et structures décrivant les événements de badge.
- `include/config.h` – constantes de planification, chemins SD et valeurs par défaut utilisées tant qu’aucun fichier de configuration externe n’est chargé.
- `config/device_config.example.json` – modèle à copier sur la carte SD sous le nom `device_config.json` en renseignant le WiFi, le backend et les métadonnées du boîtier.
- `platformio.ini` – environnements PlatformIO pour les UNO R4 Minima et UNO R4 WiFi。
- `arduino-uno/` – anciens sketches conservés pour référence.

## Mise en route

1. Installez [PlatformIO](https://platformio.org/install) (extension VS Code ou CLI).
2. Pour activer la synchronisation WiFi/SD, sélectionnez l’environnement `uno_r4_wifi` dans `platformio.ini` et placez un fichier `device_config.json` (voir plus bas) à la racine de la carte SD avec vos identifiants WiFi et le serveur qui expose `Base_Seances.sql`.
3. Branchez l’UNO R4 en USB puis exécutez `pio run -t upload` (ou utilisez l’interface PlatformIO).
4. Ouvrez un moniteur série à `115200` bauds pour interagir avec le firmware.

### Commandes série

| Commande | Description |
| --- | --- |
| `HELP` | Affiche la liste des commandes disponibles. |
| `PING` | Vérifie la connexion (réponse `PONG`). |
| `STATUS` | Affiche l’état du tampon, du WiFi/SD et de la configuration. |
| `LIST` / `LIST_PENDING` | Liste tous les badges ou uniquement ceux en attente de synchronisation. |
| `ADD <ID>` | Injecte manuellement un événement de badge (test, script hôte…). |
| `ACK <ID>` | Marque un badge comme synchronisé après traitement côté base. |
| `CLEAR` | Vide le tampon local. |
| `SET_LED ON|OFF` | Force l’état de la LED de statut. |
| `SYNC` | Déclenche immédiatement le téléchargement WiFi→SD de la base (UNO R4 WiFi). |

La broche `D2` est configurée en entrée avec résistance de tirage interne. La relier à la masse simule un badge et crée un événement synthétique—pratique pour tester sans lecteur réel.

## Fichier de configuration

Copiez `device_config.json` à la racine de la carte micro-SD. Utilisez `config/device_config.example.json` comme point de départ :

| Section | Champ | Description |
| --- | --- | --- |
| `wifi` | `ssid`, `password` | Identifiants du réseau interne sécurisé. |
| `backend` | `host`, `port`, `path` | Adresse HTTP(S) du fichier `Base_Seances.sql`. |
| `device` | `uid` | Identifiant unique du boîtier. |
| `device` | `ownerCardId` | Numéro de badge du responsable du boîtier. |

Si le fichier manque ou est invalide, le firmware retombe sur les valeurs par défaut de `include/config.h`. Pensez à y définir des valeurs sûres avant de diffuser un binaire.

## Rafraîchissement quotidien de la base (UNO R4 WiFi)

- Le firmware se connecte au WiFi interne, télécharge `Base_Seances.sql` depuis le serveur configuré et l’écrit sur la carte micro-SD toutes les 24 heures.
- L’écriture passe par un fichier temporaire ; l’ancienne copie n’est supprimée qu’une fois la nouvelle complètement enregistrée, afin d’éviter toute corruption.
- La commande `SYNC` permet de lancer manuellement l’opération (tests, maintenance, débogage).
- Les journaux `[WiFi]`, `[SD]`, `[SYNC]` sont visibles sur le port série, et `STATUS` indique la disponibilité WiFi/SD ainsi que l’heure de la dernière/prochaine synchronisation.

## Prochaines étapes

- Remplacer l’entrée simulée par le véritable lecteur de badges.
- Relier le protocole série à l’application ou au service qui insérera les événements dans la base.
- Intégrer le dump SD dans votre chaîne d’import SQL afin de maintenir la base synchronisée.
- Étendre les commandes (authentification, configuration distante, diagnostics, etc.) selon vos besoins.