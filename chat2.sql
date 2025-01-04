-- 创建 friend 表
CREATE TABLE IF NOT EXISTS friend (
    user1 INTEGER NOT NULL,
    user2 INTEGER NOT NULL,
    PRIMARY KEY (user1, user2)
);

-- 创建 group_table 表
CREATE TABLE IF NOT EXISTS group_table (
    group_account INTEGER PRIMARY KEY,
    group_name VARCHAR(255), 
    create_time DATETIME,
    group_master INTEGER
);

-- 创建 user 表
CREATE TABLE IF NOT EXISTS `user` (
    account INTEGER AUTO_INCREMENT PRIMARY KEY, 
    password VARCHAR(32),
    name VARCHAR(32),
    signature TEXT,
    online INT NOT NULL DEFAULT 0, 
    icon TEXT
);

-- 创建 member 表
CREATE TABLE IF NOT EXISTS member (
    member_id INTEGER,
    group_account INTEGER,
    group_nickname TEXT
);