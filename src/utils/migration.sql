CREATE TABLE drafts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    to TEXT,
    subject TEXT,
    body TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE replies (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    draft_id INTEGER,
    persona TEXT,
    reply_body TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
