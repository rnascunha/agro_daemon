--
-- File generated with SQLiteStudio v3.3.3 on qui. ago. 26 18:22:20 2021
--
-- Text encoding used: UTF-8
--
PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- Table: push_notify
CREATE TABLE push_notify (
    notifyid INTEGER PRIMARY KEY AUTOINCREMENT,
    userid   INTEGER REFERENCES user (userid) ON DELETE CASCADE
                     NOT NULL,
    endpoint TEXT    NOT NULL,
    p256dh   TEXT    NOT NULL,
    auth     TEXT    NOT NULL
);


-- Table: user
CREATE TABLE user (
    userid   INTEGER PRIMARY KEY AUTOINCREMENT
                     NOT NULL,
    username TEXT    UNIQUE
                     NOT NULL,
    password TEXT    NOT NULL,
    name     TEXT,
    status   INT     DEFAULT (0) 
                     NOT NULL,
    email    TEXT
);


COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
