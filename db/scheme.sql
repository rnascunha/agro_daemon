--
-- File generated with SQLiteStudio v3.3.3 on qui. dez. 9 21:51:58 2021
--
-- Text encoding used: UTF-8
--
PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- Table: app
CREATE TABLE app (
    appid       INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    UNIQUE,
    uploader_id INTEGER REFERENCES user (userid),
    description TEXT,
    date_upload BIGINT,
    hash        BLOB
);


-- Table: children_table
CREATE TABLE children_table (
    children_tableid INTEGER PRIMARY KEY AUTOINCREMENT,
    children_mac     TEXT    NOT NULL,
    deviceid         INTEGER REFERENCES device (deviceid) 
                             NOT NULL
);


-- Table: device
CREATE TABLE device (
    deviceid        INTEGER PRIMARY KEY AUTOINCREMENT,
    mac             TEXT    UNIQUE
                            NOT NULL,
    name            TEXT,
    fw_version      TEXT    DEFAULT (''),
    hw_version      TEXT    DEFAULT (''),
    netid           INTEGER REFERENCES net (netid),
    mac_ap          TEXT,
    fuse            INTEGER,
    channel         INTEGER,
    channel_config  INTEGER,
    endpoint_addr   TEXT,
    endpoint_port   INTEGER,
    has_rtc         BOOLEAN DEFAULT (false),
    has_temp_sensor BOOLEAN DEFAULT (false),
    layer           INTEGER,
    parent_mac      TEXT
);


-- Table: device_state
CREATE TABLE device_state (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    deviceid  INTEGER REFERENCES device (deviceid) 
                      NOT NULL,
    time      INTEGER NOT NULL,
    connected BOOLEAN NOT NULL
);


-- Table: image
CREATE TABLE image (
    imageid     INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT    UNIQUE
                        NOT NULL,
    uploader_id INTEGER REFERENCES user (userid),
    description TEXT,
    date_upload BIGINT
);


-- Table: instance
CREATE TABLE instance (
    instanceid         INTEGER PRIMARY KEY AUTOINCREMENT,
    name               TEXT    NOT NULL,
    description        TEXT,
    notify_private_key TEXT,
    subscribe          TEXT,
    root_password      BLOB,
    root_salt          BLOB,
    telegram_bot_token TEXT,
    smtp_server        TEXT    DEFAULT (''),
    smtp_port          TEXT    DEFAULT (''),
    smtp_user          TEXT    DEFAULT (''),
    smtp_password      TEXT    DEFAULT (''),
    push_enable        BOOLEAN DEFAULT (1),
    telegram_enable    BOOLEAN DEFAULT (1),
    mail_enable        BOOLEAN DEFAULT (1) 
);


-- Table: net
CREATE TABLE net (
    netid    INTEGER PRIMARY KEY AUTOINCREMENT,
    net_addr TEXT    NOT NULL,
    name             DEFAULT ('') 
);


-- Table: notify_device
CREATE TABLE notify_device (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    userid   INTEGER REFERENCES user (userid) 
                     NOT NULL,
    deviceid INTEGER REFERENCES device (deviceid) 
                     NOT NULL,
    notify   INTEGER DEFAULT (0) 
);


-- Table: notify_general
CREATE TABLE notify_general (
    id     INTEGER PRIMARY KEY AUTOINCREMENT,
    userid INTEGER REFERENCES user (userid) 
                   UNIQUE
                   NOT NULL,
    notify INTEGER DEFAULT (0) 
);


-- Table: notify_sensor
CREATE TABLE notify_sensor (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    userid       INTEGER REFERENCES user (userid) 
                         NOT NULL,
    deviceid     INTEGER REFERENCES device (deviceid) 
                         NOT NULL,
    sensor_type  INTEGER NOT NULL,
    sensor_index INTEGER NOT NULL,
    notify_type  INTEGER NOT NULL,
    enabled      BOOLEAN NOT NULL
                         DEFAULT (1),
    value        DOUBLE  NOT NULL
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


-- Table: report
CREATE TABLE report (
    reportid  INTEGER PRIMARY KEY,
    userid    INTEGER REFERENCES user (userid),
    command   INTEGER,
    reference TEXT,
    message   TEXT,
    argument  TEXT,
    type      INTEGER NOT NULL,
    time      BIGINT  NOT NULL
);


-- Table: sensor_type
CREATE TABLE sensor_type (
    sensor_typeid INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT    UNIQUE,
    long_name     TEXT,
    type          INTEGER NOT NULL,
    description   TEXT,
    unit          TEXT,
    unit_name     TEXT,
    add_change    BOOLEAN
);


-- Table: sensor_value
CREATE TABLE sensor_value (
    id            INTEGER  PRIMARY KEY AUTOINCREMENT,
    deviceid      INTEGER  REFERENCES device (deviceid) 
                           NOT NULL,
    sensor_typeid INTEGER  REFERENCES sensor_type (sensor_typeid),
    time          INTEGER  NOT NULL,
    value         BLOB (4) NOT NULL,
    idx           INTEGER  NOT NULL
                           DEFAULT (0) 
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
    userid           INTEGER   PRIMARY KEY AUTOINCREMENT
                               NOT NULL,
    username         TEXT      UNIQUE
                               NOT NULL,
    password         BLOB      NOT NULL,
    name             TEXT,
    status           INT       DEFAULT (0) 
                               NOT NULL,
    email            TEXT,
    salt             BLOB (32) NOT NULL,
    telegram_chat_id TEXT
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


-- Index: user_device_notify
CREATE UNIQUE INDEX user_device_notify ON notify_device (
    userid,
    deviceid
);


COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
