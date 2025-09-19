with closing(sqlite3.connect(SQLITE_PATH)) as conn, conn:
        conn.execute("PRAGMA journal_mode=WAL;")
        conn.execute("PRAGMA synchronous=NORMAL;")
        conn.execute("PRAGMA temp_store=MEMORY;")
        conn.execute("PRAGMA foreign_keys=ON;")

        conn.executescript("""
        CREATE TABLE profs (codeProf INTEGER PRIMARY KEY, nom TEXT, prenom TEXT);
        CREATE TABLE groupes (codeGroupe INTEGER PRIMARY KEY, nom TEXT, detail_nom TEXT);
        CREATE TABLE salles (codeSalle INTEGER PRIMARY KEY, nom TEXT);
        CREATE TABLE enseignements (codeEnseignement INTEGER PRIMARY KEY, nom TEXT);
        CREATE TABLE seances (
          codeSeance INTEGER PRIMARY KEY,
          dateSeance TEXT NOT NULL,
          heureSeance INTEGER NOT NULL,
          dureeSeance INTEGER NOT NULL,
          codeEnseignement INTEGER NOT NULL
        );
        CREATE TABLE seances_profs (codeSeance INTEGER, codeProf INTEGER, PRIMARY KEY(codeSeance,codeProf));
        CREATE TABLE seances_groupes (codeSeance INTEGER, codeGroupe INTEGER, PRIMARY KEY(codeSeance,codeGroupe));
        CREATE TABLE seances_salles (codeSeance INTEGER, codeSalle INTEGER, PRIMARY KEY(codeSeance,codeSalle));

        -- Fermeture ascendante des groupes (descendant -> ancêtre, identité incluse)
        CREATE TABLE grp_up (descendant INTEGER, ancestor INTEGER, PRIMARY KEY(descendant, ancestor));

        -- Élèves extraits des groupes (detail_nom)
        CREATE TABLE students (
          codeGroupe INTEGER PRIMARY KEY,   -- groupe "individuel"
          raw_name TEXT NOT NULL,
          student_norm TEXT NOT NULL,       -- NORMALISÉ (NOM PRENOM ou selon la source)
          student_norm_alt TEXT NOT NULL    -- ordre alternatif pour robustesse
        );
        CREATE INDEX idx_students_norm ON students(student_norm);
        CREATE INDEX idx_students_alt  ON students(student_norm_alt);

        -- Slots rapides (par prof + groupe)
        CREATE TABLE session_slots (
          codeSeance INTEGER,
          dateSeance TEXT,
          start_hhmm TEXT,
          end_hhmm   TEXT,
          codeProf   INTEGER,
          codeGroupe INTEGER,
          course_name TEXT,
          room_name   TEXT,
          PRIMARY KEY (codeSeance, codeProf, codeGroupe)
        );
        CREATE INDEX idx_slots_date_prof_time ON session_slots(dateSeance, codeProf, start_hhmm, end_hhmm);
        CREATE INDEX idx_slots_grp ON session_slots(codeGroupe);

        -- Slots par élève (élève -> tous ses groupes ancêtres -> slots)
        CREATE TABLE student_slots (
          student_norm TEXT,
          codeProf INTEGER,
          dateSeance TEXT,
          start_hhmm TEXT,
          end_hhmm TEXT,
          course_name TEXT,
          room_name TEXT,
          codeSeance INTEGER,
          codeGroupe INTEGER
        );
        CREATE INDEX idx_student_slots_lk ON student_slots(student_norm, codeProf, dateSeance, start_hhmm, end_hhmm);
        """)