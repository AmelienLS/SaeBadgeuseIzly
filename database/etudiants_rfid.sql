-- ================================
-- TABLE DES ÉTUDIANTS AVEC CARTES RFID
-- À ajouter à la base SRC_DB
-- ================================

USE SRC_DB;

-- Table pour stocker les étudiants et leurs cartes RFID
CREATE TABLE IF NOT EXISTS `etudiants` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `uid_rfid` varchar(20) NOT NULL,
  `nom` varchar(150) NOT NULL,
  `prenom` varchar(150) NOT NULL,
  `codeGroupe` int(11) DEFAULT NULL,
  `email` varchar(100) DEFAULT '',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `uid_rfid_unique` (`uid_rfid`),
  KEY `idx_uid_rfid` (`uid_rfid`),
  KEY `idx_groupe` (`codeGroupe`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- Exemples de données (à adapter avec vos vrais étudiants)
INSERT INTO `etudiants` (`uid_rfid`, `nom`, `prenom`, `codeGroupe`, `email`) VALUES
('04A1B2C3', 'DUPONT', 'Jean', 15824990, 'jean.dupont@example.com'),
('04D4E5F6', 'MARTIN', 'Marie', 15824990, 'marie.martin@example.com'),
('04123456', 'BERNARD', 'Paul', 15824991, 'paul.bernard@example.com'),
('04ABCDEF', 'DURAND', 'Sophie', 15824991, 'sophie.durand@example.com');

-- Script pour ajouter cette table à CopybaseSimple.sql
-- Ajouter cette section après la copie des autres tables

-- ================================
-- SNIPPET À AJOUTER À CopybaseSimple.sql
-- À placer après la section "-- 3) RÉFÉRENTIELS UTILISÉS"
-- ================================

-- Dans DST_DB : copier les étudiants qui correspondent aux groupes copiés
-- DROP TABLE IF EXISTS `DST_DB`.`etudiants`;
-- CREATE TABLE `DST_DB`.`etudiants` LIKE `SRC_DB`.`etudiants`;
-- INSERT INTO  `DST_DB`.`etudiants`
-- SELECT      e.*
-- FROM        `SRC_DB`.`etudiants` e
-- WHERE       (COALESCE(e.deleted,0)=0)
--   AND       (
--               e.codeGroupe IS NULL
--            OR e.codeGroupe IN (SELECT codeGroupe FROM `DST_DB`.`ressources_groupes`)
--            );
