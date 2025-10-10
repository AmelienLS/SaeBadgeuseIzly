-- ================================
-- SIMPLE COPY EDT -> EDT_MIN (NFC)
-- Remplacer SRC_DB et DST_DB
-- ================================

-- Fenêtre de dates pour réduire le volume
SET @PAST_DAYS   := 1;     -- jours en arrière
SET @FUTURE_DAYS := 1;    -- jours en avant

-- Crée la base destination si besoin
CREATE DATABASE IF NOT EXISTS `DST_DB`
  CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- 1) SÉANCES (filtrées par date + non annulées/supprimées si colonnes présentes)
DROP TABLE IF EXISTS `DST_DB`.`seances`;
CREATE TABLE `DST_DB`.`seances` LIKE `SRC_DB`.`seances`;
INSERT INTO  `DST_DB`.`seances`
SELECT      *
FROM        `SRC_DB`.`seances`
WHERE       dateSeance BETWEEN (CURDATE() - INTERVAL @PAST_DAYS DAY)
                           AND (CURDATE() + INTERVAL @FUTURE_DAYS DAY)
  AND       (COALESCE(annulee,0)=0)
  AND       (COALESCE(deleted,0)=0);

-- 2) LIENS SÉANCE↔PROFS / ↔GROUPES / ↔SALLES (restreints aux séances copiées)
DROP TABLE IF EXISTS `DST_DB`.`seances_profs`;
CREATE TABLE `DST_DB`.`seances_profs` LIKE `SRC_DB`.`seances_profs`;
INSERT INTO  `DST_DB`.`seances_profs`
SELECT      sp.*
FROM        `SRC_DB`.`seances_profs` sp
WHERE       sp.codeSeance IN (SELECT codeSeance FROM `DST_DB`.`seances`)
  AND       (COALESCE(sp.deleted,0)=0);

DROP TABLE IF EXISTS `DST_DB`.`seances_groupes`;
CREATE TABLE `DST_DB`.`seances_groupes` LIKE `SRC_DB`.`seances_groupes`;
INSERT INTO  `DST_DB`.`seances_groupes`
SELECT      sg.*
FROM        `SRC_DB`.`seances_groupes` sg
WHERE       sg.codeSeance IN (SELECT codeSeance FROM `DST_DB`.`seances`)
  AND       (COALESCE(sg.deleted,0)=0);

DROP TABLE IF EXISTS `DST_DB`.`seances_salles`;
CREATE TABLE `DST_DB`.`seances_salles` LIKE `SRC_DB`.`seances_salles`;
INSERT INTO  `DST_DB`.`seances_salles`
SELECT      ss.*
FROM        `SRC_DB`.`seances_salles` ss
WHERE       ss.codeSeance IN (SELECT codeSeance FROM `DST_DB`.`seances`)
  AND       (COALESCE(ss.deleted,0)=0);

-- 3) RÉFÉRENTIELS UTILISÉS (PROFS / GROUPES (incl. detail_nom) / SALLES)
DROP TABLE IF EXISTS `DST_DB`.`ressources_profs`;
CREATE TABLE `DST_DB`.`ressources_profs` LIKE `SRC_DB`.`ressources_profs`;
INSERT INTO  `DST_DB`.`ressources_profs`
SELECT      rp.*
FROM        `SRC_DB`.`ressources_profs` rp
WHERE       rp.codeProf IN (
            SELECT DISTINCT codeRessource FROM `DST_DB`.`seances_profs`
)
  AND       (COALESCE(rp.deleted,0)=0);

DROP TABLE IF EXISTS `DST_DB`.`ressources_groupes`;
CREATE TABLE `DST_DB`.`ressources_groupes` LIKE `SRC_DB`.`ressources_groupes`;
INSERT INTO  `DST_DB`.`ressources_groupes`
SELECT      rg.*
FROM        `SRC_DB`.`ressources_groupes` rg
WHERE       rg.codeGroupe IN (
            SELECT DISTINCT codeRessource FROM `DST_DB`.`seances_groupes`
)
  AND       (COALESCE(rg.deleted,0)=0);

DROP TABLE IF EXISTS `DST_DB`.`ressources_salles`;
CREATE TABLE `DST_DB`.`ressources_salles` LIKE `SRC_DB`.`ressources_salles`;
INSERT INTO  `DST_DB`.`ressources_salles`
SELECT      rs.*
FROM        `SRC_DB`.`ressources_salles` rs
WHERE       rs.codeSalle IN (
            SELECT DISTINCT codeRessource FROM `DST_DB`.`seances_salles`
)
  AND       (COALESCE(rs.deleted,0)=0);

-- 4) ENSEIGNEMENTS & MATIÈRES (pour libellés de cours)
DROP TABLE IF EXISTS `DST_DB`.`enseignements`;
CREATE TABLE `DST_DB`.`enseignements` LIKE `SRC_DB`.`enseignements`;
INSERT INTO  `DST_DB`.`enseignements`
SELECT      e.*
FROM        `SRC_DB`.`enseignements` e
WHERE       e.codeEnseignement IN (
            SELECT DISTINCT codeEnseignement FROM `DST_DB`.`seances`
)
  AND       (COALESCE(e.deleted,0)=0);

DROP TABLE IF EXISTS `DST_DB`.`matieres`;
CREATE TABLE `DST_DB`.`matieres` LIKE `SRC_DB`.`matieres`;
INSERT INTO  `DST_DB`.`matieres`
SELECT      m.*
FROM        `SRC_DB`.`matieres` m
WHERE       m.codeMatiere IN (
            SELECT DISTINCT codeMatiere FROM `DST_DB`.`enseignements`
)
  AND       (COALESCE(m.deleted,0)=0);

-- 5) HIÉRARCHIE DE GROUPES (utile si tu exploites la fermeture TD/TP/Promo)
DROP TABLE IF EXISTS `DST_DB`.`hierarchies_groupes`;
CREATE TABLE `DST_DB`.`hierarchies_groupes` LIKE `SRC_DB`.`hierarchies_groupes`;
INSERT INTO  `DST_DB`.`hierarchies_groupes`
SELECT      hg.*
FROM        `SRC_DB`.`hierarchies_groupes` hg
WHERE       (COALESCE(hg.deleted,0)=0)
  AND       (
              hg.codeRessource IN (SELECT codeGroupe FROM `DST_DB`.`ressources_groupes`)
           OR hg.codeRessourceFille IN (SELECT codeGroupe FROM `DST_DB`.`ressources_groupes`)
           );

-- 6) (OPTIONNEL) INDEXS UTILES POUR LES LOOKUPS RAPIDES
ALTER TABLE `DST_DB`.`seances`           ADD INDEX idx_date_heure (dateSeance, heureSeance);
ALTER TABLE `DST_DB`.`seances_profs`     ADD INDEX idx_sp (codeSeance, codeRessource);
ALTER TABLE `DST_DB`.`seances_groupes`   ADD INDEX idx_sg (codeSeance, codeRessource);
ALTER TABLE `DST_DB`.`seances_salles`    ADD INDEX idx_ss (codeSeance, codeRessource);