-- ================================
-- ADDON POUR CopybaseSimple.sql
-- À ajouter à la fin du fichier CopybaseSimple.sql
-- ================================

-- 7) TABLE ÉTUDIANTS (pour le système de badgeage RFID)
DROP TABLE IF EXISTS `DST_DB`.`etudiants`;
CREATE TABLE `DST_DB`.`etudiants` LIKE `SRC_DB`.`etudiants`;
INSERT INTO  `DST_DB`.`etudiants`
SELECT      e.*
FROM        `SRC_DB`.`etudiants` e
WHERE       (COALESCE(e.deleted,0)=0)
  AND       (
              e.codeGroupe IS NULL
           OR e.codeGroupe IN (SELECT codeGroupe FROM `DST_DB`.`ressources_groupes`)
           );

-- Index pour recherche rapide par UID RFID dans DST_DB
-- (l'index existe déjà dans SRC_DB via CREATE TABLE LIKE)
-- Pas besoin de le recréer
