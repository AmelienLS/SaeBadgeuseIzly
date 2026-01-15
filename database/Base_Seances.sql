-- phpMyAdmin SQL Dump
-- version 4.6.6deb4+deb9u2
-- https://www.phpmyadmin.net/
--
-- Client :  localhost:3306
-- Généré le :  Ven 19 Septembre 2025 à 13:07
-- Version du serveur :  10.1.48-MariaDB-0+deb9u2
-- Version de PHP :  7.0.33-0+deb9u12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de données :  `vt2023`
--

-- --------------------------------------------------------

--
-- Structure de la table `enseignements`
--

CREATE TABLE `enseignements` (
  `codeEnseignement` int(11) NOT NULL,
  `nom` varchar(150) NOT NULL DEFAULT 'ENS?',
  `codeMatiere` int(11) NOT NULL DEFAULT '0',
  `dureeTotale` int(5) NOT NULL DEFAULT '2000',
  `dureeSeance` int(5) NOT NULL DEFAULT '100',
  `couleurFond` bigint(20) NOT NULL DEFAULT '0',
  `couleurPolice` bigint(20) NOT NULL DEFAULT '0',
  `alias` varchar(50) NOT NULL DEFAULT 'ENS?',
  `codeTypeSalle` int(11) NOT NULL DEFAULT '0',
  `codeZoneSalle` int(11) NOT NULL DEFAULT '0',
  `nbSeancesHebdo` int(5) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `commentaire` varchar(250) NOT NULL,
  `identifiant` varchar(50) NOT NULL,
  `typePublic` tinyint(5) NOT NULL DEFAULT '0',
  `forfaitaire` tinyint(1) NOT NULL DEFAULT '0',
  `dureeForfaitaire` int(5) NOT NULL DEFAULT '0',
  `volumeReparti` int(1) NOT NULL DEFAULT '0',
  `codeTypeActivite` int(5) NOT NULL DEFAULT '2',
  `codeComposante` int(11) NOT NULL DEFAULT '-1',
  `codeNiveau` int(11) NOT NULL,
  `dateDebut` date NOT NULL DEFAULT '2014-01-01',
  `dateFin` date NOT NULL DEFAULT '2030-01-01',
  `article6` tinyint(1) NOT NULL DEFAULT '0',
  `codeTypeMateriel` int(11) NOT NULL,
  `dureeAjoutee` int(5) NOT NULL DEFAULT '0',
  `codeDiplome` int(11) NOT NULL,
  `numeroCommande` int(11) NOT NULL DEFAULT '0',
  `operateurTypesSalles` tinyint(1) NOT NULL DEFAULT '0',
  `operateurTypesMateriels` tinyint(1) NOT NULL DEFAULT '0',
  `asynchrone` tinyint(1) NOT NULL DEFAULT '0',
  `presentiel` tinyint(1) NOT NULL DEFAULT '1',
  `diffusionCommentaire` tinyint(1) DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `enseignements_groupes`
--

CREATE TABLE `enseignements_groupes` (
  `codeEnseignement` int(11) NOT NULL DEFAULT '0',
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `enseignements_profs`
--

CREATE TABLE `enseignements_profs` (
  `codeEnseignement` int(11) NOT NULL DEFAULT '0',
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `enseignements_salles`
--

CREATE TABLE `enseignements_salles` (
  `codeEnseignement` int(11) NOT NULL DEFAULT '0',
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `hierarchies_groupes`
--

CREATE TABLE `hierarchies_groupes` (
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `codeRessourceFille` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `matieres`
--

CREATE TABLE `matieres` (
  `codeMatiere` int(11) NOT NULL,
  `couleurFond` bigint(20) NOT NULL DEFAULT '0',
  `couleurPolice` bigint(20) NOT NULL DEFAULT '0',
  `nom` varchar(150) NOT NULL DEFAULT 'MATIERE?',
  `type` int(11) NOT NULL DEFAULT '0',
  `codeCNU` int(11) NOT NULL DEFAULT '0',
  `alias` varchar(50) NOT NULL DEFAULT 'MATIERE?',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `commentaire` varchar(250) NOT NULL,
  `identifiant` varchar(50) NOT NULL,
  `codeDiscipline` int(11) NOT NULL DEFAULT '0',
  `periode` int(11) NOT NULL DEFAULT '0',
  `codeComposante` int(11) NOT NULL DEFAULT '0',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `ressources_groupes`
--

CREATE TABLE `ressources_groupes` (
  `codeGroupe` int(11) NOT NULL,
  `couleurFond` bigint(20) NOT NULL DEFAULT '0',
  `couleurPolice` bigint(20) NOT NULL DEFAULT '0',
  `nom` varchar(150) NOT NULL DEFAULT 'NOM?',
  `quantite` int(10) NOT NULL DEFAULT '0',
  `alias` varchar(50) NOT NULL DEFAULT '',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `commentaire` varchar(250) NOT NULL,
  `identifiant` varchar(50) NOT NULL,
  `typePublic` tinyint(2) NOT NULL DEFAULT '0',
  `codeComposante` int(11) NOT NULL DEFAULT '-1',
  `codeNiveau` int(11) NOT NULL,
  `codeDiplome` int(11) NOT NULL,
  `email` varchar(100) NOT NULL DEFAULT '',
  `numeroCommande` int(11) NOT NULL DEFAULT '0',
  `departement` int(11) NOT NULL,
  `detail_nom` varchar(45) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `ressources_profs`
--

CREATE TABLE `ressources_profs` (
  `codeProf` int(11) NOT NULL,
  `couleurFond` bigint(20) NOT NULL DEFAULT '0',
  `couleurPolice` bigint(20) NOT NULL DEFAULT '0',
  `nom` varchar(150) NOT NULL DEFAULT 'NOM?',
  `prenom` varchar(150) NOT NULL DEFAULT 'PRENOM?',
  `codeGrade` int(11) NOT NULL DEFAULT '0',
  `numero` int(3) NOT NULL DEFAULT '0',
  `rue` varchar(50) NOT NULL DEFAULT '',
  `codePostal` int(5) NOT NULL DEFAULT '0',
  `ville` varchar(50) NOT NULL DEFAULT 'VALENCIENNES',
  `pays` varchar(50) NOT NULL DEFAULT 'FRANCE',
  `telephone1` varchar(10) NOT NULL DEFAULT '327511234',
  `telephone2` varchar(10) NOT NULL DEFAULT '0',
  `codeCnu` int(11) NOT NULL DEFAULT '0',
  `alias` varchar(50) NOT NULL DEFAULT '',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `identifiant` varchar(50) NOT NULL,
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `email` varchar(100) NOT NULL DEFAULT '',
  `commentaire` varchar(250) NOT NULL,
  `codeComposante` int(11) NOT NULL DEFAULT '-1',
  `photo` mediumblob NOT NULL,
  `titulaire` tinyint(1) NOT NULL,
  `dateValidationDossier` date NOT NULL DEFAULT '2500-01-01',
  `volStatSpecif` int(5) NOT NULL,
  `volCompSpecif` int(5) NOT NULL,
  `identifiantNational` varchar(50) NOT NULL,
  `prenom2` varchar(150) NOT NULL,
  `dateNaissance` date NOT NULL,
  `dateDebutContrat` date NOT NULL DEFAULT '2000-01-01',
  `dateFinContrat` date NOT NULL DEFAULT '5000-01-01',
  `sexe` tinyint(1) NOT NULL DEFAULT '0',
  `numeroCommande` int(11) NOT NULL DEFAULT '0',
  `volSpecif` tinyint(1) NOT NULL DEFAULT '0',
  `volHebdoSpecif` int(11) DEFAULT '2460'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `ressources_salles`
--

CREATE TABLE `ressources_salles` (
  `codeSalle` int(11) NOT NULL,
  `couleurFond` bigint(20) NOT NULL DEFAULT '0',
  `couleurPolice` bigint(20) NOT NULL DEFAULT '0',
  `nom` varchar(150) NOT NULL DEFAULT 'NOM?',
  `codeZoneSalle` int(11) NOT NULL DEFAULT '0',
  `capacite` int(5) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `alias` varchar(50) NOT NULL DEFAULT '',
  `surface` int(8) NOT NULL DEFAULT '0',
  `commentaire` varchar(250) NOT NULL,
  `identifiant` varchar(50) NOT NULL,
  `codeComposante` int(11) NOT NULL DEFAULT '-1',
  `handicap` tinyint(1) NOT NULL DEFAULT '0',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `seances`
--

CREATE TABLE `seances` (
  `codeSeance` int(11) NOT NULL,
  `dateSeance` date NOT NULL DEFAULT '2000-01-01',
  `heureSeance` int(5) NOT NULL DEFAULT '800',
  `dureeSeance` int(5) NOT NULL DEFAULT '100',
  `codeEnseignement` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `commentaire` char(250) NOT NULL DEFAULT '',
  `bloquee` tinyint(1) NOT NULL DEFAULT '0',
  `diffusable` tinyint(1) NOT NULL DEFAULT '1',
  `annulee` tinyint(1) NOT NULL DEFAULT '0',
  `controle` tinyint(1) NOT NULL DEFAULT '0',
  `numeroCommande` int(11) NOT NULL DEFAULT '0',
  `presentiel` tinyint(1) DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `seances_groupes`
--

CREATE TABLE `seances_groupes` (
  `codeSeance` int(11) NOT NULL DEFAULT '0',
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `seances_profs`
--

CREATE TABLE `seances_profs` (
  `codeSeance` int(11) NOT NULL DEFAULT '0',
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `seances_salles`
--

CREATE TABLE `seances_salles` (
  `codeSeance` int(11) NOT NULL DEFAULT '0',
  `codeRessource` int(11) NOT NULL DEFAULT '0',
  `dateModif` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `deleted` tinyint(1) NOT NULL DEFAULT '0',
  `codeProprietaire` int(11) NOT NULL DEFAULT '0',
  `dateCreation` datetime NOT NULL DEFAULT '2000-01-01 00:00:00',
  `numeroCommande` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Index pour les tables exportées
--

--
-- Index pour la table `enseignements`
--
ALTER TABLE `enseignements`
  ADD PRIMARY KEY (`codeEnseignement`),
  ADD KEY `enseignements_del` (`deleted`) USING BTREE,
  ADD KEY `enseignements_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `enseignements_ibfk_1` (`codeMatiere`) USING BTREE,
  ADD KEY `enseignements_ibfk_4` (`codeTypeActivite`) USING BTREE,
  ADD KEY `enseignements_ibfk_5` (`codeComposante`) USING BTREE,
  ADD KEY `enseignements_ibfk_8` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `enseignements_groupes`
--
ALTER TABLE `enseignements_groupes`
  ADD KEY `enseignements_groupes_del` (`deleted`) USING BTREE,
  ADD KEY `enseignements_groupes_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `enseignements_groupes_ibfk_1` (`codeEnseignement`) USING BTREE,
  ADD KEY `enseignements_groupes_ibfk_2` (`codeRessource`) USING BTREE,
  ADD KEY `enseignements_groupes_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `enseignements_profs`
--
ALTER TABLE `enseignements_profs`
  ADD KEY `enseignements_profs_del` (`deleted`) USING BTREE,
  ADD KEY `enseignements_profs_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `enseignements_profs_ibfk_1` (`codeEnseignement`) USING BTREE,
  ADD KEY `enseignements_profs_ibfk_2` (`codeRessource`) USING BTREE,
  ADD KEY `enseignements_profs_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `enseignements_salles`
--
ALTER TABLE `enseignements_salles`
  ADD KEY `enseignements_salles_del` (`deleted`) USING BTREE,
  ADD KEY `enseignements_salles_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `enseignements_salles_ibfk_1` (`codeEnseignement`) USING BTREE,
  ADD KEY `enseignements_salles_ibfk_2` (`codeRessource`) USING BTREE,
  ADD KEY `enseignements_salles_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `hierarchies_groupes`
--
ALTER TABLE `hierarchies_groupes`
  ADD KEY `hierarchies_groupe_del` (`deleted`) USING BTREE,
  ADD KEY `hierarchies_groupes_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `hierarchies_groupes_ibfk_1` (`codeRessource`) USING BTREE,
  ADD KEY `hierarchies_groupes_ibfk_2` (`codeRessourceFille`) USING BTREE,
  ADD KEY `hierarchies_groupes_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `matieres`
--
ALTER TABLE `matieres`
  ADD PRIMARY KEY (`codeMatiere`),
  ADD KEY `matieres_del` (`deleted`) USING BTREE,
  ADD KEY `matieres_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `matieres_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `ressources_groupes`
--
ALTER TABLE `ressources_groupes`
  ADD PRIMARY KEY (`codeGroupe`),
  ADD KEY `ressources_groupes_del` (`deleted`) USING BTREE,
  ADD KEY `ressources_groupes_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `ressources_groupes_ibfk_2` (`codeComposante`) USING BTREE,
  ADD KEY `ressources_groupes_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `ressources_profs`
--
ALTER TABLE `ressources_profs`
  ADD PRIMARY KEY (`codeProf`),
  ADD KEY `ressources_profs_del` (`deleted`) USING BTREE,
  ADD KEY `ressources_profs_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `ressources_profs_ibfk_1` (`codeComposante`) USING BTREE,
  ADD KEY `ressources_profs_ibfk_2` (`codeGrade`) USING BTREE,
  ADD KEY `ressources_profs_ibfk_3` (`codeProprietaire`) USING BTREE,
  ADD KEY `ressources_profs_ibfk_4` (`codeCnu`) USING BTREE;

--
-- Index pour la table `ressources_salles`
--
ALTER TABLE `ressources_salles`
  ADD PRIMARY KEY (`codeSalle`),
  ADD KEY `ressources_salles_del` (`deleted`) USING BTREE,
  ADD KEY `ressources_salles_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `ressources_salles_ibfk_1` (`codeComposante`) USING BTREE,
  ADD KEY `ressources_salles_ibfk_2` (`codeZoneSalle`) USING BTREE,
  ADD KEY `ressources_salles_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `seances`
--
ALTER TABLE `seances`
  ADD PRIMARY KEY (`codeSeance`),
  ADD KEY `seances_del` (`deleted`) USING BTREE,
  ADD KEY `seances_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `seances_ibfk_2` (`codeEnseignement`) USING BTREE,
  ADD KEY `seances_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `seances_groupes`
--
ALTER TABLE `seances_groupes`
  ADD KEY `seances_groupes_del` (`deleted`) USING BTREE,
  ADD KEY `seances_groupes_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `seances_groupes_ibfk_1` (`codeSeance`) USING BTREE,
  ADD KEY `seances_groupes_ibfk_2` (`codeRessource`) USING BTREE,
  ADD KEY `seances_groupes_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `seances_profs`
--
ALTER TABLE `seances_profs`
  ADD KEY `seances_profs_del` (`deleted`) USING BTREE,
  ADD KEY `seances_profs_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `seances_profs_ibfk_1` (`codeSeance`) USING BTREE,
  ADD KEY `seances_profs_ibfk_2` (`codeRessource`) USING BTREE,
  ADD KEY `seances_profs_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- Index pour la table `seances_salles`
--
ALTER TABLE `seances_salles`
  ADD KEY `seances_salles_del` (`deleted`) USING BTREE,
  ADD KEY `seances_salles_dateModif` (`dateModif`) USING BTREE,
  ADD KEY `seances_salles_ibfk_1` (`codeSeance`) USING BTREE,
  ADD KEY `seances_salles_ibfk_2` (`codeRessource`) USING BTREE,
  ADD KEY `seances_salles_ibfk_3` (`codeProprietaire`) USING BTREE;

--
-- AUTO_INCREMENT pour les tables exportées
--

--
-- AUTO_INCREMENT pour la table `enseignements`
--
ALTER TABLE `enseignements`
  MODIFY `codeEnseignement` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=28152048;
--
-- AUTO_INCREMENT pour la table `matieres`
--
ALTER TABLE `matieres`
  MODIFY `codeMatiere` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=27888473;
--
-- AUTO_INCREMENT pour la table `ressources_groupes`
--
ALTER TABLE `ressources_groupes`
  MODIFY `codeGroupe` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=15824992;
--
-- AUTO_INCREMENT pour la table `ressources_profs`
--
ALTER TABLE `ressources_profs`
  MODIFY `codeProf` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24718539;
--
-- AUTO_INCREMENT pour la table `ressources_salles`
--
ALTER TABLE `ressources_salles`
  MODIFY `codeSalle` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24349771;
--
-- AUTO_INCREMENT pour la table `seances`
--
ALTER TABLE `seances`
  MODIFY `codeSeance` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=151382;
--
-- Contraintes pour les tables exportées
--

--
-- Contraintes pour la table `enseignements`
--
ALTER TABLE `enseignements`
  ADD CONSTRAINT `enseignements_ibfk_1` FOREIGN KEY (`codeMatiere`) REFERENCES `matieres` (`codeMatiere`),
  ADD CONSTRAINT `enseignements_ibfk_2` FOREIGN KEY (`codeTypeActivite`) REFERENCES `types_activites` (`codeTypeActivite`),
  ADD CONSTRAINT `enseignements_ibfk_3` FOREIGN KEY (`codeComposante`) REFERENCES `composantes` (`codeComposante`),
  ADD CONSTRAINT `enseignements_ibfk_4` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `enseignements_groupes`
--
ALTER TABLE `enseignements_groupes`
  ADD CONSTRAINT `enseignements_groupes_ibfk_1` FOREIGN KEY (`codeEnseignement`) REFERENCES `enseignements` (`codeEnseignement`),
  ADD CONSTRAINT `enseignements_groupes_ibfk_2` FOREIGN KEY (`codeRessource`) REFERENCES `ressources_groupes` (`codeGroupe`),
  ADD CONSTRAINT `enseignements_groupes_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `enseignements_profs`
--
ALTER TABLE `enseignements_profs`
  ADD CONSTRAINT `enseignements_profs_ibfk_1` FOREIGN KEY (`codeEnseignement`) REFERENCES `enseignements` (`codeEnseignement`),
  ADD CONSTRAINT `enseignements_profs_ibfk_2` FOREIGN KEY (`codeRessource`) REFERENCES `ressources_profs` (`codeProf`),
  ADD CONSTRAINT `enseignements_profs_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `enseignements_salles`
--
ALTER TABLE `enseignements_salles`
  ADD CONSTRAINT `enseignements_salles_ibfk_1` FOREIGN KEY (`codeEnseignement`) REFERENCES `enseignements` (`codeEnseignement`),
  ADD CONSTRAINT `enseignements_salles_ibfk_2` FOREIGN KEY (`codeRessource`) REFERENCES `ressources_salles` (`codeSalle`),
  ADD CONSTRAINT `enseignements_salles_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `hierarchies_groupes`
--
ALTER TABLE `hierarchies_groupes`
  ADD CONSTRAINT `hierarchies_groupes_ibfk_1` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `matieres`
--
ALTER TABLE `matieres`
  ADD CONSTRAINT `matieres_ibfk_1` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `ressources_groupes`
--
ALTER TABLE `ressources_groupes`
  ADD CONSTRAINT `ressources_groupes_ibfk_1` FOREIGN KEY (`codeComposante`) REFERENCES `composantes` (`codeComposante`),
  ADD CONSTRAINT `ressources_groupes_ibfk_2` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `ressources_profs`
--
ALTER TABLE `ressources_profs`
  ADD CONSTRAINT `ressources_profs_ibfk_1` FOREIGN KEY (`codeComposante`) REFERENCES `composantes` (`codeComposante`),
  ADD CONSTRAINT `ressources_profs_ibfk_2` FOREIGN KEY (`codeGrade`) REFERENCES `grades` (`codeGrade`),
  ADD CONSTRAINT `ressources_profs_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`),
  ADD CONSTRAINT `ressources_profs_ibfk_4` FOREIGN KEY (`codeCnu`) REFERENCES `cnu` (`codeCNU`);

--
-- Contraintes pour la table `ressources_salles`
--
ALTER TABLE `ressources_salles`
  ADD CONSTRAINT `ressources_salles_ibfk_1` FOREIGN KEY (`codeComposante`) REFERENCES `composantes` (`codeComposante`),
  ADD CONSTRAINT `ressources_salles_ibfk_2` FOREIGN KEY (`codeZoneSalle`) REFERENCES `zones_salles` (`codeZoneSalle`),
  ADD CONSTRAINT `ressources_salles_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `seances`
--
ALTER TABLE `seances`
  ADD CONSTRAINT `seances_ibfk_1` FOREIGN KEY (`codeEnseignement`) REFERENCES `enseignements` (`codeEnseignement`),
  ADD CONSTRAINT `seances_ibfk_2` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `seances_groupes`
--
ALTER TABLE `seances_groupes`
  ADD CONSTRAINT `seances_groupes_ibfk_1` FOREIGN KEY (`codeSeance`) REFERENCES `seances` (`codeSeance`),
  ADD CONSTRAINT `seances_groupes_ibfk_2` FOREIGN KEY (`codeRessource`) REFERENCES `ressources_groupes` (`codeGroupe`),
  ADD CONSTRAINT `seances_groupes_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `seances_profs`
--
ALTER TABLE `seances_profs`
  ADD CONSTRAINT `seances_profs_ibfk_1` FOREIGN KEY (`codeSeance`) REFERENCES `seances` (`codeSeance`),
  ADD CONSTRAINT `seances_profs_ibfk_2` FOREIGN KEY (`codeRessource`) REFERENCES `ressources_profs` (`codeProf`),
  ADD CONSTRAINT `seances_profs_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

--
-- Contraintes pour la table `seances_salles`
--
ALTER TABLE `seances_salles`
  ADD CONSTRAINT `seances_salles_ibfk_1` FOREIGN KEY (`codeSeance`) REFERENCES `seances` (`codeSeance`),
  ADD CONSTRAINT `seances_salles_ibfk_2` FOREIGN KEY (`codeRessource`) REFERENCES `ressources_salles` (`codeSalle`),
  ADD CONSTRAINT `seances_salles_ibfk_3` FOREIGN KEY (`codeProprietaire`) REFERENCES `utilisateurs` (`codeProprietaire`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;