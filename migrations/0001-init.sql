CREATE TABLE IF NOT EXISTS spotify (
    id SERIAL PRIMARY KEY,
    spotify_username TEXT NOT NULL,
    spotify_token TEXT NOT NULL,
    spotify_token_expires TIMESTAMP NOT NULL,
    spotify_refresh_token TEXT NOT NULL
);