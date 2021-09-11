--
-- File generated with SQLiteStudio v3.3.3 on qua. set. 8 12:06:01 2021
--
-- Text encoding used: UTF-8
--
PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- Table: instance
CREATE TABLE instance (
    instanceid         INTEGER PRIMARY KEY AUTOINCREMENT,
    name               TEXT    NOT NULL,
    description        TEXT,
    notify_private_key TEXT,
    subscribe          TEXT
);


-- Table: push_notify
CREATE TABLE push_notify (
    push_notifyid INTEGER PRIMARY KEY AUTOINCREMENT,
    userid        INTEGER REFERENCES user (userid) ON DELETE CASCADE
                          NOT NULL,
    endpoint      TEXT,
    p256dh        TEXT,
    auth          TEXT,
    user_agent    TEXT
);


-- Table: session
CREATE TABLE session (
    sessionid    INTEGER  PRIMARY KEY AUTOINCREMENT,
    session_id   TEXT     NOT NULL,
    userid       INTEGER  REFERENCES user (userid) ON DELETE CASCADE,
    session_time DATETIME NOT NULL
                          DEFAULT (datetime(CURRENT_TIMESTAMP) ),
    user_agent   TEXT     NOT NULL
);


-- Table: user
CREATE TABLE user (
    userid   INTEGER   PRIMARY KEY AUTOINCREMENT
                       NOT NULL,
    username TEXT      UNIQUE
                       NOT NULL,
    password BLOB      NOT NULL,
    name     TEXT,
    status   INT       DEFAULT (0) 
                       NOT NULL,
    email    TEXT,
    salt     BLOB (32) NOT NULL
);


-- Index: user_device_idx
CREATE UNIQUE INDEX user_device_idx ON session (
    userid,
    user_agent
);


COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
