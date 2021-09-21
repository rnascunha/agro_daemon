--
-- File generated with SQLiteStudio v3.3.3 on ter. set. 21 07:33:58 2021
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
    subscribe          TEXT,
    root_password      BLOB,
    root_salt          BLOB
);


-- Table: policy
CREATE TABLE policy (
    policyid INTEGER PRIMARY KEY AUTOINCREMENT,
    groupid          REFERENCES user_group (user_groupid) 
                     NOT NULL,
    rules    INTEGER NOT NULL
);


-- Table: policy_type
CREATE TABLE policy_type (
    policyid    INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT,
    code        INTEGER NOT NULL,
    description TEXT
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


-- Table: user_group
CREATE TABLE user_group (
    user_groupid INTEGER PRIMARY KEY AUTOINCREMENT,
    name         TEXT    UNIQUE
                         NOT NULL,
    description  TEXT    DEFAULT ('') 
);


-- Table: user_user_group
CREATE TABLE user_user_group (
    user_user_groupid INTEGER PRIMARY KEY AUTOINCREMENT,
    userid            INTEGER REFERENCES user (userid) 
                              NOT NULL,
    user_groupid      INTEGER REFERENCES user_group (user_groupid) 
                              NOT NULL,
    UNIQUE (
        userid,
        user_groupid
    )
);


-- Index: 
CREATE UNIQUE INDEX "" ON push_notify (
    userid,
    user_agent
);


-- Index: user_device_idx
CREATE UNIQUE INDEX user_device_idx ON session (
    userid,
    user_agent
);


COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
